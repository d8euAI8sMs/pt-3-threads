
// ThreadsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Threads.h"
#include "ThreadsDlg.h"
#include "afxdialogex.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThreadsDlg dialog

DWORD WINAPI Thread1Proc(LPVOID lpParameter)
{
    CThreadsDlg *dlg = (CThreadsDlg *)lpParameter;
    while (WaitForSingleObject(dlg->hEvent, INFINITE) != WAIT_OBJECT_0)
        ;
    dlg->Work1 = 0; // volatile write is ok
    while (true)
    {
        LONG w = dlg->Work1; // volatile read is ok
        dlg->Work1 = w + 1; // volatile write is ok
        if (w + 1 < 0)
        {
            dlg->Work1 = 0; // volatile write is ok
        }
        if (dlg->mSleep1) // volatile read is ok
        {
            Sleep(0);
        }
    }
}

DWORD WINAPI Thread2Proc(LPVOID lpParameter)
{
    CThreadsDlg *dlg = (CThreadsDlg *)lpParameter;
    while (WaitForSingleObject(dlg->hEvent, INFINITE) != WAIT_OBJECT_0)
        ;
    dlg->Work2 = 0; // volatile write is ok
    while (true)
    {
        LONG w = dlg->Work2; // volatile read is ok
        dlg->Work2 = w + 1; // volatile write is ok
        if (w + 1 < 0)
        {
            dlg->Work2 = 0; // volatile write is ok
        }
        if (dlg->mSleep2) // volatile read is ok
        {
            Sleep(0);
        }
    }
}


CThreadsDlg::CThreadsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CThreadsDlg::IDD, pParent)
    , mSleep1(FALSE)
    , mSleep2(FALSE)
    , hThread1(INVALID_HANDLE_VALUE)
    , hThread2(INVALID_HANDLE_VALUE)
    , bWorking(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    SetProcessAffinityMask(GetCurrentProcess(), 0x1);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

CThreadsDlg::~CThreadsDlg()
{

}

void CThreadsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRIORITY1, mPriorityCtrl1);
    DDX_Control(pDX, IDC_PRIORITY2, mPriorityCtrl2);
    DDX_Control(pDX, IDC_PERCENTS1, mInfoCtrl1);
    DDX_Control(pDX, IDC_PERCENTS2, mInfoCtrl2);
    DDX_Control(pDX, IDC_PROGRESS1, mProgress1);
    DDX_Control(pDX, IDC_PROGRESS2, mProgress2);
    DDX_Control(pDX, IDC_SLEEP1, mSleepCtrl1);
    DDX_Control(pDX, IDC_SLEEP2, mSleepCtrl2);
}

BEGIN_MESSAGE_MAP(CThreadsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CThreadsDlg::OnBnClickedButton1)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_SLEEP1, &CThreadsDlg::OnBnClickedSleep1)
    ON_BN_CLICKED(IDC_SLEEP2, &CThreadsDlg::OnBnClickedSleep2)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CThreadsDlg message handlers

