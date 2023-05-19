#pragma once
#include "afxdialogex.h"
#include "..\src-tools\reports.h"

// CTooltipLengthEditingDlg dialog

class CTooltipLengthEditingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTooltipLengthEditingDlg)

public:
	CTooltipLengthEditingDlg(TooltipLengthOutput &data, size_t maxLength, CWnd *pParent = nullptr);   // standard constructor
	virtual ~CTooltipLengthEditingDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOLTIPLENGTH_EDITDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedFileprevbutton();
	afx_msg void OnBnClickedFilenextbutton();
	afx_msg void OnBnClickedResourceprevbutton();
	afx_msg void OnBnClickedResourcenextbutton();
	afx_msg void OnChangeStringedit();
	afx_msg void OnBnClickedUpdatebutton();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	void LoadData();
	void EnableControls();

private:
	TooltipLengthOutput &m_data;
	size_t m_maxLength = 80;
	size_t m_currentFile = 0;
	size_t m_currentResource = 0;
	std::vector<TooltipReplacement> m_changes;
	COLORREF m_textColour;

	CStatic m_fileLabel;
	CStatic m_resourceLabel;
	CStatic m_changesLabel;
	CEdit m_stringEdit;
	CButton m_prevFileButton;
	CButton m_nextFileButton;
	CButton m_prevResourceButton;
	CButton m_nextResourceButton;

	DECLARE_MESSAGE_MAP()
};
