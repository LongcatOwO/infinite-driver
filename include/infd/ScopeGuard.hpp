#pragma once


#include <infd/GLObject.hpp>


namespace infd {

	template <typename OnConstruct, typename OnDestruct>
	class ScopeGuard {
		OnConstruct _on_construct;
		OnDestruct _on_destruct;

	public:
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

	inline auto scopedBind(const GLObject<GLObjectType::Buffer> &obj, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindBuffer(target, obj); },
			[=] { glBindBuffer(target, 0); }
		);
	}

	inline auto scopedBind(const GLObject<GLObjectType::VertexArray> &obj) noexcept {
		return ScopeGuard(
			[=] { glBindVertexArray(obj); },
			[ ] { glBindVertexArray(0); }
		);
	}

	inline auto scopedBind(const GLObject<GLObjectType::Texture> &obj, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindTexture(target, obj); },
			[=] { glBindTexture(target, 0); }
		);
	}

	inline auto scopedBind(const GLObject<GLObjectType::Framebuffer> &obj, GLenum target) noexcept {
		return ScopeGuard(
			[=] { glBindFramebuffer(target, obj); },
			[=] { glBindFramebuffer(target, 0); }
		);
	}

	inline auto scopedAttach(const GLObject<GLObjectType::Shader> &obj, GLuint program) {
		return ScopeGuard(
			[=] { glAttachShader(program, obj); },
			[=] { glDetachShader(program, obj); }
		);
	}
}
