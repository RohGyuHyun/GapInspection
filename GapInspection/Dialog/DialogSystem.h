#pragma once
#include "DialogDriveSelect.h"
#include "DialogAlgorithmSelect.h"

// CDialogSystem 대화 상자입니다.

class CDialogSystem : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSystem)

public:
	CDialogSystem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogSystem();

// 대화 상자 데이터입니다.
#ifdef TWO_CHANNEL_MODE
	enum { IDD = IDD_DIALOG_SYSTEM_2_CHANNEL };
#else
	enum { IDD = IDD_DIALOG_SYSTEM };
#endif
private:
	TypeSystemPara m_SystemPara;
	CDialogDriveSelect *m_dlgDriveSelect;
	CDialogAlgorithmSelect *m_dlgAlgorithmSelect;
	CString m_strDrive;
	CString m_strAlgorithm;
	CListBox m_lbxListBox;
	CBtnenhctrl m_ctrlWorkModelName;
	int	m_nModelListBoxNo;
	CString m_strSelectListModel;
public:
	TypeSystemPara GetSystemPara();
	void SetSystemPara(TypeSystemPara SystemPara);
	void SetUseModel(CString strModel){m_SystemPara.strUseModelName.Format(_T("%s"), strModel);};
	CString GetUseModel(){return m_SystemPara.strUseModelName;};
	void SetSaveSystemPara(){SaveSystemPara();};
	CString GetDriveName(){m_strDrive.Format(_T("%s"), m_SystemPara.strDrive); return m_strDrive;};
	CString GetSelectAddModelName(){return m_strSelectListModel;};
	void SetLoadSystemPara(){LoadSystemPara();};

	CString GetDGSModelName();
	void SetDGSModelName(CString strModel);
protected:
	void UpdatePara(BOOL isUpdate);
	BOOL SaveSystemPara();
	BOOL LoadSystemPara();
	void DisplayModelList();
	void DisplayModelName();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickPixelSizeW();
	void ClickPixelSizeH();
	void ClickBtnenhctrlSysytemApply();
	void ClickBtnenhctrlSystemCancel();
	void ClickBtnenhctrlSystemPara8();
	void ClickBtnenhctrlSystemPara9();
	void ClickBtnenhctrlSystemPara10();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ClickBtnenhctrlSystemPara11();
	void ClickBtnenhctrlSystemPara14();
	void ClickBtnenhctrlSystemPara15();
	void ClickBtnenhctrlSystemPara16();
	void ClickBtnenhctrlSystemPara17();
	void ClickBtnenhctrlSystemPara18();
	void ClickBtnenhctrlSystemModelAdd2();
	void ClickBtnenhctrlSystemModelDel2();
	void ClickBtnenhctrlSystemModelChange2();
	afx_msg void OnLbnSelchangeListModel2();
	void ClickBtnenhctrlSystemPara24();
	void ClickBtnenhctrlSystemPara25();
};
