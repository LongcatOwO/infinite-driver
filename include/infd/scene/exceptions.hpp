// Created by Phuwasate Lutchanont

#pragma once

#include <stdexcept>
#include <string>

namespace infd::scene {

	class MissingSceneObjectException : public std::runtime_error {
	public:
		MissingSceneObjectException(const std::string& what_arg) : std::runtime_error(what_arg) {}
		MissingSceneObjectException(const char* what_arg) : std::runtime_error(what_arg) {}
	}; // class MissingSceneObjectException

	class MissingRootSceneObjectException : public MissingSceneObjectException {
	public:
		MissingRootSceneObjectException(const std::string& what_arg) : MissingSceneObjectException(what_arg) {}
		MissingRootSceneObjectException(const char* what_arg) : MissingSceneObjectException(what_arg) {}
	}; // class MissingRootSceneObjectException

	class MissingComponentException : public std::runtime_error {
	public:
		MissingComponentException(const std::string& what_arg) : std::runtime_error(what_arg) {}
		MissingComponentException(const char* what_arg) : std::runtime_error(what_arg) {}
	};

	class DuplicateComponentException : public std::runtime_error {
	public:
		DuplicateComponentException(const std::string& what_arg) : std::runtime_error(what_arg) {}
		DuplicateComponentException(const char* what_arg) : std::runtime_error(what_arg) {}
	};

} // namespace infd::scene
