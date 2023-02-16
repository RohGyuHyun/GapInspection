#include "stdafx.h"
#include "Vision.h"
#include "cvBlobFinder.h"

//CVision Class
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVision::CVision(int nChannel)
{
	for(int i = 0; i < MAX_INSP_SEQ_CNT; i++)
	{
		m_ThreadPara[i].InspImage[0] = NULL;
		m_ThreadPara[i].DispImage[0] = NULL;
		m_ThreadPara[i].InspImage[1] = NULL;
		m_ThreadPara[i].DispImage[1] = NULL;
	}

	m_nMaxStepCnt = 0;
	m_nInspStepIdx = 0;

	for(int i = 0; i < MAX_INSP_SEQ_CNT; i++)
	{
		m_CInspection[i] = NULL;
	}
	m_CManualInspection = NULL;

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_Rslt[i].RsltImage = NULL;
	}
	m_nChannel = nChannel;
	m_cLogFile = NULL;
	InitVision(nChannel);
	InitRslt();
	//InitInspection();
}

CVision::~CVision()
{
	if(m_CManualInspection != NULL)
		delete m_CManualInspection;

	ReleaseInspection(TRUE);
	ReleaseImage();
	if(m_cLogFile)	delete m_cLogFile;
}

BOOL CVision::GetManualInspectionEnd(int nCamIdx)
{
	BOOL rslt = FALSE;

	if(m_CManualInspection != NULL)
	{
		if(!m_CManualInspection->GetInspecting(nCamIdx) && m_CManualInspection->GetInspectionEnd(nCamIdx))
		{
			rslt = TRUE;
			ReleaseThreadPara();
		}
	}

	return rslt;
}

void CVision::GetManualRsltGapVal(int nCamIdx, TypeInspEachRslt *InspRslt)
{
	m_CManualInspection->GetRslt(nCamIdx, InspRslt);
	delete m_CManualInspection;
	m_CManualInspection = NULL;
}

BOOL CVision::InitRslt()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Init Rslt Start"));
	m_cLogFile->WriteText(strText);

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_Rslt[i].bGapRslt = FALSE;
		m_Rslt[i].dGapRslt = 0.;
		m_Rslt[i].nInspPoint = INSP_SKIP;
		if(m_Rslt[i].RsltImage != NULL) cvReleaseImage(&m_Rslt[i].RsltImage);

		m_Rslt[i].RsltImage = NULL;
	}

	strText.Format(_T("Init Rslt End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

void CVision::SetManualInspection(int nInspStepIdx, int nCamIdx, CWnd *hwDisplay, int nDisplayId)
{
	CRect rect;
	CString strPath;
	m_nMaxStepCnt = m_SeqPara.InspectionPara.size();

	if(m_nMaxStepCnt > nInspStepIdx)
	{
		if(m_CManualInspection == NULL)
		{
			CString strText;
			strText.Format(_T("%d Step %d Cam Manual Inspection Start"), nInspStepIdx, nCamIdx);
			m_cLogFile->WriteText(strText);
			//init ThreadPara
			InitThreadPara();
			InitRslt();

			m_nInspStepIdx = nInspStepIdx;
			m_ThreadPara[m_nInspStepIdx].nChannel = m_nChannel;
			m_ThreadPara[m_nInspStepIdx].nSeqIdx = m_nInspStepIdx;
			m_ThreadPara[m_nInspStepIdx].strDrive.Format(_T("%s"), m_SystemPara.strDrive);

			m_ThreadPara[m_nInspStepIdx].nInspPoint[0] = INSP_SKIP;
			m_ThreadPara[m_nInspStepIdx].nInspPoint[1] = INSP_SKIP;

			m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] = m_SeqPara.InspectionPara[nInspStepIdx].nInspPoint[nCamIdx];
			m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx] = m_SeqPara.InspectionPara[nInspStepIdx].InspPara[nCamIdx];

#ifdef TEST_MANUAL_INSP
			if(!TestImageLoad(nCamIdx, m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx]))
			{
				strText.Format(_T("%d Step, %d CamIdx, %d InspPoint Test Image Load Error"), m_nInspStepIdx, nCamIdx, m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx]);
				m_cLogFile->WriteText(strText);
				return;
			}
#endif
			
			m_ThreadPara[m_nInspStepIdx].dPixelSizeWidth = m_SystemPara.dPixelSizeWidth;
			m_ThreadPara[m_nInspStepIdx].dPixelSizeHeight = m_SystemPara.dPixelSizeHeight;

			m_ThreadPara[m_nInspStepIdx].bInspImageSave = m_SystemPara.bInspImageSave;
			m_ThreadPara[m_nInspStepIdx].bRsltImageSave = m_SystemPara.bRsltImageSave;

			m_ThreadPara[m_nInspStepIdx].bPartitionInsp[0] = FALSE;//m_SystemPara.bPartitionInsp[0];
			m_ThreadPara[m_nInspStepIdx].bPartitionInsp[1] = FALSE;//m_SystemPara.bPartitionInsp[1];

			//m_SystemPara.bPartitionInsp[0] = TRUE;
			//m_SystemPara.bPartitionInsp[1] = TRUE;

			m_ThreadPara[m_nInspStepIdx].hwDisplay[0] = NULL;
			m_ThreadPara[m_nInspStepIdx].hwDisplay[1] = NULL;

			m_ThreadPara[m_nInspStepIdx].hwDisplay[nCamIdx] = hwDisplay;

			m_ThreadPara[m_nInspStepIdx].nDisplayId[0] = 0;
			m_ThreadPara[m_nInspStepIdx].nDisplayId[1] = 0;

			m_ThreadPara[m_nInspStepIdx].nDisplayId[nCamIdx] = nDisplayId;

			m_ThreadPara[m_nInspStepIdx].bTrace = m_SystemPara.bTrace;

			m_ThreadPara[m_nInspStepIdx].SpecPoint[nCamIdx] = m_SeqPara.SpecPoint[m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] - 1];

			m_ThreadPara[m_nInspStepIdx].nSelectAlgorithmIdx = m_SystemPara.nAlgorithmIdx;

			m_ThreadPara[m_nInspStepIdx].nMetalAlgorithmIdx = m_SystemPara.nMetalAlgorithmIdx;

			if(m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].bOneGrabInspection)
			{
				cvCopy(m_GrabImage[nCamIdx][0], m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx]);
				cvCvtColor(m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx], m_ThreadPara[m_nInspStepIdx].DispImage[nCamIdx], CV_GRAY2BGR);
			}
			else
			{
				if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT4)
				{
					//rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + 10;
					rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom) / 2);
					rect.bottom = m_GrabImage[nCamIdx][0]->height;
					rect.left = 0;
					rect.right = m_GrabImage[nCamIdx][0]->width; 
				}
				else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT1)
				{
					rect.top = 0;
					//rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - 10;
					rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom) / 2);
					rect.left = 0;
					rect.right = m_GrabImage[nCamIdx][0]->width;
				}
				else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT5)
				{
					rect.left = 0;
					rect.right = m_GrabImage[nCamIdx][0]->width;
					rect.top = 0;
					rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom) / 2);
					//rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + 10;
					
				}
				else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT2)
				{
					rect.left = 0;
					rect.right = m_GrabImage[nCamIdx][0]->width;
					rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom) / 2);
					//rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - 10;
					rect.bottom = m_GrabImage[nCamIdx][0]->height;
				}	
				

				MergeImage(m_GrabImage[nCamIdx][0], m_GrabImage[nCamIdx][1], rect, m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx]);

				cvCvtColor(m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx], m_ThreadPara[m_nInspStepIdx].DispImage[nCamIdx], CV_GRAY2BGR);
			}

			m_CManualInspection = new CVisionInspection(TRUE);
			CTime cTempTime;
			cTempTime = CTime::GetCurrentTime();
			strPath.Format(_T("%02d%02d%02d"), cTempTime.GetHour(), cTempTime.GetMinute(), cTempTime.GetSecond());
			m_CManualInspection->SetInspectionPara(m_ThreadPara[m_nInspStepIdx], m_cStart, strPath);
		}
	}

}

BOOL CVision::InitThreadPara()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Init Thread Para Start"));
	m_cLogFile->WriteText(strText);

	rslt = ThreadParaInitImage();

	if(rslt)
	{
		for(int j = 0; j < MAX_INSP_SEQ_CNT; j++)
		{
			if(m_SeqPara.InspectionPara.size() == j)
				break;

			m_ThreadPara[j].nSelectAlgorithmIdx = 1;
			m_ThreadPara[j].nSeqIdx = 0;
			m_ThreadPara[j].dPixelSizeWidth = 0.;
			m_ThreadPara[j].dPixelSizeHeight = 0.;
			m_ThreadPara[j].bTrace = FALSE;
			m_ThreadPara[j].bInspImageSave = FALSE;
		
			for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
			{
				m_ThreadPara[j].Spec[i].dMaxSpec = 0.;
				m_ThreadPara[j].Spec[i].dMinSpec = 0.;	
				m_ThreadPara[j].nInspPoint[i] = 0;
				memset(&m_ThreadPara[j].InspPara[i], NULL, sizeof(TypeInspPara));
			}

			m_ThreadPara[j].nChannel = m_nChannel;
			m_ThreadPara[j].strDrive.Format(_T("%s"), m_SystemPara.strDrive);
			m_ThreadPara[j].nInspPoint[0] = m_SeqPara.InspectionPara[j].nInspPoint[0];
			m_ThreadPara[j].nInspPoint[1] = m_SeqPara.InspectionPara[j].nInspPoint[1];
			m_ThreadPara[j].InspPara[0] = m_SeqPara.InspectionPara[j].InspPara[0];
			m_ThreadPara[j].InspPara[1] = m_SeqPara.InspectionPara[j].InspPara[1];
			m_ThreadPara[j].dPixelSizeWidth = m_SystemPara.dPixelSizeWidth;
			m_ThreadPara[j].dPixelSizeHeight = m_SystemPara.dPixelSizeHeight;
			m_ThreadPara[j].bInspImageSave = m_SystemPara.bInspImageSave;
			m_ThreadPara[j].bRsltImageSave = m_SystemPara.bRsltImageSave;

			if(m_ThreadPara[j].nInspPoint[0] != INSP_SKIP)
			{
				m_ThreadPara[j].Spec[0] = m_SeqPara.Spec[m_ThreadPara[j].nInspPoint[0] - 1];
				m_ThreadPara[j].SpecPoint[0] = m_SeqPara.SpecPoint[m_ThreadPara[j].nInspPoint[0] - 1];
			}

			if(m_ThreadPara[j].nInspPoint[1] != INSP_SKIP)
			{
				m_ThreadPara[j].Spec[1] = m_SeqPara.Spec[m_ThreadPara[j].nInspPoint[1] - 1];
				m_ThreadPara[j].SpecPoint[1] = m_SeqPara.SpecPoint[m_ThreadPara[j].nInspPoint[1] - 1];
			}

			//m_SystemPara.bPartitionInsp[0] = TRUE;
			//m_SystemPara.bPartitionInsp[1] = TRUE;

			m_ThreadPara[j].hwDisplay[0] = NULL;
			m_ThreadPara[j].hwDisplay[1] = NULL;

			m_ThreadPara[j].nDisplayId[0] = 0;
			m_ThreadPara[j].nDisplayId[1] = 0;
			m_ThreadPara[j].bTrace = FALSE;

			m_ThreadPara[j].bPartitionInsp[0] = FALSE;//m_SystemPara.bPartitionInsp[0];
			m_ThreadPara[j].bPartitionInsp[1] = FALSE;//m_SystemPara.bPartitionInsp[1];

			m_ThreadPara[j].nSelectAlgorithmIdx = m_SystemPara.nAlgorithmIdx;

			m_ThreadPara[j].nMetalAlgorithmIdx = m_SystemPara.nMetalAlgorithmIdx;
		}
	}

	strText.Format(_T("Init Thread Para End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

BOOL CVision::ReleaseThreadPara()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Release Thread Para Start"));
	m_cLogFile->WriteText(strText);

	rslt = ThreadParaReleaseImage();

	if(rslt)
	{
		for(int j = 0; j < MAX_INSP_SEQ_CNT; j++)
		{
			m_ThreadPara[j].nSeqIdx = 0;
			m_ThreadPara[j].dPixelSizeWidth = 0.;
			m_ThreadPara[j].dPixelSizeHeight = 0.;
			m_ThreadPara[j].bTrace = FALSE;
			m_ThreadPara[j].bInspImageSave = FALSE;
		
			for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
			{
				m_ThreadPara[j].Spec[i].dMaxSpec = 0.;
				m_ThreadPara[j].Spec[i].dMinSpec = 0.;	
				m_ThreadPara[j].nInspPoint[i] = 0;
				memset(&m_ThreadPara[j].InspPara[i], NULL, sizeof(TypeInspPara));
			}
		}
	}

	strText.Format(_T("Release Thread Para End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

BOOL CVision::ThreadParaInitImage()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Thread Para Init Image Start"));
	m_cLogFile->WriteText(strText);


	for(int j = 0; j < MAX_INSP_SEQ_CNT; j++)
	{
		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			if(m_ThreadPara[j].InspImage[i] != NULL)
			{
				cvReleaseImage(&m_ThreadPara[j].InspImage[i]);
			}
			m_ThreadPara[j].InspImage[i] = NULL;
			m_ThreadPara[j].InspImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);

			if(m_ThreadPara[j].DispImage[i] != NULL)
			{
				cvReleaseImage(&m_ThreadPara[j].DispImage[i]);
			}
			m_ThreadPara[j].DispImage[i] = NULL;
			m_ThreadPara[j].DispImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
		}

		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			if(m_ThreadPara[j].InspImage[i] == NULL || m_ThreadPara[j].DispImage[i] == NULL)
			{
				rslt = FALSE;
				break;
			}
		}

		if(!rslt)
			break;
	}

	strText.Format(_T("Thread Para Init Image End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

BOOL CVision::ThreadParaReleaseImage()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Thread Para Release Image Start"));
	m_cLogFile->WriteText(strText);

	for(int j = 0; j < MAX_INSP_SEQ_CNT; j++)
	{
		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			if(m_ThreadPara[j].InspImage[i] != NULL)
			{
				cvReleaseImage(&m_ThreadPara[j].InspImage[i]);
			}
			m_ThreadPara[j].InspImage[i] = NULL;

			if(m_ThreadPara[j].DispImage[i] != NULL)
			{
				cvReleaseImage(&m_ThreadPara[j].DispImage[i]);
			}
			m_ThreadPara[j].DispImage[i] = NULL;
		}

		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			if(m_ThreadPara[j].InspImage[i] != NULL || m_ThreadPara[j].DispImage[i] != NULL)
			{
				rslt = FALSE;
				break;
			}
		}

		if(!rslt)
			break;
	}

	strText.Format(_T("Thread Para Release Image End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

void CVision::SetSystemPara(TypeSystemPara SystemPara)
{
	m_SystemPara.bInspImageSave = SystemPara.bInspImageSave;
	m_SystemPara.bRsltImageSave = SystemPara.bRsltImageSave;
	m_SystemPara.bRsltNGImageSave = SystemPara.bRsltNGImageSave;
	m_SystemPara.dPixelSizeHeight = SystemPara.dPixelSizeHeight;
	m_SystemPara.dPixelSizeWidth = SystemPara.dPixelSizeWidth;
	m_SystemPara.isUsePW = SystemPara.isUsePW;
	m_SystemPara.strUseModelName.Format(_T("%s"), SystemPara.strUseModelName);
	m_SystemPara.bTrace = SystemPara.bTrace;
	m_SystemPara.DirAutoDelPara.nSaveDay = SystemPara.DirAutoDelPara.nSaveDay;
	m_SystemPara.DirAutoDelPara.nAutoDeleHour = SystemPara.DirAutoDelPara.nAutoDeleHour;
	m_SystemPara.DirAutoDelPara.nAutoDelMinute = SystemPara.DirAutoDelPara.nAutoDelMinute;
	m_SystemPara.strDrive.Format(_T("%s"), SystemPara.strDrive);
	m_SystemPara.DirAutoDelPara.strDelPath.Format(_T("%s%s"), SystemPara.strDrive, RSLT_IMAGE_FILE_PATH);
	m_SystemPara.nAlgorithmIdx = SystemPara.nAlgorithmIdx;
	m_SystemPara.nMetalAlgorithmIdx = SystemPara.nMetalAlgorithmIdx;
}

void CVision::SetInspection()
{
	CString strText, strBar;
	int nMaxCamCnt = ONE_CHANNEL_MAX_CAM_CNT;
	if(m_nInspStepIdx == 0)
	{
		InitRslt();
		ReleaseInspection();
		InitInspection();
		InitThreadPara();
		//strBar.Format(_T("%s"), );
	}

	if(m_SeqPara.InspectionPara.size() > m_nInspStepIdx)
	{
		strText.Format(_T("%d Step Inspection Start"), m_nInspStepIdx);
		m_cLogFile->WriteText(strText);

		m_ThreadPara[m_nInspStepIdx].nSeqIdx = m_nInspStepIdx;
	
		//Grab Image Merge
		CString strPath;
		CRect rect;
		for(int nCamIdx = 0; nCamIdx < nMaxCamCnt; nCamIdx++)
		{
#ifdef TEST_MANUAL_INSP
			if(!TestImageLoad(nCamIdx, m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx]))
			{
				strText.Format(_T("%d Step, %d CamIdx, %d InspPoint Test Image Load Error"), m_nInspStepIdx, nCamIdx, m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx]);
				m_cLogFile->WriteText(strText);
				return;
			}
#endif
			if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] != INSP_SKIP)
			{
				if(m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].bOneGrabInspection)
				{
					cvCopy(m_GrabImage[nCamIdx][0], m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx]);
					cvCvtColor(m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx], m_ThreadPara[m_nInspStepIdx].DispImage[nCamIdx], CV_GRAY2BGR);
				}
				else
				{
					if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT4)
					{
						//rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + 10;
						rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom) / 2);
						rect.bottom = m_GrabImage[nCamIdx][0]->height;
						rect.left = 0;
						rect.right = m_GrabImage[nCamIdx][0]->width; 
					}
					else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT1)
					{
						rect.top = 0;
						//rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - 10;
						rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom) / 2);
						rect.left = 0;
						rect.right = m_GrabImage[nCamIdx][0]->width;
					}
					else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT5)
					{
						rect.left = 0;
						rect.right = m_GrabImage[nCamIdx][0]->width;
						rect.top = 0;
						rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom) / 2);
						//rect.bottom = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].bottom + 10;
					
					}
					else if(m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara[m_nInspStepIdx].nInspPoint[nCamIdx] == INSP_POINT2)
					{
						rect.left = 0;
						rect.right = m_GrabImage[nCamIdx][0]->width;
						rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom + ((m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[0].bottom) / 2);
						//rect.top = m_ThreadPara[m_nInspStepIdx].InspPara[nCamIdx].InspRect[1].top - 10;
						rect.bottom = m_GrabImage[nCamIdx][0]->height;
					}	

					MergeImage(m_GrabImage[nCamIdx][0], m_GrabImage[nCamIdx][1], rect, m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx]);

					cvCvtColor(m_ThreadPara[m_nInspStepIdx].InspImage[nCamIdx], m_ThreadPara[m_nInspStepIdx].DispImage[nCamIdx], CV_GRAY2BGR);
				}
			}
		}

		if(m_ThreadPara[m_nInspStepIdx].nInspPoint[0] != INSP_SKIP || m_ThreadPara[m_nInspStepIdx].nInspPoint[1] != INSP_SKIP)
		{
			strText.Format(_T("%s_%02d%02d%02d"),  m_strBarcode, m_cStart.GetHour(), m_cStart.GetMinute(), m_cStart.GetSecond());
			m_CInspection[m_nInspStepIdx]->SetInspectionPara(m_ThreadPara[m_nInspStepIdx], m_cStart, strText);
		}
	}
}

