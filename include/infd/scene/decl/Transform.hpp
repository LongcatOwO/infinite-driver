#pragma once

// std
#include <cstddef>
#include <string>
#include <vector>

// glm
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

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

		mutable bool _parent_global_transform_updated = false;
		mutable glm::mat<4, 4, Float> _parent_global_transform_cache;

		void onAttach() noexcept override;
		void onDetach() noexcept override;
		void internalOnParentAssigned(SceneObject &) noexcept;

		const glm::mat<4, 4, Float>& internalParentGlobalTransform() const noexcept;

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

		[[nodiscard]] util::random_access_view_of<Transform &> auto childrenView() noexcept;
		[[nodiscard]] util::random_access_view_of<const Transform &> auto childrenView() const noexcept;

		[[nodiscard]] std::vector<Transform *> childPointers() noexcept;
		[[nodiscard]] std::vector<const Transform *> childPointers() const noexcept;
		[[nodiscard]] std::vector<Transform *> allChildPointers() noexcept;
		[[nodiscard]] std::vector<const Transform *> allChildPointers() const noexcept;

		[[nodiscard]] util::random_access_move_only_range_of<Transform &> auto children() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const Transform &> auto children() const noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<Transform &> auto allChildren() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const Transform &> auto allChildren() const noexcept;

		template <util::consumer_of<Transform &> V>
		void visitChildren(V &&visitor);

		template <util::consumer_of<const Transform &> V>
		void visitChildren(V &&visitor) const;

		template <util::consumer_of<Transform &> V>
		void visitAllChildren(V &&visitor);

		template <util::consumer_of<const Transform &> V>
		void visitAllChildren(V &&visitor) const;

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
		[[nodiscard]] const glm::qua<Float>& localRotation() const noexcept;
		[[nodiscard]] const glm::vec<3, Float>& localScale() const noexcept;
		[[nodiscard]] glm::mat<4, 4, Float> localTransform() const noexcept;

		void localPosition(const glm::vec<3, Float> &value) noexcept;
		void localRotation(const glm::qua<Float> &value) noexcept;
		void localScale(const glm::vec<3, Float> &value) noexcept;
		void localTransform(
			const glm::vec<3, Float> &position, 
			const glm::qua<Float> &rotation  		= glm::qua<Float>{ 1, 0, 0, 0 }, 
			const glm::vec<3, Float> &scale 		= glm::vec<3, Float>{1}
		) noexcept;

		[[nodiscard]] glm::vec<3, Float> globalPosition() const noexcept;
		[[nodiscard]] glm::qua<Float> globalRotation() const noexcept;
		[[nodiscard]] glm::vec<3, Float> globalScale() const noexcept;
		[[nodiscard]] glm::mat<4, 4, Float> globalTransform() const noexcept;

		void globalTransform(
			const glm::vec<3, Float> &position,
			const glm::qua<Float> &rotation 		= glm::qua<Float>{1, 0, 0, 0},
			const glm::vec<3, Float> &scale 		= glm::vec<3, Float>{1}
		) noexcept;

		void decomposeGlobalTransform(
			glm::vec<3, Float> &out_position,
			glm::qua<Float> &out_rotation,
			glm::vec<3, Float> &out_scale
		) const noexcept;

	}; // class Transform
} // namespace infd::scene
