#pragma once
#include "afxdialogex.h"
#include "..\src-tools\reports.h"


// CComparisonEditingDlg dialog

class CComparisonEditingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CComparisonEditingDlg)

public:
	CComparisonEditingDlg(ComparisonOutput &report, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CComparisonEditingDlg();

private:
	void AddToList(int iItem, const ComparePaths &comp);
	void PopulateListAll();
	void PopulateListDifferent();
	bool ComparatorExists() const;
	CString MakeTempFileName() const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPAREEXTRAS_EDITDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedViewdiffbtn();
	afx_msg void OnBnClickedComparebtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkComparisonlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedShowradio();

private:
	ComparisonOutput &m_report;
	CListCtrl m_listCtrl;
	CImageList m_imageList;
	CMFCEditBrowseCtrl m_externalBrowseCtrl;
	CButton m_viewDiffBtn;
	CButton m_compareBtn;
	CStringArray m_tempFileNames;

	DECLARE_MESSAGE_MAP()
	};
