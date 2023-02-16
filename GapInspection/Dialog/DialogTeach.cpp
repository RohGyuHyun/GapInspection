// DialogTeach.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogTeach.h"
#include "afxdialogex.h"


// CDialogTeach 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogTeach, CDialogEx)

CDialogTeach::CDialogTeach(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogTeach::IDD, pParent)
{
	m_iSelectListItemNum = 0;
	//m_strUseModelName.Format(_T("TEST"));
#ifdef LIGHT
	m_pSerialComm = new CSerialPort;
#endif
	m_dlgSpec = NULL;
	//m_dlgSystem = NULL;
	//m_dlgJog = NULL;
	m_bCYL_FW = FALSE;
	//m_dlgSpecPoint = NULL;
}

CDialogTeach::~CDialogTeach()
{
	for(int i = 0; i < (MAX_LIGHT_CNT * MAX_CAM_CNT); i++)
	{
		SetLightOnOff(i, FALSE);
	}
#ifdef LIGHT
	if (NULL != m_pSerialComm)
	{
		m_pSerialComm->Close();

		delete m_pSerialComm;
		m_pSerialComm = NULL;
	}
#endif
	if(m_dlgSpec)
		delete m_dlgSpec;

	if(m_dlgSpecPoint != NULL)
		delete m_dlgSpecPoint;

	/*if(m_dlgSystem)
		delete m_dlgSystem;*/

	//if(m_dlgJog)
	//	delete m_dlgJog;

	if(m_SeqPara.InspectionPara.size())
		m_SeqPara.InspectionPara.clear();
}

void CDialogTeach::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_MODEL, m_lbxListBox);
	DDX_Control(pDX, IDC_LIST_SEQ_STEP, m_ctrlStepList);
	DDX_Control(pDX, IDC_LIST_INSP_PARA, m_ctrlParaList);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_SELECT_STEP, m_btn_StepIdx);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_SELECT_MODEL, m_ctrlWorkModelName);
}


BEGIN_MESSAGE_MAP(CDialogTeach, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SEQ_STEP, &CDialogTeach::OnNMRClickListSeqStep)
	ON_COMMAND(ID_MENU_UP, &CDialogTeach::OnMenuUp)
	ON_COMMAND(ID_MENU_DOWN, &CDialogTeach::OnMenuDown)
	ON_COMMAND(ID_MENU_MOVE1, &CDialogTeach::OnMenuMove1)
	ON_COMMAND(ID_MENU_MOVE2, &CDialogTeach::OnMenuMove2)
	ON_COMMAND(ID_MENU_INSERT, &CDialogTeach::OnMenuInsert)
	ON_COMMAND(ID_MENU_DELETE, &CDialogTeach::OnMenuDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SEQ_STEP, &CDialogTeach::OnNMClickListSeqStep)
	ON_NOTIFY(NM_THEMECHANGED, IDC_LIST_SEQ_STEP, &CDialogTeach::OnNMThemeChangedListSeqStep)
	//ON_LBN_SELCHANGE(IDC_LIST_MODEL, &CDialogTeach::OnLbnSelchangeListModel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INSP_PARA, &CDialogTeach::OnNMClickListInspPara)
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


// CDialogTeach 메시지 처리기입니다.


