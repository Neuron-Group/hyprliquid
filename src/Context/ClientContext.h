#pragma once
#include "Config/ConfigManager.h"
#include "Utils/Utils.hpp"
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/render/gl/GLFramebuffer.hpp>
#include <hyprgraphics/egl/Egl.hpp>
#include <map>

enum class ClientType : uint8_t
{
    UNKNOWN,
    WINDOW,
    LAYER
};

using client_t = void*;
using Render::IFramebuffer;
using Render::ITexture;

class ClientContext
{
public:
    static void Init();
    static void Destroy();
    static SP<ClientContext> GetContext(const client_t client);
    static SP<ClientContext> CreateContext(const client_t client, const ClientType client_type);

public:
    ClientContext(const client_t client, const ClientType client_type);
    ~ClientContext();
    client_t GetClient();
    PHLMONITOR GetMonitor();
    SP<std::array<Render::GL::CGLFramebuffer, 3>> GetJFAFramebuffers(const int width, const int height);
    SP<std::array<GLuint, 2>> GetDownsampleFramebuffers();
    SP<Render::ITexture>      GetDownsampleTexture();
    const Vector2D&           GetSize() const;
    const ClientType          GetType() const;
    void CreateDownsampleFramebuffersAndTexture(const Vector2D& size);

    UP<std::array<ConfigManager::Variant, ConfigManager::ConfigType::CONFIG_LAST>> RuleConfigValueCache;

    SP<Render::ITexture>                  VDFMap;
    std::chrono::steady_clock::time_point VDFMapTimestamp;
    GLuint                                VDFMapChecksum;

private:
    void CreateJFAFramebuffers();
    void ResizeJFATextures();

private:
    Vector2D   m_Size;
    ClientType m_ClientType;
    client_t   m_Client;

    SP<std::array<Render::GL::CGLFramebuffer, 3>> m_JFAFramebuffers;
    SP<std::array<GLuint, 2>>                     m_DownsampleFramebuffers;
    SP<Render::ITexture>                          m_DownsampleTexture;

public:
    int      SurfaceID;
    bool     GotBackground;
    Vector2D Position;

    SP<AsyncSSBOReadback> TextureChecksumReadback;
};

inline std::map<client_t, SP<ClientContext>> g_ClientContexts;
