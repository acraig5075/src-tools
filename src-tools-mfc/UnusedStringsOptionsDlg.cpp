// UnusedStringsOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "..\src-tools\string-utils.h"
#include "UnusedStringsOptionsDlg.h"
#include "afxdialogex.h"
#include "FolderSelectionDlg.h"


// CUnusedStringsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CUnusedStringsOptionsDlg, CDialogEx)

CUnusedStringsOptionsDlg::CUnusedStringsOptionsDlg(UnusedStringsOptions &options, const CString &rootFolder, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_UNUSEDSTRINGS_OPTSDLG, pParent)
	, m_options(options)
	, m_rootFolder(rootFolder)
{
}

CUnusedStringsOptionsDlg::~CUnusedStringsOptionsDlg()
{
}

void CUnusedStringsOptionsDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDEREDIT, m_folderEdit);
}


BEGIN_MESSAGE_MAP(CUnusedStringsOptionsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FOLDERBUTTON, &CUnusedStringsOptionsDlg::OnBnClickedFolderbutton)
END_MESSAGE_MAP()


// CUnusedStringsOptionsDlg message handlers

CString ToString(const std::vector<std::string> &folders)
{
	CString str;
	for (auto itr = folders.begin(); itr != folders.end(); ++itr)
		str.AppendFormat(_T("%s;"), CString(itr->c_str()).GetString());

	str.TrimRight(_T(";"));
	return str;
}

std::vector<std::string> FromString(const CString &str)
{
	std::vector<std::string> folders = split(std::string(CW2A(str)), ';');
	folders.erase(std::remove_if(folders.begin(), folders.end(), [](const std::string & s)
		{
		return s.empty();
		}), folders.end());
	return folders;
}

BOOL CUnusedStringsOptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_ONLYSUMMARYCHECK, m_options.m_onlySummary ? BST_CHECKED : BST_UNCHECKED);

	CString text = ToString(m_options.m_excludeFolders);
	m_folderEdit.SetWindowTextW(text);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CUnusedStringsOptionsDlg::OnOK()
{
	m_options.m_onlySummary = IsDlgButtonChecked(IDC_ONLYSUMMARYCHECK) == BST_CHECKED;

	CString text;
	m_folderEdit.GetWindowTextW(text);
	m_options.m_excludeFolders = FromString(text);

	CDialogEx::OnOK();
}


void CUnusedStringsOptionsDlg::OnBnClickedFolderbutton()
{
	CFolderSelectionDlg dlg(m_rootFolder, m_options.m_excludeFolders, this);
	if (IDOK == dlg.DoModal())
		{
		CString text = ToString(m_options.m_excludeFolders);
		m_folderEdit.SetWindowTextW(text);
		}
}
