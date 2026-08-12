#include "Render.h"
#include "Utils/Utils.hpp"
#include "Shaders/Shaders.h"
#include "Config/RuleOrConfigValue.hpp"
#include "Config/RuleOrDefaultValue.hpp"
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/gl/GLTexture.hpp>
#include <hyprland/src/render/ShaderLoader.hpp>
#include <hyprland/src/helpers/cm/ColorManagement.hpp>
#include <hyprgraphics/egl/Egl.hpp>

using namespace Shaders;

template struct Infiltrator<&Render::GL::CHyprOpenGLImpl::m_blend>;
void RenderShader(IFramebuffer& fb, const SP<ITexture>& texture, const SP<CShader>& shader, const Hyprutils::Math::CRegion* damage, const Hyprutils::Math::Vector2D& viewport)
{
    const auto  BLENDBEFORE = Robber<Render::GL::CHyprOpenGLImpl, bool>::Get(Render::GL::g_pHyprOpenGL.get());
    const auto& render_data = g_pHyprRenderer->m_renderData;
    const bool  custom_viewport = viewport.x > 0 && viewport.y > 0;

    const auto  TRANSFORM  = Math::wlTransformToHyprutils(Math::invertTransform(render_data.pMonitor->m_transform));
    CBox        MONITORBOX = {0, 0, render_data.pMonitor->m_transformedSize.x, render_data.pMonitor->m_transformedSize.y};
    const auto& glMatrix = g_pHyprRenderer->projectBoxToTarget(MONITORBOX, TRANSFORM);

    if (!shader)
        return;
    
    GLint prev_viewport[4] = {};
    if (custom_viewport)
        glGetIntegerv(GL_VIEWPORT, prev_viewport);

    shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_TRUE, glMatrix.getMatrix());

    fb.bind();
    dc<Render::GL::CGLFramebuffer*>(&fb)->clearAfterInvalidation();
    if (custom_viewport)
        glViewport(0, 0, viewport.x, viewport.y);

    if (texture)
    {
        shader->setUniformInt(SHADER_TEX, 0);
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
    }

    glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));
    Render::GL::g_pHyprOpenGL->blend(false);
    if (damage && !damage->empty())
        damage->forEachRect([](const auto& rect)
        {
            Render::GL::g_pHyprOpenGL->scissor(&rect, g_pHyprRenderer->m_renderData.transformDamage);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        });
    else
    {
        Render::GL::g_pHyprOpenGL->scissor(nullptr);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    Render::GL::g_pHyprOpenGL->blend(BLENDBEFORE);

    glBindVertexArray(0);
    if (texture)
        texture->unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (custom_viewport)
        glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);
}

void DebugVDFMap(IFramebuffer& fb, const SP<ITexture>& vdf_map, const CBox& box)
{
    if (!vdf_map)
        return;

    const auto& size   = fb.m_size;
    const auto  shader = Render::GL::g_pHyprOpenGL->useShader(g_DebugVDFMapShader).lock();
    shader->setUniformFloat2(SHADER_TOP_LEFT, box.x, box.y);
    shader->setUniformFloat2(SHADER_FULL_SIZE, size.x, size.y);
    if (vdf_map->m_size.x != 0.0 && vdf_map->m_size.y != 0.0)
        shader->setUniformFloat2((eShaderUniform)CUSTOM_SHADER_TEXTURE_SCALE, size.x / box.width, size.y / box.height);
    else
        shader->setUniformFloat2((eShaderUniform)CUSTOM_SHADER_TEXTURE_SCALE, 0.0, 0.0);
    RenderShader(fb, vdf_map, shader);
    vdf_map->unbind();
}

