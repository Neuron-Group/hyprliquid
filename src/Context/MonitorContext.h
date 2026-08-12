#pragma once
#include "Utils/BackgroundManager.h"
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <map>

using monitor_t = void*;

enum class MonitorBackgroundState : uint8_t
{
    NONE = 0,
    INTRINSIC,
    LAYERSHELL
};

class MonitorContext
{
public:
    static void Init();
    static void Destroy();
    static SP<MonitorContext> GetContext(const monitor_t client);

public:
    MonitorContext(const monitor_t monitor);
    ~MonitorContext();
    SP<Render::ITexture> GetBlurredBackground();
    void                 SetBlurredBackground(SP<Render::ITexture> texture);

    SP<Render::ITexture> GetSharedBackground();
    void                 SetSharedBackground(SP<Render::ITexture> texture);

    MonitorBackgroundState GetBackgroundState();
    void                   SetBackgroundState(const MonitorBackgroundState state);

private:
    SP<Render::ITexture>   m_BlurredBackground;
    SP<Render::ITexture>   m_SharedBackground;
    MonitorBackgroundState m_BackgroundState;
    monitor_t              m_Monitor;

public:
    SP<EGLBufferResource>          SharedBackgroundEGLResource;
    SP<Render::GL::CGLFramebuffer> SharedBackgroundFramebuffer;
};

inline std::map<monitor_t, SP<MonitorContext>> g_MonitorContexts;