// Created by Phuwasate Lutchanont

#pragma once

#include <utility>

namespace infd::util {

	template <typename T>
	class OwnableProperty;

	template <typename T>
	class PropertyOwner {
		friend class OwnableProperty<T>;

		OwnableProperty<T> *_property = nullptr;

		// does not remove old ownership
		void takeOwnership(PropertyOwner& other) noexcept;
		void acquire(OwnableProperty<T>& property) noexcept;
	public:
		[[nodiscard]] PropertyOwner() = default;
		[[nodiscard]] PropertyOwner(PropertyOwner&& other) noexcept;
		PropertyOwner& operator=(PropertyOwner&& other) noexcept;
		~PropertyOwner() noexcept;

		PropertyOwner(const PropertyOwner&) = delete;
		PropertyOwner& operator=(const PropertyOwner&) = delete;

		[[nodiscard]] T* get() noexcept;
		[[nodiscard]] const T* get() const noexcept;

		template <typename T2>
		bool set(T2&& value);

		void release() noexcept;

	}; // class PropertyOwner

	template <typename T>
	class OwnableProperty {
		friend class PropertyOwner<T>;

		T _value;
		PropertyOwner<T> *_owner = nullptr;
	public:
		template <typename... Args>
		[[nodiscard]] OwnableProperty(Args&&... args) : _value(std::forward<Args>(args)...) {}
		~OwnableProperty() noexcept {
			if (!_owner) return;
			_owner->release();
		}

		OwnableProperty(const OwnableProperty&) = delete;
		OwnableProperty(OwnableProperty&&) = delete;
		OwnableProperty& operator=(const OwnableProperty&) = delete;
		OwnableProperty& operator=(OwnableProperty&&) = delete;

		template <typename T2>
		OwnableProperty& operator=(T2&& value) {
			if (_owner) return *this;
			_value = std::forward<T2>(value);
			return *this;
		}

		[[nodiscard]] bool hasController() const noexcept {
			return !!_owner;
		}

		bool setOwner(PropertyOwner<T> *owner) noexcept {
			if (_owner) return false;
			if (!owner)
				_owner = nullptr;
			else
				owner->acquire(*this);
			return true;
		}

		[[nodiscard]] operator const T&() const noexcept {
			return _value;
		}

		[[nodiscard]] const T& get() const noexcept {
			return _value;
		}

		[[nodiscard]] T* getMutable() noexcept {
			if (_owner) return nullptr;
			return &_value;
		}

		template <typename T2>
		bool set(T2&& value) {
			if (_owner) return false;
			_value = std::forward<T2>(value);
			return true;
		}

	}; // class OwnableProperty

	template <typename T>
	void PropertyOwner<T>::takeOwnership(PropertyOwner& other) noexcept {
		_property = other._property;
		other._property = nullptr;
		if (_property)
			_property->_owner = this;
	}

	template <typename T>
	void PropertyOwner<T>::acquire(OwnableProperty<T>& property) noexcept {
		_property = &property;
		_property->_owner = this;
	}

	template <typename T>
	PropertyOwner<T>::PropertyOwner(PropertyOwner&& other) noexcept {
		takeOwnership(other);
	}

	template <typename T>
	auto PropertyOwner<T>::operator=(PropertyOwner&& other) noexcept -> PropertyOwner& {
		release();
		takeOwnership(other);
	}

	template <typename T>
	PropertyOwner<T>::~PropertyOwner() noexcept {
		release();
	}

	template <typename T>
	T* PropertyOwner<T>::get() noexcept {
		if (!_property) return nullptr;
		return &_property->_value;
	}

	template <typename T>
	const T* PropertyOwner<T>::get() const noexcept {
		if (!_property) return nullptr;
		return &_property->_value;
	}

	template <typename T> template <typename T2>
	bool PropertyOwner<T>::set(T2&& value) {
		if (!_property) return false;
		_property->_value = std::forward<T2>(value);
		return true;
	}

	template <typename T>
	void PropertyOwner<T>::release() noexcept {
		if (!_property) return;
		_property->_owner = nullptr;
		_property = nullptr;
	}

} // namespace infd::util
