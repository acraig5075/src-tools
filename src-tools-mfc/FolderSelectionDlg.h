#pragma once


// CFolderSelectionDlg dialog

class CFolderSelectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFolderSelectionDlg)

public:
	CFolderSelectionDlg(const CString &rootFolder, std::vector<std::string>& exclusions, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFolderSelectionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDERSELECTION_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedMovetorightbutton();
	afx_msg void OnBnClickedMovetoleftbutton();

private:
	CString m_rootFolder;
	std::vector<std::string>& m_exclusions;
	CStatic m_rootFolderLabel;
	CListBox m_includeListBox;
	CListBox m_excludeListBox;

	DECLARE_MESSAGE_MAP()
	};
