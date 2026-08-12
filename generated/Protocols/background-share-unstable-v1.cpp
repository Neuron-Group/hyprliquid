// Generated with hyprwayland-scanner 0.4.6. Made with vaxry's keyboard and ❤️.
// background_share_unstable_v1

/*
 This protocol's authors' copyright notice is:


*/

#define private public
#define HYPRWAYLAND_SCANNER_NO_INTERFACES
#include "background-share-unstable-v1.hpp"
#undef private
#define F std::function

static const wl_interface* backgroundShareUnstableV1_dummyTypes[] = { nullptr };

// Reference all other interfaces.
// The reason why this is in snake is to
// be able to cooperate with existing
// wayland_scanner interfaces (they are interop)
extern const wl_interface zhypr_background_share_unstable_v1_interface;
extern const wl_interface zhypr_buffer_session_v1_interface;
extern const wl_interface wl_surface_interface;

static void _CZhyprBackgroundShareUnstableV1GetBuffer(wl_client* client, wl_resource* resource, uint32_t id, wl_resource* surface) {
    const auto PO = (CZhyprBackgroundShareUnstableV1*)wl_resource_get_user_data(resource);
    if (PO && PO->requests.getBuffer)
        PO->requests.getBuffer(PO, id, surface);
}

static void _CZhyprBackgroundShareUnstableV1Destroy(wl_client* client, wl_resource* resource) {
    const auto PO = (CZhyprBackgroundShareUnstableV1*)wl_resource_get_user_data(resource);
    if (PO && PO->requests.destroy)
        PO->requests.destroy(PO);
}

static void _CZhyprBackgroundShareUnstableV1__DestroyListener(wl_listener* l, void* d) {
    CZhyprBackgroundShareUnstableV1DestroyWrapper *wrap = wl_container_of(l, wrap, listener);
    CZhyprBackgroundShareUnstableV1* pResource = wrap->parent;
    pResource->onDestroyCalled();
}

static const void* _CZhyprBackgroundShareUnstableV1VTable[] = {
    (void*)_CZhyprBackgroundShareUnstableV1GetBuffer,
    (void*)_CZhyprBackgroundShareUnstableV1Destroy,
};
static const wl_interface* _CZhyprBackgroundShareUnstableV1GetBufferTypes[] = {
    &zhypr_buffer_session_v1_interface,
    &wl_surface_interface,
};

static const wl_message _CZhyprBackgroundShareUnstableV1Requests[] = {
    { .name = "get_buffer", .signature = "no", .types = _CZhyprBackgroundShareUnstableV1GetBufferTypes + 0},
    { .name = "destroy", .signature = "", .types = backgroundShareUnstableV1_dummyTypes + 0},
};

const wl_interface zhypr_background_share_unstable_v1_interface = {
    .name = "zhypr_background_share_unstable_v1", .version = 1,
    .method_count = 2, .methods = _CZhyprBackgroundShareUnstableV1Requests,
    .event_count = 0, .events = nullptr,
};

CZhyprBackgroundShareUnstableV1::CZhyprBackgroundShareUnstableV1(wl_client* client, uint32_t version, uint32_t id) :
    pResource(wl_resource_create(client, &zhypr_background_share_unstable_v1_interface, version, id)) {

    if (!pResource)
        return;

    wl_resource_set_user_data(pResource, this);
    wl_list_init(&resourceDestroyListener.listener.link);
    resourceDestroyListener.listener.notify = _CZhyprBackgroundShareUnstableV1__DestroyListener;
    resourceDestroyListener.parent = this;
    wl_resource_add_destroy_listener(pResource, &resourceDestroyListener.listener);

    wl_resource_set_implementation(pResource, _CZhyprBackgroundShareUnstableV1VTable, this, nullptr);
}

CZhyprBackgroundShareUnstableV1::~CZhyprBackgroundShareUnstableV1() {
    wl_list_remove(&resourceDestroyListener.listener.link);
    wl_list_init(&resourceDestroyListener.listener.link);

    // if we still own the wayland resource,
    // it means we need to destroy it.
    if (pResource && wl_resource_get_user_data(pResource) == this) {
        wl_resource_set_user_data(pResource, nullptr);
        wl_resource_destroy(pResource);
    }
}

void CZhyprBackgroundShareUnstableV1::onDestroyCalled() {
    wl_resource_set_user_data(pResource, nullptr);
    wl_list_remove(&resourceDestroyListener.listener.link);
    wl_list_init(&resourceDestroyListener.listener.link);

    // set the resource to nullptr,
    // as it will be freed. If the consumer does not destroy this resource
    // in onDestroy here, we'd be doing a UAF in the ~dtor
    pResource = nullptr;

    if (onDestroy)
        onDestroy(this);
}

