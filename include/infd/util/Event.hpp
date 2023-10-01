#pragma once

// std
#include <concepts>
#include <utility>
#include <vector>

// project
#include <infd/util/Function.hpp>


namespace infd::util {

	/*
	 * Provides a public Event interface for the given FunctionSignature.
	 * A FunctionSignature can be provided as following: PublicEvent<ReturnType (ArgumentTypes...)>.
	 * The interface only provides methods for adding and removing listeners, but not invoking 
	 * the event. Useful for "publishing" an event to be (un)subscribed, while disallowing 
	 * event invocation.
	 */
	template <typename FunctionSignature>
	class PublicEvent;

	/*
	 * Provides a simple way to implement an observer pattern.
	 * Observers can subscribe to this event using addListener or operator+=,
	 * and unsubscribe from this event using removeListener or operator-=.
	 * To notify all observers, simply call invoke() or operator() with the proper arguments.
	 *
	 * A good pattern to use is declaring a variable of type Event as private variable, 
	 * and then create a public method which returns the variable as PublicEvent&.
	 * This way the observers are not allowed to invoke the event, only the event owner.
	 */
	template <typename FunctionSignature>
	class Event;


	template <typename R, typename ...Args>
	class PublicEvent<R (Args...)> {
		friend class Event<R (Args...)>;

		std::vector<Function<R (Args...)>> _listeners;
		PublicEvent() noexcept {}

		~PublicEvent() {}
	public:
		/*
		 * Add a new listener to the event.
		 */
		PublicEvent& addListener(const Function<R (Args...)> &listener) & {
			_listeners.push_back(listener);
			return *this;
		}

		/*
		 * Add a new listener to the event.
		 */
		PublicEvent& addListener(Function<R (Args...)> &&listener) & {
			_listeners.push_back(std::move(listener));
			return *this;
		}

		/*
		 * Remove every listener "e" that satisfy the condition: e == listener.
		 * This comparison relies on Function's operator==.
		 */
		PublicEvent& removeListener(const Function<R (Args...)> &listener) & {
			std::erase(_listeners, listener);
			return *this;
		}

		/*
		 * Add a new listener to the event.
		 */
		PublicEvent& operator+=(const Function<R (Args...)> &listener) & {
			addListener(listener);
			return *this;
		}

		/*
		 * Add a new listener to the event.
		 */
		PublicEvent& operator+=(Function<R (Args...)> &&listener) & {
			addListener(std::move(listener));
			return *this;
		}

		/*
		 * Remove every listener "e" that satisfy the condition: e == listener.
		 * This comparison relies on Function's operator==.
		 */
		PublicEvent& operator-=(const Function<R (Args...)> &listener) & {
			removeListener(listener);
			return *this;
		}
	};

	template <typename R, typename ...Args>
	class Event<R (Args...)> : public PublicEvent<R (Args...)> {
		using base = PublicEvent<R (Args...)>;
	public:
		/*
		 * Invoke the event, notifying all listeners with the given arguments.
		 * The order of invocation corresponds to the adding order of the listeners.
		 * If the return type of FunctionSignature is void, then the return type of this method is also void.
		 * Otherwise, this method returns a container which satisfies 
		 * the concept std::ranges::random_access_range, where each element is the return value of the 
		 * invocation of each listeners. The order of the elements are the same as the order of invocation of
		 * the listeners.
		 */
		auto invoke(Args ...args) const {
			if constexpr (std::same_as<R, void>) {
				for (const auto &listener : base::_listeners) {
					listener(std::forward<Args>(args)...);
				}
			} else {
				std::vector<R> result;
				result.reserve(base::_listeners.size());
				for (const auto &listener : base::_listeners) {
					result.push_back(listener(std::forward<Args>(args)...));
				}
				return result;
			}
		}

		/*
		 * Same as invoke().
		 */
		void operator()(Args ...args) const {
			return invoke(std::forward<Args>(args)...);
		}
	};
}
