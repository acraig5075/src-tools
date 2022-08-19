#pragma once

struct UnusedStringsOutput;

// CUnusedStringsEditingDlg dialog

class CUnusedStringsEditingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUnusedStringsEditingDlg)

public:
	CUnusedStringsEditingDlg(const UnusedStringsOutput &report, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUnusedStringsEditingDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNUSEDSTRINGS_EDITDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickedRemovebutton();
	afx_msg void OnSelchangeModulelist();

private:
	const UnusedStringsOutput& m_report;
	CListBox m_moduleList;
	CListBox m_resourceList;
	CStatic m_filenameLabel;
	CButton m_removeButton;

	DECLARE_MESSAGE_MAP()
	};
