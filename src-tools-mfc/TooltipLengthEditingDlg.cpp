// TooltipLengthEditingDlg.cpp : implementation file
//

#include "pch.h"
#include "src-tools-mfc.h"
#include "afxdialogex.h"
#include "TooltipLengthEditingDlg.h"


// CTooltipLengthEditingDlg dialog

IMPLEMENT_DYNAMIC(CTooltipLengthEditingDlg, CDialogEx)

CTooltipLengthEditingDlg::CTooltipLengthEditingDlg(TooltipLengthOutput& data, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOOLTIPLENGTH_EDITDLG, pParent)
	, m_data(data)
{
}

CTooltipLengthEditingDlg::~CTooltipLengthEditingDlg()
{
}

void CTooltipLengthEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELABEL, m_fileLabel);
	DDX_Control(pDX, IDC_RESOURCELABEL, m_resourceLabel);
	DDX_Control(pDX, IDC_STRINGEDIT, m_stringEdit);
	DDX_Control(pDX, IDC_FILEPREVBUTTON, m_prevFileButton);
	DDX_Control(pDX, IDC_FILENEXTBUTTON, m_nextFileButton);
	DDX_Control(pDX, IDC_RESOURCEPREVBUTTON, m_prevResourceButton);
	DDX_Control(pDX, IDC_RESOURCENEXTBUTTON, m_nextResourceButton);
	}


BEGIN_MESSAGE_MAP(CTooltipLengthEditingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FILEPREVBUTTON, &CTooltipLengthEditingDlg::OnBnClickedFileprevbutton)
	ON_BN_CLICKED(IDC_FILENEXTBUTTON, &CTooltipLengthEditingDlg::OnBnClickedFilenextbutton)
	ON_BN_CLICKED(IDC_RESOURCEPREVBUTTON, &CTooltipLengthEditingDlg::OnBnClickedResourceprevbutton)
	ON_BN_CLICKED(IDC_RESOURCENEXTBUTTON, &CTooltipLengthEditingDlg::OnBnClickedResourcenextbutton)
	ON_EN_CHANGE(IDC_STRINGEDIT, &CTooltipLengthEditingDlg::OnChangeStringedit)
	ON_BN_CLICKED(IDC_UPDATEBUTTON, &CTooltipLengthEditingDlg::OnBnClickedUpdatebutton)
END_MESSAGE_MAP()


// CTooltipLengthEditingDlg message handlers


BOOL CTooltipLengthEditingDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	m_data.m_projectResources.erase(std::remove_if(m_data.m_projectResources.begin(), m_data.m_projectResources.end(), [](const TooltipLength& tooltip) { return tooltip.m_stringResources.empty(); }), m_data.m_projectResources.end());

	EnableControls();
	LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CTooltipLengthEditingDlg::LoadData()
{
	m_fileLabel.SetWindowTextW(_T("<none>"));
	m_resourceLabel.SetWindowTextW(_T("<none>"));
	m_stringEdit.SetWindowTextW(_T(""));

	if (m_currentFile >= m_data.m_projectResources.size())
		return;

	TooltipLength tooltip = m_data.m_projectResources[m_currentFile];

	if (m_currentResource >= tooltip.m_stringResources.size())
		return;

	IDSResource resource = tooltip.m_stringResources[m_currentResource];

	m_fileLabel.SetWindowTextW(CA2W(tooltip.m_rcFilename.string().c_str()));
	m_resourceLabel.SetWindowTextW(CA2W(resource.m_name.c_str()));

	CString str = CA2W(resource.m_description.c_str());
	str.Remove(_T('|'));
	m_stringEdit.SetWindowTextW(str);

	CString fileCount, resourceCount, stringLength;
	fileCount.Format(_T("%zu/%zu"), m_currentFile + 1, m_data.m_projectResources.size());
	resourceCount.Format(_T("%zu/%zu"), m_currentResource + 1, tooltip.m_stringResources.size());
	stringLength.Format(_T("%zu"), resource.m_description.size());

	SetDlgItemTextW(IDC_FILECOUNT, fileCount.GetString());
	SetDlgItemTextW(IDC_RESOURCECOUNT, resourceCount.GetString());
	SetDlgItemTextW(IDC_LENGTHCOUNT, stringLength.GetString());
	}

void CTooltipLengthEditingDlg::OnBnClickedFileprevbutton()
	{
	if (m_currentFile > 0)
		{
		m_currentFile--;
		m_currentResource = 0;
		EnableControls(); 
		LoadData();
		}
	}

void CTooltipLengthEditingDlg::OnBnClickedFilenextbutton()
	{
	if (m_currentFile < m_data.m_projectResources.size() - 1)
		{
		m_currentFile++;
		m_currentResource = 0;
		EnableControls(); 
		LoadData();
		}
	}

void CTooltipLengthEditingDlg::OnBnClickedResourceprevbutton()
	{
	if (m_currentResource > 0)
		{
		m_currentResource--;
		EnableControls(); 
		LoadData();
		}
	}

void CTooltipLengthEditingDlg::OnBnClickedResourcenextbutton()
	{
	if (m_currentResource < m_data.m_projectResources[m_currentFile].m_stringResources.size())
		{
		m_currentResource++;
		EnableControls();
		LoadData();
		}
	}

void CTooltipLengthEditingDlg::EnableControls()
	{
	m_prevFileButton.EnableWindow(m_currentFile > 0);
	m_nextFileButton.EnableWindow(m_currentFile + 1 < m_data.m_projectResources.size());

	if (m_currentFile < m_data.m_projectResources.size())
		{
		m_prevResourceButton.EnableWindow(m_currentResource > 0);
		m_nextResourceButton.EnableWindow(m_currentResource + 1 < m_data.m_projectResources[m_currentFile].m_stringResources.size());
		}
	}


void CTooltipLengthEditingDlg::OnChangeStringedit()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	int length = m_stringEdit.GetWindowTextLengthW();
	CString strLength;
	strLength.Format(_T("%d"), length);
	SetDlgItemTextW(IDC_LENGTHCOUNT, strLength.GetString());
	}


void CTooltipLengthEditingDlg::OnBnClickedUpdatebutton()
	{
	Change change;
	change.m_rcFilename = m_data.m_projectResources[m_currentFile].m_rcFilename.c_str();
	change.m_oldString = m_data.m_projectResources[m_currentFile].m_stringResources[m_currentResource].m_description.c_str();
	m_stringEdit.GetWindowTextW(change.m_newString);

	if (change.m_newString != change.m_oldString)
		{
		m_changes.Add(change);

		m_currentResource++;
		if (m_currentResource == m_data.m_projectResources[m_currentFile].m_stringResources.size())
			{
			m_currentFile++;
			m_currentResource = 0;
			if (m_currentFile == m_data.m_projectResources.size())
				m_currentFile = 0;
			}

		EnableControls();
		LoadData();
		}
	else
		{
		m_stringEdit.SetFocus();
		m_stringEdit.SetSel(0, -1);
		}
	}


void CTooltipLengthEditingDlg::OnOK()
	{
	CString msg;
	msg.Format(_T("%I64d changes\nSave not yet implemented"), m_changes.GetCount());
	MessageBox(msg, _T("Warning"), MB_OK | MB_ICONWARNING);
	CDialogEx::OnOK();
	}