BOOL CVision::InitInspection()
{
	BOOL rslt = FALSE;
	CString strText;
	strText.Format(_T("Init Inspection Start"));
	m_cLogFile->WriteText(strText);

	m_nMaxStepCnt = m_SeqPara.nMaxSeqIdx;
	if(m_nMaxStepCnt == m_SeqPara.InspectionPara.size())
	{
		for(int i = 0; i < m_nMaxStepCnt; i++)
		{
			m_CInspection[i] = new CVisionInspection();
			rslt = TRUE;
		}
	}

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		memset(m_DispImage[i]->imageData, NULL, sizeof(char) * ((m_DispImage[i]->width * m_DispImage[i]->height) * m_DispImage[i]->nChannels));
	}

	strText.Format(_T("Init Inspection End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

void CVision::ReleaseInspection(BOOL isDel)
{
	BOOL rslt[2] = {FALSE,};
	CString strText;
	strText.Format(_T("Release Inspection Start"));
	m_cLogFile->WriteText(strText);

	if(isDel || (m_nInspStepIdx > 0 && m_CInspection != NULL))
	{
		for(int i = 0; i < m_SeqPara.InspectionPara.size(); i++)
		{
			delete m_CInspection[i];
			m_CInspection[i] = NULL;
		}
	}

	m_nInspStepIdx = 0;
	ReleaseThreadPara();

	strText.Format(_T("Release Inspection End"));
	m_cLogFile->WriteText(strText);
}

BOOL CVision::InitVision(int nChannel)
{
	BOOL rslt = TRUE;

	CString strText, strFileName;
	strText.Format(_T("%sVision\\"), LOG_FILE_PATH);
	strFileName.Format(_T("Vision_Ch%d.log"), nChannel + 1);
	m_cLogFile = new CLogFile(strText, strFileName);

	rslt = InitImage();

	return rslt;
}

BOOL CVision::InitImage()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Init Image Start"));
	m_cLogFile->WriteText(strText);

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		m_DispImage[i] = NULL;
		m_DispImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
		if(m_DispImage[i] == NULL)
		{
			rslt = FALSE;
		}
		//memset(m_DispImage[i]->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT * 3);

		m_InspImage[i] = NULL;
		m_InspImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
		if(m_InspImage[i] == NULL)
		{
			rslt = FALSE;
		}
		//memset(m_InspImage[i]->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT);

		for(int j = 0; j < MAX_GRAB_CNT; j++)
		{
			m_GrabImage[i][j] = NULL;
			m_GrabImage[i][j] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
			if(m_GrabImage[i][j] == NULL)
			{
				rslt = FALSE;
			}
			//memset(m_GrabImage[i][j]->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT);
		}
	}

	if(rslt)
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			m_Rslt[i].RsltImage = NULL;
			m_Rslt[i].RsltImage = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
			if(m_Rslt[i].RsltImage == NULL)
			{
				rslt = FALSE;
			}
		}
	}

	if(!rslt)
	{
		strText.Format(_T("Init Image Fail"));
		m_cLogFile->WriteText(strText);
	}

	strText.Format(_T("Init Image End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

BOOL CVision::ReleaseImage()
{
	BOOL rslt = TRUE;
	CString strText;
	strText.Format(_T("Release Image Start"));
	m_cLogFile->WriteText(strText);

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		if (m_DispImage[i])
			cvReleaseImage(&m_DispImage[i]);

		m_DispImage[i] = NULL;
		if(m_DispImage[i] == NULL)
		{
			rslt = FALSE;
		}

		if (m_InspImage[i])
			cvReleaseImage(&m_InspImage[i]);

		m_InspImage[i] = NULL;
		if(m_InspImage[i] == NULL)
		{
			rslt = FALSE;
		}

		for(int j = 0; j < MAX_GRAB_CNT; j++)
		{
			if (m_GrabImage[i][j])
				cvReleaseImage(&m_GrabImage[i][j]);

			m_GrabImage[i][j] = NULL;
			if(m_GrabImage[i][j] == NULL)
			{
				rslt = FALSE;
			}
		}
	}

	if(rslt)
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			if (m_Rslt[i].RsltImage)
				cvReleaseImage(&m_Rslt[i].RsltImage);

			m_Rslt[i].RsltImage = NULL;
			if(m_Rslt[i].RsltImage == NULL)
			{
				rslt = FALSE;
			}
		}
	}

	if(!rslt)
	{
		strText.Format(_T("Release Image Fail"));
		m_cLogFile->WriteText(strText);
	}

	strText.Format(_T("Release Image End"));
	m_cLogFile->WriteText(strText);

	return rslt;
}

BOOL CVision::MergeImage(IplImage *src1, IplImage *src2, CRect MergeRect, IplImage *dst)
{
	BOOL rslt = TRUE;
	IplImage *lDstImage;
	lDstImage = NULL;

	if(!src1 || !src2 || !dst)
	{
		rslt = FALSE;
	}

	if(!rslt && src1->width != dst->width || src2->width != dst->width || src1->width != src2->width)
	{
		rslt = FALSE;
	}

	if(rslt)
	{
		lDstImage = cvCreateImage(cvSize(dst->width, dst->height), IPL_DEPTH_8U, 1);
		cvCopy(src1, lDstImage);

		int nLeft, nRight, nTop, nBottom;
		if(MergeRect.left >= 0)
		{
			nLeft = MergeRect.left;
		}
		else
		{
			nLeft = 0;
		}

		if(MergeRect.right <= dst->width)
		{
			nRight = MergeRect.right;
		}
		else
		{
			nRight = 0;
		}

		if(MergeRect.top >= 0)
		{
			nTop = MergeRect.top;
		}
		else
		{
			nTop = 0;
		}

		if(MergeRect.bottom <= dst->height)
		{
			nBottom = MergeRect.bottom;
		}
		else
		{
			nBottom = 0;
		}

		for(int h = nTop; h < nBottom; h++)
		{
			for(int w = nLeft; w < nRight; w++)
			{
				lDstImage->imageData[w + (h * lDstImage->widthStep)] = src2->imageData[w + (h * src2->widthStep)];
			}
		}

		cvCopy(lDstImage, dst);
		cvReleaseImage(&lDstImage);
	}

	return rslt;
}

BOOL CVision::RsltSuming()
{
	BOOL rslt = TRUE;

	for(int i = 0; i < m_nMaxStepCnt; i++)
	{
		//if(m_CInspection[i])
			//m_CInspection[i]->GetRslt(m_Rslt);
	}

	return rslt;
}

void CVision::GetRsltGapVal(int nInspPoint, BOOL *bRslt, double *dRslt)
{
	if(nInspPoint >= MAX_INSP_POINT_CNT)
	{
		return;
	}

	*bRslt = m_Rslt[nInspPoint].bGapRslt;
	*dRslt =  m_Rslt[nInspPoint].dGapRslt;
}

BOOL CVision::GetPointRslt(int nInspPoint)
{
	return m_Rslt[nInspPoint].bPointRslt;
}

BOOL CVision::GetInspRsltVal()
{
	BOOL rslt = TRUE;
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		if(!m_Rslt[i].bGapRslt)
		{
			rslt = FALSE;
			break;
		}
	}

	return rslt;
}

BOOL CVision::GetInspecting(int nStepIdx)
{
	BOOL rslt = FALSE;

	if(m_CInspection[nStepIdx])
	{
		if(m_CInspection[nStepIdx]->GetInspecting(0) && m_CInspection[nStepIdx]->GetInspecting(1))
		{
			rslt = TRUE;
		}
	}

	return rslt;
}

BOOL CVision::GetInspectionEnd(int nStepIdx)
{
	BOOL rslt = FALSE;
	int nInspPoint = 0;

	if(m_CInspection[nStepIdx])
	{
		if(m_CInspection[nStepIdx]->GetInspectionEnd(0) && m_CInspection[nStepIdx]->GetInspectionEnd(1))
		{
			rslt = TRUE;

			for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
			{
				nInspPoint = m_SeqPara.InspectionPara[nStepIdx].nInspPoint[i] - 1;
				if(m_CInspection[nStepIdx])
					m_CInspection[nStepIdx]->GetRslt(i, &m_Rslt[nInspPoint]);
			}
			CString strText;
			strText.Format(_T("%d Step Inspection End"), nStepIdx);
			m_cLogFile->WriteText(strText);
		}
	}

	return rslt;
}

//CVisionInspection Class
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVisionInspection::CVisionInspection(BOOL isManualInsp)
{
	m_pThreadInsp[0] = NULL;
	m_pThreadInsp[1] = NULL;

	m_bInspectionEnd[0] = FALSE;
	m_bInspecting[0] = FALSE;
	m_bInspectionEnd[1] = FALSE;
	m_bInspecting[1] = FALSE;

	m_bManualInsp = isManualInsp;

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		m_Rslt[i].bGapRslt = FALSE;
		m_Rslt[i].dGapRslt = 0.;
		m_Rslt[i].nInspPoint = INSP_SKIP;
		m_Rslt[i].RsltImage = NULL;

		m_ThreadPara.DispImage[i] = NULL;
		m_ThreadPara.InspImage[i] = NULL;
	}
	InitImage();

	CString strText;
	strText.Format(_T("%sVision\\"), LOG_FILE_PATH);
	m_cLogFile = new CLogFile(strText, _T("Vision.log"));
}

CVisionInspection::~CVisionInspection()
{
	ReleaseThread();
	ReleaseImage();
	if(m_cLogFile)	delete m_cLogFile;
}

void CVisionInspection::SetDisplay(CStaticMatDisplay *Display, int nCamIdx)
{
	//m_Display[nCamIdx] = *Display;
}

void CVisionInspection::GetRslt(int nCamIdx, TypeInspEachRslt *Rslt)
{
	int i = nCamIdx;
	//for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		if(m_Rslt[i].nInspPoint != INSP_SKIP)
		{
			Rslt->nInspPoint = m_Rslt[i].nInspPoint;

			if(Rslt->RsltImage)
			{
				cvReleaseImage(&Rslt->RsltImage);
			}
			Rslt->RsltImage = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
			
			cvCopy(m_Rslt[i].RsltImage, Rslt->RsltImage);
			
			Rslt->dGapRslt = m_Rslt[i].dGapRslt;
			Rslt->bGapRslt = m_Rslt[i].bGapRslt;
			Rslt->bPointRslt = m_Rslt[i].bPointRslt;
		}
	}
}

BOOL CVisionInspection::InitImage()
{
	BOOL rslt = TRUE;

	if(rslt)
	{
		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			m_ThreadPara.DispImage[i] = NULL;
			m_ThreadPara.InspImage[i] = NULL;
			m_Rslt[i].RsltImage = NULL;
			if(m_ThreadPara.nInspPoint[i] != INSP_SKIP)
			{
				m_ThreadPara.DispImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
				m_ThreadPara.InspImage[i] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
				m_Rslt[i].RsltImage = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);

				if(m_ThreadPara.DispImage[i] == NULL || m_ThreadPara.InspImage[i] == NULL || m_Rslt[i].RsltImage == NULL)
				{
					rslt = FALSE;
				}
				memset(m_ThreadPara.DispImage[i]->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT * 3);
				memset(m_ThreadPara.InspImage[i]->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT);
				memset(m_Rslt[i].RsltImage->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT * 3);
			}
		}
	}

	return rslt;
}

BOOL CVisionInspection::ReleaseImage()
{
	BOOL rslt = TRUE;

	if(rslt)
	{
		for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
		{
			if (m_ThreadPara.DispImage[i])
				cvReleaseImage(&m_ThreadPara.DispImage[i]);

			m_ThreadPara.DispImage[i] = NULL;

			if (m_ThreadPara.InspImage[i])
				cvReleaseImage(&m_ThreadPara.InspImage[i]);

			m_ThreadPara.InspImage[i] = NULL;

			if (m_Rslt[i].RsltImage)
				cvReleaseImage(&m_Rslt[i].RsltImage);

			m_Rslt[i].RsltImage = NULL;

			if(m_ThreadPara.DispImage[i] != NULL || m_ThreadPara.InspImage[i] != NULL || m_Rslt[i].RsltImage != NULL)
			{
				rslt = FALSE;
			}
		}
	}

	return rslt;
}

BOOL CVisionInspection::InitThread(int nCamIdx)
{
	BOOL rslt = FALSE;

	m_bInspecting[nCamIdx] = FALSE;
	m_bInspectionEnd[nCamIdx] = FALSE;
	m_pThreadInsp[nCamIdx] = NULL;

	if(nCamIdx == 0)
	{
		m_pThreadInsp[nCamIdx] = AfxBeginThread(InspThread0, this, THREAD_PRIORITY_NORMAL);
	}
	else
	{
		m_pThreadInsp[nCamIdx] = AfxBeginThread(InspThread1, this, THREAD_PRIORITY_NORMAL);
	}

	if(m_pThreadInsp[nCamIdx] != NULL)
	{
		m_pThreadInsp[nCamIdx]->m_bAutoDelete = FALSE;
		m_pThreadInsp[nCamIdx]->ResumeThread();
		rslt = TRUE;
	}

	return rslt;
}

BOOL CVisionInspection::ReleaseThread()
{
	BOOL rslt = TRUE;

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		WaitThreadEnd(i);

		m_bInspecting[i] = FALSE;
		m_bInspectionEnd[i] = FALSE;

		if(m_pThreadInsp[i])
			delete m_pThreadInsp[i];

		m_pThreadInsp[i] = NULL;
	}

	return rslt;
}

BOOL CVisionInspection::WaitThreadEnd(int nCamIdx)
{
	BOOL rslt = TRUE;
	if(m_pThreadInsp[nCamIdx] != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadInsp[nCamIdx]->m_hThread, DEFAULT_TIMEOUT);

		if(dwRet == WAIT_FAILED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			rslt = FALSE;
		}
	}

	return rslt;
}

