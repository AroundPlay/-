#pragma once

#include <cstdint>
#include <string>

namespace encode_helper {
	std::wstring Utf82Unicode(uint32_t code, const std::string& utf8String);
	std::string  Unicode2Utf8(uint32_t code, const std::wstring& unicodeString);
}