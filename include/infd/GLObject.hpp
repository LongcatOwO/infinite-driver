#pragma once


// std
#include <atomic>
#include <type_traits>
#include <utility>

// project
#include <opengl.hpp>


namespace infd {

	template <typename Enum>
	constexpr std::underlying_type_t<Enum> enumValue(Enum e) noexcept {
		return static_cast<std::underlying_type_t<Enum>>(e);
	}

	enum class GLObjectType : GLenum {
		Buffer 				= GL_BUFFER,
		VertexArray 		= GL_VERTEX_ARRAY,
		Texture 			= GL_TEXTURE,
		Framebuffer 		= GL_FRAMEBUFFER,
		Shader 				= GL_SHADER,
		Program 			= GL_PROGRAM,
	};
	
	enum class GLShaderType : GLenum {
		Vertex 				= GL_VERTEX_SHADER,
		TessControl 		= GL_TESS_CONTROL_SHADER,
		TessEvaluation 		= GL_TESS_EVALUATION_SHADER,
		Geometry 			= GL_GEOMETRY_SHADER,
		Fragment 			= GL_FRAGMENT_SHADER,
		Compute 			= GL_COMPUTE_SHADER,
	};


	template <GLObjectType Type>
	class GLObjectTraits {};

	template <>
	class GLObjectTraits<GLObjectType::Buffer> {
	public:
		static void construct(GLuint *out_id) noexcept {
			glGenBuffers(1, out_id);
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteBuffers(1, out_id);
			*out_id = 0;
		}
	};

	template <>
	class GLObjectTraits<GLObjectType::VertexArray> {
	public:
		static void construct(GLuint *out_id) noexcept {
			glGenVertexArrays(1, out_id);
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteVertexArrays(1, out_id);
			*out_id = 0;
		}
	};

	template <>
	class GLObjectTraits<GLObjectType::Texture> {
	public:
		static void construct(GLuint *out_id) noexcept {
			glGenTextures(1, out_id);
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteTextures(1, out_id);
			*out_id = 0;
		}
	};

	template <>
	class GLObjectTraits<GLObjectType::Framebuffer> {
	public:
		static void construct(GLuint *out_id) noexcept {
			glGenFramebuffers(1, out_id);
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteFramebuffers(1, out_id);
			*out_id = 0;
		}
	};

	template <>
	class GLObjectTraits<GLObjectType::Shader> {
	public:
		static void construct(GLuint *out_id, GLShaderType type) noexcept {
			*out_id = glCreateShader(enumValue(type));
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteShader(*out_id);
		}
	};

	template <>
	class GLObjectTraits<GLObjectType::Program> {
	public:
		static void construct(GLuint *out_id) noexcept {
			*out_id = glCreateProgram();
		}

		static void destruct(GLuint *out_id) noexcept {
			glDeleteProgram(*out_id);
		}
	};


	struct {} constexpr gl_object_init_construct;
	using gl_object_init_construct_t = std::remove_const_t<decltype(gl_object_init_construct)>;

	template <GLObjectType Type>
	class GLObject {
		using traits = GLObjectTraits<Type>;

		GLuint _id;

		// shared owning raw pointer to reference counter
		std::atomic_size_t *_counter;

		void destruct() {
			--*_counter;

			if (!*_counter) {
				traits::destruct(&_id);
				delete _counter;
			}
		}

	public:
		GLObject() noexcept : _counter(new std::atomic_size_t{1}) {
			traits::construct(&_id);
		}

		template <typename ...Args>
		GLObject(gl_object_init_construct_t, Args &&...args) noexcept : _counter(new std::atomic_size_t{1}) {
			traits::construct(&_id, std::forward<Args>(args)...);
		}

		GLObject(const GLObject &other) noexcept : _id(other._id), _counter(other._counter) {
			++*_counter;
		}

		GLObject& operator=(const GLObject &other) noexcept {
			if (*this == other) return *this;
			destruct();
			_id = other._id;
			_counter = other._counter;
			++*_counter;
			return *this;
		}

		GLObject(GLObject &&other) = delete;
		GLObject& operator=(GLObject &&other) = delete;


		~GLObject() {
			destruct();
		}

		friend bool operator==(const GLObject &lhs, const GLObject &rhs) noexcept {
			return lhs._id == rhs._id;
		}

		operator GLuint() noexcept {
			return _id;
		}
	};

	class GLShader : public GLObject<GLObjectType::Shader> {
		using base = GLObject<GLObjectType::Shader>;
	
	public:
		using base::base;
		GLShader() = delete;
		GLShader(GLShaderType type) noexcept : base(gl_object_init_construct, type) {}
	};

	using GLBuffer 				= GLObject<GLObjectType::Buffer>;
	using GLVertexArray 		= GLObject<GLObjectType::VertexArray>;
	using GLTexture 			= GLObject<GLObjectType::Texture>;
	using GLFramebuffer 		= GLObject<GLObjectType::Framebuffer>;
	using GLProgram 			= GLObject<GLObjectType::Program>;
}
