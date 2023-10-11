#pragma once

#include <concepts>
#include <utility>


namespace infd::util {

	template <typename ResultT, typename TargetT, typename Converter>
	requires requires (ResultT&& result, TargetT& target, Converter&& converter) {
		{ target = std::forward<Converter>(converter)(std::move(result)) };
	}
	class ConvertingOutParam {
		Converter _converter;
		TargetT& _target;
		ResultT _result;

	public:
		template <typename Converter2, typename ResultT2>
		ConvertingOutParam(Converter2&& converter, TargetT& target, ResultT2&& result_init = ResultT{}) :
			_converter(std::forward<Converter2>(converter)),
			_target(target),
			_result(std::forward<ResultT2>(result_init)) {}

		~ConvertingOutParam() {
			_target = std::forward<Converter>(_converter)(std::move(_result));
		}

		ConvertingOutParam(const ConvertingOutParam&) = delete;
		ConvertingOutParam(ConvertingOutParam&&) = delete;
		ConvertingOutParam& operator=(const ConvertingOutParam&) = delete;
		ConvertingOutParam& operator=(ConvertingOutParam&&) = delete;

		operator ResultT&() noexcept {  return _result; }
	};

	template <typename ResultT, typename Converter, typename TargetT, typename ResultT2>
	auto convertingOutParam(Converter&& converter, TargetT& target, ResultT2&& result_init = ResultT{}) 
	-> ConvertingOutParam<ResultT, TargetT, std::remove_cvref_t<Converter>> {
		return { std::forward<Converter>(converter), target, std::forward<ResultT2>(result_init) };
	}

	template <typename ResultT, typename OrigT>
	requires std::convertible_to<ResultT, OrigT> && std::default_initializable<ResultT>
	class StaticCastOutPtr {
		ResultT _result_holder;
		OrigT *_target;

	public:
		StaticCastOutPtr(OrigT *target) noexcept(noexcept(ResultT{})) : _target(target) {}

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
