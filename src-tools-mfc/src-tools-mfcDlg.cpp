
// src-tools-mfcDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "src-tools-mfc.h"
#include "src-tools-mfcDlg.h"
#include "afxdialogex.h"
#include "..\src-tools\src-tools.h"
#include "..\src-tools\reports.h"
#include "DuplicateStringsOptionsDlg.h"
#include "RcFileRulesOptionsDlg.h"
#include "UnusedStringsOptionsDlg.h"
#include "MissingMacroOptionsDlg.h"
#include "TooltipLengthOptionsDlg.h"
#include "UnusedStringsEditingDlg.h"
#include "DuplicateStringsEditingDlg.h"
#include "TooltipLengthEditingDlg.h"
#include "ProjectUnreferencedOptionsDlg.h"
#include "CompareExtrasEditingDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fs = std::filesystem;

// CsrctoolsmfcDlg dialog



CsrctoolsmfcDlg::CsrctoolsmfcDlg(const CStringA& optionsFilename, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SRCTOOLSMFC_DIALOG, pParent)
	, m_optionsFilename(optionsFilename)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFont = new CFont;
	m_pFont->CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, 0x04 | FF_MODERN, _T("Courier New"));

	ReadOptions(m_optionsFilename.GetString(), m_options);
}

CsrctoolsmfcDlg::~CsrctoolsmfcDlg()
{
	delete m_pFont;
	m_edittingMenu.DestroyMenu();
}

void CsrctoolsmfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUTEDIT, m_outputEdit);
	DDX_Control(pDX, IDC_EDITBROWSE, m_editBrowseCtrl);
	DDX_Control(pDX, IDC_TOOLTIPSMAX_BTN, m_tooltipsMenuBtn);
	DDX_Control(pDX, IDC_DUPLICATESTRINGS_BTN, m_duplicatesMenuBtn);
	DDX_Control(pDX, IDC_UNUSEDSTRINGS_BTN, m_unusedMenuBtn);
	DDX_Control(pDX, IDC_COMPAREEXTRAS_BTN, m_compareExtrasMenuBtn);
	DDX_Text(pDX, IDC_OPTIONSFILELAB, m_optionsFilenameLabel);
	DDX_Control(pDX, IDC_COMPARERESOURCES_BTN, m_compareResourcesMenuBtn);
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
	ON_BN_CLICKED(IDC_IMAGESANDCOMMANDS_OPTS, &CsrctoolsmfcDlg::OnBnClickedImagesandcommandsOpts)
	ON_BN_CLICKED(IDC_DUPLICATESTRINGS_OPTS, &CsrctoolsmfcDlg::OnBnClickedDuplicatestringsOpts)
	ON_BN_CLICKED(IDC_UNUSEDSTRINGS_OPTS, &CsrctoolsmfcDlg::OnBnClickedUnusedstringsOpts)
	ON_BN_CLICKED(IDC_MISSINGMACRO_OPTS, &CsrctoolsmfcDlg::OnBnClickedMissingmacroOpts)
	ON_BN_CLICKED(IDC_CONFORMINGRCFILE_OPTS, &CsrctoolsmfcDlg::OnBnClickedConformingrcfileOpts)
	ON_BN_CLICKED(IDC_TOOLTIPSMAX_OPTS, &CsrctoolsmfcDlg::OnBnClickedTooltipsmaxOpts)
	ON_BN_CLICKED(IDC_REGRESETDLG_OPTS, &CsrctoolsmfcDlg::OnBnClickedRegresetdlgOpts)
	ON_BN_CLICKED(IDC_MENUTITLECASE_BTN, &CsrctoolsmfcDlg::OnBnClickedMenutitlecaseBtn)
	ON_BN_CLICKED(IDC_MENUTITLECASE_OPTS, &CsrctoolsmfcDlg::OnBnClickedMenutitlecaseOpts)
	ON_BN_CLICKED(IDC_UNREFERENCEDFILES_BTN, &CsrctoolsmfcDlg::OnBnClickedUnreferencedfilesBtn)
	ON_BN_CLICKED(IDC_UNREFERENCEDFILES_OPTS, &CsrctoolsmfcDlg::OnBnClickedUnreferencedfilesOpts)
	ON_STN_CLICKED(IDC_OPTIONSFILELAB, &CsrctoolsmfcDlg::OnStnClickedOptionsfilelab)
	ON_BN_CLICKED(IDC_COMPAREEXTRAS_BTN, &CsrctoolsmfcDlg::OnBnClickedCompareextrasBtn)
	ON_BN_CLICKED(IDC_COMPARERESOURCES_BTN, &CsrctoolsmfcDlg::OnBnClickedCompareresourcesBtn)
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

	m_edittingMenu.CreatePopupMenu();
	m_edittingMenu.AppendMenu(MF_STRING, 10000, _T("... and proceed with editing"));

	m_duplicatesMenuBtn.m_hMenu = m_edittingMenu.GetSafeHmenu();
	m_unusedMenuBtn.m_hMenu = m_edittingMenu.GetSafeHmenu();
	m_tooltipsMenuBtn.m_hMenu = m_edittingMenu.GetSafeHmenu();
	m_compareExtrasMenuBtn.m_hMenu = m_edittingMenu.GetSafeHmenu();
	m_compareResourcesMenuBtn.m_hMenu = m_edittingMenu.GetSafeHmenu();

	m_optionsFilenameLabel = CString{ _T("Options file: ") } + CA2W(m_optionsFilename);
	UpdateData(FALSE);

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
	search_commands_not_in_imageandcommands(rootPath, ss, m_options.m_imagesAndCommandsOpts);
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
	DuplicateStringsOutput out;

	BeginWaitCursor();
	search_duplicate_string_resources(rootPath, ss, m_options.m_duplicateStringsOpts, out);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);

	int chosen = m_duplicatesMenuBtn.m_nMenuResult;
	if (10000 == chosen)
		{
		if (!out.m_folders.empty())
			{
			CDuplicateStringsEditingDlg dlg(out, this);
			dlg.DoModal();
			}
		else
			MessageBox(_T("Nothing to edit"), _T("Information"), MB_OK | MB_ICONINFORMATION);
		}
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
	UnusedStringsOutput out;

	BeginWaitCursor();
	search_unused_string_resources(rootPath, ss, m_options.m_unusedStringsOpts, out);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);

	int chosen = m_unusedMenuBtn.m_nMenuResult;
	if (10000 == chosen)
		{
		if (!out.m_folders.empty())
			{
			CUnusedStringsEditingDlg dlg(out, this);
			dlg.DoModal();
			}
		else
			MessageBox(_T("Nothing to edit"), _T("Information"), MB_OK | MB_ICONINFORMATION);
		}
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
	search_cpp_files_missing_debug_new_macro(rootPath, ss, m_options.m_missingMacroOpts);
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
	examine_dialogs_for_conformity(rootPath, ss, m_options.m_rcFileRulesOpts);
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
	TooltipLengthOutput out;

	BeginWaitCursor();
	search_tooltips_exceeding_max_length(rootPath, ss, m_options.m_tooltipLengthOpts, out);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);

	int chosen = m_tooltipsMenuBtn.m_nMenuResult;
	if (10000 == chosen)
		{
		if (!out.m_projectResources.empty())
			{
			size_t maxLength = m_options.m_tooltipLengthOpts.m_maximum;
			CTooltipLengthEditingDlg dlg(out, maxLength, this);
			dlg.DoModal();
			}
		else
			MessageBox(_T("Nothing to edit"), _T("Information"), MB_OK | MB_ICONINFORMATION);
		}
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
	search_resizable_not_in_reg_reset(rootPath, ss, m_options.m_regResetOpts);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedMenutitlecaseBtn()
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
	search_menu_files_title_case(rootPath, ss, m_options.m_menuTitleCaseOpts);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
}


void CsrctoolsmfcDlg::OnBnClickedImagesandcommandsOpts()
	{
		MessageBox(_T("Not yet implemented"), _T("Info"), MB_OK | MB_ICONINFORMATION);
	}


