#version 450

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(set = 1, binding = 0) uniform sampler2D u_Texture;

layout(location = 0) out vec4 Color;

void main()
{
    Color = texture(u_Texture, v_TexCoord);
}
