// VersionUpdaterDlg.cpp : implementation file
#include "stdafx.h"

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
}

BEGIN_MESSAGE_MAP(CVersionUpdaterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED
(IDC_BTN_PROCESS, &CVersionUpdaterDlg::OnBnClickedBtnProcess)
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

	// TODO: Add extra initialization here
	return TRUE;				// return TRUE unless you set the focus to a
								///control
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

		SendMessage(WM_ICONERASEBKGND,
					reinterpret_cast<WPARAM> (dc.GetSafeHdc()), 0);

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
	CUpdateMgr::GetInstance().Update3DMotion();
	CUpdateMgr::GetInstance().UpdateGUI();
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::LoadConfig(void)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	const char *CONFIG_INI = "config.ini";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GetPrivateProfileString("Path", "Old", "",
							m_cstrPathOld.GetBuffer(MAX_PATH), MAX_PATH,
							CONFIG_INI);
	GetPrivateProfileString("Path", "New", "",
							m_cstrPathNew.GetBuffer(MAX_PATH), MAX_PATH,
							CONFIG_INI);
	GetPrivateProfileString("Path", "Before", "",
							m_cstrPathBefore.GetBuffer(MAX_PATH), MAX_PATH,
							CONFIG_INI);
	GetPrivateProfileString("Path", "After", "",
							m_cstrPathAfter.GetBuffer(MAX_PATH), MAX_PATH,
							CONFIG_INI);
	this->UpdateData(FALSE);
}

// ============================================================================
// ==============================================================================
void CVersionUpdaterDlg::SaveConfig(void)
{
	this->UpdateData(TRUE);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	const char *CONFIG_INI = "config.ini";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	WritePrivateProfileString("Path", "Old", m_cstrPathOld, CONFIG_INI);
	WritePrivateProfileString("Path", "New", m_cstrPathNew, CONFIG_INI);
	WritePrivateProfileString("Path", "Before", m_cstrPathBefore, CONFIG_INI);
	WritePrivateProfileString("Path", "After", m_cstrPathAfter, CONFIG_INI);
}
