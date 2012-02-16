// VersionUpdaterDlg.h : header file
#pragma once
#include <string>

// ============================================================================
//    CVersionUpdaterDlg dialog
// ============================================================================
class CVersionUpdaterDlg : public CDialog
{
private:
	void SaveConfig(void);
	void LoadConfig(void);

// ----------------------------------------------------------------------------
//    Construction
// ----------------------------------------------------------------------------
public:
	CVersionUpdaterDlg(CWnd *pParent = NULL);			// standard constructor

	// Dialog Data
	enum { IDD = IDD_VERSIONUPDATER_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support

// ----------------------------------------------------------------------------
//    Implementation
// ----------------------------------------------------------------------------
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnProcess();
	CString m_cstrPathOld;
	CString m_cstrPathNew;
	CString m_cstrPathBefore;
	CString m_cstrPathAfter;
};
