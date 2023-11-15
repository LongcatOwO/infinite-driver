// Created by Phuwasate Lutchanont

// std
#include <ranges>
#include <vector>

// project - scene
#include <infd/scene/Scene.hpp>

namespace infd::scene {

	std::vector<Transform *> Transform::allChildPointers() noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;
		std::vector<Transform *> result = childPointers();
		auto begin = result.begin();
		auto end = result.end();
		while (begin != end) {
			auto next_gen = rng::subrange(begin, end)
				| vw::transform([](Transform *t) { return t->childrenView(); })
				| vw::join
				| vw::transform([](Transform &t) -> Transform* { return &t; })
				| vw::common;
			result.insert(end, next_gen.begin(), next_gen.end());
			begin = end;
			end = result.end();
		}
		return result;
	}

	std::vector<const Transform *> Transform::allChildPointers() const noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;
		std::vector<const Transform *> result = childPointers();
		auto begin = result.begin();
		auto end = result.end();
		while (begin != end) {
			auto next_gen = rng::subrange(begin, end)
				| vw::transform([](const Transform *t) { return t->childrenView(); })
				| vw::join
				| vw::transform([](const Transform &t) -> const Transform* { return &t; })
				| vw::common;
			result.insert(end, next_gen.begin(), next_gen.end());
			begin = end;
			end = result.end();
		}
		return result;
	}

} // namespace infd::scene
