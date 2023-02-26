#pragma once

#include <optional>
#include <string_view>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Model.hpp"
#include "Mesh.hpp"

std::optional<Model> load_obj(std::string_view filename, std::array<GLfloat, 3> color = {1.0f, 1.0f, 1.0f})
{
	std::optional<Model> model;
	std::fstream file(filename);
	if (!file)
	{
		return model;
	}

	std::string line;
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;

	std::array<std::string, 3> pos_array_str;
	std::array<GLfloat, 3> pos_array_float;

	while (std::getline(file, line))
	{
		std::stringstream ss(line);

		std::string line_type;
		ss >> line_type;

		if (line_type == "v")
		{

			for (int i = 0; i < 3; i++)
			{
				ss >> pos_array_str[i];
			}
			for (int i = 0; i < 3; i++)
			{
				pos_array_float[i] = std::stof(pos_array_str[i]);
			}
			vertices.push_back(Vertex(pos_array_float, color));
		}

		else if (line_type == "f")
		{
			std::string index;
			for (int i = 0; i < 3; i++)
			{
				ss >> index;
				if (index.find('/') != index.npos) // unhappy path - f looks like "f 1/11/11 2/22/22 3/33/33 "
				{
					index = index.substr(0, index.find('/'));
				}
				indices.push_back(std::stoi(index) - 1); // faces start at 1 :O
			}
		}
	}

	model = std::move(Mesh(vertices, indices));
	return model;
};