SP<ITexture> GetOrUpdateVDFMap(const SP<ITexture>& texture, const Render::GL::CHyprOpenGLImpl::STextureRenderData& data, const SP<ClientContext>& client_context)
{
    static auto VDF_MAP_MODE          = RuleOrDefaultValue<Config::INTEGER>(ConfigManager::ConfigType::VDF_MAP_MODE, 0);
    static auto VDF_MAP_UPDATE_POLICY = RuleOrConfigValue<Config::INTEGER>(ConfigManager::ConfigType::VDF_MAP_UPDATE_POLICY);
    static auto CORNER_RADIUS         = RuleOrConfigValue<Config::INTEGER>(ConfigManager::ConfigType::CORNER_RADIUS);

    if (!texture)
        return nullptr;

    if (!client_context)
        return nullptr;

    int mode = VDF_MAP_MODE[client_context];
    if (mode == 0)
        return nullptr;

    auto vdf_map = client_context->VDFMap;
    int vdf_map_update_policy = VDF_MAP_UPDATE_POLICY[client_context];

    if (vdf_map_update_policy != -1 && client_context->TextureChecksumReadback)
        client_context->TextureChecksumReadback.reset();

    if (vdf_map)
    {
        if (vdf_map_update_policy == -1)
        {
            if (!client_context->TextureChecksumReadback)
                client_context->TextureChecksumReadback = makeShared<AsyncSSBOReadback>();
            auto checksum = GetTextureChecksum(texture, client_context->TextureChecksumReadback);
            if (client_context->VDFMapChecksum != checksum)
                client_context->VDFMapChecksum = checksum;
            else
                return vdf_map;
        }
        else if (vdf_map_update_policy == 0)
            return vdf_map;
        else if (vdf_map_update_policy > 0)
        {
            using namespace std::chrono;
            auto now = steady_clock::now();
            auto duration = now - client_context->VDFMapTimestamp;
            if (duration_cast<milliseconds>(duration).count() > vdf_map_update_policy)
                client_context->VDFMapTimestamp = now;
            else
                return vdf_map;
        }
    }

    float texture_size_w = texture->m_size.x;
    float texture_size_h = texture->m_size.y;

    if (texture_size_w <= 0 || texture_size_h <= 0)
        return nullptr;

    const auto& render_data = g_pHyprRenderer->m_renderData;

    SP<ITexture> tex;
    const auto& monitor_size = render_data.pMonitor->m_pixelSize;
    if (texture_size_w > monitor_size.x || texture_size_h > monitor_size.y)
    {
        tex = DownsampleTexture(texture, monitor_size, client_context);
        if (!tex)
            return nullptr;
        texture_size_w = tex->m_size.x;
        texture_size_h = tex->m_size.y;
    }
    else
        tex = texture;

    const float scale = render_data.pMonitor->m_scale;

    int corner_radius = CORNER_RADIUS[client_context];
    if (corner_radius < 0)
        corner_radius = data.round;
    else
        corner_radius *= scale;

    auto& framebuffers = *(client_context->GetJFAFramebuffers(texture_size_w, texture_size_h));

    const     float max_size = std::max(texture_size_w, texture_size_h);
    const     int   steps = std::ceil(std::log2(max_size));
    constexpr int   additional_steps = 1;
    const     int   total_steps = steps + additional_steps;

    SP<CShader>     shader;
    int             current_index = 1;

    if (mode == 1)
    {
        shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingInnerInitShader).lock();
        shader->setUniformInt((eShaderUniform)CUSTOM_SHADER_CORNER_RADIUS, corner_radius);
        RenderShader(framebuffers[1], tex, shader);
    }
    else if (mode == 2)
    {
        shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingOuterInitShader).lock();
        RenderShader(framebuffers[1], tex, shader);

        shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingShader).lock();
        glUniform2i(shader->getUniformLocation(SHADER_FULL_SIZE), texture_size_w, texture_size_h);
        shader->setUniformFloat((eShaderUniform)CUSTOM_SHADER_JFA_DIRECTION, 1.0);
        for (int i = 0; i < total_steps; i++)
        {
            float stride = std::pow(2, steps - i - 1);
            if (stride < 1)
                stride = 1;
            shader->setUniformInt((eShaderUniform)CUSTOM_SHADER_JFA_STRIDE, (int)stride);
            RenderShader(framebuffers[i % 2], framebuffers[(i + 1) % 2].getTexture(), shader);
        }
        current_index = total_steps - 1;

        shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingDistanceFilterShader).lock();
        shader->setUniformFloat((eShaderUniform)CUSTOM_SHADER_CORNER_RADIUS, (float)corner_radius);
        RenderShader(framebuffers[(current_index + 1) % 2], framebuffers[current_index % 2].getTexture(), shader);
        current_index++;
    }
    else
        return nullptr;

    shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingShader).lock();
    glUniform2i(shader->getUniformLocation(SHADER_FULL_SIZE), texture_size_w, texture_size_h);
    shader->setUniformFloat((eShaderUniform)CUSTOM_SHADER_JFA_DIRECTION, 0.0);
    for (int i = 0; i < total_steps; i++)
    {
        float stride = std::pow(2, steps - i - 1);
        if (stride < 1)
            stride = 1;
        shader->setUniformInt((eShaderUniform)CUSTOM_SHADER_JFA_STRIDE, (int)stride);
        RenderShader(framebuffers[(current_index + i + 1) % 2], framebuffers[(current_index + i) % 2].getTexture(), shader);
    }
    current_index += total_steps;

    shader = Render::GL::g_pHyprOpenGL->useShader(g_JumpFloodingFinalShader).lock();
    shader->setUniformFloat2(SHADER_FULL_SIZE, texture_size_w, texture_size_h);
    shader->setUniformInt((eShaderUniform)CUSTOM_SHADER_MASK_TEXTURE, 1);
    glActiveTexture(GL_TEXTURE1);
    tex->bind();
    RenderShader(framebuffers[2], framebuffers[current_index % 2].getTexture(), shader);

    vdf_map = framebuffers[2].getTexture();
    if (vdf_map_update_policy == 0)
    {
        using namespace std::chrono;
        auto now = steady_clock::now();
        auto duration = now - client_context->VDFMapTimestamp;
        if (duration > 100ms)
            client_context->VDFMap = vdf_map;
    }
    else
        client_context->VDFMap = vdf_map;
    return vdf_map;
}

