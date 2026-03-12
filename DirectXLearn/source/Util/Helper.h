#pragma once
#include <string>
#include <iostream>
#include <format>

namespace util {

	template<typename... Args>
	inline void Print(std::string_view str, Args&&... args) {
		std::cout << std::vformat(str, std::make_format_args(args...)) << std::endl;
	}

	template<typename... Args>
	inline void Print(std::wstring_view wstr, Args&&... args) {
		std::wcout << std::vformat(wstr, std::make_wformat_args(args...)) << std::endl;
	}

	inline void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr)) {
			throw std::exception();
		}
	}

	template<typename object>
	inline bool nullCheck(object obj) {
		if (!obj) return true;
		return false;
	}
}