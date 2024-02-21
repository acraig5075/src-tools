#pragma once


// CProjectUnreferencedOptionsDlg dialog

class CProjectUnreferencedOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectUnreferencedOptionsDlg)

public:
	CProjectUnreferencedOptionsDlg(ProjectUnreferencedOptions &options, const CString& rootFolder, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CProjectUnreferencedOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECTUNREFERENCED_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedFolderbutton();

private:
	ProjectUnreferencedOptions &m_options;
	CString m_rootFolder;
	CEdit m_folderEdit;

	DECLARE_MESSAGE_MAP()
};