BOOL CDialogTeach::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFont m_font; 
	m_font.CreatePointFont(50, _T("Arial"));
	m_ctrlStepList.SetFont(&m_font, TRUE);
	m_ctrlParaList.SetFont(&m_font, TRUE);

	m_ctrlStepList.ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	m_ctrlStepList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlParaList.ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	m_ctrlParaList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	
	
	InitList();
	InitDialog();

	SetTimer(TEACH_TIMER_WINDOWS_VISIBLE, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

TypeSeqPara CDialogTeach::GetSeqPara()
{
	//LoadSeqPara(m_strUseModelName);
	return m_SeqPara;
}

void CDialogTeach::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		m_iSelectListItemNum = 0;
		//DisplayModelList();
		//DisplayModelName();
		m_ctrlWorkModelName.SetWindowTextW(m_strUseModelName);
		if(LoadSeqPara(m_strUseModelName))
		{
			if(m_SeqPara.InspectionPara.size() > 0)
			{
				SelectSeqIdxVisionData(0);
				m_InspPara[0] = m_SeqPara.InspectionPara[0].InspPara[0];
				m_InspPara[1] = m_SeqPara.InspectionPara[0].InspPara[1];
				m_dlgSpec->SetSpec(m_SeqPara.Spec);
				m_dlgSpecPoint->SetSpec(m_SeqPara.SpecPoint);
				m_dlgSpec->SetSpread(m_SeqPara.Spread);
				m_dlgSpecPoint->SetSpread(m_SeqPara.SpreadPoint);
				m_dlgSpec->SetPointOffset(m_SeqPara.SpecPoint);
			}
			UpdatePara(FALSE);
		}
		
		AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_APPLY_SEQ_PARA, 0);

		SetTimer(TEACH_TIMER_INPUT_SEQ_LIST_VAL, 100, NULL);
		SetTimer(TEACH_TIMER_INPUT_PARA_LIST_VAL, 100, NULL);
	}
	else
	{
		AllKillTimer();
		//KillTimer(TEACH_TIMER_INPUT_SEQ_LIST_VAL);
		//KillTimer(TEACH_TIMER_INPUT_PARA_LIST_VAL);
		m_dlgSpec->ShowWindow(SW_HIDE);
		m_dlgSpecPoint->ShowWindow(SW_HIDE);
		//m_dlgSystem->ShowWindow(SW_HIDE);
		//m_dlgJog->ShowWindow(SW_HIDE);

	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDialogTeach::UpdatePara(BOOL isUpdate)
{
	UpdateParaList(isUpdate);
	UpdateSeqList(isUpdate);
}

void CDialogTeach::SelectSeqIdxVisionData(int nSeqIdx)
{
	CString strText;
	strText.Format(_T("%d"), nSeqIdx + 1);
	m_btn_StepIdx.SetWindowTextW(strText);

	m_ctrlParaList.DeleteAllItems();

	for(int i = INSP_LIST_SIDE_MAX_COL_COUNT - 1; i >= 0 ; i--)
		m_ctrlParaList.DeleteColumn(i);

	DWORD dwStyle;
	LV_COLUMN lvcolumn; 
	int nInspPoint[MAX_CAM_CNT / 2];

	TCHAR *sInspSideList[INSP_LIST_SIDE_MAX_COL_COUNT];

	nInspPoint[0] = m_SeqPara.InspectionPara[nSeqIdx].nInspPoint[0];
	nInspPoint[1] = m_SeqPara.InspectionPara[nSeqIdx].nInspPoint[1];

	char *cTemp[3];
	for(int i = 0; i < INSP_LIST_SIDE_MAX_COL_COUNT; i++)
	{
		switch(i)
		{
		case 0:
			strText.Format(_T("Parameter Name"));
			break;
		case 1:
			strText.Format(_T("R Cam : %s"), InspPointToString(nInspPoint[i - 1]));
			break;
		case 2:
			strText.Format(_T("L Cam : %s"), InspPointToString(nInspPoint[i - 1]));
			break;
		}
		cTemp[i] = new char[256];
		memset(cTemp[i], NULL, sizeof(char) * 256);
		sprintf_s(cTemp[i], strText.GetLength() + 1, "%S", strText);
		
		int nLen = MultiByteToWideChar(CP_ACP, 0, cTemp[i], strlen(cTemp[i]), NULL, NULL); 
		sInspSideList[i] = new TCHAR[256];
		memset(sInspSideList[i], NULL, sizeof(TCHAR) * 256);
		MultiByteToWideChar(CP_ACP, 0, cTemp[i], strlen(cTemp[i]), sInspSideList[i], nLen);
	}

	int iInspSideWidth[INSP_LIST_SIDE_MAX_COL_COUNT] = {270, 190, 190 };

	dwStyle = m_ctrlParaList.GetExtendedStyle(); 
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EDITLABELS ;// | LVS_EX_FULLROWSELECT; 
	m_ctrlParaList.SetExtendedStyle(dwStyle); 	

	for(int i = 0; i < INSP_LIST_SIDE_MAX_COL_COUNT; i++)
	{ 
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
		lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH; 
		lvcolumn.pszText = sInspSideList[i]; 
		lvcolumn.iSubItem = i; 
		lvcolumn.cx = iInspSideWidth[i]; 
		m_ctrlParaList.InsertColumn(i, &lvcolumn); 
	}

	for(int i = 0; i < INSP_LIST_SIDE_MAX_COL_COUNT; i++)
	{
		delete cTemp[i];
		delete sInspSideList[i];
	}

	UpdateParaList(FALSE);
}

void CDialogTeach::UpdateSeqList(BOOL isUpdate)
{
	CString strText;
	int nItemIdx = 1;
	TypeInspectionPara StepPara;

	if(m_SeqPara.InspectionPara.size() == 0)
	{
		return;
	}

	if(isUpdate)
	{
		UpdateData(TRUE);

		if(m_SeqPara.nMaxSeqIdx > 0)
		{
			vector<TypeInspectionPara>::iterator it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
			StepPara.nSeqIdx = m_iSelectListItemNum;

			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			if(_wtoi(strText) == INSP_SKIP || _wtoi(strText) == INSP_POINT1 || _wtoi(strText) == INSP_POINT2 || _wtoi(strText) == INSP_POINT3)
				StepPara.nInspPoint[TOP_CAMERA] = _wtoi(strText);

			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			if(_wtoi(strText) == INSP_SKIP || _wtoi(strText) == INSP_POINT4 || _wtoi(strText) == INSP_POINT5 || _wtoi(strText) == INSP_POINT6)
				StepPara.nInspPoint[BOTTOM_CAMERA] = _wtoi(strText);

			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			StepPara.dMotionVal[X_MOTION_INSP] = _wtof(strText);
			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			StepPara.dMotionVal[Y_MOTION_INSP] = _wtof(strText);
			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			StepPara.dMotionVal[Z_MOTION_INSP_0] = _wtof(strText);
			strText = m_ctrlStepList.GetItemText(m_iSelectListItemNum, nItemIdx++);
			StepPara.dMotionVal[Z_MOTION_INSP_1] = _wtof(strText);

			StepPara.InspPara[TOP_CAMERA] = m_InspPara[TOP_CAMERA];
			StepPara.InspPara[BOTTOM_CAMERA] = m_InspPara[BOTTOM_CAMERA];

			for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
			{
				for(int j = 0; j < MAX_INSP_CNT; j++)
				{
					for(int k = 0; k < MAX_LIGHT_CNT; k++)
					{
						StepPara.nLightVal[i][j][k] = it->nLightVal[i][j][k];
					}
				}
			}

			m_SeqPara.InspectionPara.insert(it, StepPara);
			m_SeqPara.InspectionPara.erase(m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum + 1);
		}
	}
	else
	{
		m_ctrlStepList.DeleteAllItems();

		for(int i = 0; i < m_SeqPara.InspectionPara.size(); i++)
		{
			strText.Format(_T("Step_%d"), i + 1);
			m_ctrlStepList.InsertItem(i, strText);
			strText.Format(_T("%d"), m_SeqPara.InspectionPara[i].nInspPoint[TOP_CAMERA]);
			m_ctrlStepList.SetItemText(i, 1, strText);
			strText.Format(_T("%d"), m_SeqPara.InspectionPara[i].nInspPoint[BOTTOM_CAMERA]);
			m_ctrlStepList.SetItemText(i, 2, strText);
			strText.Format(_T("%0.3f"), m_SeqPara.InspectionPara[i].dMotionVal[X_MOTION_INSP]);
			m_ctrlStepList.SetItemText(i, 3, strText);
			strText.Format(_T("%0.3f"), m_SeqPara.InspectionPara[i].dMotionVal[Y_MOTION_INSP]);
			m_ctrlStepList.SetItemText(i, 4, strText);
			strText.Format(_T("%0.3f"), m_SeqPara.InspectionPara[i].dMotionVal[Z_MOTION_INSP_0]);
			m_ctrlStepList.SetItemText(i, 5, strText);
			strText.Format(_T("%0.3f"), m_SeqPara.InspectionPara[i].dMotionVal[Z_MOTION_INSP_1]);
			m_ctrlStepList.SetItemText(i, 6, strText);
		}

		UpdateData(FALSE);
	}
}

void CDialogTeach::UpdateParaList(BOOL isUpdate)
{
	CString strText, strLabel, strCam;
	int nItemIdx = 1, nMaxInspCnt = MAX_INSP_CNT;
	TypeInspectionPara StepPara;
	double dTemp = 0.;
	
	if(m_SeqPara.InspectionPara.size() == 0)
	{
		return;
	}

	for(int i = 0; i < MAX_CAM_CNT / 2; i++)
	{
		if(m_SeqPara.InspectionPara.size() > 0)
		{
			if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT2 ||
				m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT5)
			{
				nMaxInspCnt = MAX_INSP_CNT;
				break;
			}
		}
	}

	if(isUpdate)
	{
		UpdateData(TRUE);

		if(m_SeqPara.nMaxSeqIdx > 0)
		{
			vector<TypeInspectionPara>::iterator it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
			StepPara = *it;
			StepPara.nSeqIdx = m_iSelectListItemNum;
			for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
			{				
				if(i == 0)
				{
					strCam.Format(_T("R Cam : "));
				}
				else
				{
					strCam.Format(_T("L Cam : "));
				}
				nItemIdx = 0;
				strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
				strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
				if(LimiteRangeCheck(0., 1., _wtoi(strText), StepPara.InspPara[i].bOneGrabInspection, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
					StepPara.InspPara[i].bOneGrabInspection = _wtoi(strText);

				for(int j = 0; j < nMaxInspCnt; j++)
				{
					if(j < MAX_INSP_CNT)
					{
						strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
						strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
						if(LimiteRangeCheck(0., 255., _wtoi(strText), StepPara.nLightVal[i][j][LIGHT_SIDE_LEFT], LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
							StepPara.nLightVal[i][j][LIGHT_SIDE_LEFT] = _wtoi(strText);

						strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
						strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
						if(LimiteRangeCheck(0., 255., _wtoi(strText), StepPara.nLightVal[i][j][LIGHT_SIDE_RIGHT], LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
							StepPara.nLightVal[i][j][LIGHT_SIDE_RIGHT] = _wtoi(strText);

						strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
						strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
						if(LimiteRangeCheck(0., 255., _wtoi(strText), StepPara.nLightVal[i][j][LIGHT_SIDE_COAXIAL], LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
							StepPara.nLightVal[i][j][LIGHT_SIDE_COAXIAL] = _wtoi(strText);
					}

					strText = m_ctrlParaList.GetItemText(nItemIdx++, i + 1);
					dTemp = _wtoi(strText);
					strText = m_ctrlParaList.GetItemText(nItemIdx++, i + 1);

					if(dTemp < _wtof(strText))
					{
						StepPara.InspPara[i].InspRect[j].left = (int)dTemp;
						StepPara.InspPara[i].InspRect[j].right = _wtoi(strText);
					}
					else
					{
						strLabel = m_ctrlParaList.GetItemText(nItemIdx - 1, 0);
						if(LimiteRangeCheck(dTemp, CAM_WIDTH, _wtof(strText), StepPara.InspPara[i].InspRect[j].right, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						{

						}
					}

					strText = m_ctrlParaList.GetItemText(nItemIdx++, i + 1);
					dTemp = _wtoi(strText);
					strText = m_ctrlParaList.GetItemText(nItemIdx++, i + 1);

					if(dTemp < _wtof(strText))
					{
						StepPara.InspPara[i].InspRect[j].top = (int)dTemp;
						StepPara.InspPara[i].InspRect[j].bottom = _wtoi(strText);
					}
					else
					{
						strLabel = m_ctrlParaList.GetItemText(nItemIdx - 1, 0);
						if(LimiteRangeCheck(dTemp, CAM_HEIGHT, _wtof(strText), StepPara.InspPara[i].InspRect[j].bottom, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						{

						}
					}

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(0., 1., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].nDirection, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].nDirection = _wtoi(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(3., 10., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].nGaousKenel, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].nGaousKenel = _wtoi(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(0., 10., _wtof(strText), StepPara.InspPara[i].FilterPara[j].dGaousSigma, LIMITE_RANGE_CHECK_TYPE_DOUBLE), strCam + strLabel)
						StepPara.InspPara[i].FilterPara[j].dGaousSigma = _wtof(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(0., 255., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].nThreshHold, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].nThreshHold = _wtoi(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(0., 100., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].cMorpSize.x, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].cMorpSize.x = _wtoi(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(0., 100., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].cMorpSize.y, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].cMorpSize.y = _wtoi(strText);

					strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
					strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
					if(LimiteRangeCheck(1., 100., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].nSizeThresh, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
						StepPara.InspPara[i].FilterPara[j].nSizeThresh = _wtoi(strText);
#ifndef NEW_ALG2
					if(StepPara.nInspPoint[i] == INSP_POINT1 || StepPara.nInspPoint[i] == INSP_POINT4)
#endif
					{
						strText = m_ctrlParaList.GetItemText(nItemIdx, i + 1);
						strLabel = m_ctrlParaList.GetItemText(nItemIdx++, 0);
						if(LimiteRangeCheck(0., 1., _wtoi(strText), StepPara.InspPara[i].FilterPara[j].nSearchDirection, LIMITE_RANGE_CHECK_TYPE_INT, strCam + strLabel))
							StepPara.InspPara[i].FilterPara[j].nSearchDirection = _wtoi(strText);
					}
				}
			}

			m_InspPara[TOP_CAMERA] = StepPara.InspPara[TOP_CAMERA];
			m_InspPara[BOTTOM_CAMERA] = StepPara.InspPara[BOTTOM_CAMERA];
			for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
			{
				for(int j = 0; j < MAX_INSP_CNT; j++)
				{
					for(int k = 0; k < MAX_LIGHT_CNT; k++)
					{
						m_SeqPara.InspectionPara[m_iSelectListItemNum].nLightVal[i][j][k] = StepPara.nLightVal[i][j][k];
					}
				}
			}
		}
	}
	else
	{
		m_ctrlParaList.DeleteAllItems();
		InitParaLIstLabel();

		for(int i = 0; i < MAX_CAM_CNT / 2; i++)
		{
			int z = 0;
			m_InspPara[i] = m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i];
			strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].bOneGrabInspection);
			m_ctrlParaList.SetItemText(z++, i + 1, strText);
			for(int j = 0; j < nMaxInspCnt; j++)
			{
				if(j < MAX_INSP_CNT)
				{
					for(int k = 0; k < MAX_LIGHT_CNT; k++)
					{
						strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].nLightVal[i][j][k]);
						m_ctrlParaList.SetItemText(z++, i + 1, strText);
					}
				}
				
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].InspRect[j].left);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].InspRect[j].right);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].InspRect[j].top);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].InspRect[j].bottom);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);

				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].nDirection);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].nGaousKenel);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%0.3f"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].dGaousSigma);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].nThreshHold);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].cMorpSize.x);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].cMorpSize.y);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);
				strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].nSizeThresh);
				m_ctrlParaList.SetItemText(z++, i + 1, strText);

