#pragma once
#include "Context/ClientContext.h"
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/render/gl/GLFramebuffer.hpp>

struct EGLBufferResource
{
    int         FD;
    uint32_t    Width;
    uint32_t    Height;
    int         Fourcc;
    int         Stride;
    int         Offset;
    uint32_t    ModifierLow;
    uint32_t    ModifierHigh;
    void*       EGLImage;
};

class MonitorContext;

class BackgroundManager
{
public:
    static void Init();
    static void Destroy();

    static void                 CreateBlurredBackground(PHLMONITOR monitor, SP<Render::ITexture> texture);
    static void                 ShareBackground(PHLMONITOR monitor, SP<Render::ITexture> texture);
    static SP<Render::ITexture> GetAeroReflectionMap();
public:
    static inline Hyprutils::Signal::CSignalT<std::pair<PHLMONITOR, SP<Render::ITexture>>> m_BackgroundChanged;

private:
    static void                 CreateDefaultAeroReflectionMap();
    static SP<Render::ITexture> GetDefaultAeroReflectionMap();
    static void                 CreateCustomAeroReflectionMap();
    static SP<Render::ITexture> GetCustomAeroReflectionMap();

private:
    static inline Hyprutils::Signal::CHyprSignalListener m_BackgroundChangedListener;
    static inline Hyprutils::Signal::CHyprSignalListener m_RenderStageListener;
    static inline SP<Render::ITexture> m_DefaultAeroReflectionMap;
    static inline SP<Render::ITexture> m_CustomAeroReflectionMap;

public:
    static inline bool SharingBackground;
    static bool IsEGLInitialized();
    static bool InitEGL();
    static void DestroyEGL();
    static SP<EGLBufferResource> CreateEGLImage(const GLint texture, const uint32_t width, const uint32_t height);
    static void DestroySharedBackgroundResource(MonitorContext* monitor_context);
    static void OnNewClient(SP<ClientContext> client_context);
private:
    static inline std::vector<WP<ClientContext>> EGLClients;
    static inline bool m_EGLInitialized = false;
};
