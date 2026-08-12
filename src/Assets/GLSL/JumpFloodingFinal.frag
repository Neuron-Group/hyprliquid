#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform sampler2D Mask;

out vec3 FragColor;

void main()
{
    vec2  pos = gl_FragCoord.xy;
    ivec2 pos_int = ivec2(pos);
    float mask_alpha = texelFetch(Mask, pos_int, 0).a;
    vec2  target = texelFetch(Texture, pos_int, 0).xy;

    float s = step(1e-6, mask_alpha);
    FragColor = vec3(mix(vec2(0.0), pos - target, s), s);
}
