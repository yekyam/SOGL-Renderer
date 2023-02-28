#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/Mesh.hpp"
#include "src/Model.hpp"
#include "src/Renderer.hpp"
#include "src/load_obj.hpp"

const GLint WIDTH = 800, HEIGHT = 600;

bool is_float(const std::string &str)
{
	std::istringstream iss(str);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
	// Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

int main(int argc, char **argv)
{
	int mode = 0;
	bool print_fps = false;
	if (argc < 4)
	{
		std::cout
		    << "usage: ./main (obj_file) (GL_POINTS or GL_TRIANGLES or GL_LINES) (distance) (optional: fps)\n";
		std::cout << "example: ./main teapot.obj GL_LINES 10\n";

		return 0;
	}
	std::string_view chosen_render_mode(argv[2]);

	if (chosen_render_mode == "GL_POINTS")
	{
		mode = GL_POINTS;
	} else if (chosen_render_mode == "GL_TRIANGLES")
	{
		mode = GL_TRIANGLES;
	} else if (chosen_render_mode == "GL_LINES")
	{
		mode = GL_LINES;
	} else
	{
		std::cout << "Invalid render mode, must be: GL_POINTS, GL_TRIANGLES or GL_LINES\n";
		std::cout << "You chose: " << chosen_render_mode;
		return 0;
	}

	if (!is_float(argv[3]))
	{
		std::cout << "Invalid distance, must be a number\n";
		std::cout << "You chose: " << argv[3];
	}

	if (argc == 5)
		print_fps = true;

	float distance = std::stof(argv[3]);

	std::ios_base::sync_with_stdio(false);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OGL", nullptr, nullptr);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (!window)
	{
		std::cerr << "Couldn't make window:(\n";
		return -1;
	}

	glfwMakeContextCurrent(window);
	// glfwSwapInterval(0); // vsync off - Huge difference!

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cerr << "Couldn't init glew:(\n";
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);

	Renderer r("shaders/shader.vert", "shaders/shader.frag", screenWidth, screenHeight, mode, distance);

	std::optional<Model> model = load_obj(argv[1]);

	if (!model.has_value())
	{
		std::cout << "Couldn't load file: " << argv[1] << '\n';
		return 0;
	}

	r.add_model(std::move(model.value()));

	double previous_time = glfwGetTime();
	int ticks = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		r.draw_models();

		glfwSwapBuffers(window);

		if (print_fps)
		{
			double current_time = glfwGetTime();
			double delta = current_time - previous_time;
			std::cout << "Fps: " << 1 / delta << '\n';
			std::cout << "Time: " << delta << '\n';
			std::cout << "Ticks: " << ticks << '\n';
			std::cout << '\n';
			previous_time = current_time;
			ticks += 1;
		}
	}
	glfwTerminate();
}
