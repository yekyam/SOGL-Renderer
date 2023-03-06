#pragma once

#include <list>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Shader.hpp"

/**
 * @brief Keep track of models and render them
 *
 */
class Renderer
{
    public:
	std::list<std::unique_ptr<Model>> models;
	Shader shader;
	glm::mat4 projection;
	int mode;
	float distance;

	/* Idea: Some sort of registry system that stores the indices of the mesh's
	 * vertex Example: Mesh1 is added Mesh1 has vertices 0, 1, 2 In the registry,
	 * it's stored [Mesh1, 0, 2] Mesh2 is added Mesh2 has vertices 3, 4, 5 In the
	 * registry, it's stored [Mesh2, 3, 5] I choose to delete mesh1 In the
	 * registry, look for mesh1 and delete all vertices in the range
	 */

	Renderer(const std::string &vertexPath,
		 const std::string &fragmentPath,
		 int screenWidth,
		 int screenHeight,
		 int mode,
		 float distance)
	    : shader(vertexPath, fragmentPath), projection(1.0f), mode(mode), distance(distance)
	{
		projection = glm::perspective(45.0f, (GLfloat)screenWidth / screenHeight, 0.1f, 500.0f);
	}

	/**
	 * @brief Adds a model to the current list of models
	 *
	 * @param m The model to add
	 */
	void add_model(Model m) { models.push_back(std::make_unique<Model>(std::move(m))); }

	/**
	 * @brief Draw the models to the screen
	 *
	 */
	void draw_models()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);

		model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, -1.5f, -distance));

		GLint modelLoc = glGetUniformLocation(shader.program, "model");
		GLint viewLoc = glGetUniformLocation(shader.program, "view");
		GLint projectionLoc = glGetUniformLocation(shader.program, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		for (const auto &model : models)
		{
			glBindVertexArray(model->m_vao);
			glDrawElements(mode, model->m_mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
		}
	}
};
