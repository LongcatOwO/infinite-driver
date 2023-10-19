// std
#include <functional>

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

	GLMesh generateCapsuleOutline(float radius, float height) noexcept {
		constexpr auto num_subdivision = 32;
		constexpr float ratio_per_div = 1.f / num_subdivision;
		constexpr float angle_per_div = glm::two_pi<float>() * ratio_per_div;

		GLMeshBuilder builder{GL_LINES};

		const auto make_circle = [&](float height_offset) {
			auto offset = builder.vertices.size();
			float angle = 0.f;
			for (int i = 0; i < num_subdivision; ++i, angle += angle_per_div) {
				float x = glm::cos(angle), y = glm::sin(angle);
				builder.vertices.push_back({{x * radius, height_offset, y * radius}, {x, 0, y}, {0, 0}});
			}

			for (auto i = offset + 1; i < builder.vertices.size(); ++i) {
				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(i - 1),
					static_cast<unsigned int>(i),
				});
			}
			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(builder.vertices.size() - 1),
				static_cast<unsigned int>(offset),
			});
		};

		make_circle( height / 2);
		make_circle(-height / 2);
		
		// lines on the side of capsule
		for (int i = 0; i < 4; ++i)
			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(num_subdivision / 4 * i),
				static_cast<unsigned int>(num_subdivision / 4 * i + num_subdivision),
			});

		const auto make_dome_line = [&](auto pos_mapper, auto norm_mapper) {
			auto offset = builder.vertices.size();

			float angle = 0.f;
			for (int i = 0; i < num_subdivision / 2 + 1; ++i, angle += angle_per_div) {
				float x = glm::cos(angle), y = glm::sin(angle);
				builder.vertices.push_back({pos_mapper(x * radius, y * radius), norm_mapper(x, y), {0, 0}});
			}

			for (auto i = offset; i < builder.vertices.size() - 1; ++i) {
				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(i),
					static_cast<unsigned int>(i + 1),
				});
			}
		};

		make_dome_line(
			[&](float x, float y) { return glm::vec3{x, height / 2 + y, 0}; },
			[ ](float x, float y) { return glm::vec3{x, y, 0}; }
		);
		make_dome_line(
			[&](float x, float y) { return glm::vec3{0, height / 2 + y, x}; },
			[ ](float x, float y) { return glm::vec3{0, y, x}; }
		);
		make_dome_line(
			[&](float x, float y) { return glm::vec3{x, -(height / 2 + y), 0}; },
			[ ](float x, float y) { return glm::vec3{x, -y, 0}; }
		);
		make_dome_line(
			[&](float x, float y) { return glm::vec3{0, -(height / 2 + y), x}; },
			[ ](float x, float y) { return glm::vec3{0, -y, x}; }
		);

		return builder.build();
	}

	GLMesh generateBoxMesh() noexcept {
		GLMeshBuilder builder{GL_TRIANGLES};

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

		GLMeshBuilder builder{GL_TRIANGLES};

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

	GLMesh generateCapsuleMesh(float radius, float height) noexcept {
		constexpr auto num_subdivision = 32;
		constexpr float ratio_per_div = 1.f / num_subdivision;
		constexpr float angle_per_div = glm::two_pi<float>() * ratio_per_div;

		GLMeshBuilder builder{GL_TRIANGLES};

		const auto make_dome = [&](auto pos_mapper, auto norm_mapper) {
			auto offset = builder.vertices.size();

			builder.vertices.push_back({ pos_mapper({0, 1, 0}), norm_mapper({0, 1, 0}), {0, 0} });
			float vertical_angle = angle_per_div;
			for (int i = 0; i < num_subdivision / 4; ++i, vertical_angle += angle_per_div) {
				float horizontal_angle = 0.f;
				float y = glm::cos(vertical_angle);
				float horizontal = glm::sin(vertical_angle);
				for (int j = 0; j < num_subdivision; ++j, horizontal_angle += angle_per_div) {
					float x = horizontal * glm::cos(horizontal_angle);
					float z = horizontal * glm::sin(horizontal_angle);
					builder.vertices.push_back(
						{ pos_mapper(glm::vec3{x, y, z}), norm_mapper(glm::vec3{x, y, z}), {0, 0} }
					);
				}
			}

			// ----------section connecting to the pole----------
			for (int j = 1; j < num_subdivision; ++j) {
				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(offset),
					static_cast<unsigned int>(offset + j),
					static_cast<unsigned int>(offset + j + 1),
				});
			}

			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(offset),
				static_cast<unsigned int>(offset + num_subdivision),
				static_cast<unsigned int>(offset + 1),
			});
			// --------------------------------------------------

			// ----------rest of the sections----------
			for (int i = 1, index = offset + 1 + num_subdivision; i < num_subdivision / 4; ++i, ++index) {
				for (int j = 0; j < num_subdivision - 1; ++j, ++index) {
					builder.indices.insert(builder.indices.end(), {
						static_cast<unsigned int>(index),
						static_cast<unsigned int>(index - num_subdivision),
						static_cast<unsigned int>(index + 1),

						static_cast<unsigned int>(index - num_subdivision),
						static_cast<unsigned int>(index + 1),
						static_cast<unsigned int>(index - num_subdivision + 1),
					});
				}

				builder.indices.insert(builder.indices.end(), {
					static_cast<unsigned int>(index),
					static_cast<unsigned int>(index - num_subdivision),
					static_cast<unsigned int>(index + 1 - num_subdivision),

					static_cast<unsigned int>(index - num_subdivision),
					static_cast<unsigned int>(index + 1 - num_subdivision),
					static_cast<unsigned int>(index - num_subdivision + 1 - num_subdivision),
				});
			}
			// ----------------------------------------
		};

		make_dome(
			[&](const glm::vec3& dir) { 
				return dir * radius + glm::vec3{0, height / 2, 0}; 
			},
			[ ](const glm::vec3& dir) { 
				return dir; 
			}
		);
		make_dome(
			[&](const glm::vec3& dir) { 
				return glm::vec3{dir.x, -dir.y, dir.z} * radius + glm::vec3{0, -height / 2, 0}; 
			},
			[ ](const glm::vec3& dir) { 
				return glm::vec3{dir.x, -dir.y, dir.z}; 
			}
		);

		constexpr auto num_vert_per_dome = 1 + (num_subdivision / 4) * num_subdivision;
		for (int i = 0; i < num_subdivision - 1; ++i) {
			builder.indices.insert(builder.indices.end(), {
				static_cast<unsigned int>(num_vert_per_dome     - num_subdivision + i    ),
				static_cast<unsigned int>(num_vert_per_dome * 2 - num_subdivision + i    ),
				static_cast<unsigned int>(num_vert_per_dome     - num_subdivision + i + 1),

				static_cast<unsigned int>(num_vert_per_dome * 2 - num_subdivision + i    ),
				static_cast<unsigned int>(num_vert_per_dome     - num_subdivision + i + 1),
				static_cast<unsigned int>(num_vert_per_dome * 2 - num_subdivision + i + 1),
			});
		}
		builder.indices.insert(builder.indices.end(), {
			static_cast<unsigned int>(num_vert_per_dome     - 1),
			static_cast<unsigned int>(num_vert_per_dome * 2 - 1),
			static_cast<unsigned int>(num_vert_per_dome     - num_subdivision),

			static_cast<unsigned int>(num_vert_per_dome * 2 - 1),
			static_cast<unsigned int>(num_vert_per_dome     - num_subdivision),
			static_cast<unsigned int>(num_vert_per_dome * 2 - num_subdivision),
		});

		return builder.build();
	}

} // namespace infd::debug
