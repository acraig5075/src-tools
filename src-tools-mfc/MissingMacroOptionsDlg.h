#pragma once


// CMissingMacroOptionsDlg dialog

class CMissingMacroOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMissingMacroOptionsDlg)

public:
	CMissingMacroOptionsDlg(MissingMacroOptions& options, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMissingMacroOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MISSINGMACRO_OPTSDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	MissingMacroOptions& m_options;

	DECLARE_MESSAGE_MAP()
	};