#ifndef NEW_ALG2
				if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT1 || m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT4)
#endif
				{
					strText.Format(_T("%d"), m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[i].FilterPara[j].nSearchDirection);
					m_ctrlParaList.SetItemText(z++, i + 1, strText);
				}
			}
		}

		UpdateData(FALSE);
	}
}

void CDialogTeach::InitParaLIstLabel()
{
	int nRowCnt = 0;
	CString strText, strTemp;
	int nMaxInspCnt = MAX_INSP_CNT;

	for(int i = 0; i < MAX_CAM_CNT / 2; i++)
	{
		if(m_SeqPara.InspectionPara.size() > 0)
		{
			if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT2 ||
				m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT5)
			{
				nMaxInspCnt = MAX_INSP_CNT;
				break;
			}
		}
	}

	strText.Format(_T("One Grab Insp\nRange (0 = Tow Grab, 1 = One Grab)"));
		m_ctrlParaList.InsertItem(nRowCnt++, strText);

	for(int i = 0; i < nMaxInspCnt; i++)
	{
		switch(i)
		{
		case 0:
			if(m_SeqPara.InspectionPara.size() > 0)
			{
				if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT1 ||
					m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT4)
				{
					strTemp.Format(_T("UB Search"));
				}
				else
				{
#ifdef MODEL_BLOOM
					strTemp.Format(_T("Metal Search"));
#else
					strTemp.Format(_T("Bracket Search"));
#endif
				}
			}
			else
			{
				strTemp.Format(_T("Bracket Search"));
			}
			break;
		case 1:
			if(m_SeqPara.InspectionPara.size() > 0)
			{
				if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT1 ||
					m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT4)
				{
					strTemp.Format(_T("Metal Search"));
				}
				else
				{
					strTemp.Format(_T("Projection Search"));
				}
			}
			else
			{
				strTemp.Format(_T("Projection Search"));
			}
			break;
		}
		//if(i < MAX_INSP_CNT)
		{
			strText.Format(_T("%s 동축 조명\nRange (0 ~ 255)"), strTemp);
			m_ctrlParaList.InsertItem(nRowCnt++, strText);
			strText.Format(_T("%s 왼쪽 사이드조명\nRange (0 ~ 255)"), strTemp);
			m_ctrlParaList.InsertItem(nRowCnt++, strText);
			strText.Format(_T("%s 오른쪽 사이드 조명\nRange (0 ~ 255)"), strTemp);
			m_ctrlParaList.InsertItem(nRowCnt++, strText);
		}

		strText.Format(_T("%s 왼쪽 검사 영역\nRange (0 ~ Rect Right - 5)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s 오른쪽 검사 영역\nRange (Rect Left - 5 ~ 3840)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s 위쪽 검사 영역\nRange (0 ~ Rect Bottom - 5)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s 아래쪽 검사 영역\nRange (Rect Top - 5 ~ 2748)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);

		strText.Format(_T("%s Edge Direction\nRange (0 = VERTICAL, 1 = HORIZONTAL)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s Gaous Kenel Size\nRange (3 ~ 10)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s Gaous Sigma\nRange (0 ~ 10)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s ThreshHold\nRange (0 ~ 255)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s Morp Size X\nRange (0 ~ 100)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s Morp Size Y\nRange (0 ~ 100)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);
		strText.Format(_T("%s Size Thresh\nRange (1 ~ 100)"), strTemp);
		m_ctrlParaList.InsertItem(nRowCnt++, strText);

		if(m_SeqPara.InspectionPara.size() > 0)
		{
#ifndef NEW_ALG2
			if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT1 || m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT4)
#endif
			{
				if(m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT1 ||
					m_SeqPara.InspectionPara[m_iSelectListItemNum].nInspPoint[i] == INSP_POINT4)
				{
					strText.Format(_T("%s Search Direction\nRange (0:Top To Bot, 1:Bot To Top)"), strTemp);
				}
				else
				{
					strText.Format(_T("%s Search Direction\nRange (0:Right To Left, 1:Left To Right)"), strTemp);
				}
				m_ctrlParaList.InsertItem(nRowCnt++, strText);
			}
		}
	}
}

BOOL CDialogTeach::InitList()
{
	BOOL rslt = TRUE;

	int i(0), iID(0);
	DWORD dwStyle;
	CString strText(_T(""));
	LV_COLUMN lvcolumn; 
	//	LV_ITEM lvitem; 
	TCHAR *sStepSideList[SEQ_LIST_SIDE_MAX_COL_COUNT] = {_T("Step"), _T("Right Cam"), _T("Left Cam")};
														//_T("X Pos"), _T("Y Pos"), _T("Z Pos_0"), _T("Z Pos_1")};

	int iStepSideWidth[SEQ_LIST_SIDE_MAX_COL_COUNT] = {65, 90, 90};//, 70, 70, 70, 70};

	dwStyle = m_ctrlStepList.GetExtendedStyle(); 
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EDITLABELS ;// | LVS_EX_FULLROWSELECT; 
	m_ctrlStepList.SetExtendedStyle(dwStyle); 	

	for(i = 0; i < SEQ_LIST_SIDE_MAX_COL_COUNT; i++)
	{ 
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
		lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH; 
		lvcolumn.pszText = sStepSideList[i]; 
		lvcolumn.iSubItem = i; 
		lvcolumn.cx = iStepSideWidth[i]; 
		m_ctrlStepList.InsertColumn(i, &lvcolumn); 
	}

	TCHAR *sInspSideList[INSP_LIST_SIDE_MAX_COL_COUNT] = {_T("Parameter Name"), _T("Right Cam"), _T("Left Cam")};

	int iInspSideWidth[INSP_LIST_SIDE_MAX_COL_COUNT] = {400, 117, 117}; //300+167+167

	dwStyle = m_ctrlParaList.GetExtendedStyle(); 
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EDITLABELS ;// | LVS_EX_FULLROWSELECT; 
	m_ctrlParaList.SetExtendedStyle(dwStyle); 	

	for(i = 0; i < INSP_LIST_SIDE_MAX_COL_COUNT; i++)
	{ 
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
		lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH; 
		lvcolumn.pszText = sInspSideList[i]; 
		lvcolumn.iSubItem = i; 
		lvcolumn.cx = iInspSideWidth[i]; 
		m_ctrlParaList.InsertColumn(i, &lvcolumn); 
	}

	InitParaLIstLabel();

	return rslt;
}

BOOL CDialogTeach::InitDialog()
{
	BOOL rslt = TRUE;

	m_strLogString.Format(_T("Teach Init Dialog Start"));
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_LOG_WRITE, 0);

	m_dlgSpec = new CDialogSpec();
#ifdef MODEL_BLOOM
	m_dlgSpec->Create(IDD_DIALOG_SPEC_BLOOM, this);
#else
	m_dlgSpec->Create(IDD_DIALOG_SPEC, this);
#endif
	m_dlgSpecPoint = new CDialogSpecPoint();
#ifdef MODEL_BLOOM
	m_dlgSpecPoint->Create(IDD_DIALOG_SPEC_POINT_BLOOM, this);
#else
	m_dlgSpecPoint->Create(IDD_DIALOG_SPEC_POINT, this);
#endif

	/*m_dlgSystem = new CDialogSystem();
	m_dlgSystem->Create(IDD_DIALOG_SYSTEM, this);*/

	/*m_dlgJog = new CDialogJog();
	m_dlgJog->Create(IDD_DIALOG_JOG, this);*/

	/*m_dlgSystem->GetSystemPara(&m_SystemPara);
	m_strUseModelName.Format(_T("%s"), m_SystemPara.strUseModelName);*/

	//DisplayModelList();
	//DisplayModelName();

	/*LoadSeqPara(m_strUseModelName);
	if(m_SeqPara.InspectionPara.size() > 0)
	{
		m_InspPara[0] = m_SeqPara.InspectionPara[0].InspPara[0];
		m_InspPara[1] = m_SeqPara.InspectionPara[0].InspPara[1];
		m_dlgSpec->SetSpec(m_SeqPara.Spec);
		m_dlgSpec->SetSpread(m_SeqPara.Spread);

		m_dlgSpecPoint->SetSpec(m_SeqPara.SpecPoint);
		m_dlgSpecPoint->SetSpread(m_SeqPara.SpreadPoint);
	}*/

#ifdef LIGHT
	if(!m_pSerialComm->Open(LIGHT_COM_PORT))
	{
		m_strLogString.Format(_T("LIght Com Port %d Open Fail"), LIGHT_COM_PORT);
		AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_LOG_WRITE, 0);
	}
