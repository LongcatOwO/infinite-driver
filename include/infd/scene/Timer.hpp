#pragma once

// std
#include <chrono>

// project - util
#include <infd/util/Event.hpp>

// project - math
#include <infd/math/DynamicRatio.hpp>

// project - scene
#include <infd/scene/definitions.hpp>


namespace infd::scene {
	/*
	 * Seconds as unit.
	 */
	class Timer final {
		util::Event<void (Timer &)> _on_interval_complete;
		math::DynamicRatio<Float> _interval;
		bool _is_enabled = false;

		TimePoint _last_time_point;
		Float _time_elapsed_since_last_interval = 0;
		Float _last_interval_delta_time = 0;

		[[nodiscard]] static Float secondsElapsed(const TimePoint &before, const TimePoint &after) noexcept {
			return 
				static_cast<Float>(
					std::chrono::duration_cast<Duration>(after - before).count()
				)
			  / static_cast<Float>(Duration::period::den);
		}

	public:
		[[nodiscard]] Timer() noexcept : _interval(1) {}
		[[nodiscard]] Timer(Float interval) noexcept : _interval(interval) {}

		[[nodiscard]] bool isEnabled() const noexcept {
			return _is_enabled;
		}

		void isEnabled(bool value) {
			if (value) 
				enable();
			else
				disable();
		}

		[[nodiscard]] Float interval() const noexcept {
			return _interval;
		}

		void interval(Float value) noexcept {
			_interval = value;
		}

		[[nodiscard]] Float rate() const noexcept {
			return _interval.inverse();
		}

		void rate(Float value) noexcept {
			_interval.inverse(value);
		}

		void reset() noexcept {
			_time_elapsed_since_last_interval = 0;
			_last_interval_delta_time = 0;
			_last_time_point = Clock::now();
		}

		void update() {
			if (!_is_enabled) return;
			TimePoint now = Clock::now();
			_time_elapsed_since_last_interval += secondsElapsed(_last_time_point, now);
			_last_time_point = now;
			if (_time_elapsed_since_last_interval >= _interval) {
				_last_interval_delta_time = _time_elapsed_since_last_interval;
				_time_elapsed_since_last_interval = 0;
				_on_interval_complete(*this);
			}
		}

		void enable() noexcept {
			if (_is_enabled) return;
			_last_time_point = Clock::now();
			_is_enabled = true;
		}

		void disable() {
			if (!_is_enabled) return;
			update();
			_is_enabled = false;
		}

		[[nodiscard]] util::PublicEvent<void (Timer &)>& onIntervalComplete() noexcept {
			return _on_interval_complete;
		}

		/*
		 * Time elapsed between the second last and the last interval
		 */
		[[nodiscard]] Float deltaTime() const noexcept {
			return _last_interval_delta_time;
		}
	};
} // namespace infd::scene
