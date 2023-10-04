#pragma once

#include <stdexcept>
#include <string>

namespace infd::util {

	/*
	 * Signifies that a function is called when the object is in an invalid state.
	 */
	class InvalidStateException : public std::logic_error {
	public:
		InvalidStateException(const std::string &what_arg) : std::logic_error(what_arg) {}
		InvalidStateException(const char *what_arg) : std::logic_error(what_arg) {}
	};

	class CyclicReferenceException : public std::logic_error {
	public:
		CyclicReferenceException(const std::string &what_arg) : std::logic_error(what_arg) {}
		CyclicReferenceException(const char *what_arg) : std::logic_error(what_arg) {}
	};

	class NullPointerException : public std::runtime_error {
	public:
		NullPointerException(const std::string &what_arg) : std::runtime_error(what_arg) {}
		NullPointerException(const char *what_arg) : std::runtime_error(what_arg) {}
	};
} // namespace infd::scene
