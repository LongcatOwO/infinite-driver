#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>


namespace infd::util {

	namespace detail {
		template <auto FuncPtr>
		class FuncPtrInvokerImpl {};

		template <typename R, typename ...Args, R (*FuncPtr)(Args...)>
		class FuncPtrInvokerImpl<FuncPtr> {
		public:
			static R invoke(Args ...args) {
				return FuncPtr(std::forward<Args>(args)...);
			}
		};

		template <typename C, typename R, typename ...Args, R (C::*FuncPtr)(Args...)>
		class FuncPtrInvokerImpl<FuncPtr> {
		public:
			static R invoke(C *self, Args ...args) {
				return (self->*FuncPtr)(std::forward<Args>(args)...);
			}
		};

		template <typename C, typename R, typename ...Args, R (C::*FuncPtr)(Args...) const>
		class FuncPtrInvokerImpl<FuncPtr> {
		public:
			static R invoke(const C *self, Args ...args) {
				return (self->*FuncPtr)(std::forward<Args>(args)...);
			}
		};
	}

	template <auto FuncPtr>
	constexpr auto func_ptr_invoker = detail::FuncPtrInvokerImpl<FuncPtr>::invoke;

	template <typename FunctionSignature>
	class Function;

	template <typename R, typename ...Args>
	class Function<R (Args...)> {
		class Interface {
		public:
			virtual R operator()(Args...) const = 0;
			virtual std::unique_ptr<Interface> clone() const = 0;
			virtual std::unique_ptr<Interface> moveClone() = 0;
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

			std::unique_ptr<Interface> moveClone() override {
				return std::make_unique<Impl>(std::move(*this));
			}

			bool operator==(const Interface &other) const override {
				if (this == &other) { return true; }
				
				if constexpr (std::equality_comparable<Fn>) {
					if (auto ptr = dynamic_cast<const Impl *>(&other); ptr) {
						return _fn == ptr->_fn;
					}				
					return false;
				} else {
					return reinterpret_cast<const void *>(this) == reinterpret_cast<const void *>(&other);
				}
			}
		};

		class MemberFunctionInterface : public Interface {
		public:
			virtual const void *getSelf() const noexcept = 0;
			virtual const void *getFunctionPointer() const noexcept = 0;
		};

		template <typename C>
		class MemberFunctionImpl : public MemberFunctionInterface {
			using function_pointer = R (*)(C *, Args...);
			C *_self;
			function_pointer _fn;

		public:
			MemberFunctionImpl(function_pointer fn, C *self) : _self(self), _fn(fn) {}

			R operator()(Args ...args) const override {
				return _fn(_self, std::forward<Args>(args)...);
			}

			std::unique_ptr<Interface> clone() const override {
				return std::make_unique<MemberFunctionImpl>(*this);
			}

			std::unique_ptr<Interface> moveClone() override {
				return std::make_unique<MemberFunctionImpl>(std::move(*this));
			}

			const void *getSelf() const noexcept override { 
				return reinterpret_cast<const void *>(_self); 
			}
			const void *getFunctionPointer() const noexcept override { 
				return reinterpret_cast<const void *>(_fn);
			}

			bool operator==(const Interface &other) const override {
				if (this == &other) return true;

				if (auto ptr = dynamic_cast<const MemberFunctionInterface *>(&other); ptr) {
					return getSelf() == ptr->getSelf() && getFunctionPointer() == ptr->getFunctionPointer();
				}
				return false;
			}
		};

		template <typename C>
		class MemberFunctionImpl<const C> : public MemberFunctionInterface {
			using function_pointer = R (*)(const C *, Args...);
			const C *_self;
			function_pointer _fn;

		public:
			MemberFunctionImpl(function_pointer fn, const C *self) : _self(self), _fn(fn) {}

			R operator()(Args ...args) const override {
				return _fn(_self, std::forward<Args>(args)...);
			}

			std::unique_ptr<Interface> clone() const override {
				return std::make_unique<MemberFunctionImpl>(*this);
			}

			std::unique_ptr<Interface> moveClone() override {
				return std::make_unique<MemberFunctionImpl>(std::move(*this));
			}

			const void *getSelf() const noexcept override {
				return reinterpret_cast<const void *>(_self);
			}
			const void *getFunctionPointer() const noexcept override { 
				return reinterpret_cast<const void *>(_fn);
			}

			bool operator==(const Interface &other) const override {
				if (this == &other) return true;

				if (auto ptr = dynamic_cast<const MemberFunctionInterface *>(&other); ptr) {
					return getSelf() == ptr->getSelf() && getFunctionPointer() == ptr->getFunctionPointer();
				}
				return false;
			}
		};

		template <typename C>
		MemberFunctionImpl(R (*)(C*, Args...), C*) -> MemberFunctionImpl<C>;


		std::unique_ptr<Interface> _ptr;

		template <typename Fn>
		Impl(Fn fn) -> Impl<Fn>;

	public:
		template <typename Fn> requires (!std::same_as<Function, std::remove_cvref_t<Fn>>)
		Function(Fn &&fn) : _ptr(new Impl(std::forward<Fn>(fn))) {}

		template <typename C>
		Function(R (*invoker)(C*, Args...), C *self) : _ptr(new MemberFunctionImpl(invoker, self)) {}

		Function(const Function &other) : _ptr(other._ptr->clone()) {}
		Function(Function &&other) : _ptr(other._ptr->moveClone()) {}

		Function& operator=(const Function &other) {
			_ptr = other._ptr->clone();
			return *this;
		}

		Function& operator=(Function &&other) {
			_ptr = other._ptr->moveClone();
			return *this;
		}

		R operator()(Args ...args) const {
			return (*_ptr)(std::forward<Args>(args)...);
		}

		bool operator==(const Function &other) const {
			return *_ptr == *other._ptr;
		}
	};

	template <typename C, typename R, typename ...Args>
	Function(R (*)(C *, Args...), C *) -> Function<R (Args...)>;
}
