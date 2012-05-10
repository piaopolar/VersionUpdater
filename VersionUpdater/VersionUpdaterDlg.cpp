// VersionUpdaterDlg.cpp : implementation file
#include "stdafx.h"

#include <algorithm>

#include "BaseCode/BaseFunc.h"
#include "UpdateMgr.h"

#include "VersionUpdater.h"
#include "VersionUpdaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================================
//    CAboutDlg dialog used for App About
// ============================================================================
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support

// ----------------------------------------------------------------------------
//    Implementation
// ----------------------------------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP()
};

// ============================================================================
// ==============================================================================

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

// ============================================================================
// ==============================================================================
void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ============================================================================
//    CVersionUpdaterDlg dialog
// ============================================================================
CVersionUpdaterDlg::CVersionUpdaterDlg(CWnd *pParent /* NULL */ ) : CDialog(CVersionUpdaterDlg::IDD, pParent), m_cstrPathOld(_T("")), m_cstrPathNew(_T("")), m_cstrPathBefore(_T("")), m_cstrPathAfter(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OLD, m_cstrPathOld);
	DDX_Text(pDX, IDC_EDIT_NEW, m_cstrPathNew);
	DDX_Text(pDX, IDC_EDIT_BEFORE, m_cstrPathBefore);
	DDX_Text(pDX, IDC_EDIT_AFTER, m_cstrPathAfter);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
}

BEGIN_MESSAGE_MAP(CVersionUpdaterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_PROCESS, &CVersionUpdaterDlg::OnBnClickedBtnProcess)
	ON_BN_CLICKED(IDC_RADIO_MODE_CMP, &CVersionUpdaterDlg::OnBnClickedRadioModeCmp)
	ON_BN_CLICKED(IDC_RADIO_MODE_ADD, &CVersionUpdaterDlg::OnBnClickedRadioModeAdd)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CLEARLOG, &CVersionUpdaterDlg::OnBnClickedBtnClearlog)
END_MESSAGE_MAP()

// ============================================================================
//    CVersionUpdaterDlg message handlers
// ============================================================================
BOOL CVersionUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu. IDM_ABOUTBOX must be in the
	// system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pSysMenu != NULL) {

		//~~~~~~~~~~~~~~~~~
		CString strAboutMenu;
		//~~~~~~~~~~~~~~~~~

		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialo
	SetIcon(m_hIcon, TRUE);		// Set big icon
	SetIcon(m_hIcon, FALSE);	// Set small icon

	LoadConfig();

	SetLogEdit(&m_edtLog);
	((CButton *)GetDlgItem(IDC_RADIO_MODE_CMP))->SetCheck(TRUE);

	// TODO: Add extra initialization here
	return TRUE;				// return TRUE unless you set the focus to a control
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {

		//~~~~~~~~~~~~~~~
		CAboutDlg dlgAbout;
		//~~~~~~~~~~~~~~~

		dlgAbout.DoModal();
	} else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ============================================================================
