// CProjectUnreferencedOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "..\src-tools\string-utils.h"
#include "ProjectUnreferencedOptionsDlg.h"
#include "FolderSelectionDlg.h"
#include "afxdialogex.h"


// CProjectUnreferencedOptionsDlg dialog

IMPLEMENT_DYNAMIC(CProjectUnreferencedOptionsDlg, CDialog)

CProjectUnreferencedOptionsDlg::CProjectUnreferencedOptionsDlg(ProjectUnreferencedOptions &options, const CString& rootFolder, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROJECTUNREFERENCED_OPTSDLG, pParent)
	, m_options(options)
	, m_rootFolder(rootFolder)
{
	if (options.m_outputFormat < 0)
		options.m_outputFormat = 0;
	if (options.m_outputFormat > 1)
		options.m_outputFormat = 1;
}

CProjectUnreferencedOptionsDlg::~CProjectUnreferencedOptionsDlg()
{
}

void CProjectUnreferencedOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDEREDIT, m_folderEdit);
	}


BEGIN_MESSAGE_MAP(CProjectUnreferencedOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_FOLDERBUTTON, &CProjectUnreferencedOptionsDlg::OnBnClickedFolderbutton)
END_MESSAGE_MAP()


// CProjectUnreferencedOptionsDlg message handlers


static CString ToString(const std::vector<std::string>& folders)
	{
	CString str;
	for (auto itr = folders.begin(); itr != folders.end(); ++itr)
		str.AppendFormat(_T("%s;"), CString(itr->c_str()).GetString());

	str.TrimRight(_T(";"));
	return str;
	}

static std::vector<std::string> FromString(const CString& str)
	{
	std::vector<std::string> folders = string_utils::split(std::string(CW2A(str)), ';');
	folders.erase(std::remove_if(folders.begin(), folders.end(), [](const std::string& s)
		{
		return s.empty();
		}), folders.end());
	return folders;
	}

BOOL CProjectUnreferencedOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString text = ToString(m_options.m_excludeFolders);
	m_folderEdit.SetWindowTextW(text);

	CheckRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2, IDC_FORMATRADIO1 + m_options.m_outputFormat);

	CheckDlgButton(IDC_ONLYSUMMARYCHECK, m_options.m_onlySummary ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}


void CProjectUnreferencedOptionsDlg::OnOK()
{
	m_options.m_outputFormat = GetCheckedRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2) - IDC_FORMATRADIO1;

	m_options.m_onlySummary = IsDlgButtonChecked(IDC_ONLYSUMMARYCHECK) == BST_CHECKED;

	CString text;
	m_folderEdit.GetWindowTextW(text);
	m_options.m_excludeFolders = FromString(text);

	CDialog::OnOK();
}


void CProjectUnreferencedOptionsDlg::OnBnClickedFolderbutton()
{
	CFolderSelectionDlg dlg(m_rootFolder, m_options.m_excludeFolders, this);
	if (IDOK == dlg.DoModal())
		{
		CString text = ToString(m_options.m_excludeFolders);
		m_folderEdit.SetWindowTextW(text);
		}
}