#endif
	m_strLogString.Format(_T("Teach Init Dialog End"));
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_LOG_WRITE, 0);
	return rslt;
}

void CDialogTeach::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nEvent = (int)nIDEvent;
	CRect rect;
	switch(nEvent)
	{
	case TEACH_TIMER_WINDOWS_VISIBLE:
		if(this->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_WINDOWS_VISIBLE);
			this->GetWindowRect(&rect);
			m_dlgSpec->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
			//m_dlgSystem->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
			m_dlgSpecPoint->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());

			for(int i = 0; i < (MAX_LIGHT_CNT * MAX_CAM_CNT); i++)
			{
				SetLightOnOff(i, TRUE);
			}

			this->ShowWindow(SW_HIDE);
		}
		break;
	case TEACH_TIMER_SPEC_SHOW_WINDOW:
		if(m_dlgSpec->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_SPEC_SHOW_WINDOW);
			SetTimer(TEACH_TIMER_SPEC_HIDE_WINDOW, 100, NULL);
		}
		break;
	case TEACH_TIMER_SPEC_HIDE_WINDOW:
		if(!m_dlgSpec->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_SPEC_HIDE_WINDOW);
			if(m_dlgSpec->GetSave())
			{
				m_dlgSpec->GetSpec(m_SeqPara.Spec);
				m_dlgSpec->GetSpread(m_SeqPara.Spread);
				m_dlgSpec->GetPointOffset(m_SeqPara.SpecPoint);
				ClickBtnenhctrlTeachSave();
			}
		}
		break;
	case TEACH_TIMER_SPEC_POINT_SHOW_WINDOW:
		if(m_dlgSpecPoint->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_SPEC_POINT_SHOW_WINDOW);
			SetTimer(TEACH_TIMER_SPEC_POINT_HIDE_WINDOW, 100, NULL);
		}
		break;
	case TEACH_TIMER_SPEC_POINT_HIDE_WINDOW:
		if(!m_dlgSpecPoint->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_SPEC_POINT_HIDE_WINDOW);
			if(m_dlgSpecPoint->GetSave())
			{
				m_dlgSpecPoint->GetSpec(m_SeqPara.SpecPoint);
				m_dlgSpecPoint->GetSpread(m_SeqPara.SpecPoint);
				ClickBtnenhctrlTeachSave();
			}
		}
		break;
	case TEACH_TIMER_SYSTEM_HIDE_WINDOW:
		//if(!m_dlgSystem->IsWindowVisible())
		{
			KillTimer(TEACH_TIMER_SYSTEM_HIDE_WINDOW);
			/*m_dlgSystem->GetSystemPara(&m_SystemPara);
			m_strUseModelName.Format(_T("%s"), m_SystemPara.strUseModelName);
			LoadSeqPara(m_strUseModelName);
			UpdatePara(FALSE);*/
		}
		break;
	case TEACH_TIMER_INPUT_SEQ_LIST_VAL:
		if(m_ctrlStepList.m_bInputNum)
		{
			if(!m_ctrlStepList.m_dlgNum->IsWindowVisible())
			{
				m_ctrlStepList.EndModify();
			}
		}
		break;
	case TEACH_TIMER_INPUT_PARA_LIST_VAL:
		if(m_ctrlParaList.m_bInputNum)
		{
			if(!m_ctrlParaList.m_dlgNum->IsWindowVisible())
			{
				m_ctrlParaList.EndModify();
			}
		}
		break;
	default:

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CDialogTeach, CDialogEx)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_SPEC, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachSpec, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_SYSTEM, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachSystem, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_JOG, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachJog, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_SAVE, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachSave, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_CANCEL, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachCancel, VTS_NONE)
	//ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_MODEL_ADD, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachModelAdd, VTS_NONE)
	//ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_MODEL_DEL, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachModelDel, VTS_NONE)
	//ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_MODEL_CHANGE, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachModelChange, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_SELECT_STEP_MOVE_Z0, DISPID_CLICK, CDialogTeach::SelectStepMoveZ, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_SELECT_STEP_MOVE_Z1, DISPID_CLICK, CDialogTeach::SelectStepMoveZ, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_WAIT_POS_MOVE, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachWaitPosMove, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_TEACH_CYL_ALL_FW_BK, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlTeachCylAllFwBk, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_SPEC_POINT, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlSpecPoint, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogTeach::ClickBtnenhctrlTeachSpec()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_dlgSpec->ShowWindow(SW_SHOW);
	SetTimer(TEACH_TIMER_SPEC_SHOW_WINDOW, 100, NULL);
}


