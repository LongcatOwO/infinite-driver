#pragma once

#include <concepts>

namespace infd::math {

	/*
	 * Caches inverse ratio to reduce division calculation
	 */
	template <std::floating_point T>
	class DynamicRatio;

	template <std::floating_point T>
	class DynamicRatio {
		T _ratio;
		T _inverse_ratio;

		// Aggregate constructor
		constexpr DynamicRatio(const T &ratio, const T &inverse_ratio) noexcept :
			_ratio(ratio),
			_inverse_ratio(inverse_ratio)
		{}

	public:
		static constexpr DynamicRatio fromInverse(const T &inverse_ratio) noexcept {
			return DynamicRatio(static_cast<T>(1) / inverse_ratio, inverse_ratio);
		}

		constexpr DynamicRatio() noexcept : _ratio(1), _inverse_ratio(1) {}

		constexpr DynamicRatio(const T &ratio) noexcept : 
			_ratio(ratio), 
			_inverse_ratio(static_cast<T>(1) / _ratio) 
		{}

		constexpr DynamicRatio& operator=(const T &ratio) noexcept {
			_ratio = ratio;
			_inverse_ratio = static_cast<T>(1) / _ratio;
			return *this;
		}
		
		constexpr operator const T&() const noexcept {
			return _ratio;
		}

		constexpr const T& inverse() const noexcept {
			return _inverse_ratio;
		}

		constexpr void inverse(const T& inverse_rate) noexcept {
			_inverse_ratio = inverse_rate;
			_ratio = static_cast<T>(1) / _inverse_ratio;
		}
	}; // class DynamicRatio

	template <std::floating_point T>
	DynamicRatio(const T&) -> DynamicRatio<std::remove_volatile_t<T>>;

	template <std::floating_point T>
	constexpr DynamicRatio<T> dynamicRatioFromInverse(const T &inverse_ratio) noexcept {
		return DynamicRatio<T>::fromInverse(inverse_ratio);
	}

} // namespace infd::math
