#version 450

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_Sprite;

layout(location = 1) out vec4  outAccumColor;
layout(location = 2) out float outReveal;

void main()
{
    vec4 color = texture(u_Sprite, v_TexCoord) * vec4(v_Color, 1.0);
    float z = gl_FragCoord.z;

    // Calculate the weight
    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) *
                   clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);

    // Output the premultiplied color and the weight
    outAccumColor = vec4(color.rgb * color.a, color.a) * weight;
    outReveal = color.a;
}
