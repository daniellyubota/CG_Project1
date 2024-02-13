#include <glad/glad.h>
#include <sdl2/include/SDL.h>
#undef main

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream> 
#include <vector>

#include "shader_configure.h"
#include "load_model_meshes.h"

SDL_Window* window;

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	float screenWidth = 800;
	float screenHeight = 600;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("3D FPS OPENGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "Failed to create SDL Window" << std::endl;
		SDL_Quit();
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		SDL_Quit();
		return -2;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	const char* vert_shader = "Shaders/shader_glsl.vert";
	const char* frag_shader = "Shaders/shader_glsl.frag";

	Shader main_shader(vert_shader, frag_shader);
	main_shader.use();

	unsigned int view_matrix_loc = glGetUniformLocation(main_shader.ID, "view");
	unsigned int projection_matrix_loc = glGetUniformLocation(main_shader.ID, "projection");
	unsigned int camera_position_loc = glGetUniformLocation(main_shader.ID, "camera_position");

	glm::vec3 camera_position(0.0f, 0.0f, 3.0f);
	glm::vec3 camera_target(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

	glUniform3f(camera_position_loc, camera_position.x, camera_position.y, camera_position.z);

	glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));

	Model model{"InteriorTest.obj"};

	glActiveTexture(GL_TEXTURE0);
	unsigned int image_sampler_loc = glGetUniformLocation(main_shader.ID, "image");
	glUniform1i(image_sampler_loc, 0);

	glBindVertexArray(0);

	SDL_Event windowEvent;
	while (true)
	{

		glClearColor(0.30f, 0.55f, 0.65f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < model.num_meshes; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, model.mesh_list[i].tex_handle);	

			glBindVertexArray(model.mesh_list[i].VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)model.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(main_shader.ID);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}