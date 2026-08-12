#version 300 es
precision highp float;

uniform sampler2D Texture;

out vec2 FragColor;

void main()
{
    ivec2 pos = ivec2(gl_FragCoord.xy);
    ivec2 texture_size = textureSize(Texture, 0);

    ivec2 up_pos    = pos + ivec2(0, -1);
    ivec2 down_pos  = pos + ivec2(0,  1);
    ivec2 left_pos  = pos + ivec2(-1, 0);
    ivec2 right_pos = pos + ivec2( 1, 0);

    float center = texelFetch(Texture, pos, 0).a;
    float up    = mix(0.0, texelFetch(Texture, up_pos, 0).a,    step(0.0, float(up_pos.y)));
    float down  = mix(0.0, texelFetch(Texture, down_pos, 0).a,  step(float(down_pos.y), float(texture_size.y)));
    float left  = mix(0.0, texelFetch(Texture, left_pos, 0).a,  step(0.0, float(left_pos.x)));
    float right = mix(0.0, texelFetch(Texture, right_pos, 0).a, step(float(right_pos.x), float(texture_size.x)));

    float surrouding = step(1e-6, max(max(up, down), max(left, right)));
    FragColor = mix(vec2(1e6), gl_FragCoord.xy, min(step(center, 1e-6), surrouding));
}