void CDialogTeach::ClickBtnenhctrlTeachSystem()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	/*m_dlgSystem->ShowWindow(SW_SHOW);
	SetTimer(TEACH_TIMER_SYSTEM_HIDE_WINDOW, 100, NULL);*/
}

void CDialogTeach::OnNMRClickListSeqStep(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if(pNMItemActivate->uChanged)
	{

	}
	else
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		if(pNMLV->iItem >= -1)
		{
			m_iSelectListItemNum = pNMLV->iItem;
			if(pNMLV->iItem >= 0)
				SelectSeqIdxVisionData(pNMLV->iItem);
			
			AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_SELECT_SEQ_CHANGE, m_iSelectListItemNum);

			CMenu popup;
			CMenu *MyMenu;
			CPoint pt;
			GetCursorPos(&pt);
			popup.LoadMenu(IDR_MENU1);
			MyMenu = popup.GetSubMenu(0);
			MyMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}	

	*pResult = 0;
}


void CDialogTeach::OnMenuUp()
{
	if(m_iSelectListItemNum <= 0)
		return;

	TypeInspectionPara step;
	vector<TypeInspectionPara>::iterator it;
	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;

	step.nSeqIdx = it->nSeqIdx;
	for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
	{
		step.nInspPoint[i] = it->nInspPoint[i];
		step.InspPara[i] = it->InspPara[i];
	}

	for(int i = 0 ; i < MAX_MOTION_CNT + 1; i++)
	{
		step.dMotionVal[i] = it->dMotionVal[i];
	}

	for(int i = 0; i < MAX_LIGHT_CNT; i++)
	{
		for(int j = 0; j < MAX_GRAB_CNT; j++)
		{
			step.nLightVal[0][j][i] = it->nLightVal[0][j][i];
			step.nLightVal[1][j][i] = it->nLightVal[1][j][i];
		}
	}
	
	m_SeqPara.InspectionPara.insert(it - 1, step);

	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
	m_SeqPara.InspectionPara.erase(it + 1);  // Step 레시피에 제거

	int index;
	for(index = 0, it = m_SeqPara.InspectionPara.begin(); it!=m_SeqPara.InspectionPara.end(); index++, it++)
		it->nSeqIdx = index; 

	UpdatePara(FALSE);

	m_ctrlStepList.SetItemState(-1, 0, LVIS_SELECTED);
}

void CDialogTeach::OnMenuDown()
{
	if(m_iSelectListItemNum < 0)
		return;

	if((m_SeqPara.InspectionPara.size() - 1) <= m_iSelectListItemNum)
	{
		return;
	}

	TypeInspectionPara step;
	vector<TypeInspectionPara>::iterator it;
	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum + 1;

	step.nSeqIdx = it->nSeqIdx;
	for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
	{
		step.nInspPoint[i] = it->nInspPoint[i];
		step.InspPara[i] = it->InspPara[i];
	}

	for(int i = 0 ; i < MAX_MOTION_CNT + 1; i++)
	{
		step.dMotionVal[i] = it->dMotionVal[i];
	}

	for(int i = 0; i < MAX_LIGHT_CNT; i++)
	{
		for(int j = 0; j < MAX_GRAB_CNT; j++)
		{
			step.nLightVal[0][j][i] = it->nLightVal[0][j][i];
			step.nLightVal[1][j][i] = it->nLightVal[1][j][i];
		}
	}
	
	m_SeqPara.InspectionPara.insert(it - 1, step);

	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
	m_SeqPara.InspectionPara.erase(it + 2);  // Step 레시피에 제거

	int index;
	for(index = 0, it = m_SeqPara.InspectionPara.begin(); it!=m_SeqPara.InspectionPara.end(); index++, it++)
		it->nSeqIdx = index; 

	UpdatePara(FALSE);

	m_ctrlStepList.SetItemState(-1, 0, LVIS_SELECTED);
}

void CDialogTeach::OnMenuMove1()
{
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_SELECT_SEQ_POS_MOVE_0, m_iSelectListItemNum);
}

void CDialogTeach::OnMenuMove2()
{
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_SELECT_SEQ_POS_MOVE_1, m_iSelectListItemNum);
}

void CDialogTeach::OnMenuInsert()
{
	if(m_iSelectListItemNum < 0)
		return;

	TypeInspectionPara step;
	vector<TypeInspectionPara>::iterator it;
	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
	
	for(int i = 0 ; i < MAX_CAM_CNT / 2; i++)
	{
		step.nInspPoint[i] = it->nInspPoint[i];

		step.InspPara[i] = it->InspPara[i];
	}	

	for(int i = 0 ; i < MAX_MOTION_CNT + 1; i++)
	{
		step.dMotionVal[i] = it->dMotionVal[i];
	}

	for(int i = 0; i < MAX_LIGHT_CNT; i++)
	{
		for(int j = 0; j < MAX_GRAB_CNT; j++)
		{
			step.nLightVal[0][j][i] = it->nLightVal[0][j][i];
			step.nLightVal[1][j][i] = it->nLightVal[1][j][i];
		}
	}

	m_SeqPara.InspectionPara.insert(it + 1, step);

	int index;
	for(index = 0, it = m_SeqPara.InspectionPara.begin(); it != m_SeqPara.InspectionPara.end(); index++, it++)
		it->nSeqIdx = index; 

	m_SeqPara.nMaxSeqIdx = m_SeqPara.InspectionPara.size();

	m_iSelectListItemNum++;

	UpdatePara(FALSE);

	m_ctrlStepList.SetItemState(-1, 0, LVIS_SELECTED);
}

void CDialogTeach::OnMenuDelete()
{
	if(m_iSelectListItemNum <= 0)
		return;

	vector<TypeInspectionPara>::iterator it;
	it = m_SeqPara.InspectionPara.begin() + m_iSelectListItemNum;
	
	if(it == m_SeqPara.InspectionPara.end() - 1)
		m_iSelectListItemNum--; // 맨 마지막 Step을 지우면 선택된
	m_SeqPara.InspectionPara.erase(it);  // Step 레시피에 제거

	int index;
	for(index = 0, it = m_SeqPara.InspectionPara.begin(); it != m_SeqPara.InspectionPara.end(); index++, it++)
		it->nSeqIdx = index; 

	m_SeqPara.nMaxSeqIdx = m_SeqPara.InspectionPara.size();

	UpdatePara(FALSE);

	m_ctrlStepList.SetItemState(-1, 0, LVIS_SELECTED);
}

void CDialogTeach::ClickBtnenhctrlTeachJog()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//m_dlgJog->ShowWindow(SW_SHOW);
}

void CDialogTeach::SetMainWindowRect(CRect rect)
{
	m_MainWIndowRect = rect;
	m_ctrlStepList.m_MainWindowRect = rect;
	m_ctrlParaList.m_MainWindowRect = rect;
	m_ctrlStepList.m_bDBClicNumDlg = TRUE;
	m_ctrlParaList.m_bDBClicNumDlg = TRUE;
}

