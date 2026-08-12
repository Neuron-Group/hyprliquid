#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform float     CornerRadius;

out vec2 FragColor;

void main()
{
    vec2  pos = gl_FragCoord.xy;
    ivec2 pos_int = ivec2(pos);
    vec2  target = texelFetch(Texture, pos_int, 0).xy;

    FragColor = mix(vec2(1e6), pos, step(1e-6, length(target - pos) - CornerRadius));
}
