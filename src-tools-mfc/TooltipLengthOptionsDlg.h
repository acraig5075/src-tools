#pragma once


// CTooltipLengthOptionsDlg dialog

class CTooltipLengthOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTooltipLengthOptionsDlg)

public:
	CTooltipLengthOptionsDlg(TooltipLengthOptions& options, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTooltipLengthOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOLTIPLENGTH_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	TooltipLengthOptions& m_options;

	DECLARE_MESSAGE_MAP()
	};
