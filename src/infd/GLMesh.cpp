// std
#include <cstddef>

// project
#include <opengl.hpp>
#include <infd/GLMesh.hpp>
#include <infd/GLObject.hpp>


namespace infd {

	GLMesh GLMeshBuilder::build() const {
		GLVertexArray vao;
		GLBuffer vbo;
		GLBuffer ibo;

		glBindVertexArray(vao);


		// --------------------VBO--------------------
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);

		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)(offsetof(MeshVertex, pos)));
		// normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)(offsetof(MeshVertex, norm)));
		// uv
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)(offsetof(MeshVertex, uv)));

		
		// --------------------IBO--------------------
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		// indices
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		std::size_t index_count = indices.size();

		glBindVertexArray(0);

		return GLMesh{vao, vbo, ibo, mode, index_count};
	}
}
