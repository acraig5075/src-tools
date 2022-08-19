// UnusedStringsEditingDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "UnusedStringsEditingDlg.h"
#include "afxdialogex.h"
#include "..\src-tools\reports.h"


// CUnusedStringsEditingDlg dialog

IMPLEMENT_DYNAMIC(CUnusedStringsEditingDlg, CDialogEx)

CUnusedStringsEditingDlg::CUnusedStringsEditingDlg(const UnusedStringsOutput &report, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_UNUSEDSTRINGS_EDITDLG, pParent)
	, m_report(report)
{
}

CUnusedStringsEditingDlg::~CUnusedStringsEditingDlg()
{
}

void CUnusedStringsEditingDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODULELIST, m_moduleList);
	DDX_Control(pDX, IDC_RESOURCELIST, m_resourceList);
	DDX_Control(pDX, IDC_REMOVEBUTTON, m_removeButton);
	DDX_Control(pDX, IDC_FILENAMESTATIC, m_filenameLabel);
}


BEGIN_MESSAGE_MAP(CUnusedStringsEditingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_REMOVEBUTTON, &CUnusedStringsEditingDlg::OnClickedRemovebutton)
	ON_LBN_SELCHANGE(IDC_MODULELIST, &CUnusedStringsEditingDlg::OnSelchangeModulelist)
END_MESSAGE_MAP()


// CUnusedStringsEditingDlg message handlers


BOOL CUnusedStringsEditingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (const UnusedResources &o : m_report.m_folders)
		{
		std::string moduleName = o.m_path.filename().string();
		m_moduleList.AddString(CString(moduleName.c_str()));
		}

	if (m_moduleList.GetCount() > 0)
		{
		m_moduleList.SetCurSel(0);
		OnSelchangeModulelist();
		}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CUnusedStringsEditingDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
}


void CUnusedStringsEditingDlg::OnSelchangeModulelist()
{
	m_resourceList.ResetContent();
	m_filenameLabel.SetWindowTextW(_T(""));
	m_removeButton.EnableWindow(FALSE);

	int sel = m_moduleList.GetCurSel();
	if (LB_ERR == sel)
		return;

	size_t index = static_cast<size_t>(sel);
	if (index >= m_report.m_folders.size())
		return;

	const UnusedResources &unused = m_report.m_folders.at(index);

	m_filenameLabel.SetWindowTextW(CString(unused.m_rc.string().c_str()));

	for (auto itr = unused.m_names.begin(); itr != unused.m_names.end(); ++itr)
		{
		std::string resourceName = *itr;
		m_resourceList.AddString(CString(resourceName.c_str()));
		}

	if (m_resourceList.GetCount() > 0)
		{
		m_removeButton.EnableWindow(TRUE);
		}
}


void CUnusedStringsEditingDlg::OnClickedRemovebutton()
{
	m_removeButton.EnableWindow(FALSE);
	m_resourceList.ResetContent();
}
