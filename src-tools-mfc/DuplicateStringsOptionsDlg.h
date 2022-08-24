#pragma once


// CDuplicateStringsOptionsDlg dialog

class CDuplicateStringsOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDuplicateStringsOptionsDlg)

public:
	CDuplicateStringsOptionsDlg(DuplicateStringsOptions &options, const CString& rootFolder, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDuplicateStringsOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DUPLICATESTRINGS_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedFolderbutton();

private:
	DuplicateStringsOptions& m_options;
	CString m_rootFolder;
	CEdit m_folderEdit;

	DECLARE_MESSAGE_MAP()
};
