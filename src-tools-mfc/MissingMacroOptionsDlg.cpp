// MissingMacroOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "MissingMacroOptionsDlg.h"
#include "afxdialogex.h"


// CMissingMacroOptionsDlg dialog

IMPLEMENT_DYNAMIC(CMissingMacroOptionsDlg, CDialogEx)

CMissingMacroOptionsDlg::CMissingMacroOptionsDlg(MissingMacroOptions &options, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MISSINGMACRO_OPTSDLG, pParent)
	, m_options(options)
{
}

CMissingMacroOptionsDlg::~CMissingMacroOptionsDlg()
{
}

void CMissingMacroOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMissingMacroOptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// CMissingMacroOptionsDlg message handlers


BOOL CMissingMacroOptionsDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_ONLYSUMMARYCHECK, m_options.m_onlySummary ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
	}


void CMissingMacroOptionsDlg::OnOK()
	{
	m_options.m_onlySummary = IsDlgButtonChecked(IDC_ONLYSUMMARYCHECK) == BST_CHECKED;

	CDialogEx::OnOK();
	}
