#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(set = 1, binding = 0) uniform u_ViewProjection {
    mat4 view;
    mat4 projection;
} vp;

layout(set = 0, binding = 1) uniform u_Translation {
    mat4 matrix;
} model;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = vp.projection * vp.view * model.matrix * vec4(a_Position, 1.0);
}
