
// ThreadsDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CThreadsDlg dialog
class CThreadsDlg : public CDialogEx
{
// Construction
public:
	CThreadsDlg(CWnd* pParent = NULL);	// standard constructor
    ~CThreadsDlg();

// Dialog Data
	enum { IDD = IDD_THREADS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CSliderCtrl mPriorityCtrl1;
    CSliderCtrl mPriorityCtrl2;
    CStatic mInfoCtrl1;
    CStatic mInfoCtrl2;
    volatile BOOL mSleep1;
    volatile BOOL mSleep2;
    CProgressCtrl mProgress1;
    CProgressCtrl mProgress2;
    afx_msg void OnBnClickedButton1();
    HANDLE hThread1;
    HANDLE hThread2;
    volatile LONG Work1;
    volatile LONG Work2;
    LONG TLWork1;
    LONG TLWork2;
    BOOL bWorking;
    HANDLE hEvent;
    void ShowErrorMessage(LPCTSTR pszMessage, DWORD dwErrorCode);
    CButton mSleepCtrl1;
    CButton mSleepCtrl2;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedSleep1();
    afx_msg void OnBnClickedSleep2();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
