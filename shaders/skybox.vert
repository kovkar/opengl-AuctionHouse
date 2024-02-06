#version 450

// in
layout(binding = 1, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;

layout(location = 0) in vec3 position;

layout(location = 1) uniform mat4 model_matrix;

// out
layout(location = 0) out vec3 fs_pos;

void main()
{
	fs_pos = position;

	mat4 view_without_translation = mat4(mat3(camera.view));

	vec4 pos = camera.projection * view_without_translation * mat4(1.0f) * vec4(position, 1.0);
    gl_Position = pos.xyww;
}