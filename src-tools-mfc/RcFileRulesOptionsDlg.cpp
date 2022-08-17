// RcFileRulesOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "..\src-tools\options.h"
#include "RcFileRulesOptionsDlg.h"
#include "afxdialogex.h"


// CRcFileRulesOptionsDlg dialog

IMPLEMENT_DYNAMIC(CRcFileRulesOptionsDlg, CDialogEx)

CRcFileRulesOptionsDlg::CRcFileRulesOptionsDlg(RcFileRulesOptions& options, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RCFILERULES_OPTSDLG, pParent)
	, m_options(options)
{
	if (m_options.m_outputFormat < 0)
		m_options.m_outputFormat = 0;
	if (m_options.m_outputFormat > 1)
		m_options.m_outputFormat = 1;
	}

CRcFileRulesOptionsDlg::~CRcFileRulesOptionsDlg()
{
}

void CRcFileRulesOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RULELIST, m_ruleList);
	}


BEGIN_MESSAGE_MAP(CRcFileRulesOptionsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ALLBUTTON, &CRcFileRulesOptionsDlg::OnBnClickedAllbutton)
	ON_BN_CLICKED(IDC_NONEBUTTON, &CRcFileRulesOptionsDlg::OnBnClickedNonebutton)
END_MESSAGE_MAP()


// CRcFileRulesOptionsDlg message handlers


BOOL CRcFileRulesOptionsDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2, IDC_FORMATRADIO1 + m_options.m_outputFormat);

	SetDlgItemInt(IDC_MINIMUMEDIT, m_options.m_minInputBoxHeight);
	SetDlgItemInt(IDC_MAXIMUMEDIT, m_options.m_maxInputBoxHeight);
	SetDlgItemInt(IDC_MARGINEDIT,  m_options.m_dialogMargin);
	SetDlgItemInt(IDC_GAPEDIT,     m_options.m_okCancelGap);

	m_ruleList.AddString(_T("Caption ends with the word Dialog or Window"));
	m_ruleList.AddString(_T("Dialog caption is not title case"));
	m_ruleList.AddString(_T("Group box caption is not title case"));
	m_ruleList.AddString(_T("Label ends with a colon"));
	m_ruleList.AddString(_T("Input box has the wrong height"));
	m_ruleList.AddString(_T("OK and Cancel buttons are not horizontally aligned"));
	m_ruleList.AddString(_T("OK and Cancel buttons have the wrong gap"));
	m_ruleList.AddString(_T("Control overlaps dialog margin"));
	m_ruleList.AddString(_T("Checkbox is right-aligned"));
	m_ruleList.AddString(_T("Controls don't align vertically on the left"));
	m_ruleList.AddString(_T("Controls don't align vertically on the right"));
	m_ruleList.AddString(_T("Controls don't align horizontally at the top"));
	m_ruleList.AddString(_T("Controls don't align horizontally at the bottom"));

	m_ruleList.SetCheck(0, m_options.m_redundantCaptionSuffix       ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(1, m_options.m_captionsNotTitleCase         ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(2, m_options.m_groupboxCaptionsNotTitleCase ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(3, m_options.m_labelEndsWithColon           ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(4, m_options.m_inputboxesWrongHeight        ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(5, m_options.m_okCancelNotLeftToRight       ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(6, m_options.m_okCancelWrongGap             ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(7, m_options.m_exceedsDialogMargin          ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(8, m_options.m_rightAlignedCheckbox         ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(9, m_options.m_verticalAlignmentLeft        ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(10, m_options.m_verticalAlignmentRight      ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(11, m_options.m_horizontalAlignmentTop      ? BST_CHECKED : BST_UNCHECKED);
	m_ruleList.SetCheck(12, m_options.m_horizontalAlignmentBottom   ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CRcFileRulesOptionsDlg::OnOK()
	{
	m_options.m_outputFormat = GetCheckedRadioButton(IDC_FORMATRADIO1, IDC_FORMATRADIO2) - IDC_FORMATRADIO1;

	BOOL ok;
	m_options.m_minInputBoxHeight = GetDlgItemInt(IDC_MINIMUMEDIT, &ok);
	m_options.m_maxInputBoxHeight = GetDlgItemInt(IDC_MAXIMUMEDIT, &ok);
	m_options.m_dialogMargin      = GetDlgItemInt(IDC_MARGINEDIT, &ok);
	m_options.m_okCancelGap       = GetDlgItemInt(IDC_GAPEDIT, &ok);

	m_options.m_redundantCaptionSuffix       = m_ruleList.GetCheck(0) != BST_UNCHECKED;
	m_options.m_captionsNotTitleCase         = m_ruleList.GetCheck(1) != BST_UNCHECKED;
	m_options.m_groupboxCaptionsNotTitleCase = m_ruleList.GetCheck(2) != BST_UNCHECKED;
	m_options.m_labelEndsWithColon           = m_ruleList.GetCheck(3) != BST_UNCHECKED;
	m_options.m_inputboxesWrongHeight        = m_ruleList.GetCheck(4) != BST_UNCHECKED;
	m_options.m_okCancelNotLeftToRight       = m_ruleList.GetCheck(5) != BST_UNCHECKED;
	m_options.m_okCancelWrongGap             = m_ruleList.GetCheck(6) != BST_UNCHECKED;
	m_options.m_exceedsDialogMargin          = m_ruleList.GetCheck(7) != BST_UNCHECKED;
	m_options.m_rightAlignedCheckbox         = m_ruleList.GetCheck(8) != BST_UNCHECKED;
	m_options.m_verticalAlignmentLeft        = m_ruleList.GetCheck(9) != BST_UNCHECKED;
	m_options.m_verticalAlignmentRight       = m_ruleList.GetCheck(10) != BST_UNCHECKED;
	m_options.m_horizontalAlignmentTop       = m_ruleList.GetCheck(11) != BST_UNCHECKED;
	m_options.m_horizontalAlignmentBottom    = m_ruleList.GetCheck(12) != BST_UNCHECKED;

	CDialogEx::OnOK();
	}


void CRcFileRulesOptionsDlg::OnBnClickedAllbutton()
	{
	for (int i = 0; i < m_ruleList.GetCount(); ++i)
		{
		m_ruleList.SetCheck(i, BST_CHECKED);
		}
	}


void CRcFileRulesOptionsDlg::OnBnClickedNonebutton()
	{
	for (int i = 0; i < m_ruleList.GetCount(); ++i)
		{
		m_ruleList.SetCheck(i, BST_UNCHECKED);
		}
	}
