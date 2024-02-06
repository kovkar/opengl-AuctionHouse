#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "include/stb_image.h"

/* ==================== SETTINGS ==================== */

// window
const int   WIDTH = 1024;
const int   HEIGHT = 720;
const float FOV = 45.0f; // in degrees 
const float NEAR = 1.0f;
const float FAR = 1000.0f;
// camera
const float MOVEMENT_SPEED = 0.1f;
const float ROTATION_SPEED = 0.02f;
// models
const float TRAIN_ROTATION_SPEED= 0.01f;

const glm::vec3 train_position = glm::vec3(3.49634f, 1.92977f, -1.15591f);

/* ==================== STRUCTURES ==================== */

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Camera {
    glm::vec3 eye_pos;
    glm::vec3 view_dir;
    glm::vec3 up_dir;
};

struct CameraUBO {
    glm::mat4 proj_mat;
    glm::mat4 view_mat;
    glm::vec3 position;
};

struct LightUBO {
    glm::vec3 color;
    glm::vec3 pos;
};

struct ModelUBO {
    glm::mat4 model_matrix;
    float shininess; // specular light multiplier
};

/* ==================== VARIABLES ==================== */

static Camera camera = {
	glm::vec3(0.0f, 3.0f, 0.0f), 	// position
	glm::vec3(0.0f, 0.0f, -1.0f), 	// view direction
	glm::vec3(0.0f, 1.0f, 0.0f)		// up direction
}; 

// for calculating camera rotation
static double last_cursor_x = 0.0;
static double last_cursor_y = 0.0;

// for building train model matrix
static float  train_rotation_angle = 0.0f;

// camera rotation only when LMB pressed, true if LMB down
static bool CAMERA_ROTATION_ENABLED = false;

// programs
static GLuint default_program, floor_program, texture_program, skybox_program, statue_program;

// textures
static GLuint walls_texture, stand_texture, light_wood_texture, dark_wood_texture, balcony_texture, spot_texture, skybox_texture, gold_texture;

// VBOs
static GLuint walls_vbo, chair_vbo, stand_vbo, windows_vbo, balcony_vbo, podium_vbo, statue_vbo, floor_vbo, train_vbo, pillar_vbo, skybox_vbo;

// VAOs
static GLuint walls_vao, chair_vao, stand_vao, windows_vao, balcony_vao, podium_vao, statue_vao, floor_vao, train_vao, pillar_vao, skybox_vao;

// buffers
static GLuint camera_buffer, model_buffer;

// UBOs
static CameraUBO camera_ubo = {
	glm::perspective(FOV, float(WIDTH) / float(HEIGHT), NEAR, FAR),					// projection matrix
	glm::lookAt(camera.eye_pos, camera.eye_pos + camera.view_dir, camera.up_dir),	// view matrix
	camera.eye_pos																	// position
};

static ModelUBO default_model_ubo = { glm::mat4(1.0f), 1.0f };
static ModelUBO walls_model_ubo = { glm::mat4(1.0f), 0.0f };
static ModelUBO train_model_ubo = { glm::translate(glm::mat4(1.0f), train_position) , 1.0f };




/* ==================== DATA ==================== */

static const float skybox_data[24] = {
	-1.0f, -1.0f,  1.0f, //        7--------6
	 1.0f, -1.0f,  1.0f, //       /|       /|
	 1.0f, -1.0f, -1.0f, //      4--------5 |
	-1.0f, -1.0f, -1.0f, //      | |      | |
	-1.0f,  1.0f,  1.0f, //      | 3------|-2
	 1.0f,  1.0f,  1.0f, //      |/       |/
	 1.0f,  1.0f, -1.0f, //      0--------1
	-1.0f,  1.0f, -1.0f
};

static const unsigned int skybox_indeces[36] = {
	6,2,1,	1,5,6, // right
	7,4,0,	0,3,7, // left
	6,5,4,	4,7,6, // top
	2,3,0,	0,1,2, // bottom
	5,1,0,	0,4,5, // back
	6,7,3,	3,2,6  // front
};

static const char* sky_tex_strings[6] = { 
		"images/skybox/px.png", 
		"images/skybox/nx.png",
		"images/skybox/py.png",
		"images/skybox/ny.png",
		"images/skybox/pz.png",
		"images/skybox/nz.png",
};

/* ==================== METHODS ==================== */

void init();

void draw();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void drawModel(std::vector<Vertex> model, GLuint vao, GLuint program, GLuint texture, ModelUBO ubo);

std::string getFileContent(const char* filename);

GLuint createShader(const char* filename, GLenum type);

GLuint createProgram(const char* vert_name, const char* frag_name);

GLuint createObjectVBO(std::vector<Vertex> model);

GLuint createObjectVAO(GLuint data_vbo);

GLuint createTexture(const char* file_name);

std::vector<Vertex> loadOBJFile(const char* file_name);