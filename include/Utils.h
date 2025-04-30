#pragma once

#include <charconv>
#include <concepts>
#include <string_view>

namespace heimskr {
	template <std::integral I>
	I parseNumber(std::string_view view, int base = 10) {
		I out{};
		std::from_chars_result result = std::from_chars(view.begin(), view.end(), out, base);
		if (result.ec != std::errc{} || result.ptr - view.begin() != std::ssize(view)) {
			throw std::invalid_argument("Not an integer: \"" + std::string(view) + "\"");
		}
		return out;
	}
}
