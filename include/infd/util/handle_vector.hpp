#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>


namespace infd::util {

	template <typename T, typename Allocator = std::allocator<T>, typename HandlerAllocator = void>
	class handle_vector {
	public:
		using container 				= std::vector<T, Allocator>;
		using value_type 				= container::value_type;
		using allocator_type 			= container::allocator_type;
		using size_type 				= container::size_type;
		using difference_type 			= container::difference_type;
		using reference 				= container::reference;
		using const_reference 			= container::const_reference;
		using pointer 					= container::pointer;
		using const_pointer 			= container::const_pointer;
		using iterator 					= container::iterator;
		using const_iterator 			= container::const_iterator;
		using reverse_iterator 			= container::reverse_iterator;
		using const_reverse_iterator 	= container::const_reverse_iterator;

		class element_handler {
			bool _owning;
			handle_vector* _vec;
			size_type pos;

			element_handler(handle_vector& vec, size_type pos) noexcept : 
				_owning(true), _vec(&vec), pos(pos) 
			{
				_vec->handlers[pos] = this;
			}

			void take_ownership(element_handler& other) noexcept {
				if (!other._owning) {
					_owning = false;
					return;
				}

				_owning = true;
				other._owning = false;

				_vec = other._vec;
				pos = other.pos;
				_vec->_handlers[pos] = this;
			}

		public:
			element_handler(element_handler&& other) noexcept {
				take_ownership(other);
			}

			element_handler& operator=(element_handler&& other) noexcept {
				destroy_element();
				take_ownership(other);
			}

			~element_handler() noexcept {
				destroy_element();
			}

			element_handler(const element_handler&) = delete;
			element_handler& operator=(const element_handler&) = delete;

			void destroy_element() {
				if (!_owning) return;
				_vec->destroy_element(pos);
				_owning = false;
			}

			reference operator*() noexcept {
				return (*_vec)[pos];
			}

			const_reference operator*() const noexcept {
				return (*_vec)[pos];
			}

			pointer operator->() noexcept {
				return &(*_vec)[pos];
			}

			const_pointer operator->() const noexcept {
				return &(*_vec)[pos];
			}

			iterator it() noexcept {
				return _vec->begin() + pos;
			}

			const_iterator it() const noexcept {
				return _vec->begin() + pos;
			}

			const_iterator cit() const noexcept {
				return _vec->cbegin() + pos;
			}
		};

	private:
		container _container;
		std::vector<
			element_handler*, 
			std::conditional_t<std::is_same_v<HandlerAllocator, void>,
				std::allocator<element_handler*>,
				HandlerAllocator
			>
		> _handlers;

		void destroy_element(size_type pos) {
			if (pos != _container.size() - 1) {
				_container[pos] = std::move(_container.back());
				_handlers[pos] = _handlers.back();
				_handlers[pos]->pos = pos;
				_container.pop_back();
				_handlers.pop_back();
			}
		}

	public:
		[[nodiscard]] handle_vector(handle_vector&& other) : 
			_container(std::move(other._container)), _handlers(std::move(other._handlers)) 
		{
			for (element_handler *handler : _handlers)
				handler->_vec = this;
		}

		~handle_vector() = default;

		handle_vector(const handle_vector&) 			= delete;
		handle_vector& operator=(const handle_vector&) 	= delete;
		handle_vector& operator=(handle_vector&&) 		= delete;

		constexpr void assign(size_type count, const T &value) {
			_container.assign(count, value);
		}

		template <typename InputIt>
		constexpr void assign(InputIt first, InputIt last) {
			_container.assign(first, last);
		}

		constexpr void assign(std::initializer_list<T> ilist) {
			_container.assign(ilist);
		}

		[[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
			return _container.get_allocator();
		}

		[[nodiscard]] constexpr reference at(size_type pos) {
			return _container.at(pos);
		}

		[[nodiscard]] constexpr const_reference at(size_type pos) const {
			return _container.at(pos);
		}

		[[nodiscard]] constexpr reference operator[](size_type pos) {
			return _container[pos];
		}

		[[nodiscard]] constexpr const_reference operator[](size_type pos) const {
			return _container[pos];
		}

		[[nodiscard]] constexpr reference front() {
			return _container.front();
		}

		[[nodiscard]] constexpr const_reference front() const {
			return _container.front();
		}

		[[nodiscard]] constexpr reference back() {
			return _container.back();
		}

		[[nodiscard]] constexpr const_reference back() const {
			return _container.back();
		}

		[[nodiscard]] constexpr T* data() noexcept {
			return _container.data();
		}

		[[nodiscard]] constexpr const T* data() const noexcept {
			return _container.data();
		}

		[[nodiscard]] constexpr iterator begin() noexcept {
			return _container.begin();
		}

		[[nodiscard]] constexpr const_iterator begin() const noexcept {
			return _container.begin();
		}

		[[nodiscard]] constexpr const_iterator cbegin() const noexcept {
			return _container.cbegin();
		}

		[[nodiscard]] constexpr iterator end() noexcept {
			return _container.end();
		}

		[[nodiscard]] constexpr const_iterator end() const noexcept {
			return _container.end();
		}

		[[nodiscard]] constexpr const_iterator cend() const noexcept {
			return _container.cend();
		}

		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
			return _container.rbegin();
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
			return _container.rbegin();
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
			return _container.crbegin();
		}

		[[nodiscard]] constexpr reverse_iterator rend() noexcept {
			return _container.rend();
		}

		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
			return _container.rend();
		}

		[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
			return _container.crend();
		}
		
		[[nodiscard]] constexpr bool empty() const noexcept {
			return _container.empty();
		}

		[[nodiscard]] constexpr size_type size() const noexcept {
			return _container.size();
		}

		[[nodiscard]] constexpr size_type max_size() const noexcept {
			return _container.max_size();
		}

		constexpr void reserve(size_type new_cap) {
			_container.reserve(new_cap);
		}

		[[nodiscard]] constexpr size_type capacity() const noexcept {
			return _container.capacity();
		}

		constexpr void shrink_to_fit() {
			_container.shrink_to_fit();
		}

		constexpr element_handler push_back(const T& value) {
			_container.push_back(value);
			_handlers.push_back(nullptr);
			return element_handler{*this, _handlers.size() - 1};
		}

		constexpr element_handler push_back(T&& value) {
			_container.push_back(std::move(value));
			_handlers.push_back(nullptr);
			return element_handler{*this, _handlers.size() - 1};
		}

		template <typename... Args>
		constexpr element_handler emplace_back(Args&&... args) {
			_container.emplace_back(std::forward<Args>(args)...);
			_handlers.push_back(nullptr);
			return element_handler{*this, _handlers.size() - 1};
		}

		constexpr void swap(handle_vector& other) noexcept(
			noexcept(_container.swap(other._container))
		 && noexcept(_handlers.swap(other._handlers))
		) {
			_container.swap(other._container);
			_handlers.swap(other._handlers);
			for (element_handler *handler : _handlers)
				handler->_vec = this;

			for (element_handler *handler : other._handlers)
				handler->_vec = &other;
		}

		auto operator<=>(const handle_vector& other) const = default;

	}; // class handle_vector

	template <typename T, typename Allocator>
	void swap(handle_vector<T, Allocator>& lhs, handle_vector<T, Allocator>& rhs) 
	noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}

} // namespace infd::util
