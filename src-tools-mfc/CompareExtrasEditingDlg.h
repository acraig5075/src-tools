#pragma once
#include "afxdialogex.h"
#include "..\src-tools\reports.h"


// CCompareExtrasEditingDlg dialog

class CCompareExtrasEditingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCompareExtrasEditingDlg)

public:
	CCompareExtrasEditingDlg(CompareExtrasOutput &report, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCompareExtrasEditingDlg();

private:
	void AddToList(int iItem, const ComparePaths &comp);
	void PopulateListAll();
	void PopulateListDifferent();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPAREEXTRAS_EDITDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

private:
	CompareExtrasOutput &m_report;
	CListCtrl m_listCtrl;
	CMFCEditBrowseCtrl m_externalBrowseCtrl;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedComparebtn();
	};