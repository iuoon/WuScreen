#include "stdafx.h"
#include "resource.h"
#include "CMainWnd.h"
#include <iostream> 
#include "WuScreen.h"
#include <string>
#include <sstream>
#include <shellapi.h>
using namespace std;

CMainWnd::CMainWnd(LPCTSTR pszXMLPath)
	: CXMLWnd(pszXMLPath)
{

}

CMainWnd::~CMainWnd() {

}


void CMainWnd::InitWindow()
{
   // SetIcon(IDR_MAINFRAME); // 设置任务栏图标
	CenterWindow();
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		//开始录屏
		if (msg.pSender->GetName() == _T("btStartLP"))
		{
			m_startLP = static_cast<CButtonUI*>(msg.pSender);
			if (m_startLP) {
				m_startLP->SetEnabled(false);
			}
			
			int  cx = GetSystemMetrics(SM_CXSCREEN);
			int  cy = GetSystemMetrics(SM_CYSCREEN);
			stringstream ss;
			ss << cx;
			string sx = ss.str();
			stringstream ss2;
			ss2 << cy;
			string sy = ss2.str();
			string cmd = "ffmpeg -f gdigrab -framerate 60 -offset_x 0 -offset_y 0 -video_size " + sx+ "x" + sy+ " -i desktop out.mpg";
			OutputDebugPrintf("cmd====>%s", cmd.c_str());
			WinExec(cmd.c_str(), SW_HIDE);//不会出现黑框，但是无论指令是否完成，都会进行之下代码 
			isLp = true;
						
		}
		//停止录屏
		if (msg.pSender->GetName() == _T("btStopLP")|| msg.pSender->GetName() == _T("closebtn"))
		{
			if (m_startLP) {
				m_startLP->SetEnabled(true);
			}
			m_stopLP = static_cast<CButtonUI*>(msg.pSender);
			string cmd = "taskkill /f /t /im ffmpeg.exe";
			WinExec(cmd.c_str(), SW_HIDE);
			isLp = false;
		//	TCHAR szCommandLine[] = TEXT("ffmpeg -i out.mpg -vcodec libx264 -acodec aac out.mp4");
		//	STARTUPINFO si = { sizeof(si) };
		//	PROCESS_INFORMATION  pi;
		//	si.wShowWindow = FALSE;
		//	si.dwFlags = STARTF_USESHOWWINDOW;
		//	BOOL ret = ::CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		//	WaitForSingleObject(pi.hProcess, INFINITE);//等待进程完成
		}
		if (msg.pSender->GetName() == _T("btPlayLP"))
		{
			if (isLp)
			{
				MessageBox(hWnd, "正在录屏中，停止录屏后才能播放", "提示", MB_OK);
				return;
			}
			char currPath[MAX_PATH];
			GetModuleFileName(NULL, currPath, sizeof(currPath));
			string path = currPath;
			path = path.substr(0, path.length() - 12) + "out.mpg";
			int retval = PathFileExists(path.c_str());
			if (retval!=1)
			{
				MessageBox(hWnd, "没有找到录屏文件", "提示", MB_OK);
				return;
			}
			else {
				int ret = (int)ShellExecute(NULL, _T("open"), path.c_str(), NULL, NULL, SW_NORMAL);
			}

		}
		if (msg.pSender->GetName() == _T("btClearLP"))
		{
			if (isLp)
			{
				MessageBox(hWnd, "正在录屏中", "提示", MB_OK);
				return;
			}
			char currPath[MAX_PATH];
			GetModuleFileName(NULL, currPath, sizeof(currPath));
			string path = currPath;
			path = path.substr(0,path.length()-12)+"out.mpg";	
			OutputDebugPrintf("======%s", path.c_str());
			remove(path.c_str());
		}

	}

	__super::Notify(msg);
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, _T("Wnd")) == 0)
	{
		CWndUI  *pUI = new CWndUI;
		hWnd = CreateWindow(_T("BUTTON"), _T("win32"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), NULL, NULL, NULL);
		pUI->Attach(hWnd);
		//// 上面用的是win32的按钮，下面这段用MFC的按钮
		//CButton *pBtn = new CButton;    // 记得释放内存
		//pBtn->Create(_T("MFC"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(0, 0, 0, 0), CWnd::FromHandle(m_PaintManager.GetPaintWindow()), 0);
		//pUI->Attach(*pBtn);            
		return pUI;
	}
	if (0 == _tcsicmp(pstrClassName, _T("Button"))) {
		return new CButtonUI();
	}			
	return NULL;
}



LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMainWnd::OutputDebugPrintf(const char* strOutputString, ...)
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

void CMainWnd::showRectInDesktop(RECT & rect)
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
	//画完后，恢复桌面原有样子（桌面重绘）
	::InvalidateRect(0, NULL, TRUE);
}

