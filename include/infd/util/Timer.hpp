// Created by Phuwasate Lutchanont

#pragma once

// std
#include <concepts>
#include <chrono>

// project - util
#include <infd/util/Event.hpp>

namespace infd::util {
	class Timer final {
	public:
		using Clock = std::chrono::steady_clock;
		using Duration = Clock::duration;
		using TimePoint = std::chrono::steady_clock::time_point;
		using Rep = Clock::rep;
		using Period = Clock::period;

	private:
		Event<void (Timer &self)> _on_interval_completed;
		bool _is_enabled = false;

		Duration _interval;
		Duration _duration_since_last_interval{0};
		Duration _last_interval_duration{0};
		TimePoint _last_time_point;

	public:
		Timer() noexcept : _interval(std::chrono::seconds(1)) {}

		template <typename Rep2, typename Period2>
		Timer(const std::chrono::duration<Rep2, Period2> &interval) noexcept :
			_interval(interval) {}

		template <typename Period2 = std::ratio<1>, std::floating_point Rep2>
		static Timer fromRate(const Rep2 &value) noexcept {
			return { std::chrono::duration_cast<Duration>(std::chrono::duration<Rep2, Period2>{1} / value) };
		}

		PublicEvent<void (Timer &self)>& onIntervalCompleted() noexcept {
			return _on_interval_completed;
		}

		const Duration& interval() const noexcept {
			return _interval;
		}

		template <typename Rep2, typename Period2>
		void interval(const std::chrono::duration<Rep2, Period2> &value) noexcept {
			_interval = value;
		}

		template <typename Period2 = std::ratio<1>, std::floating_point Rep2 = double>
		Rep2 rate() const noexcept {
			return std::chrono::duration<Rep2, Period2>{1} / _interval;
		}

		template <typename Period2 = std::ratio<1>, std::floating_point Rep2>
		void rate(const Rep2 &value) noexcept {
			_interval = std::chrono::duration_cast<Duration>(std::chrono::duration<Rep2, Period2>{1} / value);
		}

		bool isEnabled() const noexcept {
			return _is_enabled;
		}

		void isEnabled(bool value) noexcept {
			if (value)
				enable();
			else
				disable();
		}

		void enable() noexcept {
			if (_is_enabled) return;

			_is_enabled = true;
			_last_time_point = Clock::now();
		}

		void disable() noexcept {
			if (!_is_enabled) return;

			update();
			_is_enabled = false;
		}

		void reset() noexcept {
			_duration_since_last_interval = Duration::zero();
			_last_interval_duration = Duration::zero();
			_last_time_point = Clock::now();
		}

		void update() noexcept {
			if (!_is_enabled) return;

			TimePoint current_time_point = Clock::now();
			_duration_since_last_interval += (current_time_point - _last_time_point);
			_last_time_point = current_time_point;
			
			if (_duration_since_last_interval >= _interval) {
				_last_interval_duration = _duration_since_last_interval;
				_duration_since_last_interval = Duration::zero();
				_on_interval_completed(*this);
			}
		}

		Duration deltaTime() const noexcept {
			return _last_interval_duration;
		}
	}; // class Timer
} // namespace infd::util
