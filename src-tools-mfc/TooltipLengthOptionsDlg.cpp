// TooltipLengthOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "TooltipLengthOptionsDlg.h"
#include "afxdialogex.h"


// CTooltipLengthOptionsDlg dialog

IMPLEMENT_DYNAMIC(CTooltipLengthOptionsDlg, CDialogEx)

CTooltipLengthOptionsDlg::CTooltipLengthOptionsDlg(TooltipLengthOptions &options, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOOLTIPLENGTH_OPTSDLG, pParent)
	, m_options(options)
{
}

CTooltipLengthOptionsDlg::~CTooltipLengthOptionsDlg()
{
}

void CTooltipLengthOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTooltipLengthOptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// CTooltipLengthOptionsDlg message handlers


BOOL CTooltipLengthOptionsDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	SetDlgItemInt(IDC_MAXLENGTHEDIT, static_cast<UINT>(m_options.m_maximum));

	CheckDlgButton(IDC_ONLYSUMMARYCHECK, m_options.m_onlySummary ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CTooltipLengthOptionsDlg::OnOK()
	{

	BOOL ok;
	UINT value = GetDlgItemInt(IDC_MAXLENGTHEDIT, &ok);
	if (!ok)
		{
		GetDlgItem(IDC_MAXLENGTHEDIT)->SetFocus();
		return;
		}

	m_options.m_maximum = static_cast<size_t>(value);

	m_options.m_onlySummary = IsDlgButtonChecked(IDC_ONLYSUMMARYCHECK) == BST_CHECKED;

	CDialogEx::OnOK();
	}
