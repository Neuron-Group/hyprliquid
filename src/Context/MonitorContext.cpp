#include "MonitorContext.h"
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/state/MonitorState.hpp>

UP<CHyprSignalListener> MonitorAdded;
UP<CHyprSignalListener> MonitorRemoved;

void MonitorContext::Init()
{
    for (const auto& monitor: State::monitorState()->monitors())
        g_MonitorContexts.emplace(monitor.get(), makeShared<MonitorContext>(monitor.get()));

    MonitorAdded = makeUnique<CHyprSignalListener>(Event::bus()->m_events.monitor.added.listen([](PHLMONITOR monitor)
    {
        g_MonitorContexts.emplace(monitor.get(), makeShared<MonitorContext>(monitor.get()));
    }));

    MonitorRemoved = makeUnique<CHyprSignalListener>(Event::bus()->m_events.monitor.removed.listen([](PHLMONITOR monitor)
    {
        g_MonitorContexts.erase(monitor.get());
    }));
}

void MonitorContext::Destroy()
{
    MonitorAdded.reset();
    MonitorRemoved.reset();
    g_MonitorContexts.clear();
}

SP<MonitorContext> MonitorContext::GetContext(const monitor_t monitor)
{
    if (monitor == nullptr)
        return nullptr;
    auto iter = g_MonitorContexts.find(monitor);
    if (iter == g_MonitorContexts.end())
        return nullptr;
    return iter->second;
}

MonitorContext::MonitorContext(const monitor_t monitor)
    : m_BlurredBackground{nullptr}
    , m_BackgroundState{MonitorBackgroundState::NONE}
    , m_Monitor(monitor)
{
}

MonitorContext::~MonitorContext()
{
    BackgroundManager::DestroySharedBackgroundResource(this);
}

SP<Render::ITexture> MonitorContext::GetBlurredBackground()
{
    return m_BlurredBackground;
}

void MonitorContext::SetBlurredBackground(SP<Render::ITexture> texture)
{
    m_BlurredBackground = texture;
}

SP<Render::ITexture> MonitorContext::GetSharedBackground()
{
    return m_SharedBackground;
}

void MonitorContext::SetSharedBackground(SP<Render::ITexture> texture)
{
    m_SharedBackground = texture;
}

MonitorBackgroundState MonitorContext::GetBackgroundState()
{
    return m_BackgroundState;
}

void MonitorContext::SetBackgroundState(const MonitorBackgroundState state)
{
    m_BackgroundState = state;
}
