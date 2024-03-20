// CompareExtrasEditingDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "afxdialogex.h"
#include "ComparisonEditingDlg.h"

// CComparisonEditingDlg dialog

namespace fs = std::filesystem;

IMPLEMENT_DYNAMIC(CComparisonEditingDlg, CDialogEx)

CComparisonEditingDlg::CComparisonEditingDlg(ComparisonOutput &report, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_COMPAREEXTRAS_EDITDLG, pParent)
	, m_report(report)
{
}

CComparisonEditingDlg::~CComparisonEditingDlg()
{
	for (INT_PTR i = 0; i < m_tempFileNames.GetCount(); ++i)
		::DeleteFile(m_tempFileNames[i]);
}

void CComparisonEditingDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPARISONLIST, m_listCtrl);
	DDX_Control(pDX, IDC_EXTERNALBROWSEBTN, m_externalBrowseCtrl);
	DDX_Control(pDX, IDC_VIEWDIFFBTN, m_viewDiffBtn);
	DDX_Control(pDX, IDC_COMPAREBTN, m_compareBtn);
	}


BEGIN_MESSAGE_MAP(CComparisonEditingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_COMPAREBTN, &CComparisonEditingDlg::OnBnClickedComparebtn)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_COMPARISONLIST, &CComparisonEditingDlg::OnDblclkComparisonlist)
	ON_BN_CLICKED(IDC_VIEWDIFFBTN, &CComparisonEditingDlg::OnBnClickedViewdiffbtn)
	ON_BN_CLICKED(IDC_SHOWRADIO1, &CComparisonEditingDlg::OnBnClickedShowradio)
	ON_BN_CLICKED(IDC_SHOWRADIO2, &CComparisonEditingDlg::OnBnClickedShowradio)
END_MESSAGE_MAP()


// CComparisonEditingDlg message handlers


