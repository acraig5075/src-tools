#pragma once


// CUnusedStringsOptionsDlg dialog

class CUnusedStringsOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUnusedStringsOptionsDlg)

public:
	CUnusedStringsOptionsDlg(UnusedStringsOptions& options, const CString& rootFolder, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUnusedStringsOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNUSEDSTRINGS_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedFolderbutton();

private:
	UnusedStringsOptions& m_options;
	CString m_rootFolder;
	CEdit m_folderEdit;

	DECLARE_MESSAGE_MAP()
	};
