#include "AppWindows.h"

using namespace App;
using namespace std;
using namespace Gdiplus;

// https://docs.microsoft.com/en-us/cpp/error-uMsgs/tool-errors/linker-tools-error-lnk2005?view=vs-2019
// https://docs.microsoft.com/en-us/cpp/error-uMsgs/tool-errors/linker-tools-error-lnk2001?view=vs-2019

// https://cplusplus.com/reference/map/map/
map<HWND, set<HWND>> AppWindows::mAppWindows; // map of windows to their children
map<HWND, vector<BMPhdc>> AppWindows::mAppHdcs;
wstring AppWindows::search;

// http://cplusplus.com/doc/tutorial/classes/
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633498(v=vs.85)
BOOL CALLBACK AppWindows::getAppWindows(HWND hWnd, LPARAM lparam)
{

	// https://www.cplusplus.com/doc/tutorial/dynamic/
	wchar_t* pTitle = new wchar_t[0xff];
	GetWindowTextW(hWnd, pTitle, 0xff);
	wstring title = wstring(pTitle);
	//https://en.cppreference.com/w/cpp/string/basic_string/find
	if (title.find(search)+1)
	{

		HWND hwndParent=GetParent(hWnd);
		if (hwndParent)
		{
            mAppWindows[hwndParent].insert(hWnd);

		}
		// https://docs.microsoft.com/en-us/windows/win32/learnwin32/managing-application-state-
		// TODO: get children only for active
		EnumChildWindows(hWnd, getAppChildren, (LPARAM) hWnd);
	}
	return true;
}

BOOL CALLBACK AppWindows::getAppChildren(HWND hWnd, LPARAM lparam)
{
	AppWindows::mAppWindows[(HWND)lparam].insert(hWnd);
	return true;
}

void AppWindows::load()
{
	mAppWindows.clear();
	EnumWindows(getAppWindows, 0);
}

pair<int, int> AppWindows::getClientDimensions(HWND hwndApp)
{
	RECT rectAppClient;
	GetClientRect(hwndApp, &rectAppClient);
	int clientWidth = rectAppClient.right - rectAppClient.left;
	int clientHeight = rectAppClient.bottom - rectAppClient.top;
	// thanks to competitive programmers like Errichto who used make_pair
	return make_pair(clientWidth, clientHeight);
}

map<HWND, set<HWND>> AppWindows::getWindows()
{
	load();
	return mAppWindows;
}

BMPhdc AppWindows::getBMPhdc(HWND hwndApp)
{
	pair<int, int> clientDimensions = getClientDimensions(hwndApp);
	// http://cplusplus.com/reference/utility/pair/
	int clientWidth = clientDimensions.first;
	int clientHeight = clientDimensions.second;
	// https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
	// looking at the Captura source code may have inspired me to continue
	HDC hdcApp = GetDC(hwndApp);
	HDC hdc = CreateCompatibleDC(hdcApp);

	HBITMAP hbmApp = CreateCompatibleBitmap(hdcApp, clientWidth, clientHeight);
	SelectObject(hdc, hbmApp);
	BitBlt(hdc, 0, 0, clientWidth, clientHeight, hdcApp, 0, 0, SRCCOPY);
	ReleaseDC(hwndApp, hdcApp);
	DeleteObject(hbmApp);

	return BMPhdc{ hdc, clientWidth, clientHeight };
}

map<HWND, vector<BMPhdc>> AppWindows::getBMPhdc()
{
	mAppHdcs.clear();
	// http://cplusplus.com/reference/vector/vector/begin/
	for (map<HWND, set<HWND>>::iterator it = mAppWindows.begin(); it != mAppWindows.end(); it++)
	{
		HWND hwndApp = it->first;
		mAppHdcs[hwndApp].push_back(getBMPhdc(hwndApp));
		set<HWND> setAppChildren = it->second;
		for (set<HWND>::iterator childIt = setAppChildren.begin(); childIt != setAppChildren.end(); childIt++) mAppHdcs[hwndApp].push_back(getBMPhdc(*childIt));
	}
	return mAppHdcs;
}
