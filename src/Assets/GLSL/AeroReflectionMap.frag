#version 300 es
precision highp float;

in  vec2 v_texcoord;
out vec4 FragColor;

float DrawBand(float k, float x0, float x1, float i0, float i1)
{
    float t = clamp((v_texcoord.x * k - v_texcoord.y - x0 * k) / ((x1 - x0) * k), 0.0, 1.0);
    float clip = step(v_texcoord.y + x0 * k, v_texcoord.x * k) * step(v_texcoord.x * k, v_texcoord.y + x1 * k);
    return mix(i0, i1, t) * clip;
}

void main()
{
    float intensity = 0.0;
    const float k = 2.40840841;

    intensity += DrawBand(k, -0.41521197, -0.37406484, 0.07201, 0.003245);
    intensity += DrawBand(k, -0.37406484, -0.35411471, 0.003245, 0.067443);
    intensity += DrawBand(k, -0.35411471, -0.33042394, 0.067314, 0.064738);
    intensity += DrawBand(k, -0.33042394, -0.2755611, 0.064738, 0.0);
    intensity += DrawBand(k, -0.24064838, -0.19825436, 0.000714, 0.002052);
    intensity += DrawBand(k, -0.19825436, -0.17705736, 0.002052, 0.017574);
    intensity += DrawBand(k, -0.17705736, -0.16209476, 0.017574, 0.003496);
    intensity += DrawBand(k, -0.16209476, -0.12718204, 0.003496, 0.028607);
    intensity += DrawBand(k, -0.12718204, -0.07107232, 0.028607, 0.001039);
    intensity += DrawBand(k, -0.07107232, 0.0, 0.001039, 0.085936);
    intensity += DrawBand(k, 0.0, 0.01122195, 0.085846, 0.085846);
    intensity += DrawBand(k, 0.01122195, 0.08229426, 0.085846, 0.0);
    intensity += DrawBand(k, 0.0872818, 0.12718204, 0.0, 0.085936);
    intensity += DrawBand(k, 0.12718204, 0.14837905, 0.085936, 0.0);
    intensity += DrawBand(k, 0.14837905, 0.1521197, 0.0, 0.003915);
    intensity += DrawBand(k, 0.1521197, 0.159601, 0.003915, 0.003922);
    intensity += DrawBand(k, 0.159601, 0.17705736, 0.003922, 0.019594);
    intensity += DrawBand(k, 0.17705736, 0.18952618, 0.019594, 0.0);
    intensity += DrawBand(k, 0.22693267, 0.24314214, 0.0, 0.078272);
    intensity += DrawBand(k, 0.24314214, 0.25685786, 0.078272, 0.078272);
    intensity += DrawBand(k, 0.25685786, 0.27306733, 0.078431, 0.07035);
    intensity += DrawBand(k, 0.27306733, 0.28428928, 0.07035, 0.070064);
    intensity += DrawBand(k, 0.28428928, 0.30299252, 0.070064, 0.0);
    intensity += DrawBand(k, 0.41895262, 0.49002494, 0.000013, 0.085852);
    intensity += DrawBand(k, 0.49002494, 0.51620948, 0.085852, 0.085852);
    intensity += DrawBand(k, 0.51620948, 0.57107232, 0.085936, 0.001797);
    intensity += DrawBand(k, 0.57107232, 0.58852868, 0.001797, 0.07372);
    intensity += DrawBand(k, 0.58852868, 0.62593516, 0.07372, 0.003245);
    intensity += DrawBand(k, 0.62593516, 0.64588529, 0.003245, 0.067443);
    intensity += DrawBand(k, 0.64588529, 0.66957606, 0.067314, 0.064738);
    intensity += DrawBand(k, 0.66957606, 0.7244389, 0.064738, 0.0);
    intensity += DrawBand(k, 0.75935162, 0.80174564, 0.000714, 0.002052);
    intensity += DrawBand(k, 0.80174564, 0.82294264, 0.002052, 0.017574);
    intensity += DrawBand(k, 0.82294264, 0.83790524, 0.017574, 0.003496);
    intensity += DrawBand(k, 0.83790524, 0.87281796, 0.003496, 0.028607);
    intensity += DrawBand(k, 0.87281796, 0.92892768, 0.028607, 0.001039);
    intensity += DrawBand(k, 0.92892768, 0.99875312, 0.001039, 0.085794);

    FragColor = vec4(vec3(1.0), intensity * 2.0);
}
