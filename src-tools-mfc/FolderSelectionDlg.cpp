// CFolderSelectionDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "FolderSelectionDlg.h"
#include "afxdialogex.h"
#include "..\src-tools\filesystem-utils.h"


// CFolderSelectionDlg dialog

IMPLEMENT_DYNAMIC(CFolderSelectionDlg, CDialogEx)

CFolderSelectionDlg::CFolderSelectionDlg(const CString &rootFolder, std::vector<std::string> &exclusions, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_FOLDERSELECTION_DLG, pParent)
	, m_rootFolder(rootFolder)
	, m_exclusions(exclusions)
{
}

CFolderSelectionDlg::~CFolderSelectionDlg()
{
}

void CFolderSelectionDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROOTSTATIC, m_rootFolderLabel);
	DDX_Control(pDX, IDC_INCLUDELIST, m_includeListBox);
	DDX_Control(pDX, IDC_EXCLUDELIST, m_excludeListBox);
}


BEGIN_MESSAGE_MAP(CFolderSelectionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MOVETORIGHTBUTTON, &CFolderSelectionDlg::OnBnClickedMovetorightbutton)
	ON_BN_CLICKED(IDC_MOVETOLEFTBUTTON, &CFolderSelectionDlg::OnBnClickedMovetoleftbutton)
END_MESSAGE_MAP()


// CFolderSelectionDlg message handlers


BOOL CFolderSelectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_rootFolderLabel.SetWindowTextW(m_rootFolder);

	namespace fs = std::filesystem;

	fs::path root(m_rootFolder.GetString());

	std::vector<fs::path> list = get_directory_list(root, false);

	for (auto itr = list.cbegin(); itr != list.cend(); ++itr)
		{
		std::string name = itr->filename().string();
		bool include = m_exclusions.cend() == std::find(m_exclusions.cbegin(), m_exclusions.cend(), name);

		if (include)
			m_includeListBox.AddString(CString(name.c_str()));
		else
			m_excludeListBox.AddString(CString(name.c_str()));
		}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CFolderSelectionDlg::OnOK()
{
	m_exclusions.clear();

	for (int i = 0; i < m_excludeListBox.GetCount(); ++i)
		{
		CString name;
		m_excludeListBox.GetText(i, name);
		m_exclusions.push_back(std::string(CW2A(name)));
		}

	CDialogEx::OnOK();
}


void CFolderSelectionDlg::OnBnClickedMovetorightbutton()
{
	CString name;
	int sel = m_includeListBox.GetCurSel();
	m_includeListBox.GetText(sel, name);
	m_includeListBox.DeleteString(sel);
	m_excludeListBox.AddString(name);
}


void CFolderSelectionDlg::OnBnClickedMovetoleftbutton()
{
	CString name;
	int sel = m_excludeListBox.GetCurSel();
	m_excludeListBox.GetText(sel, name);
	m_excludeListBox.DeleteString(sel);
	m_includeListBox.AddString(name);
}
