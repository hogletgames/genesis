#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(set = 0, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} u_MVP;

layout(location = 0) out vec3 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_MVP.projection * u_MVP.view * u_MVP.model * vec4(a_Position, 1.0);
}
