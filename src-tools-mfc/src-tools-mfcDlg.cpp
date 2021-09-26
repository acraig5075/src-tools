
// src-tools-mfcDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "src-tools-mfc.h"
#include "src-tools-mfcDlg.h"
#include "afxdialogex.h"
#include "..\src-tools\src-tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fs = std::filesystem;

// CsrctoolsmfcDlg dialog



CsrctoolsmfcDlg::CsrctoolsmfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SRCTOOLSMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFont = new CFont;
	m_pFont->CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, 0x04 | FF_MODERN, _T("Courier New"));
}

CsrctoolsmfcDlg::~CsrctoolsmfcDlg()
{
	delete m_pFont;
}

void CsrctoolsmfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUTEDIT, m_outputEdit);
	DDX_Control(pDX, IDC_EDITBROWSE, m_editBrowseCtrl);
}

BEGIN_MESSAGE_MAP(CsrctoolsmfcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_IMAGESANDCOMMANDS_BTN, &CsrctoolsmfcDlg::OnBnClickedImagesandcommandsBtn)
	ON_BN_CLICKED(IDC_DUPLICATESTRINGS_BTN, &CsrctoolsmfcDlg::OnBnClickedDuplicatestringsBtn)
	ON_BN_CLICKED(IDC_UNUSEDSTRINGS_BTN, &CsrctoolsmfcDlg::OnBnClickedUnusedstringsBtn)
	ON_BN_CLICKED(IDC_MISSINGMACRO_BTN, &CsrctoolsmfcDlg::OnBnClickedMissingmacroBtn)
	ON_BN_CLICKED(IDC_CONFORMINGRCFILE_BTN, &CsrctoolsmfcDlg::OnBnClickedConformingrcfileBtn)
	ON_BN_CLICKED(IDC_TOOLTIPSMAX_BTN, &CsrctoolsmfcDlg::OnBnClickedTooltipsmaxBtn)
	ON_BN_CLICKED(IDC_REGRESETDLG_BTN, &CsrctoolsmfcDlg::OnBnClickedRegresetdlgBtn)
END_MESSAGE_MAP()


// CsrctoolsmfcDlg message handlers

BOOL CsrctoolsmfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_editBrowseCtrl.SetWindowTextW(_T("D:\\Src\\Trunk_VS2013\\CivilDesigner"));

	m_outputEdit.SetFont(m_pFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CsrctoolsmfcDlg::OnPaint()
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
HCURSOR CsrctoolsmfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CsrctoolsmfcDlg::OnBnClickedImagesandcommandsBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_commands_not_in_imageandcommands(rootPath, ss);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedDuplicatestringsBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_duplicate_string_resources(rootPath, ss, 1);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedUnusedstringsBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_unused_string_resources(rootPath, 1, ss);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedMissingmacroBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_cpp_files_missing_debug_new_macro(rootPath, ss);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedConformingrcfileBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	examine_rc_file_for_conformity(rootPath, ss, 0);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedTooltipsmaxBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_tooltips_exceeding_max_length(rootPath, ss, 80);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedRegresetdlgBtn()
{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	fs::path rootPath(strRoot.GetString());
	if (rootPath.empty() || !fs::exists(rootPath))
	{
		CString msg;
		msg.Format(_T("%ws does not exist"), strRoot.GetString());
		MessageBox(msg, _T("Error"), MB_OK | MB_ICONSTOP);
		return;
	}

	std::stringstream ss;

	BeginWaitCursor();
	search_resizable_not_in_reg_reset(rootPath, ss);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}
