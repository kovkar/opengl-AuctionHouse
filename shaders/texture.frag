#version 450

/* CONSTANTS */
const float SPOT_INNER_ANGLE = 0.94;
const float SPOT_OUTER_ANGLE = 0.96;
const float SPOT_INTENSITY = 0.8;
// attenuation coeficients
const float CONSTANT = 0.5;
const float LINEAR = -0.1;
const float QUADRATIC = 0.04;

/* IN */
layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec2 fs_uv;

/* OUT */
layout(location = 0) out vec4 final_color;

/* UNIFORMS */
layout(location = 4) uniform vec3 light_position;
layout(location = 5) uniform vec3 spotlight_position;
layout(location = 6) uniform vec3 spotlight_direction;

/* BUFFERS */
layout(binding = 0) uniform sampler2D texture_sampler;

layout(binding = 1, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;

layout(binding = 2, std140) uniform ModelUBO {
	mat4 matrix;
	float shinines;
} model;

void main()
{
    /* TEXTURE SAMPLING */

    vec4 texture_color = texture(texture_sampler, fs_uv);

    /* LIGHTING */

    vec3 N = normalize(fs_normal);                          // normal
    vec3 Lm = normalize(light_position - fs_position);      // frag to main light direcion
    vec3 Ls = normalize(spotlight_position - fs_position);  // frag to spot light direction
    vec3 V = normalize(camera.position - fs_position);      // view direction
    vec3 R = reflect(-Lm, N);                               // reflection direction
    float D = distance(light_position, fs_position);        // main light distance

    // main light
    float ambient = 0.1;
    float diffuse = max(dot(N, Lm), 0.0);
    float specular = pow(max(dot(V, R), 0.0), 8);
    float attenuation = 1.0 / (CONSTANT + LINEAR * D + QUADRATIC * pow(D, 2));
    float main_light = (ambient + diffuse + specular * model.shinines) * attenuation;
    
    // spotlight
    float spot_angle = dot(spotlight_direction, -Ls); 
    float spot_diffuse = max(dot(N, Ls), 0.0);
    float spot_light = spot_diffuse * SPOT_INTENSITY 
                     * clamp((spot_angle - SPOT_OUTER_ANGLE) / (SPOT_OUTER_ANGLE - SPOT_INNER_ANGLE), 0.0, 1.0);

    /* FINAL COLOR */

    vec3 color = texture_color.rgb * (main_light + spot_light); // lights sum
    final_color = vec4(color, texture_color.a); // original alpha, not affected by lighting
}