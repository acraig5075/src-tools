#pragma once


// CRcFileRulesOptionsDlg dialog

class CRcFileRulesOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRcFileRulesOptionsDlg)

public:
	CRcFileRulesOptionsDlg(int& outputFormatCWnd, CWnd *pParent = nullptr);   // standard constructor
	virtual ~CRcFileRulesOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCFILERULES_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	int& m_outputFormat;

	DECLARE_MESSAGE_MAP()
	};
