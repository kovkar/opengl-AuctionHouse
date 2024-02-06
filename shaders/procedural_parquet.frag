#version 450

// attenuation coeficients
const float CONSTANT = 0.5;
const float LINEAR = -0.1;
const float QUADRATIC = 0.04;

// in
layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec2 fs_uv;

// uniforms
layout(location = 4) uniform vec3 light_position;

layout(binding = 1, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;

layout(binding = 0) uniform sampler2D texture_sampler;


// out
layout(location = 0) out vec4 final_color;

// variables
vec4 BROWN = vec4(0.6, 0.3, 0.0, 1.0);
vec4 LIGHT_BROWN = vec4(1.0, 0.6, 0.2, 1.0);

float TILE_WIDTH = 1.0 / 20;
float TILE_HEIGHT = 1.0 / 10;
float GAP = TILE_WIDTH * 0.01;
float Y_OFFSET = TILE_HEIGHT / 3;


void main()
{
    float is_in_x_gap = 1 - step(GAP, mod(fs_uv.x, TILE_WIDTH));
    float is_in_y_gap = 1 - step(GAP, mod(fs_uv.y + (Y_OFFSET * floor(fs_uv.x / TILE_WIDTH)), TILE_HEIGHT));
    float is_in_gap = clamp(is_in_x_gap + is_in_y_gap, 0.0, 1.0);
    
    vec4 texture_color = LIGHT_BROWN * is_in_gap + BROWN * (1 - is_in_gap);

    /* LIGHTING */

    vec3 N = normalize(fs_normal);                          // normal
    vec3 Lm = normalize(light_position - fs_position);      // frag to main light direcion
    vec3 V = normalize(camera.position - fs_position);      // view direction
    vec3 R = reflect(-Lm, N);                               // reflection direction
    float D = distance(light_position, fs_position);        // main light distance

    // main light
    float ambient = 0.1;
    float diffuse = max(dot(N, Lm), 0.0);
    float specular = pow(max(dot(V, R), 0.0), 8);
    float attenuation = 1.0 / (CONSTANT + LINEAR * D + QUADRATIC * pow(D, 2));
    float main_light = (ambient + diffuse + specular * 0.7) * attenuation;

    /* FINAL COLOR */

    vec3 color = texture_color.rgb * (main_light);
    final_color = vec4(color, texture_color.a); // original alpha, not affected by lighting
}