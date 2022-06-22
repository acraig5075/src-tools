// CDuplicateStringsOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "DuplicateStringsOptionsDlg.h"
#include "afxdialogex.h"


// CDuplicateStringsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CDuplicateStringsOptionsDlg, CDialog)

CDuplicateStringsOptionsDlg::CDuplicateStringsOptionsDlg(DuplicateStringsOptions& options, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DUPLICATESTRINGS_OPTSDLG, pParent)
	, m_options(options)
{
	if (options.m_outputFormat < 0)
		options.m_outputFormat = 0;
	if (options.m_outputFormat > 1)
		options.m_outputFormat = 1;
}

CDuplicateStringsOptionsDlg::~CDuplicateStringsOptionsDlg()
{
}

void CDuplicateStringsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDuplicateStringsOptionsDlg, CDialog)
END_MESSAGE_MAP()


// CDuplicateStringsOptionsDlg message handlers


BOOL CDuplicateStringsOptionsDlg::OnInitDialog()
	{
	CDialog::OnInitDialog();

	CheckRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2, IDC_FORMATRADIO1 + m_options.m_outputFormat);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
	}


void CDuplicateStringsOptionsDlg::OnOK()
	{
	m_options.m_outputFormat = GetCheckedRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2) - IDC_FORMATRADIO1;

	CDialog::OnOK();
	}
