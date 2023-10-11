#pragma once

#include <utility>

namespace infd::util {

	template <typename T>
	class PropertyController;

	template <typename T>
	class PropertyController {

	}; // class PropertyController

	template <typename T>
	class ControllableProperty {
		T _value;
		PropertyController<T> *_controller = nullptr;
	public:
		template <typename... Args>
		ControllableProperty(Args&&... args) : _value(std::forward<Args>(args)...) {}
		~ControllableProperty() = default;

		ControllableProperty(const ControllableProperty&) = delete;
		ControllableProperty(ControllableProperty&&) = delete;
		ControllableProperty& operator=(const ControllableProperty&) = delete;
		ControllableProperty& operator=(ControllableProperty&&) = delete;

		bool setController(PropertyController<T> *controller) noexcept {
			if (_controller) return false;
			_controller = controller;
			return true;
		}

		operator const T&() const noexcept {
			return _value;
		}

		const T& get() const noexcept {
			return _value;
		}

		template <typename T2>
		bool set(T2&& value) {
			if (_controller) return false;
			_value = std::forward<T2>(value);
			return true;
		}

	}; // class ControllableProperty

} // namespace infd::util