void CZhyprBackgroundShareUnstableV1::setGetBuffer(F<void(CZhyprBackgroundShareUnstableV1*, uint32_t, wl_resource*)> &&handler) {
    requests.getBuffer = std::move(handler);
}

void CZhyprBackgroundShareUnstableV1::setDestroy(F<void(CZhyprBackgroundShareUnstableV1*)> &&handler) {
    requests.destroy = std::move(handler);
}

static void _CZhyprBufferSessionV1__DestroyListener(wl_listener* l, void* d) {
    CZhyprBufferSessionV1DestroyWrapper *wrap = wl_container_of(l, wrap, listener);
    CZhyprBufferSessionV1* pResource = wrap->parent;
    pResource->onDestroyCalled();
}

static const void* _CZhyprBufferSessionV1VTable[] = {
    nullptr,
};

void CZhyprBufferSessionV1::sendBuffer(int32_t fd, uint32_t width, uint32_t height, int32_t fourcc, int32_t stride, int32_t offset, uint32_t modifier_low, uint32_t modifier_high) {
    if (!pResource)
        return;
    wl_resource_post_event(pResource, 0, fd, width, height, fourcc, stride, offset, modifier_low, modifier_high);
}

void CZhyprBufferSessionV1::sendPosition(int32_t x, int32_t y) {
    if (!pResource)
        return;
    wl_resource_post_event(pResource, 1, x, y);
}

void CZhyprBufferSessionV1::sendBufferRaw(int32_t fd, uint32_t width, uint32_t height, int32_t fourcc, int32_t stride, int32_t offset, uint32_t modifier_low, uint32_t modifier_high) {
    if (!pResource)
        return;
    wl_resource_post_event(pResource, 0, fd, width, height, fourcc, stride, offset, modifier_low, modifier_high);
}

void CZhyprBufferSessionV1::sendPositionRaw(int32_t x, int32_t y) {
    if (!pResource)
        return;
    wl_resource_post_event(pResource, 1, x, y);
}
static const wl_interface* _CZhyprBufferSessionV1BufferTypes[] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};
static const wl_interface* _CZhyprBufferSessionV1PositionTypes[] = {
    nullptr,
    nullptr,
};

static const wl_message _CZhyprBufferSessionV1Events[] = {
    { .name = "buffer", .signature = "huuiiiuu", .types = _CZhyprBufferSessionV1BufferTypes + 0},
    { .name = "position", .signature = "ii", .types = _CZhyprBufferSessionV1PositionTypes + 0},
};

const wl_interface zhypr_buffer_session_v1_interface = {
    .name = "zhypr_buffer_session_v1", .version = 1,
    .method_count = 0, .methods = nullptr,
    .event_count = 2, .events = _CZhyprBufferSessionV1Events,
};

CZhyprBufferSessionV1::CZhyprBufferSessionV1(wl_client* client, uint32_t version, uint32_t id) :
    pResource(wl_resource_create(client, &zhypr_buffer_session_v1_interface, version, id)) {

    if (!pResource)
        return;

    wl_resource_set_user_data(pResource, this);
    wl_list_init(&resourceDestroyListener.listener.link);
    resourceDestroyListener.listener.notify = _CZhyprBufferSessionV1__DestroyListener;
    resourceDestroyListener.parent = this;
    wl_resource_add_destroy_listener(pResource, &resourceDestroyListener.listener);

    wl_resource_set_implementation(pResource, _CZhyprBufferSessionV1VTable, this, nullptr);
}

CZhyprBufferSessionV1::~CZhyprBufferSessionV1() {
    wl_list_remove(&resourceDestroyListener.listener.link);
    wl_list_init(&resourceDestroyListener.listener.link);

    // if we still own the wayland resource,
    // it means we need to destroy it.
    if (pResource && wl_resource_get_user_data(pResource) == this) {
        wl_resource_set_user_data(pResource, nullptr);
        wl_resource_destroy(pResource);
    }
}

void CZhyprBufferSessionV1::onDestroyCalled() {
    wl_resource_set_user_data(pResource, nullptr);
    wl_list_remove(&resourceDestroyListener.listener.link);
    wl_list_init(&resourceDestroyListener.listener.link);

    // set the resource to nullptr,
    // as it will be freed. If the consumer does not destroy this resource
    // in onDestroy here, we'd be doing a UAF in the ~dtor
    pResource = nullptr;

    if (onDestroy)
        onDestroy(this);
}

#undef F
