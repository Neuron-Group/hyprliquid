// Generated with hyprwayland-scanner 0.4.6. Made with vaxry's keyboard and ❤️.
// background_share_unstable_v1

/*
 This protocol's authors' copyright notice is:


*/

#pragma once

#include <functional>
#include <cstdint>
#include <string>
#include <wayland-server.h>

#define F std::function

struct wl_client;
struct wl_resource;


class CZhyprBackgroundShareUnstableV1;
class CZhyprBufferSessionV1;
class CWlSurface;
class CZhyprBufferSessionV1;

#ifndef HYPRWAYLAND_SCANNER_NO_INTERFACES
extern const wl_interface zhypr_background_share_unstable_v1_interface;
extern const wl_interface zhypr_buffer_session_v1_interface;

#endif

struct CZhyprBackgroundShareUnstableV1DestroyWrapper {
    wl_listener listener;
    CZhyprBackgroundShareUnstableV1* parent = nullptr;
};
            

class CZhyprBackgroundShareUnstableV1 {
  public:
    CZhyprBackgroundShareUnstableV1(wl_client* client, uint32_t version, uint32_t id);
    ~CZhyprBackgroundShareUnstableV1();


    // set a listener for when this resource is _being_ destroyed
    void setOnDestroy(F<void(CZhyprBackgroundShareUnstableV1*)> &&handler) {
        onDestroy = std::move(handler);
    }

    // set the data for this resource
    void setData(void* data) {
        pData = data;
    }

    // get the data for this resource
    void* data() {
        return pData;
    }

    // get the raw wl_resource ptr
    wl_resource* resource() {
        return pResource;
    }

    // get the client
    wl_client* client() {
        return wl_resource_get_client(pResource);
    }

    // send an error
    void error(uint32_t error, const std::string& message) {
        wl_resource_post_error(pResource, error, "%s", message.c_str());
    }

    // send out of memory
    void noMemory() {
        wl_resource_post_no_memory(pResource);
    }

    // get the resource version
    int version() {
        return wl_resource_get_version(pResource);
    }
            
    // --------------- Requests --------------- //

    void setGetBuffer(F<void(CZhyprBackgroundShareUnstableV1*, uint32_t, wl_resource*)> &&handler);
    void setDestroy(F<void(CZhyprBackgroundShareUnstableV1*)> &&handler);

    // --------------- Events --------------- //


  private:
    struct {
        F<void(CZhyprBackgroundShareUnstableV1*, uint32_t, wl_resource*)> getBuffer;
        F<void(CZhyprBackgroundShareUnstableV1*)> destroy;
    } requests;

    void onDestroyCalled();

    F<void(CZhyprBackgroundShareUnstableV1*)> onDestroy;

    wl_resource* pResource = nullptr;

    CZhyprBackgroundShareUnstableV1DestroyWrapper resourceDestroyListener;

    void* pData = nullptr;
};


struct CZhyprBufferSessionV1DestroyWrapper {
    wl_listener listener;
    CZhyprBufferSessionV1* parent = nullptr;
};
            

class CZhyprBufferSessionV1 {
  public:
    CZhyprBufferSessionV1(wl_client* client, uint32_t version, uint32_t id);
    ~CZhyprBufferSessionV1();


    // set a listener for when this resource is _being_ destroyed
    void setOnDestroy(F<void(CZhyprBufferSessionV1*)> &&handler) {
        onDestroy = std::move(handler);
    }

    // set the data for this resource
    void setData(void* data) {
        pData = data;
    }

    // get the data for this resource
    void* data() {
        return pData;
    }

    // get the raw wl_resource ptr
    wl_resource* resource() {
        return pResource;
    }

    // get the client
    wl_client* client() {
        return wl_resource_get_client(pResource);
    }

    // send an error
    void error(uint32_t error, const std::string& message) {
        wl_resource_post_error(pResource, error, "%s", message.c_str());
    }

    // send out of memory
    void noMemory() {
        wl_resource_post_no_memory(pResource);
    }

    // get the resource version
    int version() {
        return wl_resource_get_version(pResource);
    }
            
    // --------------- Requests --------------- //


    // --------------- Events --------------- //

    void sendBuffer(int32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t);
    void sendPosition(int32_t, int32_t);
    void sendBufferRaw(int32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t);
    void sendPositionRaw(int32_t, int32_t);

  private:
    struct {
    } requests;

    void onDestroyCalled();

    F<void(CZhyprBufferSessionV1*)> onDestroy;

    wl_resource* pResource = nullptr;

    CZhyprBufferSessionV1DestroyWrapper resourceDestroyListener;

    void* pData = nullptr;
};



#undef F
