#include "CMainWindow.h"
#include "SafeHandle.h"
#include "CCommandLineHelper.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	do 
	{
		CCommandLineHelper::getInstance()->parse(pCmdLine);
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to initialize COM library.", L"Error", MB_OK);
			break;
		}

		const wchar_t* mutexName = L"Global\\UniqueInstanceMutex";

		SafeHandle hMutex = SafeHandle(CreateMutex(NULL, FALSE, mutexName));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND hProgman = FindWindow(L"Progman", 0);
			auto window = FindWindowEx(hProgman,NULL, NULL, kWindowName);

			COPYDATASTRUCT datas;
			datas.dwData = 0;
			datas.cbData = (wcslen(pCmdLine) + 1) * sizeof(wchar_t);
			datas.lpData = pCmdLine;
			::SendMessage(window, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&datas));
			break;
		}

		CMainWindow mainWnd;
		if (!mainWnd.Create(NULL, CWindow::rcDefault, kWindowName, WS_POPUP, WS_EX_TOOLWINDOW))
		{
			break;
		}
		mainWnd.ShowWindow(nCmdShow);
		mainWnd.UpdateWindow();
		MSG msg = { 0 };

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
		CoUninitialize();
	} while (false);
	return 0;
}