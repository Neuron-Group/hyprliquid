#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform sampler2D ReflectionMap;
uniform sampler2D Mask;
uniform vec2      TopLeft;
uniform vec2      BottomRight;
uniform vec2      Fullsize;
uniform vec2      TextureScale;
uniform vec4      LuminosityColor;
uniform vec4      TintColor;
uniform float     Brightness;
uniform float     DiscardAlphaValue;

in  vec2 v_texcoord;
out vec4 FragColor;

float GetLuminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 ClipColor(vec3 c)
{
    float l = GetLuminance(c);
    float n = min(min(c.r, c.g), c.b);
    float x = max(max(c.r, c.g), c.b);

    float d1 = l - n;
    float d2 = x - l;
    float eps = 1e-6;

    c = mix(c, l + (c - l) * l / max(d1, eps), step(n, 0.0));
    c = mix(c, l + (c - l) * (1.0 - l) / max(d2, eps), step(1.0, x));

    return c;
}

vec3 SetLuminance(vec3 c, float target_lum)
{
    float d = target_lum - GetLuminance(c);
    return ClipColor(c + d);
}

vec3 LuminosityBlend(vec3 base, vec3 blend)
{
    return SetLuminance(base, GetLuminance(blend));
}

vec3 ColorBlend(vec3 base, vec3 blend)
{
    return SetLuminance(blend, GetLuminance(base));
}

void main()
{
    vec2 uv = v_texcoord;

    vec3 texture_color = texture(Texture, uv).rgb;
    vec4 reflection    = texture(ReflectionMap, uv);

    vec3 lum_color = LuminosityBlend(texture_color, LuminosityColor.rgb);
    vec3 color = mix(texture_color, lum_color, LuminosityColor.a);

    vec3 tinted = ColorBlend(color, TintColor.rgb);
    color = mix(color, tinted, TintColor.a);

    color *= Brightness;

    color = mix(color, reflection.rgb, reflection.a);

    float mask = 1.0;
    if (DiscardAlphaValue > 0.0)
    {
        vec2 mask_uv = (v_texcoord - TopLeft / Fullsize) * TextureScale;
        float mask_alpha = texture(Mask, mask_uv).a;
        mask = smoothstep(0.0, DiscardAlphaValue, mask_alpha);
        color = mix(texture_color, color, mask);
        color *= mask;
    }

    color = clamp(color, 0.0, 1.0);

    FragColor = vec4(color, mask);
}
