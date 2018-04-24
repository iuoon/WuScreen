#pragma once
#include "duilib.h"
#include "atlbase.h"
#include "atlstr.h"

class CMainWnd : public CXMLWnd
{
public:
	explicit CMainWnd(LPCTSTR pszXMLPath);
	~CMainWnd();

	virtual void InitWindow();
	virtual void Notify(TNotifyUI& msg);
	virtual CControlUI* CreateControl(LPCTSTR pstrClassName);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OutputDebugPrintf(const char* strOutputString, ...);
	void showRectInDesktop(RECT & rect);
	
private:
	int count = 0;
	HWND hWnd;
	CPaintManagerUI m_PaintManager;
	CButtonUI* m_startLP;
	CButtonUI* m_stopLP;
	BOOL isLp=false;
};

