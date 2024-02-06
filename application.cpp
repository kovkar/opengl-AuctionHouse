#include "application.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"


// models
std::vector<Vertex> walls_model, chair_model, windows_model, balcony_model, podium_model, statue_model,
                    stand_model, floor_model, spot_model, train_model, pillar_model;

// main pointlight properties
static glm::vec4 light_position = glm::vec4(0.2f, 4.5f, 0.7f, 1.0f);

// spotlight properties
glm::vec3 spotlight_position = glm::vec3(3.5f, 6.0f, -1.15f);
glm::vec3 spotlight_direction = glm::vec3(0.0f, -1.0f, 0.0f);

/* ========== METHODS ========== */

void init() 
{
	// programs
	floor_program   = createProgram("shaders/default.vert", "shaders/procedural_parquet.frag");
	texture_program = createProgram("shaders/default.vert", "shaders/texture.frag");
	skybox_program  = createProgram("shaders/skybox.vert" , "shaders/skybox.frag");
	statue_program  = createProgram("shaders/default.vert", "shaders/statue.frag");

	// textures
	walls_texture 	   = createTexture("images/walls.png");
	stand_texture 	   = createTexture("images/stand.png");
	light_wood_texture = createTexture("images/chair.png");
	dark_wood_texture  = createTexture("images/podium.png");
	balcony_texture	   = createTexture("images/balcony.png");
	gold_texture  	   = createTexture("images/gold.png");

	// models
	walls_model   = loadOBJFile("obj/walls.obj");
	chair_model   = loadOBJFile("obj/chair.obj");
	windows_model = loadOBJFile("obj/windows.obj");
	balcony_model = loadOBJFile("obj/balcony.obj");
	podium_model  = loadOBJFile("obj/podium.obj");
	statue_model  = loadOBJFile("obj/statue.obj");
	stand_model   = loadOBJFile("obj/stand.obj");
	floor_model   = loadOBJFile("obj/floor.obj");
	train_model	  = loadOBJFile("obj/train.obj");
	pillar_model  = loadOBJFile("obj/pillar.obj");

	// VBOs
	walls_vbo   = createObjectVBO(walls_model);
	chair_vbo   = createObjectVBO(chair_model);
	stand_vbo   = createObjectVBO(stand_model);
	windows_vbo = createObjectVBO(windows_model);
	balcony_vbo = createObjectVBO(balcony_model);
	podium_vbo  = createObjectVBO(podium_model);
	statue_vbo  = createObjectVBO(statue_model);
	floor_vbo   = createObjectVBO(floor_model);
	train_vbo   = createObjectVBO(train_model);
	pillar_vbo  = createObjectVBO(pillar_model);

	// VAOs
	walls_vao   = createObjectVAO(walls_vbo);
	chair_vao   = createObjectVAO(chair_vbo);
	stand_vao   = createObjectVAO(stand_vbo);
	windows_vao = createObjectVAO(windows_vbo);
	balcony_vao = createObjectVAO(balcony_vbo);
	podium_vao  = createObjectVAO(podium_vbo);
	statue_vao  = createObjectVAO(statue_vbo);
	floor_vao   = createObjectVAO(floor_vbo);
	train_vao   = createObjectVAO(train_vbo);
	pillar_vao  = createObjectVAO(pillar_vbo);

	/* ==================== UNIFORMS ==================== */

	// point light
	glProgramUniform3f(texture_program, 4, light_position.x, light_position.y, light_position.z); // pos
	glProgramUniform3f(floor_program, 4, light_position.x, light_position.y, light_position.z); // pos
	// spot light
	glProgramUniform3f(texture_program, 5, spotlight_position.x, spotlight_position.y, spotlight_position.z); // pos
	glProgramUniform3f(texture_program, 6, spotlight_direction.x, spotlight_direction.y, spotlight_direction.z); // dir

	/* ====================  BUFFERS ==================== */

	glCreateBuffers(1, &camera_buffer);
    glNamedBufferStorage(camera_buffer, sizeof(CameraUBO), &camera_ubo, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &model_buffer);
    glNamedBufferStorage(model_buffer, sizeof(ModelUBO), &default_model_ubo, GL_DYNAMIC_STORAGE_BIT);

	/* ===================== SKYBOX =================== */

	// VBO
	glCreateBuffers(1, &skybox_vbo);
	glNamedBufferStorage(skybox_vbo, 8 * 3 * sizeof(float), skybox_data, 0);

	// cubemap texture
	int width, height, channels;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &skybox_texture);
	glTextureStorage2D(skybox_texture, std::log2(1024)+1, GL_RGBA8, 1024, 1024);

	// load textures
	stbi_set_flip_vertically_on_load(false);
	for (int i = 0; i < 6; i++) {
		unsigned char* sky_plane = stbi_load( sky_tex_strings[i], &width, &height, &channels, 4);
		glTextureSubImage3D(skybox_texture, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, sky_plane);
	}

	// skybox vao
	glCreateVertexArrays(1, &skybox_vao);
	glVertexArrayVertexBuffer(skybox_vao, 0, skybox_vbo, 0, 3 * sizeof(float));
    glEnableVertexArrayAttrib(skybox_vao, 0);
    glVertexArrayAttribFormat(skybox_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(skybox_vao, 0, 0);

	// settings
	glGenerateTextureMipmap(skybox_texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	
}

void draw() 
{
    /* ==================== UPDATE ==================== */

	// moving camera
    camera_ubo.proj_mat = glm::perspective(FOV, float(WIDTH) / float(HEIGHT), NEAR, FAR);
    camera_ubo.view_mat = glm::lookAt(camera.eye_pos, camera.eye_pos + camera.view_dir, camera.up_dir);
	camera_ubo.position = camera.eye_pos;
    glNamedBufferSubData(camera_buffer, 0, sizeof(CameraUBO), &camera_ubo);

	// rotating train
	train_rotation_angle += TRAIN_ROTATION_SPEED;
	train_model_ubo.shininess = 1.0f; 
	train_model_ubo.model_matrix = glm::translate(glm::mat4(1.0f), train_position)
								 * glm::rotate(glm::mat4(1.0f), train_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

    /* ================================================== */
	
	glClear(GL_COLOR_BUFFER_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, camera_buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, model_buffer);
	
	/* ==================== DRAW MODELS ==================== */

	// floor, procedural texture
	glUseProgram(floor_program);
    glBindVertexArray(floor_vao);
    glDrawArrays(GL_TRIANGLES, 0, floor_model.size());

	// chairs
	ModelUBO chair_ubo = { glm::mat4(1.0f), 0.5f };
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			chair_ubo.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( i * 2.0f, 0.0f, - j * 1.5f));
			drawModel(chair_model, chair_vao, texture_program, light_wood_texture, chair_ubo);
		}
	} 

	drawModel(podium_model , podium_vao , texture_program, dark_wood_texture , default_model_ubo);
	drawModel(stand_model  , stand_vao	 , texture_program, stand_texture	  , default_model_ubo);
	drawModel(train_model  , train_vao	 , texture_program, gold_texture, train_model_ubo);
	drawModel(balcony_model, balcony_vao, texture_program, balcony_texture	  , default_model_ubo);
	drawModel(pillar_model , pillar_vao , texture_program, balcony_texture	  , default_model_ubo);
	drawModel(walls_model  , walls_vao	 , texture_program, walls_texture	  , walls_model_ubo);
	drawModel(statue_model , statue_vao , statue_program , skybox_texture	  , default_model_ubo);

	// skybox
	glDepthFunc(GL_LEQUAL); // overwrite if depth = 1 -> empty pixel
	glUseProgram(skybox_program);
    glBindVertexArray(skybox_vao);
	glBindTextureUnit(0, skybox_texture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, skybox_indeces);
	glDepthFunc(GL_LESS); 	// set depth function back

	// walls and windows rendered last -> blending
	drawModel(walls_model, walls_vao, texture_program, walls_texture, walls_model_ubo);
	drawModel(windows_model, windows_vao, texture_program, walls_texture, default_model_ubo);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    glm::vec3 side_dir = glm::normalize(glm::cross(camera.up_dir, camera.view_dir));
	glm::vec3 forward_dir = glm::normalize(glm::vec3(camera.view_dir.x, 0, camera.view_dir.z));

    if (key == GLFW_KEY_W) {
        camera.eye_pos += forward_dir * MOVEMENT_SPEED;
    }
    if (key == GLFW_KEY_S) {
        camera.eye_pos -= forward_dir * MOVEMENT_SPEED;
    }
    if (key == GLFW_KEY_A) {
        camera.eye_pos += side_dir * MOVEMENT_SPEED;
    }
    if (key == GLFW_KEY_D) {
        camera.eye_pos -= side_dir * MOVEMENT_SPEED;
    }
    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        glfwGetCursorPos(window, &last_cursor_x, &last_cursor_y);

        if (action == GLFW_PRESS) { CAMERA_ROTATION_ENABLED = true; }
        if (action == GLFW_RELEASE) { CAMERA_ROTATION_ENABLED = false; }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!CAMERA_ROTATION_ENABLED) { return; }

    double dx = last_cursor_x - xpos;
    double dy = -1 * (last_cursor_y - ypos);

    last_cursor_x = xpos;
    last_cursor_y = ypos;

    glm::vec3 side_dir = glm::normalize(glm::cross(camera.up_dir, camera.view_dir));
    glm::mat4 horizontal_rotation = glm::rotate(glm::mat4(1.0f), float(dy * ROTATION_SPEED), side_dir);
    glm::mat4 vertical_rotation = glm::rotate(glm::mat4(1.0f), float(dx * ROTATION_SPEED), glm::vec3(0.0f, 1.0f, 0.0f));

    camera.view_dir = glm::mat3(vertical_rotation * horizontal_rotation) * camera.view_dir;
    camera.up_dir = glm::mat3(vertical_rotation * horizontal_rotation) * camera.up_dir;
}

