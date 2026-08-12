#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform int       Stride;
uniform ivec2     TextureSize;
uniform float     Direction;

out vec2 FragColor;

void main()
{
    vec2  pixel_pos = gl_FragCoord.xy;
    ivec2 pixel_pos_int = ivec2(pixel_pos);
    vec2  best_pos = texelFetch(Texture, pixel_pos_int, 0).xy;
    float min_dist = length(best_pos - pixel_pos);

    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
        {
            ivec2 neighbor_pos = pixel_pos_int + ivec2(x, y) * Stride;
            vec2  lower_bound = step(vec2(0.0), vec2(neighbor_pos));
            vec2  upper_bound = step(vec2(neighbor_pos), vec2(TextureSize - 1));
            float is_inside = lower_bound.x * lower_bound.y * upper_bound.x * upper_bound.y;

            vec2 outside_seed_pos = mix(vec2(1e6), vec2(neighbor_pos), Direction);
            vec2 seed_pos = mix(outside_seed_pos, texelFetch(Texture, neighbor_pos, 0).xy, is_inside);

            float dist = length(seed_pos - pixel_pos);
            float s = step(dist, min_dist);

            min_dist = mix(min_dist, dist, s);
            best_pos = mix(best_pos, seed_pos, s);
        }

    FragColor = best_pos;
}
