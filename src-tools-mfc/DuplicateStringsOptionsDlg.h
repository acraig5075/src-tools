#pragma once


// CDuplicateStringsOptionsDlg dialog

class CDuplicateStringsOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDuplicateStringsOptionsDlg)

public:
	CDuplicateStringsOptionsDlg(int &outputFormat, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDuplicateStringsOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DUPLICATESTRINGS_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	int &m_outputFormat;

	DECLARE_MESSAGE_MAP()
};