void drawModel(std::vector<Vertex> model, GLuint vao, GLuint program, GLuint texture, ModelUBO ubo) 
{
	glUseProgram(program);
    glBindVertexArray(vao);
	glBindTextureUnit(0, texture);
	glNamedBufferSubData(model_buffer, 0, sizeof(ModelUBO), &ubo);
    glDrawArrays(GL_TRIANGLES, 0, model.size());
}

std::string getFileContent(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
    std::cout << "shader code read error" << std::endl;
	throw(errno);
}

GLuint createShader(const char* filename, GLenum type) 
{
	std::string shader_source = getFileContent(filename);
	const char* shader_string = shader_source.c_str();
	GLuint ID = glCreateShader(type);
	glShaderSource(ID, 1, &shader_string, NULL);
	return ID;
	
}

GLuint createProgram(const char* vert_name, const char* frag_name) 
{
    
	GLuint vertex_ID = createShader(vert_name, GL_VERTEX_SHADER);
	GLuint fragment_ID = createShader(frag_name, GL_FRAGMENT_SHADER);

	GLuint program_ID = glCreateProgram();

    
    glCompileShader(vertex_ID);
	glCompileShader(fragment_ID);

	glAttachShader(program_ID, vertex_ID);
	glAttachShader(program_ID, fragment_ID);

	glLinkProgram(program_ID); 

	glDeleteShader(vertex_ID);
	glDeleteShader(fragment_ID);

	return program_ID;
}

