#pragma once

#include <concepts>
#include <functional>
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

	/*
	 * Equals to pointer to an artificially created static function which invokes
	 * the given FuncPtr.
	 * Maps argument directly to FuncPtr for regular function pointer
	 * Maps the first argument as "this" binding to FuncPtr for member function pointer.
	 * 
	 * Useful for turning a member function pointer into a regular function pointer.
	 * Note that for regular function pointer, the value is NOT equal to the original function pointer.
	 */
	template <auto FuncPtr>
	constexpr auto func_ptr_invoker = detail::FuncPtrInvokerImpl<FuncPtr>::invoke;

	/*
	 * Represents a type erased callable type with the given function signature.
	 * FunctionSignature can be provided as following: Function<ReturnType (ArgumentTypes...)>.
	 * Function is equality comparable. See description on operator== for more info.
	 */
	template <typename FunctionSignature>
	class Function;

	template <typename R, typename ...Args>
	class Function<R (Args...)> {
		class Interface {
		public:
			virtual R operator()(Args...) const = 0;
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
		/*
		 * Constructs a function using the given functor as the underlying object.
		 */
		template <typename Fn> requires (!std::same_as<Function, std::remove_cvref_t<Fn>>)
		Function(Fn &&fn) : _ptr(new Impl(std::forward<Fn>(fn))) {}

		/*
		 * Constructs a Function binding the self pointer to the first argument of invoker.
		 * Two Functions constructed with this constructor are considered equal if 
		 * they refer to the same invoker and self.
		 */
		template <typename C>
		Function(R (*invoker)(C*, Args...), C *self) : _ptr(new MemberFunctionImpl(invoker, self)) {}

		/*
		 * Copy constructs this Function.
		 */
		Function(const Function &other) : _ptr(other._ptr->clone()) {}
		/*
		 * Move constructs this Function.
		 */
		Function(Function &&other) : _ptr(std::move(other._ptr)) {}

		Function& operator=(const Function &other) {
			if (other._ptr)
				_ptr = other._ptr->clone();
			else
				_ptr = nullptr;
			return *this;
		}

		Function& operator=(Function &&other) {
			_ptr = std::move(other._ptr);
			return *this;
		}

		/*
		 * Invoke the underlying functor with the given arguments.
		 * If this Function is empty as a result of move operation,
		 * then std::bad_function_call si thrown.
		 * One should never attempt to call a Function after it has been moved.
		 */
		R operator()(Args ...args) const {
			if (!_ptr) throw std::bad_function_call{};
			return (*_ptr)(std::forward<Args>(args)...);
		}

		/*
		 * Tests equality with another Function. 
		 * Always return false if either of the operand is an empty
		 * Function as a result of move operation, even if both Functions are empty.
		 *
		 * Otherwise, the two is considered equal if they refer to the same 
		 * exact Function object. 
		 *
		 * Or if both Functions have the same underlying 
		 * object type and the underlying object is equality comparable,
		 * then return the result of comparing the underlying object.
		 *
		 * Or if both Function are constructed using the invoker constructor,
		 * and both the invoker and self pointers has the same exact values 
		 * between two Functions.
		 *
		 * Copy constructed Functions may not be considered equal 
		 * if the underlying object does not support equality comparison, 
		 * or if the equality comparison results in not equal.
		 */
		bool operator==(const Function &other) const {
			if (!_ptr || !other._ptr) return false;
			return *_ptr == *other._ptr;
		}
	};

	template <typename C, typename R, typename ...Args>
	Function(R (*)(C *, Args...), C *) -> Function<R (Args...)>;
}
