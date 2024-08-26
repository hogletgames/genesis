#version 450

layout(location = 0) in flat int v_EntityId;

layout(location = 0) out int outEntityId;

void main()
{
    outEntityId = v_EntityId;
}
