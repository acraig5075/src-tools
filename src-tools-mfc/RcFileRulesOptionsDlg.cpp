// RcFileRulesOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "RcFileRulesOptionsDlg.h"
#include "afxdialogex.h"


// CRcFileRulesOptionsDlg dialog

IMPLEMENT_DYNAMIC(CRcFileRulesOptionsDlg, CDialogEx)

CRcFileRulesOptionsDlg::CRcFileRulesOptionsDlg(int& outputFormat, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RCFILERULES_OPTSDLG, pParent)
	, m_outputFormat(outputFormat)
{
	if (m_outputFormat < 0)
		m_outputFormat = 0;
	if (m_outputFormat > 1)
		m_outputFormat = 1;
	}

CRcFileRulesOptionsDlg::~CRcFileRulesOptionsDlg()
{
}

void CRcFileRulesOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRcFileRulesOptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// CRcFileRulesOptionsDlg message handlers


BOOL CRcFileRulesOptionsDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2, IDC_FORMATRADIO1 + m_outputFormat);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CRcFileRulesOptionsDlg::OnOK()
	{
	m_outputFormat = GetCheckedRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2) - IDC_FORMATRADIO1;

	CDialogEx::OnOK();
	}
