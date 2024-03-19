// CompareExtrasEditingDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "afxdialogex.h"
#include "CompareExtrasEditingDlg.h"

// CCompareExtrasEditingDlg dialog

IMPLEMENT_DYNAMIC(CCompareExtrasEditingDlg, CDialogEx)

CCompareExtrasEditingDlg::CCompareExtrasEditingDlg(CompareExtrasOutput &report, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_COMPAREEXTRAS_EDITDLG, pParent)
	, m_report(report)
{
}

CCompareExtrasEditingDlg::~CCompareExtrasEditingDlg()
{
}

void CCompareExtrasEditingDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPARISONLIST, m_listCtrl);
	DDX_Control(pDX, IDC_EXTERNALBROWSEBTN, m_externalBrowseCtrl);
}


BEGIN_MESSAGE_MAP(CCompareExtrasEditingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_COMPAREBTN, &CCompareExtrasEditingDlg::OnBnClickedComparebtn)
END_MESSAGE_MAP()


// CCompareExtrasEditingDlg message handlers


BOOL CCompareExtrasEditingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	m_listCtrl.GetWindowRect(&rect);
	int scrollWidth = GetSystemMetrics(SM_CXVSCROLL);
	int narrowColumn = (rect.Width() - scrollWidth) / 7;
	int columnWidth1 = narrowColumn * 3;
	int columnWidth2 = narrowColumn * 3;
	int columnWidth3 = narrowColumn;

	m_listCtrl.InsertColumn(0, _T("Left file"), LVCFMT_LEFT, columnWidth1);
	m_listCtrl.InsertColumn(1, _T("Right file"), LVCFMT_LEFT, columnWidth2);
	m_listCtrl.InsertColumn(2, _T("Edit Distance"), LVCFMT_LEFT, columnWidth3);

	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	PopulateListDifferent();

	m_externalBrowseCtrl.SetWindowTextW(_T("C:\\Program Files\\Beyond Compare 4\\BCompare.exe"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCompareExtrasEditingDlg::PopulateListAll()
{
	std::vector<ComparePaths> &comparisons = m_report.m_comparisons;

	int iItem = 0;
	for (auto itr = comparisons.begin(); itr != comparisons.end(); ++itr, ++iItem)
		{
		AddToList(iItem, *itr);
		}
}

void CCompareExtrasEditingDlg::PopulateListDifferent()
{
	auto IsZeroEditDistance = [](const ComparePaths &comp)
		{
		return comp.m_editDistance == 0;
		};

	std::vector<ComparePaths> comparisons = m_report.m_comparisons;
	auto newEnd = std::remove_if(comparisons.begin(), comparisons.end(), IsZeroEditDistance);

	int iItem = 0;
	for (auto itr = comparisons.begin(); itr != newEnd; ++itr, ++iItem)
		{
		AddToList(iItem, *itr);
		}
}

void CCompareExtrasEditingDlg::AddToList(int iItem, const ComparePaths &comp)
{
	CString path1{ comp.m_path1.string().c_str() };
	CString path2{ comp.m_path2.string().c_str() };
	CString editDist{ std::to_string(comp.m_editDistance).c_str() };

	LVITEM sItem = { LVIF_TEXT, iItem, 0, 0, 0, path1.GetBuffer(0) };
	int nItem = m_listCtrl.InsertItem(&sItem);
	path1.ReleaseBuffer();

	sItem.iSubItem = 1;
	sItem.pszText = path2.GetBuffer(0);
	m_listCtrl.SetItem(&sItem);
	path2.ReleaseBuffer();

	sItem.iSubItem = 2;
	sItem.pszText = editDist.GetBuffer(0);
	m_listCtrl.SetItem(&sItem);
	editDist.ReleaseBuffer();
}


void CCompareExtrasEditingDlg::OnBnClickedComparebtn()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == 0)
		return;

	int iItem = m_listCtrl.GetNextSelectedItem(pos);
	CString lhs = m_listCtrl.GetItemText(iItem, 0);
	CString rhs = m_listCtrl.GetItemText(iItem, 1);

	CString exe;
	m_externalBrowseCtrl.GetWindowTextW(exe);

	CString params;
	params.Format(_T("\"%ws\" \"%ws\""), lhs.GetString(), rhs.GetString());

	ShellExecute(nullptr, _T("open"), exe.GetString(), params.GetString(), nullptr, SW_SHOW);

	iItem++;
	if (iItem < m_listCtrl.GetItemCount())
		{
		m_listCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		m_listCtrl.SetSelectionMark(iItem);
		m_listCtrl.SetFocus();
		}
}
