#pragma once

// std
#include <concepts>
#include <utility>
#include <vector>

// project
#include <infd/util/Function.hpp>


namespace infd::util {
	template <typename FunctionSignature>
	class PublicEvent;

	template <typename R, typename ...Args>
	class PublicEvent<R (Args...)> {
	public:
		virtual PublicEvent& addListener 	(const Function<R (Args...)>&  listener) & = 0;
		virtual PublicEvent& addListener 	( 	   Function<R (Args...)>&& listener) & = 0;
		virtual PublicEvent& removeListener (const Function<R (Args...)>&  listener) & = 0;
		virtual PublicEvent& operator+= 	(const Function<R (Args...)>&  listener) & = 0;
		virtual PublicEvent& operator+= 	( 	   Function<R (Args...)>&& listener) & = 0;
		virtual PublicEvent& operator-= 	(const Function<R (Args...)>&  listener) & = 0;
	};

	template <typename FunctionSignature>
	class Event;

	template <typename R, typename ...Args>
	class Event<R (Args...)> : public PublicEvent<R (Args...)> {
		std::vector<Function<R (Args...)>> _listeners;

	public:
		Event& addListener(const Function<R (Args...)> &listener) & override {
			_listeners.push_back(listener);
			return *this;
		}

		Event& addListener(Function<R (Args...)> &&listener) & override {
			_listeners.push_back(std::move(listener));
			return *this;
		}

		Event& removeListener(const Function<R (Args...)> &listener) & override {
			std::erase(_listeners, listener);
			return *this;
		}

		Event& operator+=(const Function<R (Args...)> &listener) & override {
			return addListener(listener);
		}

		Event& operator+=(Function<R (Args...)> &&listener) & override {
			return addListener(std::move(listener));
		}

		Event& operator-=(const Function<R (Args...)> &listener) & override {
			return removeListener(listener);
		}

		auto invoke(Args ...args) const {
			if constexpr (std::same_as<R, void>) {
				for (const auto &listener : _listeners) {
					listener(std::forward<Args>(args)...);
				}
			} else {
				std::vector<R> result;
				result.reserve(_listeners.size());
				for (const auto &listener : _listeners) {
					result.push_back(listener(std::forward<Args>(args)...));
				}
				return result;
			}
		}

		void operator()(Args ...args) const {
			return invoke(std::forward<Args>(args)...);
		}
	};
}
