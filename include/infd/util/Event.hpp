#pragma once

// std
#include <utility>
#include <vector>

// project
#include <infd/util/Function.hpp>


namespace infd::util {
	template <typename ...Args>
	class PublicEvent {
		virtual PublicEvent& addListener 	(const Function<void (Args...)>&  listener) & = 0;
		virtual PublicEvent& addListener 	( 	   Function<void (Args...)>&& listener) & = 0;
		virtual PublicEvent& removeListener (const Function<void (Args...)>&  listener) & = 0;
		virtual PublicEvent& operator+= 	(const Function<void (Args...)>&  listener) & = 0;
		virtual PublicEvent& operator+= 	( 	   Function<void (Args...)>&& listener) & = 0;
		virtual PublicEvent& operator-= 	(const Function<void (Args...)>&  listener) & = 0;
	};

	template <typename ...Args>
	class Event : public PublicEvent<Args...> {
		std::vector<Function<void (Args...)>> _listeners;

	public:
		Event& addListener(const Function<void (Args...)> &listener) & override {
			_listeners.push_back(listener);
			return *this;
		}

		Event& addListener(Function<void (Args...)> &&listener) & override {
			_listeners.push_back(std::move(listener));
			return *this;
		}

		Event& removeListener(const Function<void (Args...)> &listener) & override {
			std::erase(_listeners, listener);
			return *this;
		}

		Event& operator+=(const Function<void (Args...)> &listener) & override {
			return addListener(listener);
		}

		Event& operator+=(Function<void (Args...)> &&listener) & override {
			return addListener(std::move(listener));
		}

		Event& operator-=(const Function<void (Args...)> &listener) & override {
			return removeListener(listener);
		}

		void invoke(Args ...args) const {
			for (const auto &listener : _listeners) {
				listener(std::forward<Args>(args)...);
			}
		}

		void operator()(Args ...args) const {
			for (const auto &listener : _listeners) {
				listener(std::forward<Args>(args)...);
			}
		}
	};
}
