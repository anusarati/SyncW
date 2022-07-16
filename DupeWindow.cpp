#include "DupeWindow.h"
#include "AppWindows.h"
using namespace std;
using namespace App;
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/managing-application-state-

map<HWND, DupeWindow> DupeWindow::mDupes;
map<HWND, DupeWindow> DupeWindow::mDupeHWNDs;

WNDCLASS DupeWindow::wc={ 0 };
int DupeWindow::refreshInterval=17;
//https://devdocs.io/cpp/language/new
PWINDOWINFO DupeWindow::pWi=new WINDOWINFO;
void DupeWindow::forget()
{
    DestroyWindow(hWnd);
    mDupes.erase(hwndSource);
    mDupeHWNDs.erase(hWnd);
    //delete this;
}

void DupeWindow::HandlePaint(HDC hdc)
{
    if (IsWindow(hwndSource))
    {
        BMPhdc bhdcApp = AppWindows::getBMPhdc(hwndSource);
        int cw=bhdcApp.cw, ch=bhdcApp.ch;
        SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,cw,ch,SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER);
        BitBlt(hdc, 0, 0, bhdcApp.cw, bhdcApp.ch, bhdcApp.hdc, 0, 0, SRCCOPY);
        DeleteDC(bhdcApp.hdc);
    }
    else
    {
        forget();
    }
}

void DupeWindow::deploy()
{
    // add state
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/using-timers
    SetTimer(hWnd, 1, refreshInterval, NULL);
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
}

DupeWindow::DupeWindow() : hwndSource(NULL) { }
// https://en.cppreference.com/w/cpp/language/default_arguments
// https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
#define windowClassName L"DupeWindow"
DupeWindow::DupeWindow(HWND hwndSource,
    HWND hWndParent = 0,
    HMENU hMenu = 0,
    DWORD dwStyle=WS_OVERLAPPED,
    DWORD dwExStyle = 0,
    PCWSTR lpWindowName=L"App Dupe",
    int x = CW_USEDEFAULT,
    int y = CW_USEDEFAULT,
    int nWidth = CW_USEDEFAULT,
    int nHeight = CW_USEDEFAULT
    ) : hwndSource(hwndSource)
{
    mDupes[hwndSource] = *this;
//    https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowinfo
    hWnd = CreateWindowExW(
            dwExStyle, windowClassName, lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
    );
    mDupeHWNDs[hWnd]=*this;
    deploy();
}

//https://stackoverflow.com/a/12573818
LRESULT CALLBACK DupeWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    try
    {
        return mDupeHWNDs.at(hWnd).WindowProc(uMsg,wParam,lParam);
    }
    catch (exception)
    {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }
}

LRESULT DupeWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return 0;
		// https://docs.microsoft.com/en-us/windows/win32/learnwin32/painting-the-window
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		HandlePaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	LRESULT lresult = DefWindowProc(hWnd, uMsg, wParam, lParam);
	return lresult;
}

void DupeWindow::clean()
{
    for (map<HWND, DupeWindow>::iterator it = mDupes.begin(); it != mDupes.end(); it++) DestroyWindow(it->second.hWnd);
    mDupes.clear();
    mDupeHWNDs.clear();
}

void DupeWindow::dupe()
{
    map<HWND, set<HWND>> mAppWindows = AppWindows::getWindows();
    if (mAppWindows.empty())
    {
        clean();
        return;
    }
    for (map<HWND, set<HWND>>::iterator it = mAppWindows.begin(); it != mAppWindows.end(); it++)
    {
        set<HWND> setAppWindows=it->second;
        for (map<HWND, DupeWindow>::iterator iti = mDupes.begin(); iti != mDupes.end(); iti++)
        {
            HWND source=iti->first;
            // http://cplusplus.com/reference/set/set/count/
            if (!setAppWindows.count(source))
            {
                iti->second.forget();
            }
        }
        HWND hwndParent=it->first;
        HWND hwndDupeParent=DupeWindow(hwndParent,0,0).hWnd;
        for (set<HWND>::iterator iti = setAppWindows.begin(); iti != setAppWindows.end(); iti++)
        {
        HWND hwndChild=*iti;
        //https://en.cppreference.com/w/cpp/container/map
            if (!mDupes.count(hwndChild))
            {
                DupeWindow(hwndChild,hwndDupeParent,0,WS_CHILD);
            }
            else
            {
                DupeWindow dupeWindow=mDupes[hwndChild];
                HWND hWnd=dupeWindow.hWnd;
                if (!GetParent(hWnd)) SetParent(hWnd,hwndDupeParent);
            }
        }
    }
}

void DupeWindow::prepare()
{
    wc.lpfnWndProc = DupeWindow::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = windowClassName;
    //https://docs.microsoft.com/en-us/windows/win32/menurc/using-cursors
    wc.hCursor=GetCursor();
    RegisterClass(&wc);
}
#undef windowClassName
