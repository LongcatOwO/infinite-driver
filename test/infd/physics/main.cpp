#include <iostream>

// std
#include <exception>
#include <sstream>

// glm
#include <glm/detail/setup.hpp>

// imgui
#include <imgui.h>

// project
#include <opengl.hpp>

// cgra
#include <cgra/cgra_gui.hpp>

// project - infd
#include <infd/Application.hpp>

// project - infd::util
#include <infd/util/exceptions.hpp>


#define INFD_ENABLE_DEBUG


// forward declarations
namespace {
	constexpr const char *gl_arb_debug_output_str = "GL_ARB_debug_output";

	struct unknown_error_t {} constexpr unknown_error;

	/*
	 * @throws infd::util::ExternalLibraryException
	 */
	void initializationRoutine();
	void exitRoutine();
	void exitRoutine(const std::exception &e);
	void exitRoutine(unknown_error_t);

	/*
	 * @throws infd::util::ExternalLibraryException
	 */
	[[nodiscard]] infd::GLFWWindowPtr createWindow(const char *title, int width, int height);
	void installApplicationCallbacks(infd::Application &application);

#ifdef INFD_ENABLE_DEBUG
	void installDebugCallback(GLFWwindow *window);
	void APIENTRY debugCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *, const GLvoid *);
#endif

}

int main() {
	constexpr int window_width = 800, window_height = 600;
	constexpr const char *application_name = "Infinite Driver Simulator";

	try {
		// initialize external libraries
		initializationRoutine();

		// create the application object
		infd::Application application(
				createWindow(application_name, window_width, window_height), 
				application_name
			);
		GLFWwindow *application_window = application.window();

		// install callbacks to the application window
		installApplicationCallbacks(application);

#ifdef INFD_ENABLE_DEBUG
		// install debug callbacks to the application window
		installDebugCallback(application_window);
#endif

		application.preLoopInit();

		// loop until the user closes the window
		while (!glfwWindowShouldClose(application_window))
			application.update();

	} catch (const std::exception &e) {
		// report error
		std::cerr << "Exception occurred: " << e.what() << std::endl;
		// perform clean up routine
		exitRoutine(e);
		return -1;
	} catch (...) {
		// report error
		std::cerr << "Unknown error occurred." << std::endl;
		// perform clean up routine
		exitRoutine(unknown_error);
		return -1;
	}

	// perform clean up routine
	exitRoutine();
	return 0;
}

namespace {

	void initializationRoutine() {
		if (!glfwInit())
			throw infd::util::GLFWException("Error: Could not initialize GLFW.");
	}

	void exitRoutine() {
		cgra::gui::shutdown();
		glfwTerminate();
	}

	void exitRoutine(const std::exception &e) {
		(void) e;
		exitRoutine();
	}

	void exitRoutine(unknown_error_t) {
		exitRoutine();
	}

	infd::GLFWWindowPtr createWindow(const char *title, int width, int height) {
		// OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// disallow legacy functionality (helps OS X work)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// get the version for GLFW for later
		int glfw_version_major, glfw_version_minor, glfw_version_revision;
		glfwGetVersion(&glfw_version_major, &glfw_version_minor, &glfw_version_revision);

#ifdef INFD_ENABLE_DEBUG
		// request a debug context so we get debug callbacks
		// remove this for possible GL performance increases
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

		// create a windowed mode window and its OpenGL context
		infd::GLFWWindowPtr window{glfwCreateWindow(width, height, title, nullptr, nullptr)};

		if (!window)
			throw infd::util::GLFWException("Error: Could not create GLFW window.");

		// make the window's context current
		glfwMakeContextCurrent(window.get());

		// initialize GLEW
		// must be done after making a GL context current (glfwMakeContextCurrent in this case)
		glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
		GLenum glew_init_status = glewInit();
		if (glew_init_status != GLEW_OK)
			throw infd::util::GLEWException((
					std::ostringstream{} << "Error: " << glewGetErrorString(glew_init_status) << "."
				).str());

		// print out our OpenGL versions
		std::cout << "Using OpenGL " << glGetString(GL_VERSION) << '\n';
		std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << '\n';
		std::cout << "Using GLFW " << glfw_version_major << "." << glfw_version_minor << "." 
			<< glfw_version_revision << '\n';
		std::cout << "Using " << GLM_VERSION_MESSAGE << '\n';
		std::cout << "Using Dear ImGui " << IMGUI_VERSION << '\n';

		// initialize ImGui
		if (!cgra::gui::init(window.get(), true))
			throw infd::util::ImGuiException("Error: Could not initialize ImGui.");

		return window;
	}

