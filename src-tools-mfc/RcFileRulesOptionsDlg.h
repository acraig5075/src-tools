#pragma once


// CRcFileRulesOptionsDlg dialog

class CRcFileRulesOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRcFileRulesOptionsDlg)

public:
	CRcFileRulesOptionsDlg(RcFileRulesOptions &options, CWnd *pParent = nullptr);   // standard constructor
	virtual ~CRcFileRulesOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCFILERULES_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedAllbutton();
	afx_msg void OnBnClickedNonebutton();

private:
	RcFileRulesOptions& m_options;
	CCheckListBox m_ruleList;

	DECLARE_MESSAGE_MAP()
};
