#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform int       CornerRadius;

out vec2 FragColor;

void main()
{
    int r = CornerRadius - 1;
    ivec2 pos = ivec2(gl_FragCoord.xy);
    ivec2 texture_size = textureSize(Texture, 0);

    ivec2 up_pos    = pos + ivec2(0, -r);
    ivec2 down_pos  = pos + ivec2(0,  r);
    ivec2 left_pos  = pos + ivec2(-r, 0);
    ivec2 right_pos = pos + ivec2( r, 0);

    float up    = mix(0.0, texelFetch(Texture, up_pos, 0).a,    step(0.0, float(up_pos.y)));
    float down  = mix(0.0, texelFetch(Texture, down_pos, 0).a,  step(float(down_pos.y), float(texture_size.y)));
    float left  = mix(0.0, texelFetch(Texture, left_pos, 0).a,  step(0.0, float(left_pos.x)));
    float right = mix(0.0, texelFetch(Texture, right_pos, 0).a, step(float(right_pos.x), float(texture_size.x)));

    FragColor = mix(vec2(1e6), gl_FragCoord.xy, step(1e-6, min(min(up, down), min(left, right))));
}
