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
			_on_construct();
		}

		ScopeGuard(const ScopeGuard &) = delete;
		ScopeGuard(ScopeGuard &&) = delete;
		ScopeGuard& operator=(const ScopeGuard &) = delete;
		ScopeGuard& operator=(ScopeGuard &&) = delete;

		~ScopeGuard() { _on_destruct(); }
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
}