void CDialogTeach::OnNMClickListSeqStep(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(pNMItemActivate->uChanged)
	{

	}
	else
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		if(pNMLV->iItem >= 0)
		{
			m_iSelectListItemNum = pNMLV->iItem;
			SelectSeqIdxVisionData(pNMLV->iItem);
			AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_SELECT_SEQ_CHANGE, m_iSelectListItemNum);
		}
	}

	m_ctrlStepList.m_dlgNum->ShowWindow(SW_HIDE);
	m_ctrlParaList.m_dlgNum->ShowWindow(SW_HIDE);

	*pResult = 0;
}


void CDialogTeach::OnNMThemeChangedListSeqStep(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if(pNMLV && (pNMLV->uNewState ==(UINT)(LVIS_FOCUSED | LVIS_SELECTED)))
	{
		vector<TypeInspectionPara>::iterator it;
		it = m_SeqPara.InspectionPara.begin();
		
		if(m_iSelectListItemNum < 0)
			return;

		m_iSelectListItemNum = pNMLV->iItem;
		SelectSeqIdxVisionData(m_iSelectListItemNum);

		it + m_iSelectListItemNum;

	}

	*pResult = 0;
}

void CDialogTeach::SetSelectSeqAllCurPos(double *dCurPos, int nInspIdx)
{
	for(int i = 0; i < MAX_MOTION_CNT; i++)
	{
		m_dCurPos[i] = dCurPos[i];
	}
	m_dCurPos[MAX_MOTION_CNT + nInspIdx] = dCurPos[MAX_MOTION_CNT];
}

void CDialogTeach::SetSelectSeqCurPos(int nSeqIdx, int nMotionIdx, double dCurPos, int nInspIdx)
{
	if(nMotionIdx >= MAX_MOTION_CNT - 1 && nInspIdx == 1)
	{
		m_dCurPos[nMotionIdx] = dCurPos;
		m_SeqPara.InspectionPara[nSeqIdx].dMotionVal[nMotionIdx + nInspIdx] = dCurPos;
	}
	else
	{
		m_dCurPos[nMotionIdx] = dCurPos;
		m_SeqPara.InspectionPara[nSeqIdx].dMotionVal[nMotionIdx] = dCurPos;
	}
}

BOOL CDialogTeach::SaveSeqPara(CString strModel)
{
	BOOL rslt = TRUE;
	CString key, dat, app;
	CString file_path, temp_path, strLog, strBackupFilePath;
	int nMaxInspCnt = MAX_INSP_CNT;
	
	MakeDirectories(MODEL_FILE_PATH);
	CString strFileName;
	strFileName.Format(_T("%s_Ch%d"), strModel, m_nChannelIdx + 1);
	file_path.Format(_T("%s%s"), MODEL_FILE_PATH, strFileName);

	FileWriteTimeToBackupFile(MODEL_FILE_PATH, BACKUP_FILE_PATH, strFileName);

	
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{

		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		app.Format(_T("MODEL_INFO"));
		dat.Empty();
		sprintf_s(m_SeqPara.cDGSModelName, m_strDGSModelName.GetLength() + 1, "%S", m_strDGSModelName);
		dat.Format(_T("%S"), m_SeqPara.cDGSModelName);
		WritePrivateProfileString(app, _T("DGS_MODEL_NAME"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SeqPara.InspectionPara.size());
		WritePrivateProfileString(app, _T("MAX_INSP_STEP"), dat, file_path);
		m_SeqPara.nMaxSeqIdx = m_SeqPara.InspectionPara.size();

		for(int i = 0; i < m_SeqPara.nMaxSeqIdx; i++)
		{
			app.Format(_T("STEP_INDEX_%02d"), m_SeqPara.InspectionPara[i].nSeqIdx + 1);
			for(int j = 0; j < MAX_MOTION_CNT + 1; j++)
			{
				key.Empty();
				key.Format(_T("MOTION_POS_VAL_%d"), j);
				dat.Empty();
				dat.Format(_T("%0.3f"), m_SeqPara.InspectionPara[i].dMotionVal[j]);
				WritePrivateProfileString(app, key, dat, file_path);
			}

			for(int j = 0; j < MAX_CAM_CNT / 2; j++)
			{
				//if(m_SeqPara.InspectionPara[i].nInspPoint[j] != INSP_SKIP)
				{
					key.Empty();
					key.Format(_T("CAM_%d_INSP_POINT"), j);
					dat.Empty();
					dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].nInspPoint[j]);
					WritePrivateProfileString(app, key, dat, file_path);

					key.Empty();
					key.Format(_T("CAM_%d_ONE_GRAB_INSP"), j);
					dat.Empty();
					dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].bOneGrabInspection);
					WritePrivateProfileString(app, key, dat, file_path);

					if(m_SeqPara.InspectionPara[i].nInspPoint[j] == INSP_POINT2 ||
						m_SeqPara.InspectionPara[i].nInspPoint[j] == INSP_POINT5)
					{
						nMaxInspCnt = MAX_INSP_CNT;
					}

					for(int k = 0; k < nMaxInspCnt; k++)
					{
						if(k < MAX_INSP_CNT)
						{
							for(int l = 0; l < MAX_LIGHT_CNT; l++)
							{
								key.Empty();
								key.Format(_T("CAM_%d_INSP_IDX_%d_LIGHT_IDX_%d_VAL"), j, k, l);
								dat.Empty();
								dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].nLightVal[j][k][l]);
								WritePrivateProfileString(app, key, dat, file_path);
							}
						}

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_LEFT"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].InspRect[k].left);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_RIGHT"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].InspRect[k].right);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_TOP"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].InspRect[k].top);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_BOTTOM"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].InspRect[k].bottom);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_EDGE_DIRECTION"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].nDirection);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_GAOUS_KENEL"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].nGaousKenel);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_GAOUS_SIGMA"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].dGaousSigma);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_THRESH_HOLD"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].nThreshHold);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_MORP_SIZE_X"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].cMorpSize.x);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_MORP_SIZE_Y"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].cMorpSize.y);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_SIZE_THRESH"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].nSizeThresh);
						WritePrivateProfileString(app, key, dat, file_path);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_SEARCH_DIRECTION"), j, k);
						dat.Empty();
						dat.Format(_T("%d"), m_SeqPara.InspectionPara[i].InspPara[j].FilterPara[k].nSearchDirection);
						WritePrivateProfileString(app, key, dat, file_path);
					}
				}
			}
		}
		
		app.Empty();
		app.Format(_T("MODEL_SPEC"));
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.Spec[i].dMinSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.Spec[i].dMaxSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_OFF_SET"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.Spec[i].dOffset);
			WritePrivateProfileString(app, key, dat, file_path);
		}

		app.Empty();
		app.Format(_T("MODEL_POINT_SPEC"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.SpecPoint[i].dMinSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.SpecPoint[i].dMaxSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_OFF_SET"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.SpecPoint[i].dOffset);
			WritePrivateProfileString(app, key, dat, file_path);
		}

		app.Empty();
		app.Format(_T("MODEL_SPREAD"));
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.Spread[i].dMinSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.Spread[i].dMaxSpec);
			WritePrivateProfileString(app, key, dat, file_path);
		}

		app.Empty();
		app.Format(_T("MODEL_POINT_SPREAD"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.SpreadPoint[i].dMinSpec);
			WritePrivateProfileString(app, key, dat, file_path);
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_SeqPara.SpreadPoint[i].dMaxSpec);
			WritePrivateProfileString(app, key, dat, file_path);
		}
	}

	return rslt;
}