BOOL CVisionInspection::SetInspectionPara(TypeThreadPara ThreadPara, CTime cStart, CString strBarcode)
{
	BOOL rslt = TRUE;
	//rslt = InitImage();

	m_ThreadPara.nChannel = ThreadPara.nChannel;
	m_ThreadPara.strDrive.Format(_T("%s"), ThreadPara.strDrive);

	m_ThreadPara.nSeqIdx = ThreadPara.nSeqIdx;

	m_ThreadPara.Spec[0] = ThreadPara.Spec[0];
	m_ThreadPara.Spec[1] = ThreadPara.Spec[1];
	m_ThreadPara.SpecPoint[0] = ThreadPara.SpecPoint[0];
	m_ThreadPara.SpecPoint[1] = ThreadPara.SpecPoint[1];

	m_ThreadPara.dPixelSizeWidth = ThreadPara.dPixelSizeWidth;
	m_ThreadPara.dPixelSizeHeight = ThreadPara.dPixelSizeHeight;
	m_ThreadPara.bTrace = ThreadPara.bTrace;
	m_ThreadPara.bPartitionInsp[0] = ThreadPara.bPartitionInsp[0];
	m_ThreadPara.bPartitionInsp[1] = ThreadPara.bPartitionInsp[1];

	m_ThreadPara.bInspImageSave = ThreadPara.bInspImageSave;
	m_ThreadPara.bRsltImageSave = ThreadPara.bRsltImageSave;

	m_ThreadPara.hwDisplay[0] = ThreadPara.hwDisplay[0];
	m_ThreadPara.hwDisplay[1] = ThreadPara.hwDisplay[1];

	m_ThreadPara.nDisplayId[0] = ThreadPara.nDisplayId[0];
	m_ThreadPara.nDisplayId[1] = ThreadPara.nDisplayId[1];

	m_ThreadPara.nSelectAlgorithmIdx = ThreadPara.nSelectAlgorithmIdx;

	m_ThreadPara.nMetalAlgorithmIdx = ThreadPara.nMetalAlgorithmIdx;

	if(m_ThreadPara.hwDisplay[0] != NULL)
	{
		m_Display[0] = (CStaticMatDisplay*)m_ThreadPara.hwDisplay[0];
	}

	if(m_ThreadPara.hwDisplay[1] != NULL)
	{
		m_Display[1] = (CStaticMatDisplay*)m_ThreadPara.hwDisplay[1];
	}

	m_strSavePath.Format(_T("%s%s%04d\\%02d\\%02d\\Inspecting\\Ch_%d\\%s\\"), ThreadPara.strDrive, RSLT_IMAGE_FILE_PATH, cStart.GetYear(), cStart.GetMonth(), cStart.GetDay(), m_ThreadPara.nChannel + 1, strBarcode);
	//if(m_bManualInsp)
	{
		if(ThreadPara.bInspImageSave && DriveUseSearch(m_ThreadPara.strDrive))
		{
			CreateDir(m_strSavePath);
		}
	}

	for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
	{
		if(ThreadPara.nInspPoint[i] != INSP_SKIP)
		{
			if(rslt)
			{
				m_ThreadPara.Spec[i] = ThreadPara.Spec[i];
				if(m_ThreadPara.InspImage[i] != NULL)
				{
					cvCopy(ThreadPara.InspImage[i], m_ThreadPara.InspImage[i]);
					cvCopy(ThreadPara.DispImage[i], m_ThreadPara.DispImage[i]);
					m_ThreadPara.InspPara[i] = ThreadPara.InspPara[i];
					m_ThreadPara.nInspPoint[i] = ThreadPara.nInspPoint[i];

					m_bInspectionEnd[i] = FALSE;
					m_bInspecting[i] = TRUE;

					rslt = InitThread(i);
				}
			}
		}
		else
		{
			m_bInspecting[i] = FALSE;
			m_bInspectionEnd[i] = TRUE;
		}
	}

	return rslt;
}

#ifdef MODEL_BLOOM
double CVisionInspection::GapInspection(int nCamIdx)
{
	double dRslt = 0.;
	if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ||  m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
	{
		CString strSavePath, strText;
		int iTraceStep = 0;
		BOOL bSaveImage = m_ThreadPara.bInspImageSave;
		BOOL bSaveRsltImage = m_ThreadPara.bRsltImageSave;
		BOOL bTrace = m_ThreadPara.bTrace;
		if(m_ThreadPara.InspImage[nCamIdx] == NULL)
		{
			m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
			m_Rslt[nCamIdx].dGapRslt = 0.;
			return 0.;
		}
		IplImage *SrcImage = cvCloneImage(m_ThreadPara.InspImage[nCamIdx]);
		IplImage *DispImage = cvCloneImage(m_ThreadPara.DispImage[nCamIdx]);
		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
		CvMemStorage *cvStorage;
		cvBlobFinder CvBlobs[MAX_INSP_CNT];
		CvPoint2D32f InspPoint[2];
		CvPoint SmoothKernel;
		CvPoint cMorpSize;
		int nThreshHole;
		float fSizeThresh;
		CvPoint2D32f SmoothKernelSigma;
		CvRect rcRoi;
		int nMaxInspCnt = MAX_INSP_CNT;
		IplImage *TempImage1;
		IplImage *TempImage2;
		IplImage *TempImage3;
		IplImage *TempImage4;
		IplImage *TempImage5;
		CvPoint RsltLine[MAX_INSP_CNT + 1][2];
		BOOL isSearch[2] = {FALSE,};

		m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
		m_Rslt[nCamIdx].dGapRslt = 0.;
		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
		m_Rslt[nCamIdx].bGapRslt = FALSE;

		strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection Start"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]));
		m_cLogFile->WriteText(strText);

		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			if(!m_bInspecting[nCamIdx] && !m_bInspectionEnd[nCamIdx])
			{ 
				m_bInspecting[nCamIdx] = FALSE;
				m_bInspectionEnd[nCamIdx] = TRUE;
				break;
			}
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();

			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);

			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);

			cvSetImageROI(TempImage1, rcRoi);

			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}

			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
			strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
			cvErode(TempImage4, TempImage5, element);
			cvDilate(TempImage5, TempImage4, element);
			if(element != NULL)	cvReleaseStructuringElement(&element);
			
			cvNot(TempImage4, TempImage5);
				

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);

			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
					}
					else
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			int nSearchDirection = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSearchDirection;
			int nSearchY = -1;
			int nStartH = 5, nStartW = 5;
			if(TempImage4->height <= nStartH)
			{
				nStartH = nStartH / 2;
			}

			if(TempImage4->width <= nStartW)
			{
				nStartW = nStartW / 2;
			}

			if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
			{
				int w = TempImage4->width / 2;

				if(nSearchDirection == TOP_TO_BOTTOM)
				{
					for(int h = nStartH; h < TempImage4->height; h++)
					{
						if(nInspIdx == 1)
						{
							if(TempImage4->imageData[(h * TempImage4->widthStep) + w] < 100)
							{
								nSearchY = h;
								break;
							}
						}
						else
						{
							if(TempImage4->imageData[(h * TempImage4->widthStep) + w] > 100)
							{
								nSearchY = h;
								break;
							}
						}
					}
				}
				else if(nSearchDirection == BOTTOM_TO_TOP)
				{
					for(int h = TempImage4->height - nStartH; h >= 0 ; h--)
					{
						if(TempImage4->imageData[(h * TempImage4->widthStep) + w] > 100)
						{
							nSearchY = h;
							break;
						}
					}
				}
			}
			else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
			{
				int w = TempImage4->width / 2;
				if(nSearchDirection == TOP_TO_BOTTOM)
				{
					for(int h = nStartH; h < TempImage4->height; h++)
					{
						if(TempImage4->imageData[(h * TempImage4->widthStep) + w] > 100)
						{
							nSearchY = h;
							break;
						}
					}
				}
				else if(nSearchDirection == BOTTOM_TO_TOP)
				{
					for(int h = TempImage4->height - nStartH; h >= 0 ; h--)
					{
						if(nInspIdx == 1)
						{
							if(TempImage4->imageData[(h * TempImage4->widthStep) + w] < 100)
							{
								nSearchY = h;
								break;
							}
						}
						else
						{
							if(TempImage4->imageData[(h * TempImage4->widthStep) + w] > 100)
							{
								nSearchY = h;
								break;
							}
						}
					}
				}
			}

			RsltLine[0][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
			RsltLine[0][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
			RsltLine[1][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
			RsltLine[1][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);

			RsltLine[nInspIdx][0].y = 0;
			RsltLine[nInspIdx][1].y = 0;

			RsltLine[2][0].x = 0;
			RsltLine[2][1].x = 0;
			RsltLine[2][0].y = 0;
			RsltLine[2][1].y = 0;

			if(nSearchY > -1)
			{
				isSearch[nInspIdx] = TRUE;

				if(nInspIdx == 1)
				{
					RsltLine[nInspIdx][0].y = rcRoi.y + nSearchY;
					RsltLine[nInspIdx][1].y = rcRoi.y + nSearchY;

					cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);
				}
				else
				{
					RsltLine[nInspIdx][0].y = rcRoi.y + nSearchY;
					RsltLine[nInspIdx][1].y = rcRoi.y + nSearchY;

					cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);

					RsltLine[2][0].x = RsltLine[0][0].x + ((RsltLine[0][1].x - RsltLine[0][0].x) / 2);
					RsltLine[2][0].y = RsltLine[0][0].y;

					RsltLine[2][1].x = RsltLine[0][0].x + ((RsltLine[0][1].x - RsltLine[0][0].x) / 2);
					RsltLine[2][1].y = RsltLine[1][0].y;

					InspPoint[0].x = RsltLine[2][0].x;
					InspPoint[0].y = RsltLine[2][0].y;

					InspPoint[1].x = RsltLine[2][1].x;
					InspPoint[1].y = RsltLine[2][1].y;
				}
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}

		m_Rslt[nCamIdx].bPointRslt = FALSE;
		if(isSearch[0] && isSearch[1])
		{
			m_Rslt[nCamIdx].bGapRslt = TRUE;
			m_Rslt[nCamIdx].bPointRslt = TRUE;
		}

		if(InspPoint[0].x > 0. && InspPoint[0].y > 0. && InspPoint[1].x > 0. && InspPoint[1].y > 0. &&
			RsltLine[2][0].x > 0 && RsltLine[2][0].y > 0 && RsltLine[2][1].x > 0 && RsltLine[2][1].y > 0)
		{
			m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
			m_Rslt[nCamIdx].dGapRslt = (GetDistance(InspPoint[0], InspPoint[1]) * m_ThreadPara.dPixelSizeWidth) / 1000. + m_ThreadPara.SpecPoint[nCamIdx].dOffset;

			cvLine(DispImage, RsltLine[2][0], RsltLine[2][1], CV_RGB(0, 255, 0), 11, CV_AA);
		}

		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);

		if (SrcImage)		cvReleaseImage(&SrcImage);
		if (DispImage)		cvReleaseImage(&DispImage);

		strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection End -> Rslt : %0.3f"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]), m_Rslt[nCamIdx].dGapRslt);
		m_cLogFile->WriteText(strText);
	}
	else
	{
#ifndef NEW_ALG2
		dRslt = GapInspection(nCamIdx, 0);
#else
		CString strSavePath, strText;
		int iTraceStep = 0;
		BOOL bSaveImage = m_ThreadPara.bInspImageSave;
		BOOL bSaveRsltImage = m_ThreadPara.bRsltImageSave;
		BOOL bTrace = m_ThreadPara.bTrace;
		if(m_ThreadPara.InspImage[nCamIdx] == NULL)
		{
			m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
			m_Rslt[nCamIdx].dGapRslt = 0.;
			return 0.;
		}

		IplImage *SrcImage = cvCloneImage(m_ThreadPara.InspImage[nCamIdx]);
		IplImage *DispImage = cvCloneImage(m_ThreadPara.DispImage[nCamIdx]);
		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
		CvMemStorage *cvStorage;
		cvBlobFinder CvBlobs[MAX_INSP_CNT];
		CvPoint2D32f InspPoint[2];
		CvPoint SmoothKernel;
		CvPoint cMorpSize;
		int nThreshHole;
		float fSizeThresh;
		CvPoint2D32f SmoothKernelSigma;
		CvRect rcRoi;
		int nMaxInspCnt = MAX_INSP_CNT;
		IplImage *TempImage1;
		IplImage *TempImage2;
		IplImage *TempImage3;
		IplImage *TempImage4;
		IplImage *TempImage5;
		CvPoint RsltLine[MAX_INSP_CNT + 1][2];
		BOOL isSearch[2] = {FALSE,};

		m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
		m_Rslt[nCamIdx].dGapRslt = 0.;
		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
		m_Rslt[nCamIdx].bGapRslt = FALSE;

		strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection Start"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]));
		m_cLogFile->WriteText(strText);

		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			if(!m_bInspecting[nCamIdx] && !m_bInspectionEnd[nCamIdx])
			{ 
				m_bInspecting[nCamIdx] = FALSE;
				m_bInspectionEnd[nCamIdx] = TRUE;
				break;
			}
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();

			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);

			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);

			cvSetImageROI(TempImage1, rcRoi);

			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}

			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if(nInspIdx == 0)
			{
				if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
				{
					cvNot(TempImage2, TempImage3);
					cvThreshold(TempImage3, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
					strSavePath.Format(_T("%sCvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
				else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
				{
					cvThreshold(TempImage2, TempImage3, nThreshHole, 255, CV_THRESH_BINARY);
					strSavePath.Format(_T("%sCvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage3, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage3, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}

					cvNot(TempImage3, TempImage4);
					strSavePath.Format(_T("%scvNot_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
				else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
				{
					cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
					//cvNot(TempImage3, TempImage4);
					strSavePath.Format(_T("%scvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
			}
			else
			{
				cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
				strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}
			}

			IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
			cvErode(TempImage4, TempImage5, element);
			cvDilate(TempImage5, TempImage4, element);
			if(element != NULL)	cvReleaseStructuringElement(&element);
				
			cvCopy(TempImage4, TempImage5);

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);

			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
					}
					else
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			CvBlobSearchPara BlobSearchPara;
			memset(&BlobSearchPara, NULL, sizeof(CvBlobSearchPara));

		
			if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
			{
				BlobSearchPara.dMinWidth = (double)(AM_SIZE_X_Y - AM_OFFSET_X);
				BlobSearchPara.dMinHeight = (double)(AM_SIZE_X_Y - AM_OFFSET_X);
			}
			else if(nInspIdx == 1)
			{
				BlobSearchPara.dMinWidth = (double)rcRoi.width / 2;
				BlobSearchPara.dMinHeight = (double)100.;
			}
			else if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
			{
				BlobSearchPara.dMinWidth = (double)10.;
				BlobSearchPara.dMinHeight = (double)rcRoi.height;
			}
			else if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
			{
				BlobSearchPara.dMinWidth = (double)cMorpSize.x;
				BlobSearchPara.dMinHeight = (double)50;
			}
				
			BlobSearchPara.dMinArea = (double)(BlobSearchPara.dMinWidth * cMorpSize.y) / 2.;
			BlobSearchPara.dMaxWidth = TempImage4->width * 2.;
			BlobSearchPara.dMaxHeight = TempImage4->height * 2.;
		
			BlobSearchPara.dMaxArea = BlobSearchPara.dMaxWidth * BlobSearchPara.dMaxHeight;
			BlobSearchPara.nMode = 1;
			BlobSearchPara.ucThreshold = 20;
			BlobSearchPara.bUseMask = TRUE;
			BlobSearchPara.nBlobMaxCnt = 1000;

			CvBlobs[nInspIdx].FindBlobs(TempImage4, BlobSearchPara);

			strSavePath.Format(_T("%sMask_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(CvBlobs[nInspIdx].GetMask(), strSavePath);
			
			if(bTrace)
			{
				for(int h = 0; h < TempImage1->roi->height; h++)
				{
					for(int w = 0; w < TempImage1->roi->width; w++)
					{
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = 0;
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = CvBlobs[nInspIdx].GetMask()->imageData[w + (h * CvBlobs[nInspIdx].GetMask()->widthStep)];
					}
				}
				TraceInsp(m_Display[nCamIdx], TempImage1, &rcRoi, bTrace);

				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			int nBlobCnt = CvBlobs[nInspIdx].GetBlobCount();

			InspPoint[nInspIdx].x = 0.;
			InspPoint[nInspIdx].y = 0.;
		
			RsltLine[2][nInspIdx].x = 0;
			RsltLine[2][nInspIdx].y = 0;

 			if(nBlobCnt > 0)
			{
				CvBlob1 Blob;
				int nTemp1, nTemp2;
				Blob = CvBlobs[nInspIdx].GetEachBlob(0);
				int nMaxWidth = Blob.width, nMaxWidthIdx = 0;
					
				for(int nBlobNextIdx = 1; nBlobNextIdx < CvBlobs[nInspIdx].GetBlobCount(); nBlobNextIdx++)
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobNextIdx);

					nTemp2 = Blob.width;
					nTemp1 = nMaxWidth;
					nMaxWidth = max(nTemp1, nTemp2);

					if(nMaxWidth == nTemp2)
					{
						nMaxWidthIdx = nBlobNextIdx;
					}
				}

				/*if(nInspIdx == 0)
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nMaxWidthIdx);
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = rcRoi.x + Blob.x + Blob.width;
						RsltLine[nInspIdx][1].x = rcRoi.x + Blob.x + Blob.width;
					}
					else
					{
						RsltLine[nInspIdx][0].x = (rcRoi.x +  rcRoi.width) - Blob.width;
						RsltLine[nInspIdx][1].x = (rcRoi.x +  rcRoi.width) - Blob.width;
					}

					RsltLine[nInspIdx][0].y = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].top, m_ThreadPara.InspPara[nCamIdx].InspRect[1].top);
					RsltLine[nInspIdx][1].y = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom, m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom);

					cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);

					InspPoint[nInspIdx].x = RsltLine[nInspIdx][0].x;
					InspPoint[nInspIdx].y = RsltLine[nInspIdx][0].y + ((RsltLine[nInspIdx][1].y - RsltLine[nInspIdx][0].y) / 2);

					isSearch[nInspIdx] = TRUE;

					RsltLine[2][0].x = InspPoint[0].x;
					RsltLine[2][0].y = InspPoint[0].y;
					RsltLine[2][1].x = InspPoint[1].x;
					RsltLine[2][1].y = InspPoint[1].y;
				}
				else*/
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nMaxWidthIdx);
					if (TempImage5)		cvReleaseImage(&TempImage5);

					TempImage5 = cvCreateImage(cvSize(CvBlobs[nInspIdx].GetMask()->width, CvBlobs[nInspIdx].GetMask()->height), IPL_DEPTH_8U, 1);
					cvCopy(CvBlobs[nInspIdx].GetMask(), TempImage5);

					int nSearchH = Blob.height / 2;
					int nSearchW = 0;

					if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSearchDirection == RIGHT_TO_LEFT)
					{
						nSearchW = Blob.width - 10;
					}
					else
					{
						nSearchW = 10;
					}

					/*if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						for(int h = 0; h < TempImage5->height; h++)
						{
							if(TempImage5->imageData[(h * TempImage5->widthStep) + nSearchW] != 0)
							{
								nSearchH = h - 50;
								if(nSearchH <= 0)
									nSearchH = 50;
								break;
							}
						}
					}
					else
					{
						for(int h = TempImage5->height - 1; h >= 0; h--)
						{
							if(TempImage5->imageData[(h * TempImage5->widthStep) + nSearchW] != 0)
							{
								nSearchH = h + 50;
								if(nSearchH >= Blob.height)
									nSearchH = Blob.height - 50;
								break;
							}
						}
					}*/

					int nPTX = 0;
					if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSearchDirection == RIGHT_TO_LEFT)
					{
						for(int w = TempImage5->width - 1; w >= 0; w--)
						{
							if(TempImage5->imageData[(nSearchH * TempImage5->widthStep) + w] != 0)
							{
								nPTX = w;
								break;
							}
						}
					}
					else
					{
						for(int w = 0; w < TempImage5->width; w++)
						{
							if(TempImage5->imageData[(nSearchH * TempImage5->widthStep) + w] != 0)
							{
								nPTX = w;
								break;
							}
						}
					}

					RsltLine[nInspIdx][0].x = rcRoi.x + nPTX;
					RsltLine[nInspIdx][1].x = rcRoi.x + nPTX;						
			
					RsltLine[nInspIdx][0].y = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].top, m_ThreadPara.InspPara[nCamIdx].InspRect[1].top);
					RsltLine[nInspIdx][1].y = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom, m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom);

					cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);

					InspPoint[nInspIdx].x = RsltLine[nInspIdx][0].x;
					InspPoint[nInspIdx].y = RsltLine[nInspIdx][0].y + ((RsltLine[nInspIdx][1].y - RsltLine[nInspIdx][0].y) / 2);

					isSearch[nInspIdx] = TRUE;
				}				
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}

		m_Rslt[nCamIdx].bPointRslt = FALSE;
		if(isSearch[0] && isSearch[1])
		{
			m_Rslt[nCamIdx].bGapRslt = TRUE;
			m_Rslt[nCamIdx].bPointRslt = TRUE;

			RsltLine[2][0].x = RsltLine[0][0].x;// + ((RsltLine[0][1].x - RsltLine[0][0].x) / 2);
			RsltLine[2][0].y = RsltLine[0][0].y + ((RsltLine[0][1].y - RsltLine[0][0].y) / 2);

			RsltLine[2][1].x = RsltLine[1][0].x;// + ((RsltLine[0][1].x - RsltLine[0][0].x) / 2);
			RsltLine[2][1].y = RsltLine[1][0].y + ((RsltLine[0][1].y - RsltLine[0][0].y) / 2);
		}

		if(InspPoint[0].x > 0. && InspPoint[0].y > 0. && InspPoint[1].x > 0. && InspPoint[1].y > 0. &&
			RsltLine[2][0].x > 0 && RsltLine[2][0].y > 0 && RsltLine[2][1].x > 0 && RsltLine[2][1].y > 0)
		{
			m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
			m_Rslt[nCamIdx].dGapRslt = (GetDistance(InspPoint[0], InspPoint[1]) * m_ThreadPara.dPixelSizeWidth) / 1000.;

			cvLine(DispImage, RsltLine[2][0], RsltLine[2][1], CV_RGB(0, 255, 0), 11, CV_AA);
		}

		cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);

		if (SrcImage)		cvReleaseImage(&SrcImage);
		if (DispImage)		cvReleaseImage(&DispImage);

		strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection End -> Rslt : %0.3f"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]), m_Rslt[nCamIdx].dGapRslt);
		m_cLogFile->WriteText(strText);
#endif
	}

