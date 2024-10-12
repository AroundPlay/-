#pragma once
#include <windows.h>

#include <algorithm>

class SafeHandle
{
public:

	SafeHandle()
		: m_handle(nullptr)
	{}

	explicit SafeHandle(HANDLE handle)
		: m_handle(handle)
	{}

	SafeHandle(const SafeHandle& other) = delete;

	SafeHandle(SafeHandle&& other) noexcept
		: m_handle(other.m_handle)
	{
		other.m_handle = nullptr;
	}

	~SafeHandle()
	{
		close();
	}

	SafeHandle& operator=(SafeHandle handle) noexcept
	{
		swap(handle);
		return *this;
	}

	constexpr HANDLE handle() const { return m_handle; }

	/// Calls CloseHandle() and sets the handle to NULL.
	void close() noexcept;

	void swap(SafeHandle& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
	}

	bool operator==(const SafeHandle& other) const
	{
		return m_handle == other.m_handle;
	}

	bool operator!=(const SafeHandle& other) const
	{
		return m_handle != other.m_handle;
	}

	bool operator==(HANDLE handle) const
	{
		return m_handle == handle;
	}

	bool operator!=(HANDLE handle) const
	{
		return m_handle != handle;
	}

private:

	HANDLE m_handle;
};

