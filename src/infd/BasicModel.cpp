// glm
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

// project - infd
#include <infd/BasicModel.hpp>

namespace infd {

	void BasicModel::draw(const glm::mat4 &view, const glm::mat4 &proj) const {
		glm::mat4 model_view = view * model_transform;

		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, glm::value_ptr(model_view));
		glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, glm::value_ptr(color));

		mesh.draw();
	}

} // namespace infd
