#include "AppWindows.h"
#include "DupeWindow.h"
// https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-a-line-use
/* winemaker: Added -lGdiplus to the libraries */

using namespace std;
using namespace App;
using namespace Gdiplus;

// some based on
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
// or other documentation
/*The MIT License (MIT)
Copyright (c) Microsoft Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.*/

// https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	MSG                 msg;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	DupeWindow::prepare();
	do
	{
		wcin>>AppWindows::search;

		// http://cplusplus.com/reference/vector/vector/empty/
		// http://cplusplus.com/reference/vector/vector/front/
		if (!AppWindows::getWindows().empty()) DupeWindow::dupe();
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (AppWindows::search.length());
	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}
