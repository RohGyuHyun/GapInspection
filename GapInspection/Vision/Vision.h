#pragma once
#include "logFile.h"
#include <vector>
#include "define.h"
#include "CameraManager.h"
#include "StaticMatDisplay.h"

#ifndef CAMERA
#define TEST_MANUAL_INSP
#endif

#define INSP_SEARCH_OFFSET	50
#define INSP_SEARCH_VAL		40

#define ONE_CHANNEL_MAX_CAM_CNT		MAX_CAM_CNT / 2



typedef struct typeThread
{
	int nSeqIdx;
	BOOL bInspImageSave;
	BOOL bRsltImageSave;
	double dPixelSizeWidth;
	double dPixelSizeHeight;
	int nInspPoint[ONE_CHANNEL_MAX_CAM_CNT];
	IplImage *InspImage[ONE_CHANNEL_MAX_CAM_CNT];
	IplImage *DispImage[ONE_CHANNEL_MAX_CAM_CNT];
	BOOL bPartitionInsp[ONE_CHANNEL_MAX_CAM_CNT];
	TypeInspPara InspPara[ONE_CHANNEL_MAX_CAM_CNT];
	TypeSpecPara Spec[ONE_CHANNEL_MAX_CAM_CNT];
	TypeSpecPara SpecPoint[ONE_CHANNEL_MAX_CAM_CNT];
	BOOL bTrace;
	CWnd *hwDisplay[ONE_CHANNEL_MAX_CAM_CNT];
	int nDisplayId[ONE_CHANNEL_MAX_CAM_CNT];
	CString strDrive;
	int nSelectAlgorithmIdx;
	int nMetalAlgorithmIdx;
	int nChannel;
}TypeThreadPara;

class CVisionInspection
{
public:
	CVisionInspection(BOOL isManualInsp = FALSE);
	~CVisionInspection();

private:
	CWinThread* m_pThreadInsp[ONE_CHANNEL_MAX_CAM_CNT];
	BOOL m_bInspecting[ONE_CHANNEL_MAX_CAM_CNT];
	BOOL m_bInspectionEnd[ONE_CHANNEL_MAX_CAM_CNT];
	TypeThreadPara m_ThreadPara;
	TypeInspEachRslt m_Rslt[ONE_CHANNEL_MAX_CAM_CNT];
	CString m_strSavePath;
	CStaticMatDisplay *m_Display[ONE_CHANNEL_MAX_CAM_CNT];

	//20190218 ngh
	CString m_strBarcode;

	//20190219 ngh
	CLogFile *m_cLogFile;

	BOOL m_bManualInsp;
public:
	void SetInspectionThread(int nCamIdx);
	BOOL SetInspectionPara(TypeThreadPara ThreadPara, CTime cStart, CString strBarcode);
	BOOL WaitThreadEnd(int nCamIdx);
	void GetRslt(int nCamIdx, TypeInspEachRslt *Rslt);
	BOOL GetInspectionEnd(int nCamIdx){return m_bInspectionEnd[nCamIdx];};
	BOOL GetInspecting(int nCamIdx){return m_bInspecting[nCamIdx];};

	void SetDisplay(CStaticMatDisplay *Display, int nCamIdx);

protected:
	double GapInspection(int nCamIdx);
	double GapInspection(int nCamIdx, int nk);
	UINT static InspThread0(LPVOID pParam);
	UINT static InspThread1(LPVOID pParam);

	BOOL InitThread(int nCamIdx);
	BOOL ReleaseThread();

	BOOL InitImage();
	BOOL ReleaseImage();
};

class CVision
{
public:
	CVision(int nChannel);
	~CVision();

private:
	CLogFile *m_cLogFile;
	int m_nInspStepIdx;
	int m_nMaxThreadCnt;
	int m_nMaxStepCnt;
	TypeThreadPara m_ThreadPara[MAX_INSP_SEQ_CNT];;
	TypeSeqPara m_SeqPara;
	TypeSystemPara m_SystemPara;
	TypeInspEachRslt m_Rslt[MAX_INSP_POINT_CNT];

	CVisionInspection *m_CInspection[MAX_INSP_SEQ_CNT];
	CVisionInspection *m_CManualInspection;

	IplImage *m_DispImage[ONE_CHANNEL_MAX_CAM_CNT];
	IplImage *m_InspImage[ONE_CHANNEL_MAX_CAM_CNT];
	IplImage *m_GrabImage[ONE_CHANNEL_MAX_CAM_CNT][MAX_GRAB_CNT];
	
	//20190218 ngh
	CTime m_cStart;
	CString m_strBarcode;
	int m_nChannel;
public:
	void SetInspStepIdx(int nIdx){m_nInspStepIdx = nIdx;};
	int GetInspStepIdx(){return m_nInspStepIdx;};
	void SetInspection();
	void SetManualInspection(int nInspStepIdx, int nCamIdx, CWnd *hwDisplay, int nDisplayId);
	BOOL GetManualInspectionEnd(int nCamIdx);
	void ReleaseInspection(BOOL isDel = FALSE);
	BOOL InitInspection();


	IplImage *GetDispImage(int nCamIdx){return m_DispImage[nCamIdx];};

	void GetRsltGapVal(int nInspPoint, BOOL *bRslt, double *dRslt);
	void GetManualRsltGapVal(int nCamIdx, TypeInspEachRslt *InspRslt);
	BOOL GetInspRsltVal();
	BOOL GetPointRslt(int nInspPoint);

	IplImage *GetGrabImage(int nCamIdx, int nGrabIdx){return m_GrabImage[nCamIdx][nGrabIdx];};
	BYTE *GetGrabImageData(int nCamIdx, int nGrabIdx){return (BYTE*)m_GrabImage[nCamIdx][nGrabIdx]->imageData;};

	void SetSeqPara(TypeSeqPara SeqPara){m_SeqPara = SeqPara;};

	BOOL GetInspectionEnd(int nStepIdx);
	void SetSystemPara(TypeSystemPara SystemPara);

	IplImage *GetRsltImage(int nInspPoint){return m_Rslt[nInspPoint].RsltImage;};

	BOOL GetInspecting(int nStepIdx);
	
	void SetInitRslt(){/*InitRslt();*/InitThreadPara();};

	//20190218 ngh
	void SetStartDate(CTime cStartDate){m_cStart = cStartDate;};
	void SetBarcode(CString strBarcode){m_strBarcode.Format(_T("%s"), strBarcode);};
protected:
	BOOL InitVision(int nChannel);
	
	BOOL InitImage();
	BOOL ReleaseImage();

	BOOL InitThreadPara();
	BOOL ReleaseThreadPara();

	BOOL ThreadParaInitImage();
	BOOL ThreadParaReleaseImage();

	BOOL SaveInspPara(CString strPath);
	BOOL LoadInspPara(CString strPath);

	BOOL MergeImage(IplImage *src1, IplImage *src2, CRect MergeRect, IplImage *dst);

	BOOL RsltSuming();

	BOOL InitRslt();

#ifdef TEST_MANUAL_INSP
	BOOL TestImageLoad(int nCamIdx, int nInspPoint);
#endif
};