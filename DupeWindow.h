#pragma once
#include "AppWindows.h"

// https://docs.microsoft.com/en-us/windows/win32/learnwin32/managing-application-state-
class DupeWindow
{
    //https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexw
    //https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowinfo
    static PWINDOWINFO pWi;
public:
    HWND hWnd;
    HWND hwndSource;
    DupeWindow();
    DupeWindow(HWND,HWND,HMENU,DWORD,DWORD,PCWSTR,int,int,int,int);
    void HandlePaint(HDC);
    void forget();
    void deploy();

    LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static WNDCLASS wc;
    static std::map<HWND, DupeWindow> mDupes; // source to dupe
    static std::map<HWND, DupeWindow> mDupeHWNDs; // dupe handle to dupe
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static int refreshInterval;
    static void prepare();
    static void clean();
    static void dupe();
};