	void installApplicationCallbacks(infd::Application &application) {
		class Local {
		public:
			[[nodiscard]] static infd::Application& getWindowApplication(GLFWwindow *window) {
				return *reinterpret_cast<infd::Application *>(glfwGetWindowUserPointer(window));
			}

			static void cursorPosCallback(GLFWwindow *window, double x_pos, double y_pos) {
				if (ImGui::GetIO().WantCaptureMouse) return;
				getWindowApplication(window).cursorPosCallback(x_pos, y_pos);
			}
			
			static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
				if (ImGui::GetIO().WantCaptureMouse) return;
				getWindowApplication(window).mouseButtonCallback(button, action, mods);
			}

			static void scrollCallback(GLFWwindow *window, double x_offset, double y_offset) {
				if (ImGui::GetIO().WantCaptureMouse) return;
				getWindowApplication(window).scrollCallback(x_offset, y_offset);
			}

			static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
				if (ImGui::GetIO().WantCaptureMouse) return;
				getWindowApplication(window).keyCallback(key, scancode, action, mods);
			}

			static void charCallback(GLFWwindow *window, unsigned int c) {
				if (ImGui::GetIO().WantCaptureMouse) return;
				getWindowApplication(window).charCallback(c);
			}

			static void APIENTRY debugCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const GLvoid*);
			};

		GLFWwindow *window = application.window();
		glfwSetWindowUserPointer(window, &application);

		// attach input callbacks to window
		glfwSetCursorPosCallback(window, Local::cursorPosCallback);
		glfwSetMouseButtonCallback(window, Local::mouseButtonCallback);
		glfwSetScrollCallback(window, Local::scrollCallback);
		glfwSetKeyCallback(window, Local::keyCallback);
		glfwSetCharCallback(window, Local::charCallback);
	}

#ifdef INFD_ENABLE_DEBUG
	void installDebugCallback(GLFWwindow *window) {
		// cache the current window
		GLFWwindow *current_window = glfwGetCurrentContext();
		// make the application's window current if not already for glXXX calls
		if (current_window != window)
			glfwMakeContextCurrent(window);

		// enable GL_ARB_debug_output if available (not necessary, just helpful)
		if (glfwExtensionSupported(gl_arb_debug_output_str)) {
			// this allows the error location to be determined from a stacktrace
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
			// setup the callback
			glDebugMessageCallbackARB(debugCallback, nullptr);
			glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			std::cout << gl_arb_debug_output_str << " callback installed.\n";
		} else {
			std::cout << gl_arb_debug_output_str << " not available. No worries.\n";
		}

		// restore the current context back to the original to prevent side effects
		if (current_window != window)
			glfwMakeContextCurrent(current_window);
	}

	// function to translate source to string
	[[nodiscard]] const char * getStringForSource(GLenum source) {
		switch (source) {
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "Application";
		case GL_DEBUG_SOURCE_OTHER:
			return "Other";
		default:
			return "n/a";
		}
	}

	// function to translate severity to string
	[[nodiscard]] const char * getStringForSeverity(GLenum severity) {
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "Medium";
		case GL_DEBUG_SEVERITY_LOW:
			return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "None";
		default:
			return "n/a";
		}
	}

	// function to translate type to string
	[[nodiscard]] const char * getStringForType(GLenum type) {
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "Deprecated Behaviour";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "Undefined Behaviour";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "Performance";
		case GL_DEBUG_TYPE_OTHER:
			return "Other";
		default:
			return "n/a";
		}
	}

	void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const GLvoid *) {
		// Don't report notification messages
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

		// nvidia: avoid debug spam about attribute offsets
		if (id == 131076) return;

		std::cerr << "GL [" << getStringForSource(source) << "] " 
			<< getStringForType(type) << ' ' << id << " : "
			<< message << " (Severity: " << getStringForSeverity(severity) << ")\n";

		if (type == GL_DEBUG_TYPE_ERROR_ARB) 
			throw std::runtime_error((
					std::ostringstream{} << "GL Error: " << message
				).str());
	}

#endif
}
