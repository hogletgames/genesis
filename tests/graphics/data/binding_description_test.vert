#version 450

layout(set = 0, binding = 0) uniform u_MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(set = 0, binding = 1) uniform sampler2D u_Texture[12];

layout (set = 1, binding = 0) uniform u_Elements
{
    vec4 element[32];
};

void main()
{}