#ifdef TEST_RUN
	//Sleep(500);
	//int nRnd1 = RandVal(14, 16);
	//int nRnd2 = RandVal(0, 100);
	//double dTempRslt = 0.;
	//srand(time(0));

	//nRnd1 = rand() % ((16 + 1) + 14);

	//switch(nRnd2%2)
	//{
	//case 0:
	//	dTempRslt = (double)nRnd1 * 0.001;
	//	break;
	//case 1:
	//	dTempRslt = (double)nRnd1 * 0.001;// * (-1.);
	//	break;
	//}

	//m_Rslt[nCamIdx].bGapRslt = TRUE;
	//m_Rslt[nCamIdx].bPointRslt = TRUE;

	//m_Rslt[nCamIdx].dGapRslt = dTempRslt;

	//if(m_ThreadPara.nInspPoint[nCamIdx] != INSP_POINT1 && m_ThreadPara.nInspPoint[nCamIdx] != INSP_POINT4)
	//	m_Rslt[nCamIdx].dGapRslt = (m_ThreadPara.nInspPoint[nCamIdx] + 1) % 2 + 1;
#endif

	return dRslt;
}

double CVisionInspection::GapInspection(int nCamIdx, int nk)
{
	double dRslt = 0.;
	
	CString strSavePath, strText;
	int iTraceStep = 0;
	BOOL bSaveImage = m_ThreadPara.bInspImageSave;
	BOOL bSaveRsltImage = m_ThreadPara.bRsltImageSave;
	BOOL bTrace = m_ThreadPara.bTrace;
	if(m_ThreadPara.InspImage[nCamIdx] == NULL)
	{
		m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
		m_Rslt[nCamIdx].dGapRslt = 0.;
		return 0.;
	}
	IplImage *SrcImage = cvCloneImage(m_ThreadPara.InspImage[nCamIdx]);
	IplImage *DispImage = cvCloneImage(m_ThreadPara.DispImage[nCamIdx]);
	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
	CvMemStorage *cvStorage;
	cvBlobFinder CvBlobs[MAX_INSP_CNT];
	CvPoint2D32f InspPoint[2];
	CvPoint SmoothKernel;
	CvPoint cMorpSize;
	int nThreshHole;
	float fSizeThresh;
	CvPoint2D32f SmoothKernelSigma;
	CvRect rcRoi;
	int nMaxInspCnt = MAX_INSP_CNT;
	IplImage *TempImage1;
	IplImage *TempImage2;
	IplImage *TempImage3;
	IplImage *TempImage4;
	IplImage *TempImage5;
	CvPoint RsltLine[MAX_INSP_CNT + 1][2];
	BOOL isSearch[2] = {FALSE,};

	m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
	m_Rslt[nCamIdx].dGapRslt = 0.;
	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
	m_Rslt[nCamIdx].bGapRslt = FALSE;

	strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection Start"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]));
	m_cLogFile->WriteText(strText);

	if(m_ThreadPara.nSelectAlgorithmIdx == WINNER_ALGO)
	{
		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			if(!m_bInspecting[nCamIdx] && !m_bInspectionEnd[nCamIdx])
			{ 
				m_bInspecting[nCamIdx] = FALSE;
				m_bInspectionEnd[nCamIdx] = TRUE;
				break;
			}
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();
			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);
			cvSetImageROI(TempImage1, rcRoi);
			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6) &&
				nInspIdx == 0)
			{
				cvNot(TempImage2, TempImage3);
				strSavePath.Format(_T("%scvNot_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage3, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage3, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				CvThreshHoldRange(TempImage3, TempImage2, (255 - nThreshHole), 30);

				strSavePath.Format(_T("%sCvThreshHoldRange_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				cvNot(TempImage2, TempImage4);
				strSavePath.Format(_T("%scvAnd_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}
			else
			{
				cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
				strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}

			TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
			{
				Morphology(TempImage4, TempImage4, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				cvNot(TempImage4, TempImage5);
				if(nInspIdx == 1)
					cvNot(TempImage4, TempImage5);
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
			{
				if(nInspIdx == 1)
				{
					Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				}

				if(nInspIdx == 0)
				{
					cvNot(TempImage4, TempImage4);
					Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				}
				IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
				cvErode(TempImage5, TempImage4, element);
				cvDilate(TempImage4, TempImage5, element);
				if(element != NULL)	cvReleaseStructuringElement(&element);
			}

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);
			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
					}
					else
					{
						TempImage4->imageData[w + (h * TempImage4->widthStep)] = 120;
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			CvBlobSearchPara BlobSearchPara;
			memset(&BlobSearchPara, NULL, sizeof(CvBlobSearchPara));
			if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
			{
				BlobSearchPara.dMinWidth = 10.;
				BlobSearchPara.dMinHeight = 2.;
				BlobSearchPara.dMinArea = 10.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2;
			}
			else
			{
				BlobSearchPara.dMinWidth = (double)cMorpSize.x;
				BlobSearchPara.dMinHeight = (double)cMorpSize.y;
				BlobSearchPara.dMinArea = (double)(cMorpSize.x * cMorpSize.y) / 2.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2.;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2.;
			}
		
			BlobSearchPara.dMaxArea = BlobSearchPara.dMaxWidth * BlobSearchPara.dMaxHeight;
			BlobSearchPara.nMode = 1;
			BlobSearchPara.ucThreshold = 20;
			BlobSearchPara.bUseMask = TRUE;
			BlobSearchPara.nBlobMaxCnt = 1000;
			CvBlobs[nInspIdx].FindBlobs(TempImage4, BlobSearchPara);

			strSavePath.Format(_T("%sMask_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(CvBlobs[nInspIdx].GetMask(), strSavePath);

			if(bTrace)
			{
				for(int h = 0; h < TempImage1->roi->height; h++)
				{
					for(int w = 0; w < TempImage1->roi->width; w++)
					{
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = 0;
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = CvBlobs[nInspIdx].GetMask()->imageData[w + (h * CvBlobs[nInspIdx].GetMask()->widthStep)];
					}
				}
				TraceInsp(m_Display[nCamIdx], TempImage1, &rcRoi, bTrace);

				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			int nBlobCnt = CvBlobs[nInspIdx].GetBlobCount();
			InspPoint[nInspIdx].x = 0.;
			InspPoint[nInspIdx].y = 0.;
		
			RsltLine[2][nInspIdx].x = 0;
			RsltLine[2][nInspIdx].y = 0;
 			if(nBlobCnt > 0)
			{
				CvBlobBox BlobBox;
				CvBlob1 Blob;
				int nStartX = 0, nStartY = 0, nEndX = 0, nEndY = 0;
				int nBlobIdx = 0;
				int nTemp1, nTemp2, nTemp3;

 				if(nBlobCnt > 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ||
									m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							nTemp1 = Blob.y;
						}
						else
						{
							nTemp1 = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 0)
						{
							nTemp1 = Blob.y + Blob.height;
						}
						else
						{
							nTemp1 = Blob.y;
						}
					}
					
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
						{
							if(nInspIdx == 0)
							{
								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else
						{
							if(nInspIdx == 0)
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
						}
						
						if(nTemp3 == nTemp2)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}
				else if(nBlobCnt > 1 && ((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x;
						}
						else
						{
							nTemp1 = Blob.x + Blob.width;
						}
					}
					else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x + Blob.width;
						}
						else
						{
							nTemp1 = Blob.x;
						}
					}
					
					nTemp3 = 0;
					nTemp2 = 0;
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.x;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.x + Blob.width;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.x + Blob.width;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								if(Blob.x >= 5)
								{
									nTemp2 = Blob.x;
									nTemp3 = min(nTemp1, nTemp2);
								}
							}
						}
						
						if(nTemp3 == nTemp2 && nTemp2 != 0 && nTemp3 != 0)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}

				BlobBox = CvBlobs[nInspIdx].GetEachBlobBox(nBlobIdx);
				Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

				CvPoint *arPt1;
				int nCnt = 0, nCnt1 = 0;

				if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
					m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
				{
					arPt1 = new CvPoint[Blob.height - 2];
					for(int h = Blob.y + 1; h < Blob.y + Blob.height - 1; h++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].y == Blob.y || BlobBox.arPt[nEdgeCnt].y == Blob.height)
								continue;

							if(h == BlobBox.arPt[nEdgeCnt].y)
							{
								nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].x;
								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}

						if(nSearchCnt == 2)
						{
							if((nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = max(nTemp[0], nTemp[1]);
							}
							else if((nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = min(nTemp[0], nTemp[1]);
							}
							arPt1[nCnt].y = h;
							nCnt++;
						}
					}
				}
				else
				{
					arPt1 = new CvPoint[Blob.width - 2];
					for(int w = Blob.x + 1; w < Blob.x + Blob.width - 1; w++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].x == Blob.x || BlobBox.arPt[nEdgeCnt].x == Blob.width)
								continue;

							if(w == BlobBox.arPt[nEdgeCnt].x)
							{
								nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].y;
								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}
						if(nSearchCnt == 2)
						{
							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
							{
								arPt1[nCnt].y = max(nTemp[0], nTemp[1]);
							}
							else
							{
								arPt1[nCnt].y = min(nTemp[0], nTemp[1]);
							}

							arPt1[nCnt].x = w;
							nCnt++;
						}
					}
				}

				CvPoint pt1, pt2;
				if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					pt1.x = Blob.x;
					pt1.y = 2;
					pt2.x = Blob.x;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					pt1.x = Blob.x + Blob.width;
					pt1.y = 2;
					pt2.x = Blob.x + Blob.width;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					pt1.x = Blob.x + Blob.width;
					pt1.y = 2;
					pt2.x = Blob.x + Blob.width;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					pt1.x = Blob.x;
					pt1.y = 2;
					pt2.x = Blob.x;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else
				{
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 1)
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
					}
				}

				if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 ||
									 m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2))
				{
					int nCenterY = 0;

					switch(m_ThreadPara.nInspPoint[nCamIdx])
					{
					case INSP_POINT2:
					case INSP_POINT3:
						nCenterY = (int)((double)Blob.y + ((double)Blob.height / 3.));
						break;
					case INSP_POINT5:
					case INSP_POINT6:
						nCenterY = (int)((double)(Blob.y + Blob.height) - ((double)(Blob.height) / 2.));
						break;
					default:

						break;
					}

					//if((Blob.width >= (rcRoi.width - 5)) && (nCenterY > 0))
					if(nCenterY > 0)
					{
						for(int nEdgeCnt = 0; nEdgeCnt < Blob.blobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(Blob.blobBox.arPt[nEdgeCnt].x < 5 || Blob.blobBox.arPt[nEdgeCnt].x > (rcRoi.width - 5))
							{
								continue;
							}

							if(nCenterY == Blob.blobBox.arPt[nEdgeCnt].y)
							{
								if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
								{
									double w1 = 0, w2 = 0;
									int nSearchCnt = 0;
									w1 = Blob.blobBox.arPt[nEdgeCnt].x;
									for(int nEdgeCnt1 = 0; nEdgeCnt1 < BlobBox.nEdgeCnt; nEdgeCnt1++)
									{
										if(nCenterY == BlobBox.arPt[nEdgeCnt1].y)
										{
											if(w1 == Blob.blobBox.arPt[nEdgeCnt1].x)
												continue;

											if(Blob.blobBox.arPt[nEdgeCnt1].x < (Blob.x + (Blob.width / 2.)))
												continue;

											w2 = Blob.blobBox.arPt[nEdgeCnt1].x;
											nSearchCnt++;
										}
										if(nSearchCnt == 2)
										{
											break;
										}
									}

									pt1.x = max(w1, w2);
									pt1.y = 2;
									pt2.x = pt1.x;
									pt2.y = Blob.height - 2;

								}
								else
								{
									pt1.x = Blob.blobBox.arPt[nEdgeCnt].x;
									pt1.y = 2;
									pt2.x = pt1.x;
									pt2.y = Blob.height - 2;
								}
								break;
							}
						}
					}
				}
			
				InspPoint[nInspIdx].x = 0.;
				InspPoint[nInspIdx].y = 0.;
				RsltLine[nInspIdx][0].x = 0;
				RsltLine[nInspIdx][0].y = 0;
				RsltLine[nInspIdx][1].x = 0;
				RsltLine[nInspIdx][1].y = 0;

				if(pt1.x > 0 && pt1.y > 0 && pt2.x > 0 && pt2.y > 0)
				{
					if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
					}
					else if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						RsltLine[nInspIdx][0].x = pt1.x + rcRoi.x;
						RsltLine[nInspIdx][0].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
						RsltLine[nInspIdx][1].x = pt2.x + rcRoi.x;
						RsltLine[nInspIdx][1].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
					}

					if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						if(nInspIdx == 0)
						{
							RsltLine[0][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[0][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							RsltLine[1][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[1][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							cvLine(DispImage, RsltLine[0][0], RsltLine[0][1], CV_RGB(0, 0, 255), 11, CV_AA);
							cvLine(DispImage, RsltLine[1][0], RsltLine[1][1], CV_RGB(0, 0, 255), 11, CV_AA);
							InspPoint[0].x = ((double)RsltLine[0][0].x + (double)RsltLine[0][1].x) / 2.;
							InspPoint[0].y = ((double)RsltLine[0][1].y - (double)RsltLine[0][0].y) / 2. + (double)RsltLine[0][0].y;
							InspPoint[1].x = ((double)RsltLine[1][0].x + (double)RsltLine[1][1].x) / 2.;
							InspPoint[1].y = ((double)RsltLine[1][1].y - (double)RsltLine[1][0].y) / 2. + (double)RsltLine[1][0].y;

							RsltLine[2][0].x = InspPoint[0].x;
							RsltLine[2][0].y = InspPoint[0].y;
							RsltLine[2][1].x = InspPoint[1].x;
							RsltLine[2][1].y = InspPoint[1].y;
						}
					}
					else
					{
						cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						RsltLine[2][nInspIdx].x = InspPoint[nInspIdx].x;
						RsltLine[2][nInspIdx].y = InspPoint[nInspIdx].y;
					}
				}
				
				delete arPt1;
				isSearch[nInspIdx] = TRUE;
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}
	}
	else if(m_ThreadPara.nSelectAlgorithmIdx == BLOOM_ALGO)
	{
		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			if(!m_bInspecting[nCamIdx] && !m_bInspectionEnd[nCamIdx])
			{ 
				m_bInspecting[nCamIdx] = FALSE;
				m_bInspectionEnd[nCamIdx] = TRUE;
				break;
			}
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();

			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);

			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);

			cvSetImageROI(TempImage1, rcRoi);

			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}

			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6) &&
				nInspIdx == 0)
			{
				if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
				{
					cvNot(TempImage2, TempImage3);
					cvThreshold(TempImage3, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
					strSavePath.Format(_T("%sCvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
				else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
				{
					cvThreshold(TempImage2, TempImage3, nThreshHole, 255, CV_THRESH_BINARY);
					strSavePath.Format(_T("%sCvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage3, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage3, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}

					cvNot(TempImage3, TempImage4);
					strSavePath.Format(_T("%scvNot_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
				else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
				{
					cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
					//cvNot(TempImage3, TempImage4);
					strSavePath.Format(_T("%scvThreshHold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
					if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
						SaveIplImage(TempImage4, strSavePath);

					TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

					if(bTrace)
					{
						strText.Format(_T("Step #%d OK?"), iTraceStep++);
						if(AfxMessageBox(strText, MB_YESNO) == IDNO)
							break;
					}
				}
			}
			else
			{
				cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
				strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
			{
				IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
				cvErode(TempImage4, TempImage5, element);
				cvDilate(TempImage5, TempImage4, element);
				if(element != NULL)	cvReleaseStructuringElement(&element);
				
				cvCopy(TempImage4, TempImage5);
			}
			else if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
			{
				if(nInspIdx == 1)
				{
					IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
					cvErode(TempImage4, TempImage5, element);
					cvDilate(TempImage5, TempImage4, element);
					if(element != NULL)	cvReleaseStructuringElement(&element);
			
					cvNot(TempImage4, TempImage5);
				}
				else
				{
					cvNot(TempImage4, TempImage5);
					IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
					cvErode(TempImage5, TempImage4, element);
					cvDilate(TempImage4, TempImage5, element);

					if(element != NULL)	cvReleaseStructuringElement(&element);
				}
			}

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);

			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
					}
					else
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 127;
						}
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			CvBlobSearchPara BlobSearchPara;
			memset(&BlobSearchPara, NULL, sizeof(CvBlobSearchPara));

			if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
			{
				BlobSearchPara.dMinWidth = 10.;
				BlobSearchPara.dMinHeight = 5.;
				BlobSearchPara.dMinArea = 10.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2;
			}
			else
			{
				if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
				{
					BlobSearchPara.dMinWidth = (double)(AM_SIZE_X_Y - AM_OFFSET_X);
					BlobSearchPara.dMinHeight = (double)(AM_SIZE_X_Y - AM_OFFSET_X);
				}
				else if(nInspIdx == 1)
				{
					BlobSearchPara.dMinWidth = (double)rcRoi.width / 2;
					BlobSearchPara.dMinHeight = (double)100.;
				}
				else if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
				{
					BlobSearchPara.dMinWidth = (double)10.;
					BlobSearchPara.dMinHeight = (double)rcRoi.height;
				}
				else if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
				{
					BlobSearchPara.dMinWidth = (double)cMorpSize.x;
					BlobSearchPara.dMinHeight = (double)50;
				}
				
				BlobSearchPara.dMinArea = (double)(BlobSearchPara.dMinWidth * cMorpSize.y) / 2.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2.;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2.;
			}
		
			BlobSearchPara.dMaxArea = BlobSearchPara.dMaxWidth * BlobSearchPara.dMaxHeight;
			BlobSearchPara.nMode = 1;
			BlobSearchPara.ucThreshold = 20;
			BlobSearchPara.bUseMask = TRUE;
			BlobSearchPara.nBlobMaxCnt = 1000;

			CvBlobs[nInspIdx].FindBlobs(TempImage4, BlobSearchPara);

			strSavePath.Format(_T("%sMask_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(CvBlobs[nInspIdx].GetMask(), strSavePath);
			
			if(bTrace)
			{
				for(int h = 0; h < TempImage1->roi->height; h++)
				{
					for(int w = 0; w < TempImage1->roi->width; w++)
					{
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = 0;
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = CvBlobs[nInspIdx].GetMask()->imageData[w + (h * CvBlobs[nInspIdx].GetMask()->widthStep)];
					}
				}
				TraceInsp(m_Display[nCamIdx], TempImage1, &rcRoi, bTrace);

				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			int nBlobCnt = CvBlobs[nInspIdx].GetBlobCount();

			InspPoint[nInspIdx].x = 0.;
			InspPoint[nInspIdx].y = 0.;
		
			RsltLine[2][nInspIdx].x = 0;
			RsltLine[2][nInspIdx].y = 0;

 			if(nBlobCnt > 0)
			{
				CvBlob1 Blob;
				int nStartX = 0, nStartY = 0, nEndX = 0, nEndY = 0;
				int nBlobIdx = 0;
				int nTemp1, nTemp2, nTemp3;

 				if(nBlobCnt > 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ||
									m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y;
						}
						else
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 0)
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
						else
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
					}
					
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
						{
							if(nInspIdx == 0)
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else
						{
							if(nInspIdx == 0)
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = min(nTemp1, nTemp2);
							}
						}
						
						if(nTemp3 == nTemp2)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}
				else if(nBlobCnt > 0 && ((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);
					int nMaxWidth = Blob.width, nMaxWidthIdx = 0;
					
					for(int nBlobNextIdx = 1; nBlobNextIdx < CvBlobs[nInspIdx].GetBlobCount(); nBlobNextIdx++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobNextIdx);

						nTemp2 = Blob.width;
						nTemp1 = nMaxWidth;
						nMaxWidth = max(nTemp1, nTemp2);

						if(nMaxWidth == nTemp2)
						{
							nMaxWidthIdx = nBlobNextIdx;
						}
					}

					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x;
							nTemp1 = Blob.area;
						}
						else
						{
							if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
							{
								nTemp1 = Blob.x + Blob.width;
							}
							else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
							{
								if((Blob.height) < rcRoi.height - 5)
								{
									Blob = CvBlobs[nInspIdx].GetEachBlob(1);
									nTemp1 = Blob.x + Blob.width;
								}
							}
							else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
							{
								nTemp1 = Blob.width;
							}
						}
					}
					else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.height;
							nTemp1 = Blob.area;
						}
						else
						{
							if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
							{
								nTemp1 = Blob.x;
							}
							else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
							{
								if((Blob.height) < rcRoi.height - 5)
								{
									Blob = CvBlobs[nInspIdx].GetEachBlob(1);
									nTemp1 = Blob.x;
								}
							}
							else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
							{
								nTemp1 = Blob.width;
							}
						}
					}

					nTemp3 = 0;
					nTemp2 = 0;

					if(nInspIdx == 0 && m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP &&
						(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
						m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
					{
						BOOL isLongSearch = FALSE;

						if(CvBlobs[nInspIdx].GetBlobCount() == 2)
						{
							BOOL isLong[2];
							isLong[0] = FALSE;
							isLong[1] = FALSE;

							for(int i = 0; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
							{
								Blob = CvBlobs[nInspIdx].GetEachBlob(i);
								if(Blob.width > AM_SIZE_X_Y / 2)
								{
									isLong[i] = TRUE;
								}
							}

							if(isLong[0] && isLong[1])
							{
								isLongSearch = TRUE;
								CvBlob1 Blob2;
								Blob = CvBlobs[nInspIdx].GetEachBlob(0);
								Blob2 = CvBlobs[nInspIdx].GetEachBlob(1);

								int nMinX = min(Blob.x, Blob2.x);

								if(nMinX == Blob.x)
								{
									Blob.blobBox.center.x = (((Blob2.x + Blob2.width) - Blob.x) / 2) + Blob.x;
									Blob.blobBox.center.y = Blob.y + (Blob.height / 2);
									Blob.width = AM_SIZE_X_Y;
								}
								else
								{
									Blob.blobBox.center.x = (((Blob.x + Blob.width) - Blob2.x) / 2) + Blob2.x;
									Blob.blobBox.center.y = Blob2.y + (Blob2.height / 2);
									Blob.width = AM_SIZE_X_Y;
								}
							}
						}
						
						if(!isLongSearch && CvBlobs[nInspIdx].GetBlobCount() > 1)
						{
							BOOL isBlobSearch = FALSE;
							for(int i = 0; i < CvBlobs[nInspIdx].GetBlobCount() - 1; i++)
							{
								Blob = CvBlobs[nInspIdx].GetEachBlob(i);

								CvBlob1 Blob2;

								for(int j = i + 1; j < CvBlobs[nInspIdx].GetBlobCount(); j++)
								{
									Blob2 = CvBlobs[nInspIdx].GetEachBlob(j);

									int nMinX = min(Blob.x, Blob2.x);

									if(nMinX == Blob.x)
									{
										if((Blob2.blobBox.LeftTop.x - Blob.blobBox.RightTop.x) > CI_PI_SPACE)
											continue;
									}
									else
									{
										if((Blob.blobBox.LeftTop.x - Blob2.blobBox.RightTop.x) > CI_PI_SPACE)
											continue;
									}

									int nStartX, nEndX, nSEWidth;
									nStartX = min(Blob.blobBox.LeftTop.x, Blob2.blobBox.LeftTop.x);
									nEndX = max(Blob.blobBox.RightTop.x, Blob2.blobBox.RightTop.x);
									nSEWidth = nEndX - nStartX;

									if((AM_SIZE_X_Y - AM_OFFSET_X) <= (nSEWidth) && (nSEWidth) <= (AM_SIZE_X_Y + AM_OFFSET_X))
									{
										Blob.x = min(Blob.blobBox.LeftTop.x, Blob2.blobBox.LeftTop.x);
										Blob.y = min(Blob.blobBox.LeftTop.y, Blob2.blobBox.LeftTop.y);
										Blob.width = nSEWidth;
										Blob.height = max(Blob.blobBox.LeftBottom.y, Blob2.blobBox.LeftBottom.y) - Blob.y;
										Blob.blobBox.center.x = Blob.x + (Blob.width / 2);
										Blob.blobBox.center.y = Blob.y + (Blob.height / 2);

										isBlobSearch = TRUE;
										break;
									}
								}
								if(isBlobSearch)
									break;
							}
						}
						else if(!isLongSearch && CvBlobs[nInspIdx].GetBlobCount() == 1)
						{
							int nSEWidth;
							nSEWidth = Blob.width;
							if((AM_SIZE_X_Y - AM_OFFSET_X) <= (nSEWidth) && (nSEWidth) <= (AM_SIZE_X_Y + AM_OFFSET_X))
							{
								Blob.x = Blob.x;
								Blob.y = Blob.y;
								Blob.width = nSEWidth;
								Blob.height = Blob.height;
								Blob.blobBox.center.x = Blob.x + (Blob.width / 2);
								Blob.blobBox.center.y = Blob.y + (Blob.height / 2);
							}
						}
					}
					else
					{
						if(nInspIdx == 0)
						{
							if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
							{
								if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
								{
									nTemp3 = Blob.x;
									nTemp2 = Blob.x;
								}

								for(int i = 0; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
								{
									//if(nInspIdx == 0 && nMaxWidthIdx == i)
									//	continue;

									Blob = CvBlobs[nInspIdx].GetEachBlob(i);

									if(Blob.height < 100)
										continue;

									if(Blob.width > (rcRoi.width / 2))
										//continue;
						
									if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
									{
										if(nInspIdx == 1)
										{
											nTemp2 = Blob.x;
											nTemp2 = Blob.area;
											nTemp3 = max(nTemp1, nTemp2);
										}
										else
										{
											nTemp2 = Blob.x + Blob.width;
											nTemp3 = max(nTemp1, nTemp2);
										}
									}
									else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
									{
										if(nInspIdx == 1)
										{
											nTemp2 = Blob.height;
											nTemp2 = Blob.area;
											nTemp3 = max(nTemp1, nTemp2);
										}
										else
										{
											nTemp2 = Blob.x;
											nTemp3 = min(nTemp1, nTemp2);
										}
									}
						
									if(nInspIdx == 1)
									{
										if(nTemp3 == nTemp2 && nTemp2 != 0 && nTemp3 != 0)
										{
											nBlobIdx = i;
											nTemp1 = nTemp3;
										}
									}
									else
									{
										if(nTemp3 == nTemp2)
										{
											nBlobIdx = i;
											nTemp1 = nTemp3;
										}
									}
								}
							}
							else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
							{
								for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
								{
									Blob = CvBlobs[nInspIdx].GetEachBlob(i);

									if(Blob.height <= (rcRoi.height - 5))
										continue;

									if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
									{
										nTemp2 = Blob.x + Blob.width;
										nTemp3 = max(nTemp1, nTemp2);
									}
									else
									{
										nTemp2 = Blob.x;
										nTemp3 = min(nTemp1, nTemp2);
									}

									if(nTemp3 == nTemp2)
									{
										nBlobIdx = i;
										nTemp1 = nTemp3;
									}
								}
							}
						}
						else
						{
							Blob = CvBlobs[nInspIdx].GetEachBlob(0);
							int nBlobWidth = 0, nBlobX = 0;
							int nBlobYMax = 0, nBlobYMin = rcRoi.height, nBlobYTemp = 0;

							for(int i = 0; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
							{
								Blob = CvBlobs[nInspIdx].GetEachBlob(i);
								nBlobWidth = Blob.width;
								nBlobX = Blob.x;
							
								if(nBlobWidth < (rcRoi.width / 2))
									continue;

								if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
								{
									nBlobYTemp = Blob.y;
									nBlobYMin = min(nBlobYTemp, nBlobYMin);

									if(nBlobYMin == Blob.y)
										nBlobIdx = i;
								}
								else
								{
									nBlobYTemp = Blob.y;
									nBlobYMax = max(nBlobYTemp, nBlobYMax);

									if(nBlobYMax == Blob.y)
										nBlobIdx = i;
								}
							}
						}
					}
				}

				CvPoint pt1(0, 0), pt2(0, 0);
				if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

						pt1.x = Blob.x + Blob.width;
						pt1.y = 2;
						pt2.x = Blob.x + Blob.width;
						pt2.y = Blob.height - 2;
					}
					else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

						pt1.x = Blob.x + Blob.width;
						pt1.y = 2;
						pt2.x = Blob.x + Blob.width;
						pt2.y = Blob.height - 2;
					}
					else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
					{
						pt1.x = Blob.blobBox.center.x;
						pt1.y = 2;
						pt2.x = Blob.blobBox.center.x;
						pt2.y = Blob.height - 2;

						CvPoint cvCenter;
						cvCenter.x = Blob.blobBox.center.x + rcRoi.x;
						cvCenter.y = Blob.blobBox.center.y + rcRoi.y;
						cvCircle(DispImage, cvCenter, Blob.width / 2, CV_RGB(255, 0, 255), 11);
					}
				}
				else if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					if(m_ThreadPara.nMetalAlgorithmIdx == METAL_RIV_INSP)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

						pt1.x = Blob.x;
						pt2.x = Blob.x;
						pt1.y = 2;
						pt2.y = Blob.height - 2;
					}
					else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_HOUSING_INSP)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

						pt1.x = Blob.x;
						pt2.x = Blob.x;
						pt1.y = 2;
						pt2.y = Blob.height - 2;
					}
					else if(m_ThreadPara.nMetalAlgorithmIdx == METAL_AM_INSP)
					{
						pt1.x = Blob.blobBox.center.x;
						pt1.y = 2;
						pt2.x = Blob.blobBox.center.x;
						pt2.y = Blob.height - 2;

						CvPoint cvCenter;
						cvCenter.x = Blob.blobBox.center.x + rcRoi.x;
						cvCenter.y = Blob.blobBox.center.y + rcRoi.y;
						cvCircle(DispImage, cvCenter, Blob.width / 2, CV_RGB(255, 0, 255), 11);
					}
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);
					IplImage *SearchImage = cvCloneImage(CvBlobs[nInspIdx].GetMask());
					int nTempX1, nTempCnt = 0, nSerVal = 0, nSerchOffset = INSP_SEARCH_OFFSET;

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						nSerVal = Blob.y + Blob.height - INSP_SEARCH_VAL;
					}
					else
					{
						nSerVal = Blob.y - INSP_SEARCH_VAL;
					}

					int nMaxSerCnt = Blob.height / nSerchOffset;
					int nSerY[5] = {0,};
					int nSerX[5] = {0,};
					int nMinY = nSerY[0], nTempY = nSerY[0], nSerCnt = 4;

					for(int nSerRep = 0; nSerRep < 5; nSerRep++)
					{
						int nSerXVal = Blob.width - ((Blob.width / 6) * (nSerRep + 1));
						int nSerTemp[2] = {0,}, nTempIdx = 0;
						int *nYIdx, nYIdxCnt = 0;
						nYIdx = new int[SearchImage->height];
						nSerX[nSerRep] = nSerXVal;

						//20191217 ngh
						for(int nEdgeCnt = 0; nEdgeCnt < Blob.blobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(Blob.blobBox.arPt[nEdgeCnt].x == nSerXVal)
							{
								nYIdx[nYIdxCnt++] = Blob.blobBox.arPt[nEdgeCnt].y;
							}
						}

						nMinY = nYIdx[0];

						for(int nYCnt = 1; nYCnt < nYIdxCnt; nYCnt++)
						{
							nTempY = nYIdx[nYCnt];

							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
								nMinY = min(nTempY, nMinY);
							else
								nMinY = max(nTempY, nMinY);
						}

						nSerY[nSerRep] = nMinY;

						delete nYIdx;
					}
					
					for(int nSerRep = 0; nSerRep < 5; nSerRep++)
					{
						if(nSerY[nSerRep] <= 0)
							nSerCnt = nSerRep + 1;
					}

					nMinY = nSerY[0];

					for(int nSerRep = 1; nSerRep < nSerCnt; nSerRep++)
					{
						nTempY = nSerY[nSerRep];

						if(nSerY[nSerRep] <= 0)
							continue;

						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
							nMinY = max(nMinY, nTempY);
						else
							nMinY = min(nMinY, nTempY);
					}

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						nSerVal = nMinY - (INSP_SEARCH_VAL * 2);

						if(nSerVal >= 0)
						{
							for(int nXCnt = SearchImage->width - 1; nXCnt > 0; nXCnt--)
							{
								if(SearchImage->imageData[nXCnt + (nSerVal * SearchImage->widthStep)] != 0)
								{
									nTempX1 = nXCnt;
									break;
								}
							}
						}
					}
					else
					{
						nSerVal = nMinY + (INSP_SEARCH_VAL * 2);

						if(nSerVal <= SearchImage->height)
						{
							for(int nXCnt = SearchImage->width - 1; nXCnt > 0; nXCnt--)
							{
								if(SearchImage->imageData[nXCnt + (nSerVal * SearchImage->widthStep)] != 0)
								{
									nTempX1 = nXCnt;
									break;
								}
							}
						}
					}

					pt1.x = nTempX1;
					pt2.x = nTempX1;
					pt1.y = 2;
					pt2.y = Blob.y + Blob.height - 2;

					if (SearchImage)		cvReleaseImage(&SearchImage);
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);
					IplImage *SearchImage = cvCloneImage(CvBlobs[nInspIdx].GetMask());
					int nTempX1, nTempCnt = 0, nSerVal = 0, nSerchOffset = INSP_SEARCH_OFFSET;

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						nSerVal = Blob.y + INSP_SEARCH_VAL;
					}
					else
					{
						nSerVal = Blob.y + Blob.height - INSP_SEARCH_VAL;
					}

					int nMaxSerCnt = Blob.height / nSerchOffset;
					int nSerY[5] = {0,};
					int nSerX[5] = {0,};
					int nSerTemp[2] = {0,}, nTempIdx = 0;
					int nMinY = nSerY[0], nTempY = nSerY[0], nSerCnt = 4;

					for(int nSerRep = 0; nSerRep < 5; nSerRep++)
					{
						int nSerXVal = Blob.x + ((Blob.width / 6) * (nSerRep + 1));
						int nSerTemp[2] = {0,}, nTempIdx = 0;
						int *nYIdx, nYIdxCnt = 0;
						nYIdx = new int[SearchImage->height];
						nSerX[nSerRep] = nSerXVal;

						//20191217 ngh
						for(int nEdgeCnt = 0; nEdgeCnt < Blob.blobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(Blob.blobBox.arPt[nEdgeCnt].x == nSerXVal)
							{
								nYIdx[nYIdxCnt++] = Blob.blobBox.arPt[nEdgeCnt].y;
							}
						}

						nMinY = nYIdx[0];

						for(int nYCnt = 1; nYCnt < nYIdxCnt; nYCnt++)
						{
							nTempY = nYIdx[nYCnt];

							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
								nMinY = min(nTempY, nMinY);
							else
								nMinY = max(nTempY, nMinY);
						}

						nSerY[nSerRep] = nMinY;

						delete nYIdx;
					}

					for(int nSerRep = 0; nSerRep < 5; nSerRep++)
					{
						if(nSerY[nSerRep] == 0)
							nSerCnt = nSerRep + 1;
					}

					nMinY = nSerY[0];

					for(int nSerRep = 1; nSerRep < nSerCnt; nSerRep++)
					{
						nTempY = nSerY[nSerRep];

						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
							nMinY = max(nMinY, nTempY);
						else
							nMinY = min(nMinY, nTempY);
					}

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						nSerVal = nMinY - (INSP_SEARCH_VAL * 2);

						if(nSerVal >= 0)
						{
							for(int nXCnt = 0; nXCnt < SearchImage->width; nXCnt++)
							{
								if(SearchImage->imageData[nXCnt + (nSerVal * SearchImage->widthStep)] != 0)
								{
									nTempX1 = nXCnt;
									break;
								}
							}
						}
					}
					else
					{
						nSerVal = nMinY + (INSP_SEARCH_VAL * 2);

						if(nSerVal <= SearchImage->height)
						{
							for(int nXCnt = 0; nXCnt < SearchImage->width; nXCnt++)
							{
								if(SearchImage->imageData[nXCnt + (nSerVal * SearchImage->widthStep)] != 0)
								{
									nTempX1 = nXCnt;
									break;
								}
							}
						}
					}

					pt1.x = nTempX1;
					pt2.x = nTempX1;
					pt1.y = 2;
					pt2.y = Blob.y + Blob.height - 2;

					if (SearchImage)		cvReleaseImage(&SearchImage);
				}
				else
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 1)
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
					}
				}
			
				InspPoint[nInspIdx].x = 0.;
				InspPoint[nInspIdx].y = 0.;

				RsltLine[nInspIdx][0].x = 0;
				RsltLine[nInspIdx][0].y = 0;
				RsltLine[nInspIdx][1].x = 0;
				RsltLine[nInspIdx][1].y = 0;

				if(pt1.x > 0 && pt1.y > 0 && pt2.x > 0 && pt2.y > 0)
				{
					if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
					}
					else if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						RsltLine[nInspIdx][0].x = pt1.x + rcRoi.x;
						RsltLine[nInspIdx][0].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
						RsltLine[nInspIdx][1].x = pt2.x + rcRoi.x;
						RsltLine[nInspIdx][1].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
					}

					if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						
						if(nInspIdx == 0)
						{
							RsltLine[0][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[0][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							RsltLine[1][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[1][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);

							cvLine(DispImage, RsltLine[0][0], RsltLine[0][1], CV_RGB(0, 0, 255), 11, CV_AA);
							cvLine(DispImage, RsltLine[1][0], RsltLine[1][1], CV_RGB(0, 0, 255), 11, CV_AA);

							InspPoint[0].x = ((double)RsltLine[0][0].x + (double)RsltLine[0][1].x) / 2.;
							InspPoint[0].y = ((double)RsltLine[0][1].y - (double)RsltLine[0][0].y) / 2. + (double)RsltLine[0][0].y;
							InspPoint[1].x = ((double)RsltLine[1][0].x + (double)RsltLine[1][1].x) / 2.;
							InspPoint[1].y = ((double)RsltLine[1][1].y - (double)RsltLine[1][0].y) / 2. + (double)RsltLine[1][0].y;

							RsltLine[2][0].x = InspPoint[0].x;
							RsltLine[2][0].y = InspPoint[0].y;
							RsltLine[2][1].x = InspPoint[1].x;
							RsltLine[2][1].y = InspPoint[1].y;
						}
					}
					else
					{
						cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);

						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						
						RsltLine[2][nInspIdx].x = InspPoint[nInspIdx].x;
						RsltLine[2][nInspIdx].y = InspPoint[nInspIdx].y;
					}

					isSearch[nInspIdx] = TRUE;
				}
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}
	}

	if(isSearch[0] && isSearch[1])
		m_Rslt[nCamIdx].bGapRslt = TRUE;

	if(InspPoint[0].x > 0. && InspPoint[0].y > 0. && InspPoint[1].x > 0. && InspPoint[1].y > 0. &&
		RsltLine[2][0].x > 0 && RsltLine[2][0].y > 0 && RsltLine[2][1].x > 0 && RsltLine[2][1].y > 0)
	{
		m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
		m_Rslt[nCamIdx].dGapRslt = (GetDistance(InspPoint[0], InspPoint[1]) * m_ThreadPara.dPixelSizeWidth) / 1000. + m_ThreadPara.SpecPoint[nCamIdx].dOffset;

		cvLine(DispImage, RsltLine[2][0], RsltLine[2][1], CV_RGB(0, 255, 0), 11, CV_AA);
	}

	//m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
	//m_Rslt[nCamIdx].dGapRslt = 1.;

	m_Rslt[nCamIdx].bPointRslt = FALSE;

	if((m_ThreadPara.SpecPoint[nCamIdx].dMinSpec / 2.) < (m_Rslt[nCamIdx].dGapRslt / 2.) && (m_Rslt[nCamIdx].dGapRslt / 2.) < (m_ThreadPara.SpecPoint[nCamIdx].dMaxSpec / 2.))
	{
		m_Rslt[nCamIdx].bPointRslt = TRUE;
	}

	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);

	if (SrcImage)		cvReleaseImage(&SrcImage);
	if (DispImage)		cvReleaseImage(&DispImage);

	strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection End -> Rslt : %0.3f"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]), m_Rslt[nCamIdx].dGapRslt);
	m_cLogFile->WriteText(strText);

	return dRslt;
}
#else
double CVisionInspection::GapInspection(int nCamIdx)
{
	double dRslt = 0.;
	
	CString strSavePath, strText;
	int iTraceStep = 0;
	BOOL bSaveImage = m_ThreadPara.bInspImageSave;
	BOOL bSaveRsltImage = m_ThreadPara.bRsltImageSave;
	BOOL bTrace = m_ThreadPara.bTrace;
	IplImage *SrcImage = cvCloneImage(m_ThreadPara.InspImage[nCamIdx]);
	IplImage *DispImage = cvCloneImage(m_ThreadPara.DispImage[nCamIdx]);
	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
	CvMemStorage *cvStorage;
	cvBlobFinder CvBlobs[MAX_INSP_CNT];
	CvPoint2D32f InspPoint[2];
	CvPoint SmoothKernel;
	CvPoint cMorpSize;
	int nThreshHole;
	float fSizeThresh;
	CvPoint2D32f SmoothKernelSigma;
	CvRect rcRoi;
	int nMaxInspCnt = MAX_INSP_CNT;
	IplImage *TempImage1;
	IplImage *TempImage2;
	IplImage *TempImage3;
	IplImage *TempImage4;
	IplImage *TempImage5;
	CvPoint RsltLine[MAX_INSP_CNT + 1][2];
	BOOL isSearch[2] = {FALSE,};

	m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
	m_Rslt[nCamIdx].dGapRslt = 0.;
	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);
	m_Rslt[nCamIdx].bGapRslt = FALSE;

	strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection Start"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]));
	m_cLogFile->WriteText(strText);

	if(m_ThreadPara.nSelectAlgorithmIdx == WINNER_ALGO)
	{
		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();
			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);
			cvSetImageROI(TempImage1, rcRoi);
			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6) &&
				nInspIdx == 0)
			{
				cvNot(TempImage2, TempImage3);
				strSavePath.Format(_T("%scvNot_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage3, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage3, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				CvThreshHoldRange(TempImage3, TempImage2, (255 - nThreshHole), 30);

				strSavePath.Format(_T("%sCvThreshHoldRange_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				cvNot(TempImage2, TempImage4);
				strSavePath.Format(_T("%scvAnd_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}
			else
			{
				cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
				strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}

			TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
			{
				Morphology(TempImage4, TempImage4, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				cvNot(TempImage4, TempImage5);
				if(nInspIdx == 1)
					cvNot(TempImage4, TempImage5);
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
			{
				if(nInspIdx == 1)
				{
					Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				}

				if(nInspIdx == 0)
				{
					cvNot(TempImage4, TempImage4);
					Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				}
				IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
				cvErode(TempImage5, TempImage4, element);
				cvDilate(TempImage4, TempImage5, element);
				if(element != NULL)	cvReleaseStructuringElement(&element);
			}

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);
			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
					}
					else
					{
						TempImage4->imageData[w + (h * TempImage4->widthStep)] = 120;
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			CvBlobSearchPara BlobSearchPara;
			memset(&BlobSearchPara, NULL, sizeof(CvBlobSearchPara));
			if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
			{
				BlobSearchPara.dMinWidth = 10.;
				BlobSearchPara.dMinHeight = 2.;
				BlobSearchPara.dMinArea = 10.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2;
			}
			else
			{
				BlobSearchPara.dMinWidth = (double)cMorpSize.x;
				BlobSearchPara.dMinHeight = (double)cMorpSize.y;
				BlobSearchPara.dMinArea = (double)(cMorpSize.x * cMorpSize.y) / 2.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2.;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2.;
			}
		
			BlobSearchPara.dMaxArea = BlobSearchPara.dMaxWidth * BlobSearchPara.dMaxHeight;
			BlobSearchPara.nMode = 1;
			BlobSearchPara.ucThreshold = 20;
			BlobSearchPara.bUseMask = TRUE;
			CvBlobs[nInspIdx].FindBlobs(TempImage4, BlobSearchPara);

			strSavePath.Format(_T("%sMask_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(CvBlobs[nInspIdx].GetMask(), strSavePath);

			if(bTrace)
			{
				for(int h = 0; h < TempImage1->roi->height; h++)
				{
					for(int w = 0; w < TempImage1->roi->width; w++)
					{
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = 0;
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = CvBlobs[nInspIdx].GetMask()->imageData[w + (h * CvBlobs[nInspIdx].GetMask()->widthStep)];
					}
				}
				TraceInsp(m_Display[nCamIdx], TempImage1, &rcRoi, bTrace);

				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			int nBlobCnt = CvBlobs[nInspIdx].GetBlobCount();
			InspPoint[nInspIdx].x = 0.;
			InspPoint[nInspIdx].y = 0.;
		
			RsltLine[2][nInspIdx].x = 0;
			RsltLine[2][nInspIdx].y = 0;
 			if(nBlobCnt > 0)
			{
				CvBlobBox BlobBox;
				CvBlob1 Blob;
				int nStartX = 0, nStartY = 0, nEndX = 0, nEndY = 0;
				int nBlobIdx = 0;
				int nTemp1, nTemp2, nTemp3;

 				if(nBlobCnt > 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ||
									m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							nTemp1 = Blob.y;
						}
						else
						{
							nTemp1 = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 0)
						{
							nTemp1 = Blob.y + Blob.height;
						}
						else
						{
							nTemp1 = Blob.y;
						}
					}
					
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
						{
							if(nInspIdx == 0)
							{
								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else
						{
							if(nInspIdx == 0)
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
						}
						
						if(nTemp3 == nTemp2)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}
				else if(nBlobCnt > 1 && ((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x;
						}
						else
						{
							nTemp1 = Blob.x + Blob.width;
						}
					}
					else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x + Blob.width;
						}
						else
						{
							nTemp1 = Blob.x;
						}
					}
					
					nTemp3 = 0;
					nTemp2 = 0;
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.x;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.x + Blob.width;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.x + Blob.width;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								if(Blob.x >= 5)
								{
									nTemp2 = Blob.x;
									nTemp3 = min(nTemp1, nTemp2);
								}
							}
						}
						
						if(nTemp3 == nTemp2 && nTemp2 != 0 && nTemp3 != 0)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}

				BlobBox = CvBlobs[nInspIdx].GetEachBlobBox(nBlobIdx);
				Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);

				CvPoint *arPt1;
				int nCnt = 0, nCnt1 = 0;

				if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
					m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
				{
					arPt1 = new CvPoint[Blob.height - 2];
					for(int h = Blob.y + 1; h < Blob.y + Blob.height - 1; h++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].y == Blob.y || BlobBox.arPt[nEdgeCnt].y == Blob.height)
								continue;

							if(h == BlobBox.arPt[nEdgeCnt].y)
							{
								nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].x;
								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}

						if(nSearchCnt == 2)
						{
							if((nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = max(nTemp[0], nTemp[1]);
							}
							else if((nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = min(nTemp[0], nTemp[1]);
							}
							arPt1[nCnt].y = h;
							nCnt++;
						}
					}
				}
				else
				{
					arPt1 = new CvPoint[Blob.width - 2];
					for(int w = Blob.x + 1; w < Blob.x + Blob.width - 1; w++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].x == Blob.x || BlobBox.arPt[nEdgeCnt].x == Blob.width)
								continue;

							if(w == BlobBox.arPt[nEdgeCnt].x)
							{
								nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].y;
								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}
						if(nSearchCnt == 2)
						{
							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
							{
								arPt1[nCnt].y = max(nTemp[0], nTemp[1]);
							}
							else
							{
								arPt1[nCnt].y = min(nTemp[0], nTemp[1]);
							}

							arPt1[nCnt].x = w;
							nCnt++;
						}
					}
				}

				CvPoint pt1, pt2;
				if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					pt1.x = Blob.x;
					pt1.y = 2;
					pt2.x = Blob.x;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					pt1.x = Blob.x + Blob.width;
					pt1.y = 2;
					pt2.x = Blob.x + Blob.width;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					pt1.x = Blob.x + Blob.width;
					pt1.y = 2;
					pt2.x = Blob.x + Blob.width;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					pt1.x = Blob.x;
					pt1.y = 2;
					pt2.x = Blob.x;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else
				{
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 1)
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;
						}
					}
				}

				if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 ||
									 m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2))
				{
					int nCenterY = 0;

					switch(m_ThreadPara.nInspPoint[nCamIdx])
					{
					case INSP_POINT2:
					case INSP_POINT3:
						nCenterY = (int)((double)Blob.y + ((double)Blob.height / 3.));
						break;
					case INSP_POINT5:
					case INSP_POINT6:
						nCenterY = (int)((double)(Blob.y + Blob.height) - ((double)(Blob.height) / 2.));
						break;
					default:

						break;
					}

					//if((Blob.width >= (rcRoi.width - 5)) && (nCenterY > 0))
					if(nCenterY > 0)
					{
						for(int nEdgeCnt = 0; nEdgeCnt < Blob.blobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(Blob.blobBox.arPt[nEdgeCnt].x < 5 || Blob.blobBox.arPt[nEdgeCnt].x > (rcRoi.width - 5))
							{
								continue;
							}

							if(nCenterY == Blob.blobBox.arPt[nEdgeCnt].y)
							{
								if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
								{
									double w1 = 0, w2 = 0;
									int nSearchCnt = 0;
									w1 = Blob.blobBox.arPt[nEdgeCnt].x;
									for(int nEdgeCnt1 = 0; nEdgeCnt1 < BlobBox.nEdgeCnt; nEdgeCnt1++)
									{
										if(nCenterY == BlobBox.arPt[nEdgeCnt1].y)
										{
											if(w1 == Blob.blobBox.arPt[nEdgeCnt1].x)
												continue;

											if(Blob.blobBox.arPt[nEdgeCnt1].x < (Blob.x + (Blob.width / 2.)))
												continue;

											w2 = Blob.blobBox.arPt[nEdgeCnt1].x;
											nSearchCnt++;
										}
										if(nSearchCnt == 2)
										{
											break;
										}
									}

									pt1.x = max(w1, w2);
									pt1.y = 2;
									pt2.x = pt1.x;
									pt2.y = Blob.height - 2;

								}
								else
								{
									pt1.x = Blob.blobBox.arPt[nEdgeCnt].x;
									pt1.y = 2;
									pt2.x = pt1.x;
									pt2.y = Blob.height - 2;
								}
								break;
							}
						}
					}
				}
			
				InspPoint[nInspIdx].x = 0.;
				InspPoint[nInspIdx].y = 0.;
				RsltLine[nInspIdx][0].x = 0;
				RsltLine[nInspIdx][0].y = 0;
				RsltLine[nInspIdx][1].x = 0;
				RsltLine[nInspIdx][1].y = 0;

				if(pt1.x > 0 && pt1.y > 0 && pt2.x > 0 && pt2.y > 0)
				{
					if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
					}
					else if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						RsltLine[nInspIdx][0].x = pt1.x + rcRoi.x;
						RsltLine[nInspIdx][0].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
						RsltLine[nInspIdx][1].x = pt2.x + rcRoi.x;
						RsltLine[nInspIdx][1].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
					}

					if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						if(nInspIdx == 0)
						{
							RsltLine[0][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[0][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							RsltLine[1][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[1][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							cvLine(DispImage, RsltLine[0][0], RsltLine[0][1], CV_RGB(0, 0, 255), 11, CV_AA);
							cvLine(DispImage, RsltLine[1][0], RsltLine[1][1], CV_RGB(0, 0, 255), 11, CV_AA);
							InspPoint[0].x = ((double)RsltLine[0][0].x + (double)RsltLine[0][1].x) / 2.;
							InspPoint[0].y = ((double)RsltLine[0][1].y - (double)RsltLine[0][0].y) / 2. + (double)RsltLine[0][0].y;
							InspPoint[1].x = ((double)RsltLine[1][0].x + (double)RsltLine[1][1].x) / 2.;
							InspPoint[1].y = ((double)RsltLine[1][1].y - (double)RsltLine[1][0].y) / 2. + (double)RsltLine[1][0].y;

							RsltLine[2][0].x = InspPoint[0].x;
							RsltLine[2][0].y = InspPoint[0].y;
							RsltLine[2][1].x = InspPoint[1].x;
							RsltLine[2][1].y = InspPoint[1].y;
						}
					}
					else
					{
						cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						RsltLine[2][nInspIdx].x = InspPoint[nInspIdx].x;
						RsltLine[2][nInspIdx].y = InspPoint[nInspIdx].y;
					}
				}
				
				delete arPt1;
				isSearch[nInspIdx] = TRUE;
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}
	}
	else if(m_ThreadPara.nSelectAlgorithmIdx == BLOOM_ALGO)
	{
		for(int nInspIdx = nMaxInspCnt - 1; nInspIdx > -1; nInspIdx--)
		{
			cvStorage = cvCreateMemStorage(0);

			rcRoi.x = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].left;
			rcRoi.y = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].top;
			rcRoi.width = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Width();
			rcRoi.height = m_ThreadPara.InspPara[nCamIdx].InspRect[nInspIdx].Height();
			TempImage1 = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
			cvCopy(m_ThreadPara.InspImage[nCamIdx], TempImage1);
			cvSetImageROI(TempImage1, rcRoi);
			TempImage2 = cvCloneImage(TempImage1);
			TempImage3 = cvCloneImage(TempImage1);
			TempImage4 = cvCloneImage(TempImage1);
			TempImage5 = cvCloneImage(TempImage1);

			cMorpSize = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].cMorpSize;
			nThreshHole = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nThreshHold;
			fSizeThresh = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nSizeThresh / (float)100;

			SmoothKernel.x = 1;
			SmoothKernel.y = 1;
			SmoothKernelSigma.x = 0.;
			SmoothKernelSigma.y = 0.;

			if(m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nDirection ==INSP_DIRECTION_VERTICAL)
			{
				SmoothKernel.x = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.x = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			else
			{
				SmoothKernel.y = m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].nGaousKenel;
				SmoothKernelSigma.y = (float)m_ThreadPara.InspPara[nCamIdx].FilterPara[nInspIdx].dGaousSigma;
			}
			SmoothImage(TempImage1, TempImage2, CV_GAUSSIAN, SmoothKernel, SmoothKernelSigma);
			strSavePath.Format(_T("%sSmoothImage_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage2, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);
		
			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6) &&
				nInspIdx == 0)
			{
				cvNot(TempImage2, TempImage3);
				strSavePath.Format(_T("%scvNot_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage3, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage3, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				CvThreshHoldRange(TempImage3, TempImage2, (255 - nThreshHole), 30);
				//cvThreshold(TempImage3, TempImage2, nThreshHole, 255, CV_THRESH_BINARY);

				strSavePath.Format(_T("%sCvThreshHoldRange_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);

				TraceInsp(m_Display[nCamIdx], TempImage2, &rcRoi, bTrace);

				if(bTrace)
				{
					strText.Format(_T("Step #%d OK?"), iTraceStep++);
					if(AfxMessageBox(strText, MB_YESNO) == IDNO)
						break;
				}

				cvNot(TempImage2, TempImage4);
				strSavePath.Format(_T("%scvAnd_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}
			else
			{
				cvThreshold(TempImage2, TempImage4, nThreshHole, 255, CV_THRESH_BINARY);
				strSavePath.Format(_T("%scvThreshold_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
				if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
					SaveIplImage(TempImage4, strSavePath);
			}

			TraceInsp(m_Display[nCamIdx], TempImage4, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
			{
				//Morphology(TempImage4, TempImage4, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				cvNot(TempImage4, TempImage5);
				if(nInspIdx == 1)
				{
					IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
					cvErode(TempImage4, TempImage5, element);
					cvDilate(TempImage5, TempImage4, element);
					if(element != NULL)	cvReleaseStructuringElement(&element);
			
					cvNot(TempImage4, TempImage5);
				}
			}

			if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
				m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
			{
				//if(nInspIdx == 1)
				//{
				//	Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				//}

				//if(nInspIdx == 0)
				//{
				//	//cvNot(TempImage4, TempImage4);
				//	Morphology(TempImage4, TempImage5, cMorpSize, CV_SHAPE_RECT, CV_MOP_CLOSE);
				//}
				cvCopy(TempImage4, TempImage5);
				IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
				cvErode(TempImage5, TempImage4, element);
				cvDilate(TempImage4, TempImage5, element);
				if(element != NULL)	cvReleaseStructuringElement(&element);
			}
			else if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
			{
				//cvCopy(TempImage4, TempImage5);
				IplConvKernel *element = cvCreateStructuringElementEx(cMorpSize.x, cMorpSize.y, (int)((double)cMorpSize.x * 0.5), (int)((double)cMorpSize.y * 0.5), CV_SHAPE_RECT, NULL);
				cvErode(TempImage5, TempImage4, element);
				cvDilate(TempImage4, TempImage5, element);
				if(element != NULL)	cvReleaseStructuringElement(&element);
			}

			strSavePath.Format(_T("%sMorphology_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage5, strSavePath);

			TraceInsp(m_Display[nCamIdx], TempImage5, &rcRoi, bTrace);

			if(bTrace)
			{
				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			if (TempImage4)		cvReleaseImage(&TempImage4);
			TempImage4 = cvCreateImage(cvSize(TempImage5->roi->width, TempImage5->roi->height), IPL_DEPTH_8U, 1);

			for(int h = 0; h < TempImage5->roi->height; h++)
			{
				for(int w = 0; w < TempImage5->roi->width; w++)
				{
					if(TempImage5->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage5->widthStep)] == 0)
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 255;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
					}
					else
					{
						if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4 ||
							m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ) && nInspIdx == 0)
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 0;
						}
						else
						{
							TempImage4->imageData[w + (h * TempImage4->widthStep)] = 255;
						}
					}
				}
			}

			strSavePath.Format(_T("%sCrop_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(TempImage4, strSavePath);

			CvBlobSearchPara BlobSearchPara;
			memset(&BlobSearchPara, NULL, sizeof(CvBlobSearchPara));
			if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
			{
				BlobSearchPara.dMinWidth = 10.;
				BlobSearchPara.dMinHeight = 2.;
				BlobSearchPara.dMinArea = 10.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2;
			}
			else
			{
				BlobSearchPara.dMinWidth = (double)cMorpSize.x;
				BlobSearchPara.dMinHeight = (double)cMorpSize.y;
				BlobSearchPara.dMinArea = (double)(cMorpSize.x * cMorpSize.y) / 2.;
				BlobSearchPara.dMaxWidth = TempImage4->width * 2.;
				BlobSearchPara.dMaxHeight = TempImage4->height * 2.;
			}
		
			BlobSearchPara.dMaxArea = BlobSearchPara.dMaxWidth * BlobSearchPara.dMaxHeight;
			BlobSearchPara.nMode = 1;
			BlobSearchPara.ucThreshold = 20;
			BlobSearchPara.bUseMask = TRUE;
			CvBlobs[nInspIdx].FindBlobs(TempImage4, BlobSearchPara);

			strSavePath.Format(_T("%sMask_Cam_%d_%d_%d.jpg"), m_strSavePath, nCamIdx, m_ThreadPara.nInspPoint[nCamIdx], nInspIdx);
 			if(bSaveImage && DriveUseSearch(m_ThreadPara.strDrive))
				SaveIplImage(CvBlobs[nInspIdx].GetMask(), strSavePath);

			if(bTrace)
			{
				for(int h = 0; h < TempImage1->roi->height; h++)
				{
					for(int w = 0; w < TempImage1->roi->width; w++)
					{
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = 0;
						TempImage1->imageData[(rcRoi.x + w) + ((rcRoi.y + h) * TempImage1->widthStep)] = CvBlobs[nInspIdx].GetMask()->imageData[w + (h * CvBlobs[nInspIdx].GetMask()->widthStep)];
					}
				}
				TraceInsp(m_Display[nCamIdx], TempImage1, &rcRoi, bTrace);

				strText.Format(_T("Step #%d OK?"), iTraceStep++);
				if(AfxMessageBox(strText, MB_YESNO) == IDNO)
					break;
			}

			int nBlobCnt = CvBlobs[nInspIdx].GetBlobCount();
			InspPoint[nInspIdx].x = 0.;
			InspPoint[nInspIdx].y = 0.;
		
			RsltLine[2][nInspIdx].x = 0;
			RsltLine[2][nInspIdx].y = 0;
 			if(nBlobCnt > 0)
			{
				CvBlobBox BlobBox;
				CvBlob1 Blob;
				int nStartX = 0, nStartY = 0, nEndX = 0, nEndY = 0;
				int nBlobIdx = 0;
				int nTemp1, nTemp2, nTemp3;

 				if(nBlobCnt > 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1 ||
									m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y;
						}
						else
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 0)
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
						else
						{
							if(Blob.y == 0)
								continue;

							nTemp1 = Blob.y + Blob.height;
						}
					}
					
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
						{
							if(nInspIdx == 0)
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y;
								nTemp3 = min(nTemp1, nTemp2);
							}
							else
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else
						{
							if(nInspIdx == 0)
							{
								if(Blob.y == 0)
									continue;

								nTemp2 = Blob.y + Blob.height;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.y + Blob.height;
								nTemp3 = min(nTemp1, nTemp2);
							}
						}
						
						if(nTemp3 == nTemp2)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}
				else if(nBlobCnt > 1 && ((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
										(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)))
				{
					Blob = CvBlobs[nInspIdx].GetEachBlob(0);

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.x;
							nTemp1 = Blob.area;
						}
						else
						{
							nTemp1 = Blob.x + Blob.width;
						}
					}
					else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						if(nInspIdx == 1)
						{
							nTemp1 = Blob.height;
							nTemp1 = Blob.area;
						}
						else
						{
							nTemp1 = Blob.x;
						}
					}
					
					nTemp3 = 0;
					nTemp2 = 0;
					for(int i = 1; i < CvBlobs[nInspIdx].GetBlobCount(); i++)
					{
						Blob = CvBlobs[nInspIdx].GetEachBlob(i);
						
						if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.x;
								nTemp2 = Blob.area;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								nTemp2 = Blob.x + Blob.width;
								nTemp3 = max(nTemp1, nTemp2);
							}
						}
						else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
						{
							if(nInspIdx == 1)
							{
								nTemp2 = Blob.height;
								nTemp2 = Blob.area;
								nTemp3 = max(nTemp1, nTemp2);
							}
							else
							{
								if(Blob.x >= 5)
								{
									nTemp2 = Blob.x;
									nTemp3 = min(nTemp1, nTemp2);
								}
							}
						}
						
						if(nTemp3 == nTemp2 && nTemp2 != 0 && nTemp3 != 0)
						{
							nBlobIdx = i;
							nTemp1 = nTemp3;
						}
					}
				}

				BlobBox = CvBlobs[nInspIdx].GetEachBlobBox(nBlobIdx);
				Blob = CvBlobs[nInspIdx].GetEachBlob(nBlobIdx);



				CvPoint *arPt1;
				int nCnt = 0, nCnt1 = 0;

				if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 ||
					m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
				{
					arPt1 = new CvPoint[Blob.height - 2];
					for(int h = Blob.y + 1; h < Blob.y + Blob.height - 1; h++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].y == Blob.y || BlobBox.arPt[nEdgeCnt].y == Blob.height)
								continue;

							if(h == BlobBox.arPt[nEdgeCnt].y)
							{
								nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].x;
								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}

						if(nSearchCnt == 2)
						{
							if((nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = max(nTemp[0], nTemp[1]);
							}
							else if((nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3) || (nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5) ||
								(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2) || (nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
							{
								arPt1[nCnt].x = min(nTemp[0], nTemp[1]);
							}
							arPt1[nCnt].y = h;
							nCnt++;
						}
					}
				}
				else
				{
					arPt1 = new CvPoint[Blob.width - 2];
					for(int w = Blob.x + 1; w < Blob.x + Blob.width - 1; w++)
					{
						int nSearchCnt = 0;
						int nTemp[2] = {0,};
						for(int nEdgeCnt = 0; nEdgeCnt < BlobBox.nEdgeCnt; nEdgeCnt++)
						{
							if(BlobBox.arPt[nEdgeCnt].x == Blob.x || BlobBox.arPt[nEdgeCnt].x == Blob.width)
								continue;

							if(w == BlobBox.arPt[nEdgeCnt].x)
							{
								if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
									nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].y;
								else
									nTemp[nSearchCnt] = BlobBox.arPt[nEdgeCnt].y + Blob.height;

								nSearchCnt++;
							}
							if(nSearchCnt == 2)
							{
								break;
							}
						}
						if(nSearchCnt == 2)
						{
							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
							{
								arPt1[nCnt].y = max(nTemp[0], nTemp[1]);
							}
							else
							{
								arPt1[nCnt].y = min(nTemp[0], nTemp[1]);
							}

							arPt1[nCnt].x = w;
							nCnt++;
						}
					}
				}

				CvPoint pt1, pt2;
				if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					pt1.x = Blob.x + Blob.width;
					pt1.y = 2;
					pt2.x = Blob.x + Blob.width;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 0 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					pt1.x = Blob.x;
					pt2.x = Blob.x;
					pt1.y = 2;
					pt2.y = Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6))
				{
					int nTempX0, nTempX1, nTempCnt = 0, nSerVal = 0, nSerchOffset = INSP_SEARCH_OFFSET;
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						nSerVal = Blob.y + Blob.height - INSP_SEARCH_VAL;
					}
					else
					{
						nSerVal = Blob.y - INSP_SEARCH_VAL;
					}

					int nMaxSerCnt = Blob.height / nSerchOffset;
				
					for(int nSerCnt = 0; nSerCnt < nMaxSerCnt; nSerCnt++)
					{
						BOOL isSerch = TRUE;
						for(int nPtCnt = 0; nPtCnt < BlobBox.nEdgeCnt; nPtCnt++)
						{
							if(nSerVal == BlobBox.arPt[nPtCnt].y)
							{
								if((Blob.x + (Blob.width / 2)) < BlobBox.arPt[nPtCnt].x)
								{
									if(nTempCnt == 0)
										nTempX0 = BlobBox.arPt[nPtCnt].x;
									else
										nTempX1 = BlobBox.arPt[nPtCnt].x;

									nTempCnt++;
								}
							}
							if(nTempCnt == 2)
								break;
						}

						if(isSerch && nTempCnt == 1 && (nTempX0 >= ((rcRoi.width / 2) - 20) && (rcRoi.width - 20) >= nTempX0))
						{
							nTempX1 = nTempX0;
							break;
						}

						if((nTempX0 <= 20 || ((rcRoi.width / 2) - 20) <= nTempX0) ||
							(nTempX1 <= 20 || ((rcRoi.width / 2) - 20) <= nTempX1))
						{
							nTempCnt = 0;
							nTempX0 = 0;
							nTempX1 = 0;
							isSerch = FALSE;
							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
							{
								nSerVal += nSerchOffset;
							}
							else
							{
								nSerVal -= nSerchOffset;
							}
						}
					
						if(isSerch)
							break;
					}

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						pt1.x = max(nTempX0, nTempX1);
						pt2.x = max(nTempX0, nTempX1);
					}
					else
					{
						pt1.x = max(nTempX0, nTempX1);
						pt2.x = max(nTempX0, nTempX1);
					}
				
					pt1.y = 2;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else if(nInspIdx == 1 && (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2 || m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5))
				{
					int nTempX0, nTempX1, nTempCnt = 0, nSerVal = 0, nSerchOffset = INSP_SEARCH_OFFSET;
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						nSerVal = Blob.y + INSP_SEARCH_VAL;
					}
					else
					{
						nSerVal = Blob.y + Blob.height - INSP_SEARCH_VAL;
					}

					int nMaxSerCnt = Blob.height / nSerchOffset;

					for(int nSerCnt = 0; nSerCnt < nMaxSerCnt; nSerCnt++)
					{
						BOOL isSerch = TRUE;
						for(int nPtCnt = 0; nPtCnt < BlobBox.nEdgeCnt; nPtCnt++)
						{
							if(nSerVal == BlobBox.arPt[nPtCnt].y)
							{
								if((Blob.x + (Blob.width / 2)) > BlobBox.arPt[nPtCnt].x)
								{
									if(nTempCnt == 0)
										nTempX0 = BlobBox.arPt[nPtCnt].x;
									else
										nTempX1 = BlobBox.arPt[nPtCnt].x;

									nTempCnt++;
								}
							}
							if(nTempCnt == 2)
								break;
						}

						if(isSerch && nTempCnt == 1 && (nTempX0 >= 20 && ((rcRoi.width / 2) - 20) >= nTempX0))
						{
							nTempX1 = nTempX0;
							break;
						}

						if((nTempX0 <= 20 || ((rcRoi.width / 2) - 20) <= nTempX0) ||
							(nTempX1 <= 20 || ((rcRoi.width / 2) - 20) <= nTempX1))
						{
							nTempCnt = 0;
							nTempX0 = 0;
							nTempX1 = 0;
							isSerch = FALSE;
							if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
							{
								nSerVal += nSerchOffset;
							}
							else
							{
								nSerVal -= nSerchOffset;
							}
						}
					
						if(isSerch)
							break;
					}

					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						pt1.x = min(nTempX0, nTempX1);
						pt2.x = min(nTempX0, nTempX1);
					}
					else
					{
						pt1.x = min(nTempX0, nTempX1);
						pt2.x = min(nTempX0, nTempX1);
					}
				
					pt1.y = 2;
					pt2.y = Blob.y + Blob.height - 2;
				}
				else
				{
					if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
					{
						if(nInspIdx == 0)
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y + Blob.height;
						}
					}
					else
					{
						if(nInspIdx == 1)
						{
							pt1.x = 2;
							pt1.y = Blob.y;// + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;// + Blob.height;
						}
						else
						{
							pt1.x = 2;
							pt1.y = Blob.y;// + Blob.height;
							pt2.x = Blob.width - 2;
							pt2.y = Blob.y;// + Blob.height;
						}
					}
				}
			
				InspPoint[nInspIdx].x = 0.;
				InspPoint[nInspIdx].y = 0.;
				RsltLine[nInspIdx][0].x = 0;
				RsltLine[nInspIdx][0].y = 0;
				RsltLine[nInspIdx][1].x = 0;
				RsltLine[nInspIdx][1].y = 0;

				if(pt1.x > 0 && pt1.y > 0 && pt2.x > 0 && pt2.y > 0)
				{
					if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT6)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT3)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT2)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].bottom;//pt2.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].top;
					}
					else if(nInspIdx == 0 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = RsltLine[1][0].y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = RsltLine[1][1].y;
					}
					else if(nInspIdx == 1 && m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT5)
					{
						RsltLine[nInspIdx][0].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][0].y = m_ThreadPara.InspPara[nCamIdx].InspRect[1].top;//pt1.y + rcRoi.y;
						RsltLine[nInspIdx][1].x = (pt1.x + pt2.x) / 2 + rcRoi.x;
						RsltLine[nInspIdx][1].y = m_ThreadPara.InspPara[nCamIdx].InspRect[0].bottom;
					}
					else if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						RsltLine[nInspIdx][0].x = pt1.x + rcRoi.x;
						RsltLine[nInspIdx][0].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
						RsltLine[nInspIdx][1].x = pt2.x + rcRoi.x;
						RsltLine[nInspIdx][1].y = (pt1.y + pt2.y) / 2 + rcRoi.y;
					}

					if((m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1) || (m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4))
					{
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						if(nInspIdx == 0)
						{
							RsltLine[0][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[0][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							RsltLine[1][0].x = min(m_ThreadPara.InspPara[nCamIdx].InspRect[0].left, m_ThreadPara.InspPara[nCamIdx].InspRect[1].left);
							RsltLine[1][1].x = max(m_ThreadPara.InspPara[nCamIdx].InspRect[0].right, m_ThreadPara.InspPara[nCamIdx].InspRect[1].right);
							cvLine(DispImage, RsltLine[0][0], RsltLine[0][1], CV_RGB(0, 0, 255), 11, CV_AA);
							cvLine(DispImage, RsltLine[1][0], RsltLine[1][1], CV_RGB(0, 0, 255), 11, CV_AA);
							InspPoint[0].x = ((double)RsltLine[0][0].x + (double)RsltLine[0][1].x) / 2.;
							InspPoint[0].y = ((double)RsltLine[0][1].y - (double)RsltLine[0][0].y) / 2. + (double)RsltLine[0][0].y;
							InspPoint[1].x = ((double)RsltLine[1][0].x + (double)RsltLine[1][1].x) / 2.;
							InspPoint[1].y = ((double)RsltLine[1][1].y - (double)RsltLine[1][0].y) / 2. + (double)RsltLine[1][0].y;

							RsltLine[2][0].x = InspPoint[0].x;
							RsltLine[2][0].y = InspPoint[0].y;
							RsltLine[2][1].x = InspPoint[1].x;
							RsltLine[2][1].y = InspPoint[1].y;
						}
					}
					else
					{
						cvLine(DispImage, RsltLine[nInspIdx][0], RsltLine[nInspIdx][1], CV_RGB(0, 0, 255), 11, CV_AA);
						InspPoint[nInspIdx].x = ((double)RsltLine[nInspIdx][0].x + (double)RsltLine[nInspIdx][1].x) / 2.;
						InspPoint[nInspIdx].y = ((double)RsltLine[nInspIdx][1].y - (double)RsltLine[nInspIdx][0].y) / 2. + (double)RsltLine[nInspIdx][0].y;
						RsltLine[2][nInspIdx].x = InspPoint[nInspIdx].x;
						RsltLine[2][nInspIdx].y = InspPoint[nInspIdx].y;
					}
				}
				
				delete arPt1;
				isSearch[nInspIdx] = TRUE;
			}

			cvResetImageROI(TempImage1);

			if (TempImage1)		cvReleaseImage(&TempImage1);
			if (TempImage2)		cvReleaseImage(&TempImage2);
			if (TempImage3)		cvReleaseImage(&TempImage3);
			if (TempImage4)		cvReleaseImage(&TempImage4);
			if (TempImage5)		cvReleaseImage(&TempImage5);
			if (cvStorage)		cvReleaseMemStorage(&cvStorage);
		}
	}


	if(isSearch[0] && isSearch[1])
		m_Rslt[nCamIdx].bGapRslt = TRUE;

	if(InspPoint[0].x > 0. && InspPoint[0].y > 0. && InspPoint[1].x > 0. && InspPoint[1].y > 0. &&
		RsltLine[2][0].x > 0 && RsltLine[2][0].y > 0 && RsltLine[2][1].x > 0 && RsltLine[2][1].y > 0)
	{
		m_Rslt[nCamIdx].nInspPoint = m_ThreadPara.nInspPoint[nCamIdx];
		m_Rslt[nCamIdx].dGapRslt = (GetDistance(InspPoint[0], InspPoint[1]) * m_ThreadPara.dPixelSizeWidth) / 1000.;
		cvLine(DispImage, RsltLine[2][0], RsltLine[2][1], CV_RGB(0, 255, 0), 11, CV_AA);

		if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT1)
		{
			m_Rslt[nCamIdx].dGapRslt = 2.4;
		}
		else if(m_ThreadPara.nInspPoint[nCamIdx] == INSP_POINT4)
		{
			m_Rslt[nCamIdx].dGapRslt = 2.2;
		}

	}

	m_Rslt[nCamIdx].bPointRslt = FALSE;
	if((m_ThreadPara.SpecPoint[nCamIdx].dMinSpec / 2.) < (m_Rslt[nCamIdx].dGapRslt / 2.) && (m_Rslt[nCamIdx].dGapRslt / 2.) < (m_ThreadPara.SpecPoint[nCamIdx].dMaxSpec / 2.))
	{
		m_Rslt[nCamIdx].bPointRslt = TRUE;
	}

	cvCopy(DispImage, m_Rslt[nCamIdx].RsltImage);

	/*if (TempImage1)
	{
		cvResetImageROI(TempImage1);
		cvReleaseImage(&TempImage1);
	}

	if (TempImage2)		cvReleaseImage(&TempImage2);
	if (TempImage3)		cvReleaseImage(&TempImage3);
	if (TempImage4)		cvReleaseImage(&TempImage4);
	if (TempImage5)		cvReleaseImage(&TempImage5);

	if (cvStorage)		cvReleaseMemStorage(&cvStorage);*/
	if (SrcImage)		cvReleaseImage(&SrcImage);
	if (DispImage)		cvReleaseImage(&DispImage);

	strText.Format(_T("StepIdx %d, CamIdx %d, InspPoint %s Inspection End -> Rslt : %0.4f"), m_ThreadPara.nSeqIdx, nCamIdx, InspPointToString(m_ThreadPara.nInspPoint[nCamIdx]), m_Rslt[nCamIdx].dGapRslt);
	m_cLogFile->WriteText(strText);

	return dRslt;
}

