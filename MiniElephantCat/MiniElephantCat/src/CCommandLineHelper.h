#pragma once

#include <map>
#include <string>
#include <ranges>
#include <vector>

class CCommandLineHelper
{
public:
	~CCommandLineHelper() = default;
	CCommandLineHelper(std::wstring_view cmdLine);
	bool hasOption(const std::wstring& option) const;

	std::wstring getOption(const std::wstring& option) const;

	static CCommandLineHelper* getInstance()
	{
		static CCommandLineHelper instance;
		return &instance;
	}

	static void addOption(const std::wstring& option, const std::wstring& value)
	{
		getInstance()->options_[option] = value;
	}

	static void removeOption(const std::wstring& option)
	{
		getInstance()->options_.erase(option);
	}

	static void clearOptions()
	{
		getInstance()->options_.clear();
	}

	void parse(std::wstring_view cmdLine);

private:
	std::vector<std::wstring> split(std::wstring_view str, wchar_t delimiter = ' ');

private:
	CCommandLineHelper(const CCommandLineHelper&) = delete;
	CCommandLineHelper& operator=(const CCommandLineHelper&) = delete;
	CCommandLineHelper() = default;

private:
	std::map<std::wstring, std::wstring> options_;
};

