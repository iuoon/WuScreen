// WuScreen.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WuScreen.h"
#include "CMainWnd.h"

HHOOK mouse_Hook=NULL;
tagPOINT ps;
tagPOINT pe;
BOOL is_up = false;

BOOL LB_DOWN_STATUS = false;

void PrintDebug(const char* strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	//OutputDebugString(CA2W(strBuffer));
	OutputDebugString(strBuffer);
}

BOOL UninstallHook()
{

	BOOL jud = FALSE;
	if (mouse_Hook) {
		jud = UnhookWindowsHookEx(mouse_Hook);
		mouse_Hook = NULL;  //set NULL  
	}

	return jud;
}

void showRectInDesktop(RECT & rect)
{
	// 获得DC
	HDC hDC = ::GetDC(0);
	// 创建实线，宽度为1，红色的笔
	HPEN hPen = ::CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
	// 将笔选入DC
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	// 创建一个透明画刷
	HBRUSH hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	// 将画刷选入DC
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
	// 绘制矩形
	::Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	// 恢复绘图对象
	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
	// 删除绘图对象
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
	// 释放DC
	::ReleaseDC(0, hDC);
}

LRESULT CALLBACK LowLevelMouseProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT *pkbhs = (MSLLHOOKSTRUCT *)lParam;
	switch (nCode)
	{

	case HC_ACTION:
	{ 
	//	if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP) {
			//获取鼠标的位置，并进行必要的判断
			//鼠标右键按下
	//		PrintDebug("cliss==%s \n", "test");
	//	}
		if (LB_DOWN_STATUS && wParam == WM_MOUSEMOVE) {
			GetCursorPos(&pe);
			PrintDebug("x==%d  y==%d \n", pe.x, pe.y);
			//Rect := Rect(20, 30, 50, 90);
		}
		else {

		}
		
		//鼠标左键按下
		if (wParam == WM_LBUTTONDOWN) {
			LB_DOWN_STATUS = true;
			GetCursorPos(&ps);
			PrintDebug("x==%d  y==%d \n", ps.x, ps.y);
		}
		//鼠标左键松开
		if (wParam == WM_LBUTTONUP) {
			LB_DOWN_STATUS = false;
		}
		//鼠标右键按下
		if (wParam == WM_RBUTTONDOWN) {

		}
		//鼠标右键松开
		if (wParam == WM_RBUTTONUP) {

		}
	}
	default:   break;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL InstallKbHook(HINSTANCE hInstance)
{

	if (mouse_Hook)   UninstallHook();
	mouse_Hook = SetWindowsHookEx(WH_MOUSE_LL,
		(HOOKPROC)(LowLevelMouseProc), hInstance, NULL);

	return(mouse_Hook != NULL);
}





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	::CoInitialize(NULL);
	CPaintManagerUI::SetInstance(hInstance);
	InstallKbHook(hInstance);
	CMainWnd *pFrame = new CMainWnd(_T("duilib.xml"));
	pFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowModal();

	//窗口关掉之后杀掉录屏
	string cmd = "taskkill /f /t /im ffmpeg.exe";
	WinExec(cmd.c_str(), SW_HIDE);

	UninstallHook();
	delete pFrame;
	::CoUninitialize();
	return 0;
}


