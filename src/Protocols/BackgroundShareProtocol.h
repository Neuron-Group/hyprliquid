#pragma once
#include <background-share-unstable-v1.hpp>
#include <hyprland/src/protocols/WaylandProtocol.hpp>

class BackgroundShareProtocolResource
{
public:
    BackgroundShareProtocolResource(UP<CZhyprBackgroundShareUnstableV1>&& resource);

    bool good();
    int                     SurfaceID;
    CZhyprBufferSessionV1*  GetSession();

private:
    UP<CZhyprBackgroundShareUnstableV1> m_resource;
    UP<CZhyprBufferSessionV1>           m_session;
};


class BackgroundShareProtocol : public IWaylandProtocol
{
public:
    BackgroundShareProtocol(const wl_interface* iface, const int& ver, const std::string& name);
    virtual void bindManager(wl_client* client, void* data, uint32_t ver, uint32_t id);
    CZhyprBufferSessionV1* GetSession(int surface_id);

private:
    void DestroyResource(BackgroundShareProtocolResource* res);
    std::vector<UP<BackgroundShareProtocolResource>> m_managers;

    friend class BackgroundShareProtocolResource;
};

namespace PROTO
{
    inline UP<BackgroundShareProtocol> g_BackgroundShare;
};
