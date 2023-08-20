#version 450

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_Sprite;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(u_Sprite, v_TexCoord) * vec4(v_Color, 1.0);

    if (outColor.w < 0.8) {
        discard;
    }
}
