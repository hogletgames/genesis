#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(push_constant) uniform u_PushConstants {
    mat4 mvp;
} pc;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = pc.mvp * vec4(a_Position, 1.0);
}
