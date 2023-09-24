#pragma once


// std
#include <cstddef>
#include <memory>
#include <vector>

// glm
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// project
#include <infd/GLObject.hpp>


namespace infd {

	/*
	 * POD vertex data
	 */
	struct MeshVertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	/*
	 * Contains mesh data handlers
	 */
	class GLMesh {
		friend class GLMeshBuilder;

		// vertex array object
		GLVertexArray _vao;
		// vertex buffer object
		GLBuffer _vbo;
		// index buffer object
		GLBuffer _ibo;
		// drawing mode
		GLenum _mode;
		// number of indices to be drawn
		std::size_t _index_count = 0;

		GLMesh(
			const GLVertexArray &vao, 
			const GLBuffer &vbo, 
			const GLBuffer &ibo, 
			GLenum mode, 
			std::size_t index_count
		) :
			_vao(vao), 
			_vbo(vbo), 
			_ibo(ibo), 
			_mode(mode), 
			_index_count(index_count) {}

	public:
		void draw() const {
			glBindVertexArray(_vao);
			glDrawElements(_mode, _index_count, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	};

	/*
	 * Used to build an instance of GLMesh
	 */
	class GLMeshBuilder {
	public:
		GLenum mode = GL_TRIANGLES;
		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;

		GLMeshBuilder() {}
		GLMeshBuilder(GLenum mode) : mode(mode) {}

		/*
		 * use the set data to build a GLMesh.
		 * After build completion, this builder can be destroyed.
		 */
		GLMesh build() const;
	};
}
