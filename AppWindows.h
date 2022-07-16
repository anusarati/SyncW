#pragma once
// thanks to https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <vector>
#include <map>
#include <set>
#include <gdiplus.h>
#include <string>
#include <iterator>
#include <iostream>

// https://docs.microsoft.com/en-us/cpp/cpp/header-files-cpp?view=vs-2019

namespace App
{

	struct BMPhdc
	{
		HDC hdc;
		int cw;
		int ch;
	};

	class AppWindows
	{

		static std::map<HWND, std::set<HWND>> mAppWindows;
		static std::map<HWND, std::vector<BMPhdc>> mAppHdcs;

		static BOOL CALLBACK getAppWindows(HWND, LPARAM);
		static BOOL CALLBACK getAppChildren(HWND, LPARAM);


	public:
		static std::wstring search;
		static void load();
		static std::pair<int, int> getClientDimensions(HWND);
		static std::map<HWND, std::set<HWND>> getWindows();
// https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
		static BMPhdc getBMPhdc(HWND);
		static std::map<HWND, std::vector<BMPhdc>> getBMPhdc();

	};
}
