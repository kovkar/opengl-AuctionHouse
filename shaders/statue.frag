#version 450

// in
layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec2 fs_uv;

layout(binding = 1, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;


// out
layout(location = 0) out vec4 final_color;

// uniforms
layout(binding = 0) uniform samplerCube skybox_sampler;


void main()
{
    vec3 I = normalize(fs_position - camera.position);
    vec3 R = reflect(I, normalize(fs_normal));
    final_color = texture(skybox_sampler, R);
}
