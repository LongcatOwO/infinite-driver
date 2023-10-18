// glm
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// project - infd
#include <opengl.hpp>
#include <infd/GLMesh.hpp>
#include <infd/debug/geometry.hpp>


namespace infd::debug {
	GLMesh generateBoxOutline() noexcept {
		GLMeshBuilder builder{GL_LINES};

		constexpr auto add_face = [](auto&& vertex_mapper, auto&& index_mapper) {
			vertex_mapper( 1,  1);
			vertex_mapper( 1, -1);
			vertex_mapper(-1,  1);
			vertex_mapper(-1, -1);
			index_mapper(0);
			index_mapper(1);
			index_mapper(1);
			index_mapper(3);
			index_mapper(3);
			index_mapper(2);
			index_mapper(2);
			index_mapper(0);
		};

		const auto make_vertex_mapper = [&builder](auto pos_mapper, const glm::vec3& norm) {
			return [&builder, pos_mapper, norm](float x, float y) {
				builder.vertices.push_back({pos_mapper(x, y), norm, glm::vec2{x + 1.f, y + 1.f} / 2.f});
			};
		};

		const auto make_index_mapper = [&builder](int face) {
			return [&builder, offset = face * 4](unsigned int i) {
				builder.indices.push_back(offset + i);
			};
		};

		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  y,  1}; }, { 0,  0,  1}), make_index_mapper(0));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  y, -1}; }, { 0,  0, -1}), make_index_mapper(1));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  1,  y}; }, { 0,  1,  0}), make_index_mapper(2));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x, -1,  y}; }, { 0, -1,  0}), make_index_mapper(3));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ 1,  x,  y}; }, { 1,  0,  0}), make_index_mapper(4));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{-1,  x,  y}; }, {-1,  0,  0}), make_index_mapper(5));

		return builder.build();
	}

	GLMesh generateCylinderOutline() noexcept {
		constexpr auto num_subdivision = 32;
		constexpr float ratio_per_div = 1.f / num_subdivision;
		constexpr float angle_per_div = glm::two_pi<float>() * ratio_per_div;

		GLMeshBuilder builder{GL_LINES};

		const auto make_circle_face = [&builder](auto pos_mapper, const glm::vec3& normal) {

			auto offset = builder.vertices.size();
			float angle = 0.f;

			for (int i = 0; i < num_subdivision; ++i, angle += angle_per_div) {
				float x = glm::cos(angle), y = glm::sin(angle);
				builder.vertices.push_back({pos_mapper(x, y), normal, glm::vec2{x + 1.f, y + 1.f} / 2.f});
			}

			for (auto i = offset + 1; i < builder.vertices.size() - 1; ++i)
				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(i),
					static_cast<unsigned int>(i + 1),
				});

			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(builder.vertices.size() - 1),
				static_cast<unsigned int>(offset + 1),
			});
		};


		make_circle_face([](float x, float y) { return glm::vec3{x,  1, y}; }, {0,  1, 0});
		make_circle_face([](float x, float y) { return glm::vec3{x, -1, y}; }, {0, -1, 0});

		float angle = 0.f;
		float u = 0.f;

		for (int i = 0; i < num_subdivision; ++i, angle += angle_per_div, u += ratio_per_div) {
			float x = glm::cos(angle), y = glm::sin(angle);
			builder.vertices.push_back({{x,  1, y}, {x, 0, y}, {u, 1}});
			builder.vertices.push_back({{x, -1, y}, {x, 0, y}, {u, 0}});
		}

		for (int i = 0, index = 2 * (num_subdivision); i < num_subdivision; ++i, index += 2) {
			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(index),
				static_cast<unsigned int>(index + 1),
			});
		}

		return builder.build();
	}

	GLMesh generateBoxMesh() noexcept {
		GLMeshBuilder builder;

		constexpr auto add_face = [](auto&& vertex_mapper, auto&& index_mapper) {
			vertex_mapper( 1,  1);
			vertex_mapper( 1, -1);
			vertex_mapper(-1,  1);
			vertex_mapper(-1, -1);
			index_mapper(0);
			index_mapper(1);
			index_mapper(2);
			index_mapper(1);
			index_mapper(2);
			index_mapper(3);
		};

		const auto make_vertex_mapper = [&builder](auto pos_mapper, const glm::vec3& norm) {
			return [&builder, pos_mapper, norm](float x, float y) {
				builder.vertices.push_back({pos_mapper(x, y), norm, glm::vec2{x + 1.f, y + 1.f} / 2.f});
			};
		};

		const auto make_index_mapper = [&builder](int face) {
			return [&builder, offset = face * 4](unsigned int i) {
				builder.indices.push_back(offset + i);
			};
		};

		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  y,  1}; }, { 0,  0,  1}), make_index_mapper(0));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  y, -1}; }, { 0,  0, -1}), make_index_mapper(1));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x,  1,  y}; }, { 0,  1,  0}), make_index_mapper(2));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ x, -1,  y}; }, { 0, -1,  0}), make_index_mapper(3));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{ 1,  x,  y}; }, { 1,  0,  0}), make_index_mapper(4));
		add_face(make_vertex_mapper([](float x, float y) { return glm::vec3{-1,  x,  y}; }, {-1,  0,  0}), make_index_mapper(5));

		return builder.build();
	}

	GLMesh generateCylinderMesh() noexcept {
		constexpr auto num_subdivision = 32;
		constexpr float ratio_per_div = 1.f / num_subdivision;
		constexpr float angle_per_div = glm::two_pi<float>() * ratio_per_div;

		GLMeshBuilder builder{GL_LINES};

		const auto make_circle_face = [&builder](auto pos_mapper, const glm::vec3& normal) {

			auto offset = builder.vertices.size();
			builder.vertices.push_back({pos_mapper(0.f, 0.f), normal, {0.5f, 0.5f}});
			float angle = 0.f;

			for (int i = 0; i < num_subdivision; ++i, angle += angle_per_div) {
				float x = glm::cos(angle), y = glm::sin(angle);
				builder.vertices.push_back({pos_mapper(x, y), normal, glm::vec2{x + 1.f, y + 1.f} / 2.f});
			}

			for (auto i = offset + 1; i < builder.vertices.size() - 1; ++i)
				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(offset), 
					static_cast<unsigned int>(i), 
					static_cast<unsigned int>(i + 1),
				});

			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(offset),
				static_cast<unsigned int>(builder.vertices.size() - 1),
				static_cast<unsigned int>(offset + 1),
			});
		};


		make_circle_face([](float x, float y) { return glm::vec3{x,  1, y}; }, {0,  1, 0});
		make_circle_face([](float x, float y) { return glm::vec3{x, -1, y}; }, {0, -1, 0});

		float angle = 0.f;
		float u = 0.f;

		for (int i = 0; i < num_subdivision; ++i, angle += angle_per_div, u += ratio_per_div) {
			float x = glm::cos(angle), y = glm::sin(angle);
			builder.vertices.push_back({{x,  1, y}, {x, 0, y}, {u, 1}});
			builder.vertices.push_back({{x, -1, y}, {x, 0, y}, {u, 0}});
		}

		for (int i = 0, index = 2 * (num_subdivision + 1); i < num_subdivision - 1; ++i, index += 2) {
			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(index),
				static_cast<unsigned int>(index + 1),
				static_cast<unsigned int>(index + 2),

				static_cast<unsigned int>(index + 1),
				static_cast<unsigned int>(index + 2),
				static_cast<unsigned int>(index + 3),
			});
		}

		builder.indices.insert(builder.indices.end(), {
			static_cast<unsigned int>(4 * num_subdivision),
			static_cast<unsigned int>(4 * num_subdivision + 1),
			static_cast<unsigned int>(2 * (num_subdivision + 1)),

			static_cast<unsigned int>(4 * num_subdivision + 1),
			static_cast<unsigned int>(2 * (num_subdivision + 1)),
			static_cast<unsigned int>(2 * (num_subdivision + 1) + 1),
		});

		return builder.build();
	}

} // namespace infd::debug
