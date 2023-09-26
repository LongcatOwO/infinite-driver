#pragma once

// std
#include <map>
#include <memory>
#include <string>

// project
#include <opengl.hpp>
#include <infd/GLObject.hpp>


namespace infd {


	class ShaderBuilder {
	private:
		std::map<GLenum, GLShader> m_shaders;

	public:
		ShaderBuilder() { }
		void setShader(GLenum type, const std::string &filename);
		void setShaderSource(GLenum type, const std::string &shadersource);

		GLProgram build();
	};

}
