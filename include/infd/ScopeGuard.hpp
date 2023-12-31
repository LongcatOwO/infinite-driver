 // Created by Phuwasate Lutchanont

#pragma once


#include <infd/GLObject.hpp>


namespace infd {

	/*
	 * A utility class which calls the functor on_construct on its construction
	 * and calls the on_destruct functor on its destruction.
	 * Useful for RAII style resource management.
	 */
	template <typename OnConstruct, typename OnDestruct>
	class ScopeGuard {
		OnConstruct _on_construct;
		OnDestruct _on_destruct;
		bool _alive;

	public:
		/*
		 * @param on_construct the functor to be called on construction.
		 * @param on_destruct the functor to be called on destruction.
		 */
		ScopeGuard(
			OnConstruct on_construct = {}, 
			OnDestruct on_destruct = {}
		) :
			_on_construct(on_construct),
			_on_destruct(on_destruct) {
			_alive = true;
			_on_construct();
		}

		ScopeGuard(ScopeGuard&& other) :
			_on_construct(other._on_construct),
			_on_destruct(other._on_destruct) {
			if (other._alive) {
				_alive = true;
				other._alive = false;
			} else {
				_alive = false;
			}
		}

		ScopeGuard& operator=(ScopeGuard&& other) {
			if (_alive)
				_on_destruct();

			_on_construct = other._on_construct;
			_on_destruct = other._on_destruct;
			if (other._alive) {
				_alive = true;
				other._alive = false;
			} else {
				_alive = false;
			}
		}

		ScopeGuard(const ScopeGuard &) = delete;
		ScopeGuard& operator=(const ScopeGuard &) = delete;

		~ScopeGuard() { if (_alive) _on_destruct(); }
	};

	template <typename C, typename D>
	ScopeGuard(C, D) -> ScopeGuard<C, D>;

	/*
	 * @return 	a ScopeGuard which will immediately call glBindBuffer(target, buffer) upon this function call.
	 * 			The returned ScopeGuard will call glBindBuffer(target, 0) when it gets destroyed.
	 */
	inline auto scopedBind(const GLObject<GLObjectType::Buffer> &buffer, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindBuffer(target, buffer); },
			[=] { glBindBuffer(target, 0); }
		);
	}

	/*
	 * @return 	a ScopeGuard which will immediately call glBindVertexArray(vertexArray) upon this function call.
	 * 			The returned ScopeGuard will call glBindVertexArray(0) when it gets destroyed.
	 */
	inline auto scopedBind(const GLObject<GLObjectType::VertexArray> &vertexArray) noexcept {
		return ScopeGuard(
			[=] { glBindVertexArray(vertexArray); },
			[ ] { glBindVertexArray(0); }
		);
	}

	/*
	 * @return 	a ScopeGuard which will immediately call glBindTexture(target, texture) upon this function call.
	 * 			The returned ScopeGuard will call glBindTexture(target, 0) when it gets destroyed.
	 */
	inline auto scopedBind(const GLObject<GLObjectType::Texture> &texture, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindTexture(target, texture); },
			[=] { glBindTexture(target, 0); }
		);
	}

	/*
	 * @return 	a ScopeGuard which will immediately call glBindFramebuffer(target, framebuffer) upon this function call.
	 * 			The returned ScopeGuard will call glBindFramebuffer(target, 0) when it gets destroyed.
	 */
	inline auto scopedBind(const GLObject<GLObjectType::Framebuffer> &framebuffer, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindFramebuffer(target, framebuffer); },
			[=] { glBindFramebuffer(target, 0); }
		);
	}

	/*
	 * @return 	a ScopeGuard which will immediately call glAttachShader(program, shader) upon this function call.
	 * 			The returned ScopeGuard will call glDetachShader(program, shader) when it gets destroyed.
	 */
	inline auto scopedAttach(const GLObject<GLObjectType::Shader> &shader, const GLObject<GLObjectType::Program> &program) {
		return ScopeGuard(
			[=] { glAttachShader(program, shader); },
			[=] { glDetachShader(program, shader); }
		);
	}

    /*
	 * @return 	a ScopeGuard which will immediately call glUseProgram(program) upon this function call.
	 * 			The returned ScopeGuard will call glUseProgram(0) when it gets destroyed.
	 */
    inline auto scopedProgram(const GLObject<GLObjectType::Program> &program) noexcept {
        return ScopeGuard(
                [=] { glUseProgram(program); },
                [ ] { glUseProgram(0); }
        );
    }

    /*
	 * @return 	a ScopeGuard which will immediately call glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); upon this function call.
	 * 			The returned ScopeGuard will call glBindRenderbuffer(GL_RENDERBUFFER, 0); when it gets destroyed.
	 */
    inline auto scopedRenderbuffer(const GLObject<GLObjectType::Renderbuffer> &renderbuffer) noexcept {
        return ScopeGuard(
                [=] { glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); },
                [ ] { glBindRenderbuffer(GL_RENDERBUFFER, 0); }
        );
    }
}