BOOL CComparisonEditingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageList.Create(16, 16, ILC_COLOR32, 1, 1);
	m_imageList.Add(theApp.LoadIcon(IDI_MENUICON2));
	m_imageList.Add(theApp.LoadIcon(IDI_TOOLBARICON));
	m_imageList.Add(theApp.LoadIcon(IDI_ACCELERATORICON2));
	m_imageList.Add(theApp.LoadIcon(IDI_TEXTTEMPLATECON1));
	m_listCtrl.SetImageList(&m_imageList, LVSIL_SMALL);

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

	CheckRadioButton(IDC_SHOWRADIO1, IDC_SHOWRADIO2, IDC_SHOWRADIO1);
	OnBnClickedShowradio();

	m_externalBrowseCtrl.SetWindowTextW(_T("C:\\Program Files\\Beyond Compare 4\\BCompare.exe"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CComparisonEditingDlg::PopulateListAll()
{
	std::vector<ComparePaths> &comparisons = m_report.m_comparisons;

	int iItem = 0;
	for (auto itr = comparisons.begin(); itr != comparisons.end(); ++itr, ++iItem)
		{
		AddToList(iItem, *itr);
		}
}

void CComparisonEditingDlg::PopulateListDifferent()
{
	m_listCtrl.DeleteAllItems();

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

void CComparisonEditingDlg::AddToList(int iItem, const ComparePaths &comp)
{
	CString path1{ comp.m_path1.string().c_str() };
	CString path2{ comp.m_path2.string().c_str() };
	CString editDist{ std::to_string(comp.m_editDistance).c_str() };

	int iImage = -1;
	fs::path ext = comp.m_path1.extension();
	if (ext == ".mnu")
		iImage = 0;
	else if (ext == ".tbr")
		iImage = 1;
	else if (ext == ".acc")
		iImage = 2;
	else if (ext == ".rc")
		iImage = 3;

	LVITEM sItem = { LVIF_TEXT | LVIF_IMAGE, iItem, 0, 0, 0, path1.GetBuffer(0) };
	sItem.iImage = iImage;
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


void CComparisonEditingDlg::OnBnClickedViewdiffbtn()
	{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == 0)
		{
		MessageBox(_T("Nothing selected"), _T("Warning"), MB_OK | MB_ICONWARNING);
		return;
		}

	int iItem = m_listCtrl.GetNextSelectedItem(pos);
	size_t index = static_cast<size_t>(iItem);
	if (index > m_report.m_comparisons.size())
		{
		MessageBox(_T("No matching comparison found"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
		}

	if (m_report.m_comparisons[index].m_unifiedDiff.empty())
		{
		MessageBox(_T("No diff exists for the comparison"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
		}

	if (!ComparatorExists())
		{
		MessageBox(_T("External comparator not found"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
		}

	// Make a temporary diff file, will be deleted in destructor
	CString diffFile = MakeTempFileName();
	m_tempFileNames.Add(diffFile);
	CStdioFile ofs(diffFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	for (const auto &line : m_report.m_comparisons[index].m_unifiedDiff)
		ofs.WriteString(CString{ line });
	ofs.Close();

	CString exe;
	m_externalBrowseCtrl.GetWindowTextW(exe);

	CString params;
	params.Format(_T("\"%ws\""), diffFile.GetString());

	ShellExecute(nullptr, _T("open"), exe.GetString(), params.GetString(), nullptr, SW_SHOW);

	iItem++;
	if (iItem < m_listCtrl.GetItemCount())
		{
		m_listCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		m_listCtrl.SetSelectionMark(iItem);
		m_listCtrl.SetFocus();
		}
	}


void CComparisonEditingDlg::OnBnClickedComparebtn()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == 0)
		{
		MessageBox(_T("Nothing selected"), _T("Warning"), MB_OK | MB_ICONWARNING);
		return;
		}

	int iItem = m_listCtrl.GetNextSelectedItem(pos);
	CString lhs = m_listCtrl.GetItemText(iItem, 0);
	CString rhs = m_listCtrl.GetItemText(iItem, 1);

	CString exe;
	m_externalBrowseCtrl.GetWindowTextW(exe);

	if (!ComparatorExists())
		{
		MessageBox(_T("External comparator not found"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
		}

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


void CComparisonEditingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd() == 0)
		return;

	CRect rect;
	m_listCtrl.GetWindowRect(&rect);
	int scrollWidth = GetSystemMetrics(SM_CXVSCROLL);
	int narrowColumn = (rect.Width() - scrollWidth) / 7;
	int columnWidth1 = narrowColumn * 3;
	int columnWidth2 = narrowColumn * 3;
	int columnWidth3 = narrowColumn;

	m_listCtrl.SetColumnWidth(0, columnWidth1);
	m_listCtrl.SetColumnWidth(1, columnWidth2);
	m_listCtrl.SetColumnWidth(2, columnWidth3);
}


void CComparisonEditingDlg::OnDblclkComparisonlist(NMHDR *pNMHDR, LRESULT *pResult)
	{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	OnBnClickedComparebtn();

	*pResult = 0;
	}

bool CComparisonEditingDlg::ComparatorExists() const
{
	CString exe;
	m_externalBrowseCtrl.GetWindowText(exe);

	std::string exestr{ CW2A(exe.GetString()) };

	fs::path exepath{ exestr };

	return fs::exists(exepath);
}

CString CComparisonEditingDlg::MakeTempFileName() const
{
	_TCHAR tempPath[MAX_PATH], tempFile[MAX_PATH];
	::GetTempPath(MAX_PATH, tempPath);
	::GetTempFileNameW(tempPath, _T("tmp"), 0, tempFile);
	return CString{ tempFile } + _T(".diff");
}


void CComparisonEditingDlg::OnBnClickedShowradio()
	{
	int radio = GetCheckedRadioButton(IDC_SHOWRADIO1, IDC_SHOWRADIO2);

	if (radio == IDC_SHOWRADIO2)
		PopulateListAll();
	else
		PopulateListDifferent();
	}
