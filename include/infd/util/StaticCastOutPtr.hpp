#pragma once

#include <concepts>

namespace infd::util {

	template <typename ResultT, typename OrigT>
	requires std::convertible_to<ResultT, OrigT> && std::default_initializable<ResultT>
	class StaticCastOutPtr {
		OrigT *_target;
		ResultT _result_holder;

	public:
		StaticCastOutPtr(OrigT *target, const ResultT &init = {}) noexcept(noexcept(ResultT{})) : 
			_target(target), _result_holder(init) {}

		StaticCastOutPtr(const StaticCastOutPtr &) = delete;
		StaticCastOutPtr(StaticCastOutPtr &&) = delete;

		StaticCastOutPtr& operator=(const StaticCastOutPtr &) = delete;
		StaticCastOutPtr& operator=(StaticCastOutPtr &&) = delete;

		operator ResultT*() noexcept { return &_result_holder; }

		~StaticCastOutPtr() {
			*_target = static_cast<OrigT>(_result_holder);
		}
	};

	template <typename ResultT, typename OrigT>
	StaticCastOutPtr<ResultT, OrigT> staticCastOutPtr(OrigT *target)
	noexcept(noexcept(StaticCastOutPtr<ResultT, OrigT>{target})) {
		return { target };
	}

} // namespace infd
