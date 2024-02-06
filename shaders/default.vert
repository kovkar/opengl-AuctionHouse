#version 450

// in
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;


layout(binding = 1, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;

layout(binding = 2, std140) uniform ModelMatrixUBO {
	mat4 matrix;
	float shinines;
} model;

// out
layout(location = 0) out vec3 fs_position;
layout(location = 1) out vec3 fs_normal;
layout(location = 2) out vec2 fs_uv;

void main()
{
	fs_position = (model.matrix * vec4(position, 1.0f)).xyz;
	fs_uv = uv;
	fs_normal = normal;

    gl_Position = camera.projection * camera.view * model.matrix * vec4(position, 1.0);
}