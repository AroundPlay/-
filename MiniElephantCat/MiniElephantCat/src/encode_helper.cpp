#include "encode_helper.h"
#include <windows.h>
#include <vector>
namespace encode_helper {
	std::wstring Utf82Unicode(uint32_t code, const std::string& utf8String) {
		std::wstring result;
		do
		{
			int widesize = ::MultiByteToWideChar(code, 0, utf8String.c_str(), -1, NULL, 0);
			if (widesize == ERROR_NO_UNICODE_TRANSLATION)
			{
				break;
			}
			if (widesize == 0)
			{
				throw std::exception("Error in conversion.");
			}
			std::vector<wchar_t> resultstring(widesize);
			int convresult = ::MultiByteToWideChar(code, 0, utf8String.c_str(), -1, resultstring.data(), widesize);
			if (convresult != widesize)
			{
				break;
			}
			result = resultstring.data();
		} while (false);
		return result;
	}


	std::string Unicode2Utf8(uint32_t code, const std::wstring& unicodeString) {
		std::string result;
		do 
		{
			int dessize = ::WideCharToMultiByte(code, 0, unicodeString.c_str(), -1, NULL, 0, NULL, NULL);
			if (dessize == ERROR_NO_UNICODE_TRANSLATION)
			{
				break;
			}
			if (dessize == 0)
			{
				break;
			}
			std::vector<char> resultstring(dessize);
			int convresult = ::WideCharToMultiByte(code, 0, unicodeString.c_str(), -1, resultstring.data(), dessize, NULL, NULL);
			if (convresult != dessize)
			{
				break;
			}
			result = resultstring.data();
		} while (false);
		return result;
	}
}