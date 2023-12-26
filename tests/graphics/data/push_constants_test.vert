#version 450

layout(push_constant) uniform u_PC {
    bool bool_type;
    int int_type;
    uint uint_type;
    float float_type;
    double double_type;
    vec3 vec3_type;
    mat4 mat4_type;
} pc;

void main()
{}
