#include "SafeHandle.h"

void SafeHandle::close() noexcept
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}
