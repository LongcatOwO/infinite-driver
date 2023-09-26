#pragma once


// std
#include <atomic>
#include <type_traits>
#include <utility>

// project
#include <opengl.hpp>


namespace infd {

	/*
	 * Converts an enum class into its underlying integral value.
	 * Useful for passing enum class as integral type, since static_cast is a pain.
	 *
	 * @param <typename Enum> the type of enum class.
	 * @param e an instance of Enum.
	 * @return the underlying integral value of e.
	 */
	template <typename Enum>
	constexpr std::underlying_type_t<Enum> enumValue(Enum e) noexcept {
		return static_cast<std::underlying_type_t<Enum>>(e);
	}

	/*
	 * Represents the type of an OpenGL object.
	 */
	enum class GLObjectType : GLenum {
		Buffer 				= GL_BUFFER,
		VertexArray 		= GL_VERTEX_ARRAY,
		Texture 			= GL_TEXTURE,
		Framebuffer 		= GL_FRAMEBUFFER,
		Shader 				= GL_SHADER,
		Program 			= GL_PROGRAM,
	};
	
	/*
	 * Represents the type of an OpenGL shader.
	 */
	enum class GLShaderType : GLenum {
		Vertex 				= GL_VERTEX_SHADER,
		TessControl 		= GL_TESS_CONTROL_SHADER,
		TessEvaluation 		= GL_TESS_EVALUATION_SHADER,
		Geometry 			= GL_GEOMETRY_SHADER,
		Fragment 			= GL_FRAGMENT_SHADER,
		Compute 			= GL_COMPUTE_SHADER,
	};



	/*
	 * Provide the method construct() and destruct() for 
	 * constructing and destructing OpenGL objects.
	 * 
	 * The first parameter for both construct() and destruct()
	 * must always be GLuint* to the object.
	 *
	 * For construct(), the first parameter GLuint *out_id will be 
	 * assigned the OpenGL object handler id after successful construction.
	 *
	 * For destruct(), the first and only parameter GLuint *out_id will 
	 * be assigned 0 after successful destruction on the OpenGL object handler given by out_id.
	 */
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

	/*
	 * Used to disambiguate constructor overload for GLObject.
	 * Passing this variable to the first parameter in the constructor of GLObject
	 * and then passing the rest of the parameters after it will forward the 
	 * rest of the parameters to GLObjectTraits::construct().
	 */
	struct {} constexpr gl_object_init_construct;

	/*
	 * The type of gl_object_init_construct
	 */
	using gl_object_init_construct_t = std::remove_const_t<decltype(gl_object_init_construct)>;


	/*
	 * Contains an OpenGL object handler. The contract enforced by this class guarantees that 
	 * the handler is always going to be valid, UNLESS YOU CALL glDeleteXXX ON INSTANCE OF THIS CLASS
	 * PLEASE NEVER DO IT.
	 *
	 * This class supports shared reference semantic, meaning that you can freely copy 
	 * this class, and the new instance will refer to the same object handler.
	 * Once a handler is not being referenced, the handler will be automatically destroyed.
	 *
	 * Please avoid caching the handler from this class as it could lead to dangling handlers.
	 * You should use the copy constructor of this class instead.
	 *
	 * Instance of this class is implicitly convertible to GLuint which is the handler contained by
	 * this class.
	 */
	template <GLObjectType Type>
	class GLObject {

		// Construction and destruction trait
		using traits = GLObjectTraits<Type>;

		// OpenGL object handler
		GLuint _id;

		// Shared owning raw pointer to reference counter
		std::atomic_size_t *_counter;

		/*
		 * Decrement the reference counter.
		 * Destroy the reference counter and the handler 
		 * if the reference counter reaches zero.
		 */
		void destruct() {
			--*_counter;

			if (!*_counter) {
				traits::destruct(&_id);
				delete _counter;
			}
		}

	public:
		/*
		 * Constructs a new OpenGL object handler
		 */
		GLObject() noexcept : _counter(new std::atomic_size_t{1}) {
			traits::construct(&_id);
		}

		/*
		 * Constructs a new OpenGL object handler.
		 *
		 * @param ...args the parameters to be forwarded to GLObjectTraits::construct.
		 */
		template <typename ...Args>
		GLObject(gl_object_init_construct_t, Args &&...args) noexcept : _counter(new std::atomic_size_t{1}) {
			traits::construct(&_id, std::forward<Args>(args)...);
		}

		/*
		 * Constructs a new instance sharing the OpenGL object handler.
		 *
		 * @param other the GLObject whose handler will be shared.
		 */
		GLObject(const GLObject &other) noexcept : _id(other._id), _counter(other._counter) {
			++*_counter;
		}

		/*
		 * Constructs a new instance sharing the same OpenGL object handler.
		 * This constructor has exactly the same effect as copy constructor
		 *
		 * @param other the GLObject whose handler will be shared.
		 */
		GLObject(GLObject &&other) noexcept : _id(other._id), _counter(other._counter) {
			++*_counter;
		}

		/*
		 * Shares the OpenGL object handler with other.
		 * Destroy the old handler if it is no longer being referenced after this assignment.
		 *
		 * @param other the GLObject whose handler will be shared.
		 */
		GLObject& operator=(const GLObject &other) noexcept {
			if (*this == other) return *this;
			destruct();
			_id = other._id;
			_counter = other._counter;
			++*_counter;
			return *this;
		}

		/*
		 * Shares the OpenGL object handler with other.
		 * Destroy the old handler if it is no longer being referenced after this assignment.
		 * This assignment operator has exactly the same effect as copy assignment.
		 *
		 * @param other the GLObject whose handler will be shared.
		 */
		GLObject& operator=(GLObject &&other) noexcept {
			if (*this == other) return *this;
			destruct();
			_id = other._id;
			_counter = other._counter;
			++*_counter;
			return *this;
		}

		~GLObject() {
			destruct();
		}

		/*
		 * @returns true if lhs and rhs shares the same handler.
		 */
		friend bool operator==(const GLObject &lhs, const GLObject &rhs) noexcept {
			return lhs._id == rhs._id;
		}

		/*
		 * Implicit conversion to GLuint. 		 
		 * @return the OpenGL object handler of this GLObject.
		 */
		operator GLuint() const noexcept {
			return _id;
		}
	};

	/*
	 * Alias to GLObject<GLObjectType::Shader>.
	 */
	class GLShader : public GLObject<GLObjectType::Shader> {
		using base = GLObject<GLObjectType::Shader>;
	
	public:
		using base::base;
		GLShader() = delete;
		GLShader(GLShaderType type) noexcept : base(gl_object_init_construct, type) {}
	};

	// Alias to GLObject<GLObjectType::Buffer>
	using GLBuffer 				= GLObject<GLObjectType::Buffer>;

	// Alias to GLObject<GLObjectType::VertexArray>
	using GLVertexArray 		= GLObject<GLObjectType::VertexArray>;

	// Alias to GLObject<GLObjectType::Texture>
	using GLTexture 			= GLObject<GLObjectType::Texture>;

	// Alias to GLObject<GLObjectType::Framebuffer>
	using GLFramebuffer 		= GLObject<GLObjectType::Framebuffer>;

	// Alias to GLObject<GLObjectType::Program>
	using GLProgram 			= GLObject<GLObjectType::Program>;
}
