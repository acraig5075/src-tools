#pragma once

#include "..\src-tools\options.h"

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
	afx_msg void OnBnClickedTooltipsmaxBtn();
	afx_msg void OnBnClickedRegresetdlgBtn();
	afx_msg void OnBnClickedImagesandcommandsOpts();
	afx_msg void OnBnClickedDuplicatestringsOpts();
	afx_msg void OnBnClickedUnusedstringsOpts();
	afx_msg void OnBnClickedMissingmacroOpts();
	afx_msg void OnBnClickedConformingrcfileOpts();
	afx_msg void OnBnClickedTooltipsmaxOpts();
	afx_msg void OnBnClickedRegresetdlgOpts();

private:
	CEdit m_outputEdit;
	CMFCEditBrowseCtrl m_editBrowseCtrl;
	CFont* m_pFont = nullptr;
	DuplicateStringsOptions m_duplicateStringsOpts;
	RcFileRulesOptions m_rcFileRulesOpts;

	DECLARE_MESSAGE_MAP()
	};