SP<ITexture> DownsampleTexture(const SP<ITexture>& texture, const Hyprutils::Math::Vector2D& size, const SP<ClientContext>& client_context)
{
    if (!texture)
        return nullptr;

    if (!client_context)
        return nullptr;
    
    const auto& texture_size  = texture->m_size;
    const float scale = std::min(size.x / texture_size.x, size.y / texture_size.y);
    const Vector2D new_size(int(scale * texture_size.x), int(scale * texture_size.y));

    if (client_context->GetSize() != new_size)
        client_context->CreateDownsampleFramebuffersAndTexture(new_size);

    const auto pframebuffers = client_context->GetDownsampleFramebuffers();
    SP<ITexture> tex = client_context->GetDownsampleTexture();

    if (!pframebuffers && !tex)
        return nullptr;

    const auto& framebuffers = *pframebuffers;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->m_texID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->m_texID, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[1]);
    glBlitFramebuffer(0, 0, texture_size.x, texture_size.y,
                      0, 0, new_size.x, new_size.y,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

void RenderPassthrough(IFramebuffer& fb, const SP<ITexture>& texture, const Hyprutils::Math::Vector2D& viewport)
{
    if (!texture)
        return;

    GLint prev_read_fb = 0;
    GLint prev_draw_fb = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_read_fb);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_draw_fb);

    auto shader = Render::GL::g_pHyprOpenGL->useShader(Render::GL::g_pHyprOpenGL->getShaderVariant(Render::SH_FRAG_PASSTHRURGBA)).lock();
    RenderShader(fb, texture, shader, nullptr, viewport);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, prev_read_fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_draw_fb);
}

