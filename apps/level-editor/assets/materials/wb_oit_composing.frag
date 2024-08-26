#version 450

layout(set = 0, binding = 0) uniform sampler2D u_ColorTex;  // Opaque color texture
layout(set = 1, binding = 0) uniform sampler2D u_AccumTex;  // Accumulation texture
layout(set = 2, binding = 0) uniform sampler2D u_RevealTex; // Reveal texture

layout(location = 0) out vec4 outColor;

// epsilon number
const float EPSILON = 0.00001f;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    // fragment coordination
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // base color
    outColor = texelFetch(u_ColorTex, coords, 0);

    // fragment revealage
    float revealage = texelFetch(u_RevealTex, coords, 0).r;

    // save the blending and color texture fetch cost if there is not a transparent fragment
    if (isApproximatelyEqual(revealage, 1.0f)) {
        return;
    }

    // fragment color
    vec4 accumulation = texelFetch(u_AccumTex, coords, 0);

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    // prevent floating point precision bug
    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    // final transparent color
    vec4 final_color = vec4(average_color, 1.0f - revealage);

    // blend with the underlying color buffer
    outColor = mix(outColor, final_color, final_color.a);
}
