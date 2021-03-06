
// BuildMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildMFC.h"
#include "BuildMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBuildMFCDlg dialog

CBuildMFCDlg* CBuildMFCDlg::m_the_dialog = NULL;
CBuildMFCDlg::CBuildMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BUILDMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBuildMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBuildMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CBuildMFCDlg message handlers

BOOL CBuildMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_the_dialog = this;
	m_ui_width = 1024;
	m_ui_height = 768;
	m_is_mouse_down = FALSE;
		
	m_ui_bm_info.bmiHeader.biSize = sizeof(BITMAPINFO);
	m_ui_bm_info.bmiHeader.biWidth = m_ui_width;
	m_ui_bm_info.bmiHeader.biHeight = (0 - m_ui_height);//fix upside down. 
	m_ui_bm_info.bmiHeader.biBitCount = 32;
	m_ui_bm_info.bmiHeader.biCompression = BI_RGB;
	m_ui_bm_info.bmiHeader.biPlanes = 1;
	m_ui_bm_info.bmiHeader.biSizeImage = m_ui_width * m_ui_height * 2;

	AfxBeginThread(CBuildMFCDlg::ThreadHostMonitor, NULL);
	AfxBeginThread(CBuildMFCDlg::ThreadRefreshUI, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBuildMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBuildMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		renderUI();
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBuildMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CBuildMFCDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CBuildMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_is_mouse_down = TRUE;
	CPoint guilitePos = pointMFC2GuiLite(point);
	OUTMSGINFO msg;
	msg.dwMsgId = 0x4700;
	msg.dwParam1 = guilitePos.x;
	msg.dwParam2 = guilitePos.y;
	sendTouch2HostMonitor(&msg, sizeof(msg), 0);
}

void CBuildMFCDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_is_mouse_down = FALSE;
	CPoint guilitePos = pointMFC2GuiLite(point);
	OUTMSGINFO msg;
	msg.dwMsgId = 0x4600;
	msg.dwParam1 = guilitePos.x;
	msg.dwParam2 = guilitePos.y;
	sendTouch2HostMonitor(&msg, sizeof(msg), 0);
}

void CBuildMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_is_mouse_down == FALSE)
	{
		return;
	}

	CPoint guilitePos = pointMFC2GuiLite(point);
	OUTMSGINFO msg;
	msg.dwMsgId = 0x4700;
	msg.dwParam1 = guilitePos.x;
	msg.dwParam2 = guilitePos.y;
	sendTouch2HostMonitor(&msg, sizeof(msg), 0);
}

CPoint CBuildMFCDlg::pointMFC2GuiLite(CPoint point)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	return CPoint(m_ui_width * point.x / rcClient.Width(), m_ui_height * point.y / rcClient.Height());
}

void CBuildMFCDlg::renderUI()
{	
	CPaintDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, m_ui_width, m_ui_height);
	memDC.SelectObject(&bmp);
	::SetStretchBltMode(memDC, STRETCH_HALFTONE);

	CRect rcClient;
	GetClientRect(&rcClient);
	
	int width, height;
	void* data = getUiOfHostMonitor(0, &width, &height);
	::StretchDIBits(memDC.m_hDC, 0, 0, rcClient.Width(), rcClient.Height(),
								 0, 0, width, height,
								 data, &m_ui_bm_info, DIB_RGB_COLORS, SRCCOPY);
	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

	bmp.DeleteObject();
	memDC.DeleteDC();
}

UINT CBuildMFCDlg::ThreadHostMonitor(LPVOID pParam)
{
	return startHostMonitor(1, m_the_dialog->m_ui_width, m_the_dialog->m_ui_height, 0, 0, 0, 4);
}

UINT CBuildMFCDlg::ThreadRefreshUI(LPVOID pParam)
{
	while (true)
	{
		Sleep(30);
		m_the_dialog->Invalidate();
	}
}

///////////////// Implement functions for libs //////////////////////
void do_assert(const char* file, int line)
{	
	ASSERT(FALSE);
}

void log_out(const char* log)
{
	::OutputDebugStringA(log);
}
