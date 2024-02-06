#version 450

// in
layout(location = 0) in vec3 fs_pos;

// out
layout(location = 0) out vec4 final_color;

// uniforms
layout(binding = 0) uniform samplerCube skybox_sampler;


void main()
{
    final_color = texture(skybox_sampler, fs_pos);;
}