GLuint createObjectVBO(std::vector<Vertex> model)
{
	// data pointer
	Vertex* model_data = model.data();

	// create buffer in GPU
	GLuint vbo;	
	glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, model.size() * sizeof(Vertex), model_data, 0);

	return vbo;
}

GLuint createObjectVAO(GLuint data_vbo)
{
	GLuint vao;
	glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, data_vbo, 0, sizeof(Vertex));
    glVertexArrayVertexBuffer(vao, 1, data_vbo, 0, sizeof(Vertex));
    glVertexArrayVertexBuffer(vao, 2, data_vbo, 0, sizeof(Vertex));
    glVertexArrayVertexBuffer(vao, 3, data_vbo, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(vao, 0); // position
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    
    glEnableVertexArrayAttrib(vao, 1); // normal
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    glVertexArrayAttribBinding(vao, 1, 2);

    glEnableVertexArrayAttrib(vao, 2); // uv
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
    glVertexArrayAttribBinding(vao, 2, 3);

	return vao;
}

GLuint createTexture(const char* file_name) 
{
	GLuint texture;
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texture_data = stbi_load(file_name, &width, &height, &channels, 4);

	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, std::log2(width), GL_RGBA8, width, height);

	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	glGenerateTextureMipmap(texture);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(texture_data);

	return texture;
}

std::vector<Vertex> loadOBJFile(const char* file_name)
{
	//Vertex portions
	std::vector<glm::fvec3> vertex_positions;
	std::vector<glm::fvec2> vertex_texcoords;
	std::vector<glm::fvec3> vertex_normals;

	//Face vectors
	std::vector<GLint> vertex_position_indicies;
	std::vector<GLint> vertex_texcoord_indicies;
	std::vector<GLint> vertex_normal_indicies;

	//Vertex array
	std::vector<Vertex> vertices;

	std::stringstream ss;
	std::ifstream in_file(file_name);
	std::string line = "";
	std::string prefix = "";
	glm::vec3 temp_vec3;
	glm::vec2 temp_vec2;
	GLint temp_glint = 0;

	//File open error check
	if (!in_file.is_open())
	{
		throw "ERROR::OBJLOADER::Could not open file.";
	}

	//Read one line at a time
	while (std::getline(in_file, line))
	{
		//Get the prefix of the line
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "#")
		{

		}
		else if (prefix == "o")
		{

		}
		else if (prefix == "s")
		{

		}
		else if (prefix == "use_mtl")
		{

		}
		else if (prefix == "v") //Vertex position
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_positions.push_back(temp_vec3);
		}
		else if (prefix == "vt")
		{
			ss >> temp_vec2.x >> temp_vec2.y;
			vertex_texcoords.push_back(temp_vec2);
		}
		else if (prefix == "vn")
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_normals.push_back(temp_vec3);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			while (ss >> temp_glint)
			{
				//Pushing indices into correct arrays
				if (counter == 0)
					vertex_position_indicies.push_back(temp_glint);
				else if (counter == 1)
					vertex_texcoord_indicies.push_back(temp_glint);
				else if (counter == 2)
					vertex_normal_indicies.push_back(temp_glint);

				//Handling characters
				if (ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ')
				{
					++counter;
					ss.ignore(1, ' ');
				}

				//Reset the counter
				if (counter > 2)
					counter = 0;
			}
		}
		else
		{

		}
	}

	//Build final vertex array (mesh)
	vertices.resize(vertex_position_indicies.size(), Vertex());

	//Load in all indices
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position = vertex_positions[vertex_position_indicies[i] - 1];
		vertices[i].uv = vertex_texcoords[vertex_texcoord_indicies[i] - 1];
		vertices[i].normal = vertex_normals[vertex_normal_indicies[i] - 1];
	}

	//DEBUG
	std::cout << "Nr of vertices: " << vertices.size() << "\n";

	//Loaded success
	std::cout << "OBJ file loaded!" << "\n";
	return vertices;
}