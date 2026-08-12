#include "Utils.hpp"
#include "Render/Render.h"
#include <hyprland/src/render/gl/GLFramebuffer.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/ShaderLoader.hpp>
#include <hyprland/src/managers/eventLoop/EventLoopManager.hpp>

FrameDelayGate::FrameDelayGate(int max_frames)
    : m_MaxFrames{max_frames}
{
}

bool FrameDelayGate::operator()(bool should_delay)
{
    if (m_Counter > 0)
    {
        m_Counter--;
        return false;
    }
    else if (should_delay)
    {
        m_Counter = m_MaxFrames;
        return false;
    }

    m_Counter = m_MaxFrames;
    return true;
}

bool FrameDelayGate::operator()(std::function<bool()> func)
{
    if (m_Counter > 0)
    {
        m_Counter--;
        return false;
    }
    else if (func())
    {
        m_Counter = m_MaxFrames;
        return false;
    }

    m_Counter = m_MaxFrames;
    return true;
}

AsyncSSBOReadback::AsyncSSBOReadback()
    : m_LastResult{0}
    , m_WriteSlotIndex{0}
{
    for (auto& slot : m_Slots)
    {
        glGenBuffers(1, &slot.SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, slot.SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_READ);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

AsyncSSBOReadback::~AsyncSSBOReadback()
{
    for (auto& slot : m_Slots)
    {
        if (slot.Fence)
            glDeleteSync(slot.Fence);
        if (slot.SSBO)
        {
            glDeleteBuffers(1, &slot.SSBO);
            slot.SSBO = 0;
        }
    }
}

AsyncSSBOReadbackSlot& AsyncSSBOReadback::GetReadSlot()
{
    return m_Slots[1 - m_WriteSlotIndex];
}

AsyncSSBOReadbackSlot& AsyncSSBOReadback::GetWriteSlot()
{
    return m_Slots[m_WriteSlotIndex];
}

bool AsyncSSBOReadback::IsReadReady()
{
    auto& read_slot = GetReadSlot();
    if (!read_slot.Fence)
        return false;

    GLenum res = glClientWaitSync(read_slot.Fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
    return res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED;
}

GLuint AsyncSSBOReadback::GetLatestResult()
{
    if (!IsReadReady())
        return m_LastResult;

    auto& read_slot = GetReadSlot();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, read_slot.SSBO);
    void* presult = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
    if (presult)
    {
        m_LastResult = *static_cast<GLuint*>(presult);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    glDeleteSync(read_slot.Fence);
    read_slot.Fence = nullptr;

    return m_LastResult;
}

void AsyncSSBOReadback::PrepareWrite()
{
    auto& write_slot = GetWriteSlot();
    if (write_slot.Fence)
    {
        glDeleteSync(write_slot.Fence);
        write_slot.Fence = nullptr;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, write_slot.SSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &ZERO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, write_slot.SSBO);
}

void AsyncSSBOReadback::CommitWrite()
{
    auto& write_slot = GetWriteSlot();
    write_slot.Fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    m_WriteSlotIndex = 1 - m_WriteSlotIndex;
}

CHyprColor MakeColor(const uint8_t rgb, const float alpha)
{
    const float rgb_f = rgb / 255.0;
    return CHyprColor(rgb_f, rgb_f, rgb_f, alpha);
}

CHyprColor MakeColor(const uint8_t r, const uint8_t g, const uint8_t b, const float alpha)
{
    return CHyprColor(r / 255.0, g  / 255.0, b  / 255.0, alpha);
}

void DoAfterNLoops(const std::function<void()> fn, int loops)
{
    if (!g_pEventLoopManager)
        return;

    if (loops > 1)
    {
        g_pEventLoopManager->doLater([fn, loops] { DoAfterNLoops(fn, loops - 1); });
        return;
    }
    g_pEventLoopManager->doLater(fn);
}