BOOL CDialogTeach::LoadSeqPara(CString strModel)
{
	BOOL rslt = TRUE;
	CString key, dat, app, temp_default;
	CString file_path;
	TypeInspectionPara insPara;
	int nMaxInspCnt = MAX_INSP_CNT;
	//memset(&insPara, NULL, sizeof(TypeInspectionPara));
	CString strFileName;
	strFileName.Format(_T("%s_Ch%d"), strModel, m_nChannelIdx + 1);
	file_path.Format(_T("%s%s"), MODEL_FILE_PATH, strFileName);

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		m_SeqPara.nMaxSeqIdx = 0;
		m_SeqPara.InspectionPara.clear();
		app.Empty();
		app.Format(_T("MODEL_INFO"));
		dat.Empty();
		GetPrivateProfileString(app, _T("DGS_MODEL_NAME"), _T("F-700"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		if(dat.GetLength() == 0)
			dat.Format(_T("F-700"));
		
		sprintf_s(m_SeqPara.cDGSModelName, dat.GetLength() + 1, "%S", dat);
		m_strDGSModelName.Format(_T("%s"), dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("MAX_INSP_STEP"), _T("1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SeqPara.nMaxSeqIdx = _wtoi(dat);

		for(int i = 0; i < m_SeqPara.nMaxSeqIdx; i++)
		{
			insPara.nSeqIdx = i;
			app.Format(_T("STEP_INDEX_%02d"), insPara.nSeqIdx + 1);
			for(int j = 0; j < MAX_MOTION_CNT + 1; j++)
			{
				key.Empty();
				key.Format(_T("MOTION_POS_VAL_%d"), j);
				dat.Empty();
				GetPrivateProfileString(app, key, _T("10.000"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				insPara.dMotionVal[j] = _wtof(dat);
			}

			for(int j = 0; j < MAX_CAM_CNT / 2; j++)
			{
				key.Empty();
				key.Format(_T("CAM_%d_INSP_POINT"), j);
				dat.Empty();
				temp_default.Format(_T("%d"), j + 1);
				GetPrivateProfileString(app, key, temp_default, (LPWSTR)(LPCWSTR)dat, 256, file_path);
				insPara.nInspPoint[j] = _wtoi(dat);
				//if(insPara.nInspPoint[j] != INSP_SKIP)
				{
					key.Empty();
					key.Format(_T("CAM_%d_ONE_GRAB_INSP"), j);
					dat.Empty();
					GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
					insPara.InspPara[j].bOneGrabInspection = _wtoi(dat);

					if(insPara.nInspPoint[j] == INSP_POINT2 ||
						insPara.nInspPoint[j] == INSP_POINT5)
					{
						nMaxInspCnt = MAX_INSP_CNT;
					}

					for(int k = 0; k < nMaxInspCnt; k++)
					{
						if(k < MAX_INSP_CNT)
						{
							for(int l = 0; l < MAX_LIGHT_CNT; l++)
							{
								key.Empty();
								key.Format(_T("CAM_%d_INSP_IDX_%d_LIGHT_IDX_%d_VAL"), j, k, l);
								dat.Empty();
								GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
								insPara.nLightVal[j][k][l] = _wtoi(dat);
							}
						}
						
						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_LEFT"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].InspRect[k].left = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_RIGHT"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].InspRect[k].right = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_TOP"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].InspRect[k].top = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_INSP_RECT_BOTTOM"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].InspRect[k].bottom = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_EDGE_DIRECTION"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].nDirection = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_GAOUS_KENEL"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("3"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].nGaousKenel = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_GAOUS_SIGMA"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("0.000"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].dGaousSigma = _wtof(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_THRESH_HOLD"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("125"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].nThreshHold = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_MORP_SIZE_X"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("10"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].cMorpSize.x = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_MORP_SIZE_Y"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("10"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].cMorpSize.y = _wtoi(dat);

						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_FILTER_SIZE_THRESH"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("50"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].nSizeThresh = _wtoi(dat);


						key.Empty();
						key.Format(_T("CAM_%d_INSP_IDX_%d_SEARCH_DIRECTION"), j, k);
						dat.Empty();
						GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						insPara.InspPara[j].FilterPara[k].nSearchDirection = _wtoi(dat);
					}
				}
			}
			m_SeqPara.InspectionPara.push_back(insPara);
		}

		app.Empty();
		app.Format(_T("MODEL_SPEC"));
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.015"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.Spec[i].dMinSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.055"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.Spec[i].dMaxSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_OFF_SET"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.000"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.Spec[i].dOffset = _wtof(dat);
		}

		app.Empty();
		app.Format(_T("MODEL_POINT_SPEC"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("1.015"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.SpecPoint[i].dMinSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("1.055"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.SpecPoint[i].dMaxSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_OFF_SET"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.000"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.SpecPoint[i].dOffset = _wtof(dat);
		}

		app.Empty();
		app.Format(_T("MODEL_SPREAD"));
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.050"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.Spread[i].dMinSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.050"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.Spread[i].dMaxSpec = _wtof(dat);

			m_SeqPara.Spread[i].dOffset = 0.;
		}

		app.Empty();
		app.Format(_T("MODEL_POINT_SPREAD"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			key.Empty();
			key.Format(_T("INSP_POINT_%d_MIN_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.050"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.SpreadPoint[i].dMinSpec = _wtof(dat);

			key.Empty();
			key.Format(_T("INSP_POINT_%d_MAX_VAL"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, key, _T("0.050"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_SeqPara.SpreadPoint[i].dMaxSpec = _wtof(dat);

			m_SeqPara.SpreadPoint[i].dOffset = 0.;
		}
	}

	return rslt;
}


void CDialogTeach::ClickBtnenhctrlTeachSave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdatePara(TRUE);
	SaveSeqPara(m_strUseModelName);
	UpdatePara(FALSE);

	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_APPLY_SEQ_PARA, 0);
}


void CDialogTeach::ClickBtnenhctrlTeachCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	this->ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_DIALOG_CANCEL, m_nChannelIdx);
}

void CDialogTeach::GetSpec(TypeSpecPara *Spec, int nSpectType)
{
	if(nSpectType == INSP_SPEC_TYPE_0)
	{
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			Spec[i].dMinSpec = m_SeqPara.Spec[i].dMinSpec;
			Spec[i].dMaxSpec = m_SeqPara.Spec[i].dMaxSpec;
			Spec[i].dOffset = m_SeqPara.Spec[i].dOffset;
		}
	}
	else
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			Spec[i].dMinSpec = m_SeqPara.SpecPoint[i].dMinSpec;
			Spec[i].dMaxSpec = m_SeqPara.SpecPoint[i].dMaxSpec;
			Spec[i].dOffset = m_SeqPara.SpecPoint[i].dOffset;
		}
	}
}

double CDialogTeach::GetSeqMovePos(int nSeqIdx, int nMotionIdx)
{
	return m_SeqPara.InspectionPara[nSeqIdx].dMotionVal[nMotionIdx];
}

BOOL CDialogTeach::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogTeach::SetSelectSeqInspRect(int nCamIdx, int nInspIdx, int nRectIdx, CRect rect)
{
	if(m_iSelectListItemNum >= 0)
	{
		m_SeqPara.InspectionPara[m_iSelectListItemNum].InspPara[nCamIdx].InspRect[nRectIdx] = rect;
		UpdateParaList(FALSE);
	}
}

void CDialogTeach::DisplayModelList()
{
	CString strFilePath,strFileName,strName,strInch;
	strFilePath = MODEL_FILE_PATH;
	strFileName = _T("*.*");

	CFileFind cFFind;	
	int nCount = 0;	
	m_lbxListBox.ResetContent();	
	strFilePath += strFileName;

	TRY { 
		BOOL bIsFile = cFFind.FindFile(strFilePath, 0);
		while(bIsFile)
		{
			bIsFile = cFFind.FindNextFile();
			strName = cFFind.GetFileName();
			if(strName != _T(".") && strName != _T("..") )
			{
				//strFilePath += strName;
				m_lbxListBox.AddString(strName);
			}
			Delay(2,TRUE);
		}
	}

	CATCH (CException, e)
	{
		cFFind.Close();
		e->ReportError();
		e->Delete();
		return;
	}
	END_CATCH

		cFFind.Close();
}

void CDialogTeach::DisplayModelName()
{
	m_ctrlWorkModelName.SetCaption(m_strUseModelName);
}
/*
void CDialogTeach::ClickBtnenhctrlTeachModelAdd()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetTextDlg	dlg(30, _T(""), _T("Input New Model Name"), NULL, FALSE);

	if(dlg.DoModal() == IDCANCEL) return;
	CString strModelName = dlg.GetStringValue();

	SaveSeqPara(strModelName);
	m_strSelectListModel = strModelName;
	LoadSeqPara(m_strUseModelName);

	DisplayModelList();
}


void CDialogTeach::ClickBtnenhctrlTeachModelDel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString str;
	CString strMsg; 

	CString cFile;
	CString strFilePath = MODEL_FILE_PATH + m_strSelectListModel;

	if(m_strSelectListModel == m_strUseModelName)
	{
		//g_cMsgView.OkMsgBox(_T("현재 사용 중인 Model은 삭제할 수 없습니다."));
		AfxMessageBox(_T("Use Model Not Delete."));
		return;
	}

	strMsg.Format(_T("Select Model Name < %s > is Delete?"), m_strSelectListModel);
	if(AfxMessageBox(strMsg, MB_YESNO) != IDYES)
	{
		return;
	}

	TRY
	{
		DeleteFolderAndFile(strFilePath);
	}

	CATCH( CFileException, e ) 
	{
		//g_cMsgView.OkMsgBox(_T("삭제 실패하였습니다."));
		return;
	}
	END_CATCH

	DisplayModelList();
}


void CDialogTeach::ClickBtnenhctrlTeachModelChange()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strMsg;

	strMsg.Format(_T("Select Model Name < %s > is Change?"), m_strSelectListModel);
	if(AfxMessageBox(strMsg, MB_YESNO) != IDYES)
	{
		return;
	}
	
	m_strUseModelName = m_strSelectListModel;
	//m_dlgSystem->GetSystemPara(&m_SystemPara);
	//m_SystemPara.strUseModelName.Format(_T("%s"), m_strUseModelName);
	//m_dlgSystem->SetSystemPara(m_SystemPara);
	//m_dlgSystem->SetSaveSystemPara();
	DisplayModelName();

	LoadSeqPara(m_strUseModelName);

	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_MODEL_CHANGE, m_iSelectListItemNum);
}


void CDialogTeach::OnLbnSelchangeListModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str,strModel;
	
	m_nModelListBoxNo = m_lbxListBox.GetCurSel();

	if(m_nModelListBoxNo >= 0)
	{
		m_lbxListBox.GetText(m_nModelListBoxNo, str);

		m_strSelectListModel = str;
	}
}*/

void CDialogTeach::SetLightOnOff(int nCh, BOOL isOn)
{
	char sPacket[MAX_PATH];
	char sLogPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(char) * MAX_PATH);
	memset(sLogPacket, 0, sizeof(char) * MAX_PATH);

	if(isOn)
	{
		sprintf_s(sPacket, "H%dON%c%c\0", nCh + 1, 0x0D,0x0A);	//ON
	}
	else
	{
		sprintf_s(sPacket, "H%dOF%c%c\0", nCh + 1, 0x0D,0x0A);	//OFF
	}
#ifdef LIGHT
	if(m_pSerialComm->Write(sPacket, (DWORD)strlen(sPacket)) == 0)
	{
		memcpy(sLogPacket, sPacket, 5);
		m_strLogString.Format(_T("Light Packet Write Fail, Packet : %S"), sLogPacket);
		AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_LOG_WRITE, 0);
	}
#endif
	Sleep(SERIAL_WRITE_DELAY);
}

void CDialogTeach::WritePacket(int nCh, BYTE byVal)
{
	char sPacket[MAX_PATH];
	char sLogPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(char) * MAX_PATH);
	memset(sLogPacket, 0, sizeof(char) * MAX_PATH);

	if(nCh > 8)
	{
		return;
	}
#ifdef LIGHT
	sprintf_s(sPacket, "C%d%02X%c%c\0", nCh + 1, byVal, 0x0D, 0x0A);

	if(m_pSerialComm->Write(sPacket, (DWORD)strlen(sPacket)) == 0)
	{
		memcpy(sLogPacket, sPacket, 5);
		m_strLogString.Format(_T("Light Packet Write Fail, Packet : %S"), sLogPacket);
		AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_LOG_WRITE, 0);
	}
