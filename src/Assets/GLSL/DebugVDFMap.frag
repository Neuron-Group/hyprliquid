#version 300 es
precision highp float;

uniform sampler2D Texture;
uniform vec2      TopLeft;
uniform vec2      Fullsize;
uniform vec2      TextureScale;

in  vec2 v_texcoord;
out vec4 FragColor;

void main()
{
    vec2 uv = (v_texcoord - TopLeft / Fullsize) * TextureScale;
    float dist = length(texture(Texture, uv).xy);
    FragColor = mix(vec4(vec3(0.0), 1.0), vec4(1.0), step(1.0, dist));
}