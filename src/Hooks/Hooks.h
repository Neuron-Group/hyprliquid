#pragma once
#include <hyprland/src/plugins/HookSystem.hpp>
#include <hyprland/src/render/pass/SurfacePassElement.hpp>
#include <hyprland/src/render/GLRenderer.hpp>
#include <hyprland/src/desktop/rule/layerRule/LayerRuleApplicator.hpp>

inline CFunctionHook* g_IElementRendererDrawSurfaceHook;
using IElementRendererDrawSurface_t = void(*)(Render::IElementRenderer* this_ptr, WP<CSurfacePassElement> element, const CRegion& damage);
void HookIElementRendererDrawSurface(Render::IElementRenderer* this_ptr, WP<CSurfacePassElement> element, const CRegion& damage);

inline CFunctionHook* g_CHyprOpenGLImplRenderTextureInternalHook;
using CHyprOpenGLImplRenderTextureInternal_t = void(*)(Render::GL::CHyprOpenGLImpl* this_ptr, SP<Render::ITexture>, const CBox&, const Render::GL::CHyprOpenGLImpl::STextureRenderData& data);
void HookCHyprOpenGLImplRenderTextureInternal(Render::GL::CHyprOpenGLImpl* this_ptr, SP<Render::ITexture>, const CBox&, const Render::GL::CHyprOpenGLImpl::STextureRenderData& data);

inline CFunctionHook* g_CHyprOpenGLImplRenderTextureWithBlurInternalHook;
using CHyprOpenGLImplRenderTextureWithBlurInternal_t = void(*)(Render::GL::CHyprOpenGLImpl* this_ptr, SP<Render::ITexture>, const CBox&, const Render::GL::CHyprOpenGLImpl::STextureRenderData& data);
void HookCHyprOpenGLImplRenderTextureWithBlurInternal(Render::GL::CHyprOpenGLImpl* this_ptr, SP<Render::ITexture>, const CBox&, const Render::GL::CHyprOpenGLImpl::STextureRenderData& data);

inline CFunctionHook* g_CWindowRoundingHook;
using CWindowRounding_t = float(*)(Desktop::View::CWindow* this_ptr);
float HookCWindowRounding(Desktop::View::CWindow* this_ptr);