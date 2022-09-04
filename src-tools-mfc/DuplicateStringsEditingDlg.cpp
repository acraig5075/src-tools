// DuplicateStringsEditingDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "DuplicateStringsEditingDlg.h"
#include "afxdialogex.h"
#include "..\src-tools\reports.h"
#include "..\src-tools\src-tools.h"


// CDuplicateStringsEditingDlg dialog

IMPLEMENT_DYNAMIC(CDuplicateStringsEditingDlg, CDialogEx)

CDuplicateStringsEditingDlg::CDuplicateStringsEditingDlg(DuplicateStringsOutput& report, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DUPLICATESTRINGS_EDITDLG, pParent)
	, m_report(report)
{

}

CDuplicateStringsEditingDlg::~CDuplicateStringsEditingDlg()
{
}

void CDuplicateStringsEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODULELIST, m_moduleList);
	DDX_Control(pDX, IDC_RESOURCELIST, m_resourceList);
	DDX_Control(pDX, IDC_FILENAMESTATIC, m_filenameLabel);
	DDX_Control(pDX, IDC_DUPLICATESEDIT, m_duplicatesEdit);
	}


BEGIN_MESSAGE_MAP(CDuplicateStringsEditingDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_MODULELIST, &CDuplicateStringsEditingDlg::OnSelchangeModulelist)
	ON_LBN_SELCHANGE(IDC_RESOURCELIST, &CDuplicateStringsEditingDlg::OnSelchangeResourcelist)
	ON_BN_CLICKED(IDC_REPLACEBUTTON, &CDuplicateStringsEditingDlg::OnBnClickedReplacebutton)
	ON_BN_CLICKED(IDC_ROTATEBUTTON, &CDuplicateStringsEditingDlg::OnBnClickedRotatebutton)
END_MESSAGE_MAP()


// CDuplicateStringsEditingDlg message handlers


BOOL CDuplicateStringsEditingDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	for (const DuplicateResources& o : m_report.m_folders)
		{
		std::string moduleName = o.m_filename.filename().string();
		m_moduleList.AddString(CString(moduleName.c_str()));
		}

	if (m_moduleList.GetCount() > 0)
		{
		m_moduleList.SetCurSel(0);
		OnSelchangeModulelist();
		}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CDuplicateStringsEditingDlg::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
	}


void CDuplicateStringsEditingDlg::OnSelchangeModulelist()
	{
	m_resourceList.ResetContent();
	m_filenameLabel.SetWindowTextW(_T(""));

	int sel = m_moduleList.GetCurSel();
	if (LB_ERR == sel)
		return;

	size_t index = static_cast<size_t>(sel);
	if (index >= m_report.m_folders.size())
		return;

	const DuplicateResources& resources = m_report.m_folders.at(index);

	m_filenameLabel.SetWindowTextW(CString(resources.m_filename.string().c_str()));

	for (auto itr = resources.m_duplicates.begin(); itr != resources.m_duplicates.end(); ++itr)
		{
		std::string resourceName = itr->m_description;
		m_resourceList.AddString(CString(resourceName.c_str()));
		}
	}

static CString ToString(const std::vector<std::string>& resources)
	{
	CString str;
	for (auto itr = resources.begin(); itr != resources.end(); ++itr)
		str.AppendFormat(_T("%s; "), CString(itr->c_str()).GetString());

	str.TrimRight(_T("; "));
	return str;
	}


void CDuplicateStringsEditingDlg::OnSelchangeResourcelist()
	{
	m_duplicatesEdit.SetWindowTextW(_T(""));
	int sel1 = m_moduleList.GetCurSel();
	int sel2 = m_resourceList.GetCurSel();
	if (LB_ERR == sel1 || LB_ERR == sel2)
		return;

	size_t index1 = static_cast<size_t>(sel1);
	size_t index2 = static_cast<size_t>(sel2);
	if (index1 >= m_report.m_folders.size() || index2 >= m_report.m_folders.at(index1).m_duplicates.size())
		return;

	const DuplicateResources& resources = m_report.m_folders.at(index1);
	const Duplicate& duplicates = resources.m_duplicates.at(index2);

	m_duplicatesEdit.SetWindowTextW(ToString(duplicates.m_names));
	}


void CDuplicateStringsEditingDlg::OnBnClickedRotatebutton()
	{
	int sel1 = m_moduleList.GetCurSel();
	int sel2 = m_resourceList.GetCurSel();
	if (LB_ERR == sel1 || LB_ERR == sel2)
		return;

	size_t index1 = static_cast<size_t>(sel1);
	size_t index2 = static_cast<size_t>(sel2);
	if (index1 >= m_report.m_folders.size() || index2 >= m_report.m_folders.at(index1).m_duplicates.size())
		return;

	DuplicateResources& resources = m_report.m_folders.at(index1);
	Duplicate& duplicates = resources.m_duplicates.at(index2);

	std::rotate(duplicates.m_names.begin(), duplicates.m_names.begin() + 1, duplicates.m_names.end());

	OnSelchangeResourcelist();
	}


static void RemoveReadOnly(const std::string& filename)
{
	CString file = CString(filename.c_str());
	::SetFileAttributes(file, GetFileAttributes(file) & ~FILE_ATTRIBUTE_READONLY);
}


void CDuplicateStringsEditingDlg::OnBnClickedReplacebutton()
	{
	int sel1 = m_moduleList.GetCurSel();
	int sel2 = m_resourceList.GetCurSel();
	if (LB_ERR == sel1 || LB_ERR == sel2)
		return;

	size_t index1 = static_cast<size_t>(sel1);
	size_t index2 = static_cast<size_t>(sel2);
	if (index1 >= m_report.m_folders.size() || index2 >= m_report.m_folders.at(index1).m_duplicates.size())
		return;

	DuplicateResources& resources = m_report.m_folders.at(index1);
	Duplicate& duplicates = resources.m_duplicates.at(index2);

	CString replacer;
	CString replacee;

	for (size_t i = 0; i < duplicates.m_names.size(); ++i)
		{
		if (0 == i)
			replacer = CString(duplicates.m_names[i].c_str());
		else
			replacee.AppendFormat(_T("%ws; "), CString(duplicates.m_names[i].c_str()).GetString());
		}
	replacee.Trim(_T("; "));

	CString msg;
	msg.Format(_T("Proceed with using %ws as a replacement for %ws"), replacer.GetString(), replacee.GetString());
	int ret = MessageBox(msg, _T("Proceed"), MB_YESNO | MB_ICONQUESTION);
	if (IDYES == ret)
		{
		std::filesystem::path folder = resources.m_filename.parent_path();

		size_t numExpectedRemovals = resources.m_duplicates.at(index2).m_names.size() - 1;
		size_t numDuplicatesBefore = resources.count();
		size_t numUnusedBefore = count_unused_string_resources(folder);

		replace_duplicate_string_resources(resources, index2, RemoveReadOnly);

		size_t numDuplicatesAfter = resources.count();
		size_t numUnusedAfter = count_unused_string_resources(folder);

		size_t numRemoved = numDuplicatesBefore - numDuplicatesAfter;
		size_t numNewUnused = numUnusedAfter - numUnusedBefore;

		if (numRemoved)
			OnSelchangeModulelist();

		CString msg;
		msg.Format(_T("%zu Duplicate resource(s) removed\n%zu New unused resources"),
			numExpectedRemovals,
			numNewUnused);

		MessageBox(msg, _T("Unused string resources"), (numExpectedRemovals == numNewUnused ? (MB_OK | MB_ICONINFORMATION) : (MB_OK | MB_ICONERROR)));
		}
	}