//    If you add a minimize button to your dialog, you will need the code below
//    to draw the icon. For MFC applications using the document/view model, this
//    is automatically done for you by the framework.
// ============================================================================
void CVersionUpdaterDlg::OnPaint()
{
	if (IsIconic()) {

		//~~~~~~~~~~~~~~
		CPaintDC dc(this);	// device context for painting
		//~~~~~~~~~~~~~~

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM> (dc.GetSafeHdc()), 0);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GetClientRect(&rect);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// ============================================================================
//    The system calls this function to obtain the cursor to display while the
//    user drags the minimized window.
// ============================================================================
HCURSOR CVersionUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnBnClickedBtnProcess()
{
	this->SaveConfig();

	CUpdateMgr::GetInstance().SetEnvPath(m_cstrPathOld, m_cstrPathNew, m_cstrPathBefore, m_cstrPathAfter);
	CUpdateMgr::GetInstance().SetMode(this->GetInputMode());

	std::vector<FILE_TYPE_INFO>::const_iterator it(m_vecFileType.begin());
	for (; it != m_vecFileType.end(); ++it) {
		if (it->m_pChk->GetCheck()) {
			CUpdateMgr::GetInstance().UpdateFile(it->m_strFile.c_str(), it->m_nType);
		}
	}

	LogInfoIn("Process Finish");
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::LoadConfig(void)
{
	GetPrivateProfileString("Path", "Old", "", m_cstrPathOld.GetBuffer(MAX_STRING), MAX_STRING, CONFIG_INI);
	GetPrivateProfileString("Path", "New", "", m_cstrPathNew.GetBuffer(MAX_STRING), MAX_STRING, CONFIG_INI);
	GetPrivateProfileString("Path", "Before", "", m_cstrPathBefore.GetBuffer(MAX_STRING), MAX_STRING, CONFIG_INI);
	GetPrivateProfileString("Path", "After", "", m_cstrPathAfter.GetBuffer(MAX_STRING), MAX_STRING, CONFIG_INI);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nAmount = GetPrivateProfileInt("Type", "Amount", 0, CONFIG_INI);
	int nX = 66;
	int nY = 210;
	int nW = 150;
	int nH = 25;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	for (int i = 1; i <= nAmount; ++i) {

		//~~~~~~~~~~~~~~~~~~~~
		char szKey[MAX_STRING];
		char szData[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~

		_snprintf(szKey, sizeof(szKey), "File%d", i);
		GetPrivateProfileString("Type", szKey, "", szData, sizeof(szData), CONFIG_INI);

		//~~~~~~~~~~~~~~~~~~~~
		char szName[MAX_STRING];
		int nType;
		int nDefaultCheck;
		//~~~~~~~~~~~~~~~~~~~~

		if (3 == sscanf(szData, "%[^|]|%d|%d", szName, &nType, &nDefaultCheck)) {

			//~~~~~~~~~~~~~~~~
			FILE_TYPE_INFO info;
			//~~~~~~~~~~~~~~~~

			std::replace(szName, szName + sizeof(szName), '/', '\\');
			info.m_strFile = szName;
			info.m_nType = nType;
			info.m_pChk = new CButton;

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			CRect rect(nX + m_vecFileType.size() % 3 * nW, nY + m_vecFileType.size() / 3 * nH,
					   nX + m_vecFileType.size() % 3 * nW + nW, nY + m_vecFileType.size() / 3 * nH + nH);
			BOOL bRet = info.m_pChk->Create(szName, BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD, rect, this,
											2000 + m_vecFileType.size());
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			info.m_pChk->SetCheck(nDefaultCheck);
			info.m_pChk->ShowWindow(SW_SHOW);

			m_vecFileType.push_back(info);
		}
	}

	this->UpdateData(FALSE);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::SaveConfig(void)
{
	this->UpdateData(TRUE);

	WritePrivateProfileString("Path", "Old", m_cstrPathOld, CONFIG_INI);
	WritePrivateProfileString("Path", "New", m_cstrPathNew, CONFIG_INI);
	WritePrivateProfileString("Path", "Before", m_cstrPathBefore, CONFIG_INI);
	WritePrivateProfileString("Path", "After", m_cstrPathAfter, CONFIG_INI);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnBnClickedRadioModeCmp()
{
	GetDlgItem(IDC_EDIT_OLD)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_NEW)->EnableWindow(TRUE);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnBnClickedRadioModeAdd()
{
	GetDlgItem(IDC_EDIT_OLD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NEW)->EnableWindow(FALSE);
}

// ============================================================================
// ==============================================================================
int CVersionUpdaterDlg::GetInputMode(void) const
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nMode = CUpdateMgr::MODE_CMP;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (IsDlgButtonChecked(IDC_RADIO_MODE_ADD)) {
		nMode = CUpdateMgr::MODE_ADD;
	}

	return nMode;
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnDestroy()
{
	for (std::vector<FILE_TYPE_INFO>::const_iterator it(m_vecFileType.begin()); it != m_vecFileType.end(); ++it) {
		delete it->m_pChk;
	}

	CDialog::OnDestroy();
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::OnBnClickedBtnClearlog()
{
	m_edtLog.SetWindowText("");
}
