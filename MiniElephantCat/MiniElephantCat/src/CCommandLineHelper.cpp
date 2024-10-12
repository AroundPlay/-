#include "CCommandLineHelper.h"

#include <algorithm>
#include <vector>

CCommandLineHelper::CCommandLineHelper(std::wstring_view cmdLine)
{
	parse(cmdLine);
}

bool CCommandLineHelper::hasOption(const std::wstring& option) const
{
	return options_.find(option) != options_.end();
}

std::wstring CCommandLineHelper::getOption(const std::wstring& option) const
{
	if (options_.find(option) == options_.end())
	{
		return L"";
	}
	return options_.at(option);
}

void CCommandLineHelper::parse(std::wstring_view cmdLine)
{
	options_.clear();
	std::wstring optionName;
	std::wstring optionValue;

	std::vector<std::wstring> tokens = split(cmdLine);

	for(auto& token: tokens)
	{
		if (token.substr(0, 1) == L"/" || token.substr(0, 2) == L"--")
		{
			size_t equalPos = token.find_first_of(L'=');
			if (equalPos != std::wstring::npos)
			{

				optionName = token.substr(0, equalPos);

				optionValue = token.substr(equalPos + 1);
				options_[optionName] = optionValue;
			}
			else
			{
				optionName = token;
				options_[optionName] = L"";
			}
		}
	}
}

std::vector<std::wstring> CCommandLineHelper::split(std::wstring_view str, wchar_t delimiter)
{
	std::vector<std::wstring> tokens;
	bool inQuote = false;
	std::wstring token;
	int lastPos = 0;
	for(int curPos = 0; curPos < str.size(); curPos++)
	{
		if (str[curPos] == '"')
		{
			inQuote = !inQuote;
		}

		if(((str[curPos] == delimiter) || (curPos == str.size() - 1)) && !inQuote)
		{
			token.assign(str.substr(lastPos, curPos - lastPos));
			lastPos = curPos + 1;
			if (token.empty())
			{
				continue;
			}
			tokens.push_back(token);
		}
	}
	return tokens;
}
