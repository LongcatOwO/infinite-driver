#pragma once

#include <infd/GLMesh.hpp>

namespace infd::debug {

	GLMesh generateBoxOutline() noexcept;
	GLMesh generateCylinderOutline() noexcept;

	GLMesh generateBoxMesh() noexcept;
	GLMesh generateCylinderMesh() noexcept;

} // namespace infd::debug
