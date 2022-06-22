// CDuplicateStringsOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "DuplicateStringsOptionsDlg.h"
#include "afxdialogex.h"


// CDuplicateStringsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CDuplicateStringsOptionsDlg, CDialog)

CDuplicateStringsOptionsDlg::CDuplicateStringsOptionsDlg(int& outputFormat, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DUPLICATESTRINGS_OPTSDLG, pParent)
	, m_outputFormat(outputFormat)
{
	if (m_outputFormat < 0)
		m_outputFormat = 0;
	if (m_outputFormat > 1)
		m_outputFormat = 1;
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

	CheckRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2, IDC_FORMATRADIO1 + m_outputFormat);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
	}


void CDuplicateStringsOptionsDlg::OnOK()
	{
	m_outputFormat = GetCheckedRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2) - IDC_FORMATRADIO1;

	CDialog::OnOK();
	}
