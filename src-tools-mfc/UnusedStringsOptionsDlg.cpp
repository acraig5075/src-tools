// UnusedStringsOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "UnusedStringsOptionsDlg.h"
#include "afxdialogex.h"


// CUnusedStringsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CUnusedStringsOptionsDlg, CDialogEx)

CUnusedStringsOptionsDlg::CUnusedStringsOptionsDlg(UnusedStringsOptions &options, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UNUSEDSTRINGS_OPTSDLG, pParent)
	, m_options(options)
{
}

CUnusedStringsOptionsDlg::~CUnusedStringsOptionsDlg()
{
}

void CUnusedStringsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUnusedStringsOptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// CUnusedStringsOptionsDlg message handlers


BOOL CUnusedStringsOptionsDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_ONLYSUMMARYCHECK, m_options.m_onlySummary ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
	}


void CUnusedStringsOptionsDlg::OnOK()
	{
	m_options.m_onlySummary = IsDlgButtonChecked(IDC_ONLYSUMMARYCHECK) == BST_CHECKED;

	CDialogEx::OnOK();
	}
