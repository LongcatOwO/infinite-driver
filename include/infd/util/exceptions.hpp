// Created by Phuwasate Lutchanont

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

	class ExternalLibraryException : public std::runtime_error {
	public:
		ExternalLibraryException(const std::string &what_arg) : std::runtime_error(what_arg) {}
		ExternalLibraryException(const char *what_arg) : std::runtime_error(what_arg) {}
	};

	class GLFWException : public ExternalLibraryException {
	public:
		GLFWException(const std::string &what_arg) : ExternalLibraryException(what_arg) {}
		GLFWException(const char *what_arg) : ExternalLibraryException(what_arg) {}
	};

	class GLEWException : public ExternalLibraryException {
	public:
		GLEWException(const std::string &what_arg) : ExternalLibraryException(what_arg) {}
		GLEWException(const char *what_arg) : ExternalLibraryException(what_arg) {}
	};

	class ImGuiException : public ExternalLibraryException {
	public:
		ImGuiException(const std::string &what_arg) : ExternalLibraryException(what_arg) {}
		ImGuiException(const char *what_arg) : ExternalLibraryException(what_arg) {}
	};
} // namespace infd::scene