#endif
	Sleep(SERIAL_WRITE_DELAY);
}


void CDialogTeach::OnNMClickListInspPara(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ctrlStepList.m_dlgNum->ShowWindow(SW_HIDE);
	m_ctrlParaList.m_dlgNum->ShowWindow(SW_HIDE);

	*pResult = 0;
}


void CDialogTeach::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(GetDlgItem(IDC_LIST_SEQ_STEP) || GetDlgItem(IDC_LIST_INSP_PARA))
	{
		lpMeasureItemStruct->itemHeight += 20;
	}

	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void CDialogTeach::SelectStepMoveZ()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	int nCtrlId = pBtnEnh->GetDlgCtrlID();
	
	switch(nCtrlId)
	{
	case IDC_BTNENHCTRL_TEACH_SELECT_STEP_MOVE_Z0:
		OnMenuMove1();
		break;
	case IDC_BTNENHCTRL_TEACH_SELECT_STEP_MOVE_Z1:
		OnMenuMove2();
		break;
	}
}


void CDialogTeach::ClickBtnenhctrlTeachWaitPosMove()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_SELECT_SEQ_POS_MOVE_0, -1);
}

BOOL CDialogTeach::GetJogDlgIsVisible()
{
	BOOL rslt = FALSE;
	//if(m_dlgJog) 
	//	rslt = m_dlgJog->IsWindowVisible();

	return rslt;
};

void CDialogTeach::ClickBtnenhctrlTeachCylAllFwBk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AfxGetMainWnd()->SendMessage(USE_MSG_TEACH_TO_MAIN_DIALOG, TEACH_TO_MAIN_CYL_ALL_FW_BK, m_bCYL_FW);
}


void CDialogTeach::ClickBtnenhctrlSpecPoint()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_dlgSpecPoint->ShowWindow(SW_SHOW);
	SetTimer(TEACH_TIMER_SPEC_POINT_SHOW_WINDOW, 100, NULL);
}

//20190424 ngh
void CDialogTeach::GetSpread(TypeSpecPara *Spread, int nSpreadType)
{
	if(nSpreadType == 0)
	{
		m_dlgSpec->GetSpread(Spread);
	}
	else
	{
		m_dlgSpecPoint->GetSpread(Spread);
	}
}

void CDialogTeach::SetModel(CString strModel)
{
	m_strUseModelName.Format(_T("%s"), strModel);
	if(LoadSeqPara(m_strUseModelName))
	{
		if(m_SeqPara.InspectionPara.size() > 0)
		{
			m_InspPara[0] = m_SeqPara.InspectionPara[0].InspPara[0];
			m_InspPara[1] = m_SeqPara.InspectionPara[0].InspPara[1];
			m_dlgSpec->SetSpec(m_SeqPara.Spec);
			m_dlgSpecPoint->SetSpec(m_SeqPara.SpecPoint);
			m_dlgSpec->SetSpread(m_SeqPara.Spread);
			m_dlgSpecPoint->SetSpread(m_SeqPara.SpreadPoint);
		}
	}
}

void CDialogTeach::SetModelAdd(CString strModel)
{
	SaveSeqPara(strModel);
}

void CDialogTeach::AllKillTimer()
{
	KillTimer(TEACH_TIMER_WINDOWS_VISIBLE);
	KillTimer(TEACH_TIMER_SPEC_SHOW_WINDOW);
	KillTimer(TEACH_TIMER_SPEC_HIDE_WINDOW);	
	KillTimer(TEACH_TIMER_SPEC_POINT_SHOW_WINDOW);
	KillTimer(TEACH_TIMER_SPEC_POINT_HIDE_WINDOW);
	KillTimer(TEACH_TIMER_SYSTEM_HIDE_WINDOW);
	KillTimer(TEACH_TIMER_INPUT_SEQ_LIST_VAL);
	KillTimer(TEACH_TIMER_INPUT_PARA_LIST_VAL);	
}