BOOL CThreadsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    mProgress1.SetRange(0, 100);
    mProgress2.SetRange(0, 100);
    mPriorityCtrl1.SetRange(THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_HIGHEST, TRUE);
    mPriorityCtrl2.SetRange(THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_HIGHEST, TRUE);
    mPriorityCtrl1.SetPos(THREAD_PRIORITY_NORMAL);
    mPriorityCtrl2.SetPos(THREAD_PRIORITY_NORMAL);

    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!hEvent)
    {
        ShowErrorMessage(_T("Cannot create event"), GetLastError());
        DestroyWindow();
    }

    SetTimer(123456, 100, NULL);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CThreadsDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CThreadsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CThreadsDlg::OnBnClickedButton1()
{
    if (!bWorking)
    {
        bWorking = TRUE;
        hThread1 = CreateThread(NULL, 0, &Thread1Proc, this, 0, NULL);
        if (!hThread1)
        {
            ShowErrorMessage(_T("Cannot create thread1"), GetLastError());
            DestroyWindow();
        }
        hThread2 = CreateThread(NULL, 0, &Thread2Proc, this, 0, NULL);
        if (!hThread2)
        {
            ShowErrorMessage(_T("Cannot create thread2"), GetLastError());
            DestroyWindow();
        }
        if (!SetThreadPriority(hThread1, mPriorityCtrl1.GetPos()))
        {
            ShowErrorMessage(_T("Cannot set thread1 priority"), GetLastError());
        }
        if (!SetThreadPriority(hThread2, mPriorityCtrl2.GetPos()))
        {
            ShowErrorMessage(_T("Cannot set thread2 priority"), GetLastError());
        }
        SetEvent(hEvent);
    }
    else
    {
        bWorking = FALSE;
        TerminateThread(hThread1, 0);
        TerminateThread(hThread2, 0);
        WaitForSingleObject(hThread1, INFINITE);
        WaitForSingleObject(hThread2, INFINITE);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        hThread1 = INVALID_HANDLE_VALUE;
        hThread2 = INVALID_HANDLE_VALUE;
        ResetEvent(hEvent);
    }
}



void CThreadsDlg::ShowErrorMessage(LPCTSTR pszMessage, DWORD dwErrorCode)
{
    LPTSTR messageBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);
    CString str; str.Format(_T("%s\r\n(%d) %s"), pszMessage, dwErrorCode, messageBuffer);
    AfxMessageBox(str);
	LocalFree(messageBuffer);
}

void CThreadsDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (!bWorking) return;

    LONG w1 = Work1; // volatile read is ok
    LONG w2 = Work2; // volatile read is ok

    LONG diff1 = w1 - TLWork1;
    LONG diff2 = w2 - TLWork2;

    TLWork1 = w1;
    TLWork2 = w2;

    if (diff1 < 0) diff1 = 0;
    if (diff2 < 0) diff2 = 0;

    float summary = (float)((ULONG)diff1 + (ULONG)diff2);

    float p1 = (diff1 / summary) * 100;
    float p2 = (diff2 / summary) * 100;

    CString str;

    str.Format(_T("%.1f%%"), p1);
    mInfoCtrl1.SetWindowText(str);
    str.Format(_T("%.1f%%"), p2);
    mInfoCtrl2.SetWindowText(str);

    mProgress1.SetPos((int)p1);
    mProgress2.SetPos((int)p2);

    CDialogEx::OnTimer(nIDEvent);
}


BOOL CThreadsDlg::DestroyWindow()
{
    if (bWorking)
    {
        bWorking = FALSE;
        TerminateThread(hThread1, 0);
        TerminateThread(hThread2, 0);
        WaitForSingleObject(hThread1, INFINITE);
        WaitForSingleObject(hThread2, INFINITE);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
    }
    CloseHandle(hEvent);

    return CDialogEx::DestroyWindow();
}


void CThreadsDlg::OnBnClickedSleep1()
{
    mSleep1 = mSleepCtrl1.GetCheck(); // volatile write is ok
}


void CThreadsDlg::OnBnClickedSleep2()
{
    mSleep2 = mSleepCtrl2.GetCheck(); // volatile write is ok
}


void CThreadsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (!bWorking) return;
    if ((CSliderCtrl *)pScrollBar == &mPriorityCtrl1)
    {
        int idfgfdg = mPriorityCtrl1.GetPos();
        if (!SetThreadPriority(hThread1, mPriorityCtrl1.GetPos()))
        {
            ShowErrorMessage(_T("Cannot set thread1 priority"), GetLastError());
        }
    }
    else if ((CSliderCtrl *)pScrollBar == &mPriorityCtrl2)
    {
        int idfgfdg = mPriorityCtrl2.GetPos();
        if (!SetThreadPriority(hThread2, mPriorityCtrl2.GetPos()))
        {
            ShowErrorMessage(_T("Cannot set thread2 priority"), GetLastError());
        }
    }
    
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
