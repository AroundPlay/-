#pragma once

#include <atlbase.h>
#include <atlwin.h>

#include <thread>

#define WM_PROCESSCREATE WM_USER + 1

static wchar_t kWindowName[] = L"MiniElephantCat";

class CMainWindow: public ATL::CWindowImpl<CMainWindow>
{
public:
	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimeOut)
	END_MSG_MAP()

public:
		CMainWindow();
		~CMainWindow();

protected:
	void Player();

protected:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled);
	LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled);
	LRESULT OnTimeOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled);
private:
	std::thread playerThread_;
};

