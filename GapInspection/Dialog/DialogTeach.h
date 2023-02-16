#pragma once
#include "SerialPort.h"
#include "DialogSpec.h"
#include "DialogSystem.h"
#include "DialogJog.h"

// CDialogTeach 대화 상자입니다.

class CDialogTeach : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogTeach)

public:
	CDialogTeach(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogTeach();

// 대화 상자 데이터입니다.
#ifdef TWO_CHANNEL_MODE
	enum { IDD = IDD_DIALOG_TEACH_2_CHANNEL };
#else
	enum { IDD = IDD_DIALOG_TEACH };
#endif
private:
	CListBox m_lbxListBox;
	CMyListCtrl m_ctrlStepList;
	CMyListCtrl m_ctrlParaList;

	CBtnenhctrl m_btn_StepIdx;
	CBtnenhctrl m_ctrlWorkModelName;

	CDialogSpec *m_dlgSpec;
	CDialogSpecPoint *m_dlgSpecPoint;
	//CDialogSystem *m_dlgSystem;
	//CDialogJog *m_dlgJog;

	TypeSeqPara m_SeqPara;
	TypeInspPara m_InspPara[MAX_CAM_CNT / 2];
	TypeSystemPara m_SystemPara;

	int	m_nModelListBoxNo;
	int m_iSelectListItemNum;

	double m_dCurPos[MAX_MOTION_CNT];

	CString m_strUseModelName;
	CString m_strSelectListModel;
	CString m_strDGSModelName;
#ifdef LIGHT
	CSerialPort* m_pSerialComm;
#endif

	//20190218 ngh
	CRect m_MainWIndowRect;

	//20190221 ngh
	CString m_strLogString;

	//20190403 ngh
	BOOL m_bCYL_FW;

	int m_nChannelIdx;
public:
	TypeSeqPara GetSeqPara();
	//void SetJogCurPos(int nMotionIdx, double dCurPos){m_dlgJog->SetCurPos(nMotionIdx, dCurPos);};
	//double GetJogStepPitch(int nMotionIdx){return m_dlgJog->GetStepPitch(nMotionIdx);};
	void SetSelectSeqAllCurPos(double *dCurPos, int nInspIdx = 0);
	void SetSelectSeqCurPos(int nSeqIdx, int nMotionIdx, double dCurPos, int nInspIdx = 0);

	void GetSpec(TypeSpecPara *Spec, int nSpectType = INSP_SPEC_TYPE_0);

	double GetSeqMovePos(int nSeqIdx, int nMotionIdx);
	void SetUpdatePara(BOOL isUpdate){UpdatePara(isUpdate);};
	int GetSelectSeqIdx(){return m_iSelectListItemNum;};
	CString GetUseModel(){return m_strUseModelName;};

	void SetSelectSeqInspRect(int nCamIdx, int nInspIdx, int nRectIdx, CRect rect);
	//TypeSystemPara GetSystemPara(){m_dlgSystem->GetSystemPara(&m_SystemPara); return m_SystemPara;};

	int GetMaxSeqCnt(){return m_SeqPara.InspectionPara.size();};
	BOOL GetUsePW(){return m_SystemPara.isUsePW;};

	void WritePacket(int nCh, BYTE byVal);

	//BOOL GetJogPlusMinuse(){return m_dlgJog->GetStepPlusMinuse();};

	//20190218 ngh
	void SetMainWindowRect(CRect rect);

	//20190220 ngh
	//void SetSystemPara(TypeSystemPara SystemPara){m_SystemPara = SystemPara; m_dlgSystem->SetSystemPara(m_SystemPara);};
	BOOL GetJogDlgIsVisible();

	//20190221 ngh
	void SetLightOnOff(int nCh, BOOL isOn);
	CString GetLogString(){return m_strLogString;};

	//20190307 ngh
	//CString GetDriveName(){return m_dlgSystem->GetDriveName();};

	//20190403 ngh
	BOOL GetCylFw(){return m_bCYL_FW;};
	void SetCylFw(BOOL isFw){m_bCYL_FW = isFw;};

	//20190424 ngh
	void GetSpread(TypeSpecPara *Spread, int nSpreadType = 0);

	void SetChannelIdx(int nChannel){m_nChannelIdx = nChannel;};

	void SetModel(CString strModel);

	void SetAlgorithmIdx(int nAlgorithmIdx){m_SeqPara.nSelectAlgorithmIdx = nAlgorithmIdx;};

	void SetModelAdd(CString strModel);
	void SetLoadSeqPara(){LoadSeqPara(m_strUseModelName);};

	void SetDGSModelName(CString strModel){m_strDGSModelName.Format(_T("%s"), strModel); SaveSeqPara(m_strUseModelName);};
	CString GetDGSModelName(){return m_strDGSModelName;};
protected:
	BOOL InitList();
	BOOL InitDialog();

	void UpdatePara(BOOL isUpdate);
	void UpdateSeqList(BOOL isUpdate);
	void UpdateParaList(BOOL isUpdate);
	void InitParaLIstLabel();
	
	void SelectSeqIdxVisionData(int nSeqIdx);
	BOOL SaveSeqPara(CString strModel);
	BOOL LoadSeqPara(CString strModel);

	void DisplayModelList();
	void DisplayModelName();

	void AllKillTimer();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlTeachSpec();
	void ClickBtnenhctrlTeachSystem();
	afx_msg void OnNMRClickListSeqStep(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuUp();
	afx_msg void OnMenuDown();
	afx_msg void OnMenuMove1();
	afx_msg void OnMenuMove2();
	afx_msg void OnMenuInsert();
	afx_msg void OnMenuDelete();
	void ClickBtnenhctrlTeachJog();
	afx_msg void OnNMClickListSeqStep(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedListSeqStep(NMHDR *pNMHDR, LRESULT *pResult);
	void ClickBtnenhctrlTeachSave();
	void ClickBtnenhctrlTeachCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//void ClickBtnenhctrlTeachModelAdd();
	//void ClickBtnenhctrlTeachModelDel();
	//void ClickBtnenhctrlTeachModelChange();
	//afx_msg void OnLbnSelchangeListModel();
	afx_msg void OnNMClickListInspPara(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void SelectStepMoveZ();
	void ClickBtnenhctrlTeachWaitPosMove();
	void ClickBtnenhctrlTeachCylAllFwBk();
	void ClickBtnenhctrlSpecPoint();
};
