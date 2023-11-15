// Created by Phuwasate Lutchanont

#pragma once

// std
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace infd::util {

	template <typename MemFnPtrT>
	class MemberFunc {};

	template <typename R, typename ...Args>
	class MemberFunc<R (*)(Args...)> {
		R (*_fn)(Args...);

	public:
		constexpr MemberFunc(R (*fn)(Args...)) noexcept : _fn(fn) {}

		constexpr R operator()(Args ...args) const {
			return _fn(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename R, typename ...Args>
	class MemberFunc<R (*)(Args...) noexcept> {
		R (*_fn)(Args...) noexcept;

	public:
		constexpr MemberFunc(R (*fn)(Args...) noexcept) noexcept : _fn(fn) {}

		constexpr R operator()(Args ...args) const noexcept {
			return _fn(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...)> {
		R (C::*_fn)(Args...);

	public:
		constexpr MemberFunc(R (C::*fn)(Args...)) noexcept : _fn(fn) {}

		constexpr R operator()(C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(C &&self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const> {
		R (C::*_fn)(Args...) const;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const) noexcept : _fn(fn) {}

		constexpr R operator()(const C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(const C &&self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile> {
		R (C::*_fn)(Args...) volatile;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(volatile C &&self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile> {
		R (C::*_fn)(Args...) const volatile;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(const volatile C &&self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) &> {
		R (C::*_fn)(Args...) &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) &) noexcept : _fn(fn) {}

		constexpr R operator()(C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const &> {
		R (C::*_fn)(Args...) const &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const &) noexcept : _fn(fn) {}

		constexpr R operator()(const C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile &> {
		R (C::*_fn)(Args...) volatile &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile &) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile &> {
		R (C::*_fn)(Args...) const volatile &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile &) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &self, Args ...args) const {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) &&> {
		R (C::*_fn)(Args...) &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) &&) noexcept : _fn(fn) {}

		constexpr R operator()(C &&self, Args ...args) const {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const &&> {
		R (C::*_fn)(Args...) const &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const &&) noexcept : _fn(fn) {}

		constexpr R operator()(const C &&self, Args ...args) const {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile &&> {
		R (C::*_fn)(Args...) volatile &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile &&) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &&self, Args ...args) const {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile &&> {
		R (C::*_fn)(Args...) const volatile &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile &&) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &&self, Args ...args) const {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) noexcept> {
		R (C::*_fn)(Args...);

	public:
		constexpr MemberFunc(R (C::*fn)(Args...)) noexcept : _fn(fn) {}

		constexpr R operator()(C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(C &&self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const noexcept> {
		R (C::*_fn)(Args...) const;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const) noexcept : _fn(fn) {}

		constexpr R operator()(const C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(const C &&self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile noexcept> {
		R (C::*_fn)(Args...) volatile;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(volatile C &&self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile noexcept> {
		R (C::*_fn)(Args...) const volatile;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr R operator()(const volatile C &&self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) & noexcept> {
		R (C::*_fn)(Args...) &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) &) noexcept : _fn(fn) {}

		constexpr R operator()(C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const & noexcept> {
		R (C::*_fn)(Args...) const &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const &) noexcept : _fn(fn) {}

		constexpr R operator()(const C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile & noexcept> {
		R (C::*_fn)(Args...) volatile &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile &) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile & noexcept> {
		R (C::*_fn)(Args...) const volatile &;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile &) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &self, Args ...args) const noexcept {
			return (self.*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) && noexcept> {
		R (C::*_fn)(Args...) &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) &&) noexcept : _fn(fn) {}

		constexpr R operator()(C &&self, Args ...args) const noexcept {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const && noexcept> {
		R (C::*_fn)(Args...) const &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const &&) noexcept : _fn(fn) {}

		constexpr R operator()(const C &&self, Args ...args) const noexcept {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) volatile && noexcept> {
		R (C::*_fn)(Args...) volatile &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) volatile &&) noexcept : _fn(fn) {}

		constexpr R operator()(volatile C &&self, Args ...args) const noexcept {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename C, typename R, typename ...Args>
	class MemberFunc<R (C::*)(Args...) const volatile && noexcept> {
		R (C::*_fn)(Args...) const volatile &&;

	public:
		constexpr MemberFunc(R (C::*fn)(Args...) const volatile &&) noexcept : _fn(fn) {}

		constexpr R operator()(const volatile C &&self, Args ...args) const noexcept {
			return (std::move(self).*_fn)(std::forward<Args>(args)...);
		}

		constexpr bool operator==(const MemberFunc &other) const noexcept = default;
	};

	template <typename MemFnPtrT>
	MemberFunc(MemFnPtrT) -> MemberFunc<MemFnPtrT>;


	template <typename MemFnPtrT, typename BindedSelfType>
	class BindedMemberFunc {
		MemberFunc<MemFnPtrT> _fn;
		BindedSelfType _self;

	public:
		constexpr BindedMemberFunc(MemberFunc<MemFnPtrT> fn, BindedSelfType &&self)
		noexcept(noexcept(BindedSelfType(std::forward<BindedSelfType>(self)))) : 
			_fn(fn), _self(std::forward<BindedSelfType>(self)) {}

		template <typename ...Args>
		constexpr decltype(auto) operator()(Args &&...args) 
		noexcept(noexcept(_fn(_self, std::forward<Args>(args)...))) {
			return _fn(_self, std::forward<Args>(args)...);
		}

		template <typename ...Args>
		constexpr decltype(auto) operator()(Args &&...args) const
		noexcept(noexcept(_fn(_self, std::forward<Args>(args)...))) {
			return _fn(_self, std::forward<Args>(args)...);
		}

		constexpr bool operator==(const BindedMemberFunc &other) const noexcept {
			return _fn == other._fn && &_self == &other._self;
		}
	};

	template <typename MemFnPtrT, typename BindedSelfType>
	class BindedMemberFunc<MemFnPtrT, BindedSelfType &&> {
		MemberFunc<MemFnPtrT> _fn;
		BindedSelfType &&_self;

	public:
		constexpr BindedMemberFunc(MemberFunc<MemFnPtrT> fn, BindedSelfType &&self) noexcept :
			_fn(fn), _self(std::move(self)) {}

		template <typename ...Args>
		constexpr decltype(auto) operator()(Args &&...args) 
		noexcept(noexcept(_fn(std::move(_self), std::forward<Args>(args)...))) {
			return _fn(std::move(_self), std::forward<Args>(args)...);
		}

		template <typename ...Args>
		constexpr decltype(auto) operator()(Args &&...args) const
		noexcept(noexcept(_fn(std::move(_self), std::forward<Args>(args)...))) {
			return _fn(std::move(_self), std::forward<Args>(args)...);
		}

		constexpr bool operator==(const BindedMemberFunc &other) const noexcept {
			return _fn == other._fn && &_self == &other._self;
		}
	};

	template <typename MemFnPtrT, typename BindedSelfType>
	BindedMemberFunc(MemFnPtrT, BindedSelfType &&) -> BindedMemberFunc<MemFnPtrT, BindedSelfType>;

	template <typename MemFnPtrT, typename BindedSelfType>
	BindedMemberFunc<MemFnPtrT, BindedSelfType &&> refBindedMemberFunc(MemFnPtrT fn, BindedSelfType &&self) noexcept {
		return { fn, std::forward<BindedSelfType>(self) };
	}

	
	template <typename FunctionSignature>
	class Function;

	template <typename R, typename ...Args>
	class Function<R (Args...)> {
		class Interface {
		public:
			virtual R operator()(Args ...) const = 0;
			virtual std::unique_ptr<Interface> clone() const = 0;
			virtual bool operator==(const Interface &other) const = 0;
			virtual ~Interface() {}
		};

		template <typename Fn>
		class Impl : public Interface {
			Fn _fn;

		public:
			Impl(const Fn &fn) : _fn(fn) {}
			Impl(Fn &&fn) : _fn(std::move(fn)) {}

			R operator()(Args ...args) const override {
				return _fn(std::forward<Args>(args)...);
			}

			std::unique_ptr<Interface> clone() const override {
				return std::make_unique<Impl>(*this);
			}

			bool operator==(const Interface &other) const override {
				if (const Impl *other_ptr = dynamic_cast<const Impl *>(&other); other_ptr) {
					if (this == other_ptr)
						return true;

					if constexpr (std::equality_comparable<Fn>)
						return _fn == other_ptr->_fn;
					else
						return false;
				}

				return false;
			}
		};

		template <typename Fn>
		Impl(Fn) -> Impl<Fn>;

		std::unique_ptr<Interface> _ptr;

	public:
		// Constructs a Function using the given functor as the underlying object.
		template <typename Fn> requires (!std::same_as<Function, std::remove_cvref_t<Fn>>)
		Function(Fn &&fn) : _ptr{new Impl{std::forward<Fn>(fn)}} {}

		// copy constructor
		Function(const Function &other) : _ptr{!!other._ptr ? other._ptr->clone() : nullptr} {}

		// move constructor
		Function(Function &&other) noexcept : _ptr{std::move(other._ptr)} {}

		// copy assignment
		Function& operator=(const Function &other) {
			_ptr = !!other._ptr ? other._ptr->clone() : nullptr;
			return *this;
		}

		Function& operator=(Function &&other) {
			_ptr = std::move(other._ptr);
			return *this;
		}

		/*
		 * Invoke the underlying functor with the given arguments
		 * If this function is empty as a result of move operation,
		 * the std::bad_function_call is thrown.
		 */
		R operator()(Args ...args) const {
			if (!_ptr) throw std::bad_function_call{};
			return (*_ptr)(std::forward<Args>(args)...);
		}

		/*
		 * Tests equality with another function
		 * Always return false if either of the operand is empty from move operations,
		 * even if both are empty.
		 *
		 * Otherwise, the two are considered equal if they refer to the same exact Function object.
		 *
		 * If both Functions have exactly the same underlying functor type that is equality comparable,
		 * then return the result of comparing the underlying object.
		 *
		 * Copy constructed Functions may not be considered equal if the underlying object does not 
		 * support equality comparison, or if the equality comparison results in non-equal.
		 */
		bool operator==(const Function &other) const {
			if (!_ptr || !other._ptr) return false;
			return *_ptr == *other._ptr;
		}
	};


} // namespace infd::util
