#include <minigui/stdio.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
static int TestWinProc(HWND hWnd, int message, WPARAM
		wParam, LPARAM lParam)
{
	HDC hdc;
	switch (message)
	{
		case MSG_PAINT:
			hdc = BeginPaint (hWnd);
			TextOut(hdc,100,100,"hello\n!");
			EndPaint(hWnd,hdc);
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			return 0;
	}
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}
void InitCreateInfo(MAINWINCREATE * pCreateInfo)
{
	pCreateInfo->dwstyle = WS_CAPTION|WS_VISIBLE;
	pCreateInfo->dwExstyle = WS_EX_NONE;
	pCreateInfo->spCaption = "Hello world!";
	pCreateInfo->hMenu = 0;
	pCreateInfo->hCursor = GetSystemCursor(0);
	pCreateInfo->hIcon = 0;
	pCreateInfo->MainWindowProc = TestWinProc;
	pCreateInfo->lx = 0;
	pCreateInfo->ty = 0;
	pCreateInfo->rx = 319;
	pCreateInfo->by = 239;
	pCreateInfo->iBkColor = PIXEL_lightwhite;
	pCreateInfo->dwAddData = 0;
	pCreateInfo->hHosting = HWND_DESKTOP;
}
int MiniGUIMain (int argc, const char* argv[])
{
	MSG Msg;
	HWND hMainWnd;
	MAINWINCREATE CreateInfo;
	InitCreateInfo(&CreateInfo);
	hMainWnd = CreateMainWindow (&CreateInfo);
	printf ("The main window created.\n");
	if (hMainWnd == HWND_INVALID)
		return -1;
	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	printf ("The main window showed.\n");
	while (GetMessage(&Msg, hMainWnd)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	MainWindowThreadCleanup (hMainWnd);
	return 0;
}
