#pragma once

// glm
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

// project
#include <opengl.hpp>

// project - infd
#include <infd/GLMesh.hpp>
#include <infd/GLObject.hpp>

namespace infd {

	struct BasicModel {
		infd::GLProgram shader;
		infd::GLMesh mesh;
		glm::vec3 color{0.7};
		glm::mat4 model_transform{1.0};
		GLuint texture = 0;

		explicit BasicModel(infd::GLMesh _mesh) : mesh{_mesh} {}

		void draw(const glm::mat4 &view, const glm::mat4 &proj) const;
	};

} // namespace infd