void CsrctoolsmfcDlg::OnBnClickedDuplicatestringsOpts()
	{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	CDuplicateStringsOptionsDlg dlg(m_options.m_duplicateStringsOpts, strRoot, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnBnClickedUnusedstringsOpts()
	{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	CUnusedStringsOptionsDlg dlg(m_options.m_unusedStringsOpts, strRoot, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnBnClickedMissingmacroOpts()
	{
	CMissingMacroOptionsDlg dlg(m_options.m_missingMacroOpts, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnBnClickedConformingrcfileOpts()
	{
	CRcFileRulesOptionsDlg dlg(m_options.m_rcFileRulesOpts, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnBnClickedTooltipsmaxOpts()
	{
	CTooltipLengthOptionsDlg dlg(m_options.m_tooltipLengthOpts, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnBnClickedRegresetdlgOpts()
	{
	MessageBox(_T("Not yet implemented"), _T("Info"), MB_OK | MB_ICONINFORMATION);
	}


void CsrctoolsmfcDlg::OnBnClickedMenutitlecaseOpts()
	{
	MessageBox(_T("Not yet implemented"), _T("Info"), MB_OK | MB_ICONINFORMATION);
	}


void CsrctoolsmfcDlg::OnOK()
{
	WriteOptions(m_optionsFilename.GetString(), m_options);

	CDialogEx::OnOK();
}


void CsrctoolsmfcDlg::OnBnClickedUnreferencedfilesBtn()
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
	search_files_not_referenced_by_project(rootPath, ss, m_options.m_projectUnreferencedOpts);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);
	}


void CsrctoolsmfcDlg::OnBnClickedUnreferencedfilesOpts()
	{
	CString strRoot;
	m_editBrowseCtrl.GetWindowTextW(strRoot);
	if (strRoot.IsEmpty())
		return;

	CProjectUnreferencedOptionsDlg dlg(m_options.m_projectUnreferencedOpts, strRoot, this);
	dlg.DoModal();
	}


void CsrctoolsmfcDlg::OnStnClickedOptionsfilelab()
{
	CString msg = _T("Yes to Save As, No to Open");
	switch (MessageBox(msg, _T("Save/Load Options"), MB_YESNOCANCEL | MB_ICONQUESTION))
		{
		case IDYES:
			{
			auto dlg = std::make_unique<CFileDialog>(FALSE, _T("options"), CA2W(m_optionsFilename), OFN_HIDEREADONLY | OFN_EXPLORER, _T("Option file (*.options)|*.options||"), this);
			if (dlg->DoModal() == IDOK)
				{
				CString newName = dlg->m_ofn.lpstrFile;
				CString oldName = CA2W(m_optionsFilename);
				if (newName != oldName)
					{
					m_optionsFilename = CW2A(newName);
					m_optionsFilenameLabel = CString{ _T("Options file: ") } + CA2W(m_optionsFilename);
					UpdateData(FALSE);
					WriteOptions(m_optionsFilename.GetString(), m_options);
					}
				}
			}
		break;
		case IDNO:
			{
			auto dlg = std::make_unique<CFileDialog>(TRUE, _T("options"), CA2W(m_optionsFilename), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, _T("Option file (*.options)|*.options||"), this);
			if (dlg->DoModal() == IDOK)
				{
				CString newName = dlg->m_ofn.lpstrFile;
				CString oldName = CA2W(m_optionsFilename);
				if (newName != oldName)
					{
					m_optionsFilename = CW2A(newName);
					m_optionsFilenameLabel = CString{ _T("Options file: ") } + CA2W(m_optionsFilename);
					UpdateData(FALSE);
					ReadOptions(m_optionsFilename.GetString(), m_options);
					}
				}
			}
		break;
		case IDCANCEL:
		default:
			break;
		}
}


void CsrctoolsmfcDlg::OnBnClickedCompareextrasBtn()
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
	CompareExtrasOutput out;

	BeginWaitCursor();
	compare_extras(rootPath, ss, m_options.m_compareExtrasOpts, out);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);

	int chosen = m_compareExtrasMenuBtn.m_nMenuResult;
	if (10000 == chosen)
		{
		if (!out.m_comparisons.empty())
			{
			CCompareExtrasEditingDlg dlg(out, this);
			dlg.DoModal();
			}
		else
			MessageBox(_T("Nothing to edit"), _T("Information"), MB_OK | MB_ICONINFORMATION);
		}
	}


void CsrctoolsmfcDlg::OnBnClickedCompareresourcesBtn()
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
	CompareExtrasOutput out;

	BeginWaitCursor();
	compare_resources(rootPath, ss, m_options.m_compareExtrasOpts, out);
	EndWaitCursor();

	CString output(ss.str().c_str());
	output.Replace(_T("\n"), _T("\r\n"));
	m_outputEdit.SetWindowTextW(output);

	int chosen = m_compareResourcesMenuBtn.m_nMenuResult;
	if (10000 == chosen)
		{
		if (!out.m_comparisons.empty())
			{
			CCompareExtrasEditingDlg dlg(out, this);
			dlg.DoModal();
			}
		else
			MessageBox(_T("Nothing to edit"), _T("Information"), MB_OK | MB_ICONINFORMATION);
		}
	}
