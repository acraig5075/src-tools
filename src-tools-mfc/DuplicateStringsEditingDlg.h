#pragma once

struct DuplicateStringsOutput;

// CDuplicateStringsEditingDlg dialog

class CDuplicateStringsEditingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDuplicateStringsEditingDlg)

public:
	CDuplicateStringsEditingDlg(DuplicateStringsOutput &report, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDuplicateStringsEditingDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DUPLICATESTRINGS_EDITDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeModulelist();
	afx_msg void OnSelchangeResourcelist();
	afx_msg void OnBnClickedRotatebutton();
	afx_msg void OnBnClickedReplacebutton();

private:
	DuplicateStringsOutput &m_report;
	CListBox m_moduleList;
	CListBox m_resourceList;
	CStatic m_filenameLabel;
	CEdit m_duplicatesEdit;

	DECLARE_MESSAGE_MAP()
	};
