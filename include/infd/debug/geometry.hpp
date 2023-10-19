#pragma once

#include <infd/GLMesh.hpp>

namespace infd::debug {

	GLMesh generateBoxOutline() noexcept;
	GLMesh generateCylinderOutline() noexcept;
	GLMesh generateCapsuleOutline(float radius, float height) noexcept;

	GLMesh generateBoxMesh() noexcept;
	GLMesh generateCylinderMesh() noexcept;
	GLMesh generateCapsuleMesh(float radius, float height) noexcept;

} // namespace infd::debug
