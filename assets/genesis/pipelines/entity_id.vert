#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(push_constant) uniform u_PushConstants {
    mat4 mvp;
    int entityId;
} pc;

layout(location = 0) out int v_EntityId;

void main()
{
    v_EntityId = pc.entityId;
    gl_Position = pc.mvp * vec4(a_Position, 1.0);
}