SP<ITexture> ResizeTexture(const SP<ITexture>& texture, const Hyprutils::Math::Vector2D& size)
{
    if (!texture)
        return nullptr;

    if (size.x <= 0 || size.y <= 0)
        return nullptr;

    Render::GL::CGLFramebuffer fb;
    fb.alloc(size.x, size.y);
    RenderPassthrough(fb, texture, size);

    return fb.getTexture();
}

SP<Render::ITexture> CopyTexture(const SP<ITexture>& texture)
{
    return ResizeTexture(texture, texture->m_size);
}

GLuint GetTextureChecksum(const SP<ITexture>& texture, SP<AsyncSSBOReadback> readback)
{
    if (!texture)
        return 0;

    if (!readback)
        return 0;

    auto checksum = readback->GetLatestResult();
    readback->PrepareWrite();
    
    auto shader = Render::GL::g_pHyprOpenGL->useShader(g_ChecksumShader);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    const auto& size = texture->m_size;
    constexpr int STRIDE = 16;
    constexpr int LOCAL_SIZE = 16;
    const int sample_x = (size.x + STRIDE - 1) / STRIDE;
    const int sample_y = (size.y + STRIDE - 1) / STRIDE;
    const int group_x = (sample_x + LOCAL_SIZE - 1) / LOCAL_SIZE;
    const int group_y = (sample_y + LOCAL_SIZE - 1) / LOCAL_SIZE;

    glDispatchCompute(group_x, group_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    readback->CommitWrite();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return checksum;
}

bool IsTextureBlack(const SP<ITexture>& texture, SP<AsyncSSBOReadback> readback)
{
    if (!texture)
        return true;

    if (!readback)
        return true;

    auto output = readback->GetLatestResult();
    readback->PrepareWrite();

    auto shader = Render::GL::g_pHyprOpenGL->useShader(g_BlackDetectionShader);
    shader->setUniformInt(SHADER_TEX, 0);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    const auto& size = texture->m_size;
    constexpr int STRIDE = 16;
    constexpr int LOCAL_SIZE = 16;
    const int sample_x = (size.x + STRIDE - 1) / STRIDE;
    const int sample_y = (size.y + STRIDE - 1) / STRIDE;
    const int group_x = (sample_x + LOCAL_SIZE - 1) / LOCAL_SIZE;
    const int group_y = (sample_y + LOCAL_SIZE - 1) / LOCAL_SIZE;

    glDispatchCompute(group_x, group_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    readback->CommitWrite();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return output == 0;
}

struct Tag_CHyprOpenGLImpl_PassCMUniforms
{
    using type = void(Render::GL::CHyprOpenGLImpl::*)(WP<CShader>, const NColorManagement::PImageDescription, const NColorManagement::PImageDescription, bool, float, int);
};
template struct InfiltratorAlt<Tag_CHyprOpenGLImpl_PassCMUniforms, static_cast<void(Render::GL::CHyprOpenGLImpl::*)(WP<CShader>, const NColorManagement::PImageDescription, const NColorManagement::PImageDescription, bool, float, int)>(&Render::GL::CHyprOpenGLImpl::passCMUniforms)>;

struct Tag_CHyprOpenGLImpl_CMSupported
{
    using type = bool Render::GL::CHyprOpenGLImpl::*;
};
template struct InfiltratorAlt<Tag_CHyprOpenGLImpl_CMSupported, &Render::GL::CHyprOpenGLImpl::m_cmSupported>;

SP<IFramebuffer> BlurTextureWithDamage(const float a, CRegion* originalDamage, SP<ITexture> texture, const Config::INTEGER blur_size, const Config::INTEGER blur_passes)
{
    using namespace Render;
    using namespace NColorManagement;
    using Render::GL::CGLFramebuffer;
    using Render::GL::g_pHyprOpenGL;

    TRACY_GPU_ZONE("RenderBlurFramebufferWithDamage");
    auto&      m_renderData = g_pHyprRenderer->m_renderData;

    auto& m_cmSupported = RobberAlt<Tag_CHyprOpenGLImpl_CMSupported>::Get(g_pHyprOpenGL.get());
    const auto BLENDBEFORE = Robber<Render::GL::CHyprOpenGLImpl, bool>::Get(g_pHyprOpenGL.get());
    g_pHyprOpenGL->blend(false);
    g_pHyprOpenGL->setCapStatus(GL_STENCIL_TEST, false);

    // get transforms for the full monitor
    const auto  TRANSFORM  = Math::wlTransformToHyprutils(Math::invertTransform(m_renderData.pMonitor->m_transform));
    CBox        MONITORBOX = {0, 0, m_renderData.pMonitor->m_transformedSize.x, m_renderData.pMonitor->m_transformedSize.y};

    const auto& glMatrix = g_pHyprRenderer->projectBoxToTarget(MONITORBOX, TRANSFORM);

    // get the config settings
    const  auto PBLURSIZE             = &blur_size;
    const  auto PBLURPASSES           = &blur_passes;
    static auto PBLURVIBRANCY         = CConfigValue<Config::FLOAT>("decoration:blur:vibrancy");
    static auto PBLURVIBRANCYDARKNESS = CConfigValue<Config::FLOAT>("decoration:blur:vibrancy_darkness");

    const auto  BLUR_PASSES = std::clamp(*PBLURPASSES, sc<int64_t>(1), sc<int64_t>(8));

    // prep damage
    CRegion damage{*originalDamage};
    damage.transform(Math::wlTransformToHyprutils(Math::invertTransform(m_renderData.pMonitor->m_transform)), m_renderData.pMonitor->m_transformedSize.x,
                     m_renderData.pMonitor->m_transformedSize.y);
    damage.expand(std::clamp(*PBLURSIZE, sc<int64_t>(1), sc<int64_t>(40)) * pow(2, BLUR_PASSES));

    // helper
    const auto PMIRRORFB     = g_pHyprRenderer->m_renderData.pMonitor->resources()->getUnusedWorkBuffer();
    const auto PMIRRORSWAPFB = g_pHyprRenderer->m_renderData.pMonitor->resources()->getUnusedWorkBuffer();

    auto       currentRenderToFB = PMIRRORFB;

    // Begin with base color adjustments - global brightness and contrast
    // TODO: make this a part of the first pass maybe to save on a drawcall?
    {
        static auto PBLURCONTRAST   = CConfigValue<Config::FLOAT>("decoration:blur:contrast");
        static auto PBLURBRIGHTNESS = CConfigValue<Config::FLOAT>("decoration:blur:brightness");
        static auto PBLEND          = CConfigValue<Config::INTEGER>("render:use_shader_blur_blend");

        PMIRRORSWAPFB->bind();
        GLFB(PMIRRORSWAPFB)->clearAfterInvalidation();

        glActiveTexture(GL_TEXTURE0);

        auto currentTex = texture;

        currentTex->bind();
        currentTex->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        WP<CShader> shader;

        // From FB to sRGB
        const bool skipCM = !m_cmSupported || !g_pHyprRenderer->workBufferImageDescription()->needsCM(getDefaultImageDescription());
        if (!skipCM) {
            shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLURPREPARE, SH_FEAT_CM));
            RobberAlt<Tag_CHyprOpenGLImpl_PassCMUniforms>::Call(g_pHyprOpenGL.get(), shader, getDefaultImageDescription(), g_pHyprRenderer->workBufferImageDescription(), false, -1.0f, -1);
            shader->setUniformFloat(SHADER_SDR_SATURATION,
                                    m_renderData.pMonitor->m_sdrSaturation > 0 &&
                                            g_pHyprRenderer->workBufferImageDescription()->value().transferFunction == NColorManagement::CM_TRANSFER_FUNCTION_ST2084_PQ ?
                                        m_renderData.pMonitor->m_sdrSaturation :
                                        1.0f);
            shader->setUniformFloat(SHADER_SDR_BRIGHTNESS,
                                    m_renderData.pMonitor->m_sdrBrightness > 0 &&
                                            g_pHyprRenderer->workBufferImageDescription()->value().transferFunction == NColorManagement::CM_TRANSFER_FUNCTION_ST2084_PQ ?
                                        m_renderData.pMonitor->m_sdrBrightness :
                                        1.0f);
        } else
            shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLURPREPARE));

        const auto& glMatrix = g_pHyprRenderer->projectBoxToTarget(MONITORBOX, *PBLEND ? HYPRUTILS_TRANSFORM_NORMAL : TRANSFORM);
        shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_TRUE, glMatrix.getMatrix());
        shader->setUniformFloat(SHADER_CONTRAST, *PBLURCONTRAST);
        shader->setUniformFloat(SHADER_BRIGHTNESS, *PBLURBRIGHTNESS);
        shader->setUniformInt(SHADER_TEX, 0);

        glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));

        if (!damage.empty()) {
            damage.forEachRect([](const auto& RECT) {
                g_pHyprOpenGL->scissor(&RECT, false /* this region is already transformed */);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            });
        }

        glBindVertexArray(0);
        currentRenderToFB = PMIRRORSWAPFB;
    }

    // declare the draw func
    auto drawPass = [&](WP<CShader> shader, ePreparedFragmentShader frag, CRegion* pDamage) {
        if (currentRenderToFB == PMIRRORFB)
            PMIRRORSWAPFB->bind();
        else
            PMIRRORFB->bind();

        glActiveTexture(GL_TEXTURE0);

        auto currentTex = currentRenderToFB->getTexture();

        currentTex->bind();

        currentTex->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // prep two shaders
        shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_TRUE, glMatrix.getMatrix());
        shader->setUniformFloat(SHADER_RADIUS, *PBLURSIZE * a); // this makes the blursize change with a
        if (frag == SH_FRAG_BLUR1) {
            shader->setUniformFloat2(SHADER_HALFPIXEL, 0.5f / (m_renderData.pMonitor->m_pixelSize.x / 2.f), 0.5f / (m_renderData.pMonitor->m_pixelSize.y / 2.f));
            shader->setUniformInt(SHADER_PASSES, BLUR_PASSES);
            shader->setUniformFloat(SHADER_VIBRANCY, *PBLURVIBRANCY);
            shader->setUniformFloat(SHADER_VIBRANCY_DARKNESS, *PBLURVIBRANCYDARKNESS);
        } else
            shader->setUniformFloat2(SHADER_HALFPIXEL, 0.5f / (m_renderData.pMonitor->m_pixelSize.x * 2.f), 0.5f / (m_renderData.pMonitor->m_pixelSize.y * 2.f));
        shader->setUniformInt(SHADER_TEX, 0);

        glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));

        if (!pDamage->empty()) {
            pDamage->forEachRect([](const auto& RECT) {
                g_pHyprOpenGL->scissor(&RECT, false /* this region is already transformed */);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            });
        }

        glBindVertexArray(0);

        if (currentRenderToFB != PMIRRORFB)
            currentRenderToFB = PMIRRORFB;
        else
            currentRenderToFB = PMIRRORSWAPFB;
    };

    // draw the things.
    // first draw is swap -> mirr
    PMIRRORFB->bind();
    GLFB(PMIRRORFB)->clearAfterInvalidation();
    PMIRRORSWAPFB->getTexture()->bind();

    // damage region will be scaled, make a temp
    CRegion tempDamage{damage};

    // and draw
    auto shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLUR1));
    for (auto i = 1; i <= BLUR_PASSES; ++i) {
        tempDamage = damage.copy().scale(1.f / (1 << i));
        drawPass(shader, SH_FRAG_BLUR1, &tempDamage); // down
    }

    shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLUR2));
    for (auto i = BLUR_PASSES - 1; i >= 0; --i) {
        tempDamage = damage.copy().scale(1.f / (1 << i)); // when upsampling we make the region twice as big
        drawPass(shader, SH_FRAG_BLUR2, &tempDamage);     // up
    }

    // finalize the image
    {
        static auto PBLURNOISE      = CConfigValue<Config::FLOAT>("decoration:blur:noise");
        static auto PBLURBRIGHTNESS = CConfigValue<Config::FLOAT>("decoration:blur:brightness");

        if (currentRenderToFB == PMIRRORFB)
            PMIRRORSWAPFB->bind();
        else
            PMIRRORFB->bind();

        glActiveTexture(GL_TEXTURE0);

        auto currentTex = currentRenderToFB->getTexture();

        currentTex->bind();

        currentTex->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // From FB to sRGB
        const bool skipCM = !m_cmSupported || !g_pHyprRenderer->workBufferImageDescription()->needsCM(getDefaultImageDescription());
        if (!skipCM) {
            shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLURFINISH, SH_FEAT_CM));
            RobberAlt<Tag_CHyprOpenGLImpl_PassCMUniforms>::Call(g_pHyprOpenGL.get(), shader, getDefaultImageDescription(), g_pHyprRenderer->workBufferImageDescription(), false, -1.0f, -1);
            shader->setUniformFloat(SHADER_SDR_SATURATION,
                                    m_renderData.pMonitor->m_sdrSaturation > 0 &&
                                            g_pHyprRenderer->workBufferImageDescription()->value().transferFunction == NColorManagement::CM_TRANSFER_FUNCTION_ST2084_PQ ?
                                        m_renderData.pMonitor->m_sdrSaturation :
                                        1.0f);
            shader->setUniformFloat(SHADER_SDR_BRIGHTNESS,
                                    m_renderData.pMonitor->m_sdrBrightness > 0 &&
                                            g_pHyprRenderer->workBufferImageDescription()->value().transferFunction == NColorManagement::CM_TRANSFER_FUNCTION_ST2084_PQ ?
                                        m_renderData.pMonitor->m_sdrBrightness :
                                        1.0f);
        } else
            shader = g_pHyprOpenGL->useShader(g_pHyprOpenGL->getShaderVariant(SH_FRAG_BLURFINISH));

        shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_TRUE, glMatrix.getMatrix());
        shader->setUniformFloat(SHADER_NOISE, *PBLURNOISE);
        shader->setUniformFloat(SHADER_BRIGHTNESS, *PBLURBRIGHTNESS);

        shader->setUniformInt(SHADER_TEX, 0);

        glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));

        if (!damage.empty()) {
            damage.forEachRect([](const auto& RECT) {
                g_pHyprOpenGL->scissor(&RECT, false /* this region is already transformed */);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            });
        }

        glBindVertexArray(0);

        if (currentRenderToFB != PMIRRORFB)
            currentRenderToFB = PMIRRORFB;
        else
            currentRenderToFB = PMIRRORSWAPFB;
    }

    // finish
    PMIRRORFB->getTexture()->unbind();

    g_pHyprOpenGL->blend(BLENDBEFORE);

    return currentRenderToFB;
}

SP<ITexture> EnsureBlur(const float a, CRegion *originalDamage, SP<ITexture> texture, const Config::INTEGER blur_size, const Config::INTEGER blur_passes)
{
    static auto PBLURSIZE   = CConfigValue<Config::INTEGER>("decoration:blur:size");
    static auto PBLURPASSES = CConfigValue<Config::INTEGER>("decoration:blur:passes");

    if (blur_size != *PBLURSIZE || blur_passes != *PBLURPASSES || !texture)
        return BlurTextureWithDamage(a, originalDamage, g_pHyprRenderer->m_renderData.currentFB->getTexture(), blur_size, blur_passes)->getTexture();
    else
        return texture;
}
