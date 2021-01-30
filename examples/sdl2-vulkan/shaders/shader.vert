#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform u_MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = mvp.proj * mvp.view *mvp.model * vec4(a_Position, 1.0);
}