#endif
void CVisionInspection::SetInspectionThread(int nCamIdx)
{
	m_bInspecting[nCamIdx] = TRUE;
	m_bInspectionEnd[nCamIdx] = FALSE;

	if(!m_bInspecting[nCamIdx] && !m_bInspectionEnd[nCamIdx])
	{ 
		m_bInspecting[nCamIdx] = FALSE;
		m_bInspectionEnd[nCamIdx] = TRUE;
		return;
	}

	GapInspection(nCamIdx);

	m_bInspecting[nCamIdx] = FALSE;
	m_bInspectionEnd[nCamIdx] = TRUE;
}

UINT CVisionInspection::InspThread0(LPVOID pParam)
{
	CVisionInspection *pdlg = (CVisionInspection *)pParam;

	pdlg->SetInspectionThread(0);

	return 0;
}

UINT CVisionInspection::InspThread1(LPVOID pParam)
{
	CVisionInspection *pdlg = (CVisionInspection *)pParam;

	pdlg->SetInspectionThread(1);

	return 0;
}


#ifdef TEST_MANUAL_INSP
BOOL CVision::TestImageLoad(int nCamIdx, int nInspPoint)
{
	CString strPath;
	if(nInspPoint == INSP_POINT1)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_c_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_c_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else if(nInspPoint == INSP_POINT2)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_r_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_r_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else if(nInspPoint == INSP_POINT3)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_l_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\cam_l_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else if(nInspPoint == INSP_POINT4)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_c_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_c_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else if(nInspPoint == INSP_POINT5)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_r_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_r_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else if(nInspPoint == INSP_POINT6)
	{
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_l_1.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][0], strPath);
		strPath.Format(_T("D:\\Project\\Samsung\\FOLDABLE\\20190117\\usb_l_2.jpg"));
		LoadIplImage(m_GrabImage[nCamIdx][1], strPath);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
#endif