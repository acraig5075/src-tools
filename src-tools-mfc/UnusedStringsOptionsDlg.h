#pragma once


// CUnusedStringsOptionsDlg dialog

class CUnusedStringsOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUnusedStringsOptionsDlg)

public:
	CUnusedStringsOptionsDlg(UnusedStringsOptions& options, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUnusedStringsOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNUSEDSTRINGS_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	UnusedStringsOptions& m_options;

	DECLARE_MESSAGE_MAP()
	};
