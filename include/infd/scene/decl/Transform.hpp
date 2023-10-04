#pragma once

// std
#include <cstddef>
#include <string>
#include <vector>

// glm
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

// project - scene
#include <infd/scene/definitions.hpp>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// forward declarations
#include <infd/scene/fwd/Transform.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>


namespace infd::scene {
	class Transform : public Component {
		glm::vec<3, Float> _local_position;
		glm::qua<Float> _local_rotation;
		glm::vec<3, Float> _local_scale;

	public:
		[[nodiscard]] Transform() noexcept;

		[[nodiscard]] std::string& name() noexcept;
		[[nodiscard]] const std::string& name() const noexcept;

		[[nodiscard]] bool isRoot() const noexcept;
		[[nodiscard]] bool hasParent() const noexcept;

		[[nodiscard]] Transform* parent() noexcept;
		[[nodiscard]] const Transform* parent() const noexcept;

		[[nodiscard]] bool hasChildren() const noexcept;
		[[nodiscard]] std::size_t numChildren() const noexcept;

		[[nodiscard]] util::random_access_view_of<Transform *> auto childPointersView() noexcept;
		[[nodiscard]] util::random_access_view_of<const Transform *> auto childPointersView() const noexcept;

		[[nodiscard]] std::vector<Transform *> childPointers() noexcept;
		[[nodiscard]] std::vector<const Transform *> childPointers() const noexcept;
		[[nodiscard]] std::vector<Transform *> allChildPointers() noexcept;
		[[nodiscard]] std::vector<const Transform *> allChildPointers() const noexcept;

		[[nodiscard]] util::random_access_move_only_range_of<Transform &> auto children() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const Transform &> auto children() const noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<Transform &> auto allChildren() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const Transform &> auto allChildren() const noexcept;

		Transform& addChild(std::string name) noexcept;

		/*
		 * @throws infd::util::CyclicReferenceException if new_parent is child of this object
		 * @throws infd::util::InvalidStateException if parent is null
		 */
		void removeFromParent(Transform &new_parent);

		/*
		 * Preserves the global transform at default.
		 */
		Transform& attachChild(std::string name) noexcept;

		/*
		 * Preserves the global transform.
		 *
		 * @throws infd::util::CyclicReferenceException if new_parent is child of this object.
		 * @throws infd::util::InvalidStateException if parent is null
		 */
		void detachFromParent(Transform &new_parent);

		[[nodiscard]] const glm::vec<3, Float>& localPosition() const noexcept;
		void localPosition(const glm::vec<3, Float> &value) noexcept;

		[[nodiscard]] const glm::qua<Float>& localRotation() const noexcept;
		void localRotation(const glm::qua<Float> &value) noexcept;

		[[nodiscard]] const glm::vec<3, Float>& localScale() const noexcept;
		void localScale(const glm::vec<3, Float> &value) noexcept;

		[[nodiscard]] glm::mat<4, 4, Float> localTransform() const noexcept;
		void localTransform(const glm::mat<4, 4, Float> &value) noexcept;

		[[nodiscard]] glm::vec<3, Float> globalPosition() const noexcept;
		void globalPosition(const glm::vec<3, Float> &value) noexcept;

		[[nodiscard]] glm::qua<Float> globalRotation() const noexcept;
		void globalRotation(const glm::qua<Float> &value) noexcept;

		[[nodiscard]] glm::vec<3, Float> globalScale() const noexcept;
		void globalScale(const glm::vec<3, Float> &value) noexcept;

	}; // class Transform
} // namespace infd::scene
