
// src-tools-mfcDlg.h : header file
//

#pragma once


// CsrctoolsmfcDlg dialog
class CsrctoolsmfcDlg : public CDialogEx
{
// Construction
public:
	CsrctoolsmfcDlg(CWnd* pParent = nullptr);	// standard constructor
	~CsrctoolsmfcDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SRCTOOLSMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedImagesandcommandsBtn();
	afx_msg void OnBnClickedDuplicatestringsBtn();
	afx_msg void OnBnClickedUnusedstringsBtn();
	afx_msg void OnBnClickedMissingmacroBtn();
	afx_msg void OnBnClickedConformingrcfileBtn();

private:
	CEdit m_outputEdit;
	CMFCEditBrowseCtrl m_editBrowseCtrl;
	CFont* m_pFont = nullptr;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTooltipsmaxBtn();
};
