#pragma once
#include <hyprland/src/render/Shader.hpp>
#include <hyprland/src/render/shaders/Shaders.hpp>

namespace Shaders
{
    inline constexpr unsigned char JumpFloodingInnerInitSource[]
    {
        #embed "../Assets/GLSL/JumpFloodingInnerInit.frag" suffix(, 0)
    };

    inline constexpr unsigned char JumpFloodingOuterInitSource[]
    {
        #embed "../Assets/GLSL/JumpFloodingOuterInit.frag" suffix(, 0)
    };

    inline constexpr unsigned char JumpFloodingSource[]
    {
        #embed "../Assets/GLSL/JumpFlooding.frag" suffix(, 0)
    };

    inline constexpr unsigned char JumpFloodingDistanceFilterSource[]
    {
        #embed "../Assets/GLSL/JumpFloodingDistanceFilter.frag" suffix(, 0)
    };

    inline constexpr unsigned char JumpFloodingFinalSource[]
    {
        #embed "../Assets/GLSL/JumpFloodingFinal.frag" suffix(, 0)
    };

    inline constexpr unsigned char DebugVDFMapSource[]
    {
        #embed "../Assets/GLSL/DebugVDFMap.frag" suffix(, 0)
    };

    inline constexpr unsigned char ChecksumSource[]
    {
        #embed "../Assets/GLSL/Checksum.comp" suffix(, 0)
    };

    inline constexpr unsigned char BlackDetectionSource[]
    {
        #embed "../Assets/GLSL/BlackDetection.comp" suffix(, 0)
    };

    inline constexpr unsigned char LiquidGlassShaderSource[]
    {
        #embed "../Assets/GLSL/LiquidGlass.frag" suffix(, 0)
    };

    inline constexpr unsigned char FluentMaterialShaderSource[]
    {
        #embed "../Assets/GLSL/FluentMaterial.frag" suffix(, 0)
    };

    inline constexpr unsigned char AeroShaderSource[]
    {
        #embed "../Assets/GLSL/Aero.frag" suffix(, 0)
    };

    inline constexpr unsigned char AeroReflectionMapShaderSource[]
    {
        #embed "../Assets/GLSL/AeroReflectionMap.frag" suffix(, 0)
    };

    enum CustomShaderUniform : unsigned char
    {
        CUSTOM_SHADER_CORNER_RADIUS   = SHADER_RADIUS,
        CUSTOM_SHADER_Z_RADIUS        = SHADER_RADIUS_OUTER,
        CUSTOM_SHADER_GLASS_THICKNESS = SHADER_THICK,

        CUSTOM_SHADER_TEXTURE_SCALE = 58u,
        CUSTOM_SHADER_RECTANGLE_SIZE,
        CUSTOM_SHADER_GLASS_IOR,
        CUSTOM_SHADER_GLASS_IOR_RGB,
        CUSTOM_SHADER_GLASS_DISPERSION,
        CUSTOM_SHADER_BRIGHTNESS,
        CUSTOM_SHADER_HAS_VDF_MAP,
        CUSTOM_SHADER_VDF_MAP,
        CUSTOM_SHADER_HIGHLIGHT_STYLE,
        CUSTOM_SHADER_LUMINOSITY_COLOR,
        CUSTOM_SHADER_REFLECTION_MAP,

        CUSTOM_SHADER_JFA_STRIDE,
        CUSTOM_SHADER_JFA_DIRECTION,
        CUSTOM_SHADER_MASK_TEXTURE

    };

    inline SP<CShader> g_JumpFloodingInnerInitShader;
    SP<CShader> CreateJumpFloodingInnerInitShader();

    inline SP<CShader> g_JumpFloodingOuterInitShader;
    SP<CShader> CreateJumpFloodingOuterInitShader();

    inline SP<CShader> g_JumpFloodingShader;
    SP<CShader> CreateJumpFloodingShader();

    inline SP<CShader> g_JumpFloodingDistanceFilterShader;
    SP<CShader> CreateJumpFloodingDistanceFilterShader();

    inline SP<CShader> g_JumpFloodingFinalShader;
    SP<CShader> CreateJumpFloodingFinalShader();

    inline SP<CShader> g_DebugVDFMapShader;
    SP<CShader> CreateDebugVDFMapShader();

    inline SP<CShader> g_ChecksumShader;
    SP<CShader> CreateChecksumShader();

    inline SP<CShader> g_BlackDetectionShader;
    SP<CShader> CreateBlackDetectionShader();

    inline SP<CShader> g_LiquidGlassShader;
    SP<CShader> CreateLiquidGlassShader();

    inline SP<CShader> g_FluentMaterialShader;
    SP<CShader> CreateFluentMaterialShader();

    inline SP<CShader> g_AeroShader;
    SP<CShader> CreateAeroShader();

    inline SP<CShader> g_AeroReflectionMapShader;
    SP<CShader> CreateAeroReflectionMapShader();


    void Init();
    void Destroy();
    inline bool IsInitialized = false;
}