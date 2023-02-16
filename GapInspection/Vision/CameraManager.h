#pragma once

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/gige/BaslerGigECamera.h>
#include <pylon/gige/GigETransportLayer.h>
#include <pylon/gige/BaslerGigEDeviceInfo.h>
//#include <pylon/gige/PylonGigEDevice.h>

#include "Define.h"
#include <afxmt.h> 

using namespace Pylon;
using namespace GenApi;

//class CCameraEndofExposureEventHandler : public CCameraEventHandler;

class CCameraManager : public CImageEventHandler ,              // For receiving grabbed images.
                       public CConfigurationEventHandler     // For getting notified about device removal.
{	
public:
	CCameraManager(void);
	~CCameraManager(void);

	CCriticalSection	m_csGrab;

	// Camera 관련 멤버 변수 선언
    CTlFactory *m_tlFactory;                       // Get the transport layer factory.
	CInstantCameraArray m_pCamera;                 // Multi Camera
	CInstantCameraArray m_pCopyCamera;                 // Multi Camera
	CPylonImage Image[MAX_CAM_NUM];
    CImageFormatConverter converter[MAX_CAM_NUM];
    CIntegerPtr m_pHeartbeatTimeout[MAX_CAM_NUM];
    IGigETransportLayer * m_pIP;

	INodeMap *m_pCameraNodeMap;                           
	DeviceInfoList_t devices;
	
   // CBaslerGigEDeviceInfo di;
    //   IPylonGigEDevice *mm;

	CEnumerationPtr			ptrEnumeration[MAX_CAM_NUM];
	CIntegerPtr				ptrInteger[MAX_CAM_NUM];
	CBooleanPtr             ptrBoolean[MAX_CAM_NUM];
	CFloatPtr				ptrFloat[MAX_CAM_NUM];
	CCommandPtr             ptrCommand[MAX_CAM_NUM];
/*************************************************************************************************/
/*							log 변수															 */
/*************************************************************************************************/
	 bool  bLogUse;
	 bool  bMessageBoxUse;
	 bool  bTraceUse;
     bool  bStopLiveThread;
	 bool  bStopFlag;
	 FILE *log;
	 char filename[256];
	 time_t t;
	 int Hour,Min,Sec;
/*************************************************************************************************/
/*							Image buffer														 */
/*************************************************************************************************/
	 unsigned char*   pImage8Buffer[MAX_CAM_NUM];              // 8bit image receive buffer
	 unsigned short*  pImage12Buffer[MAX_CAM_NUM];      	   // 8bit 이상 image receive buffer
	 unsigned char*   pImage24Buffer[MAX_CAM_NUM];             // 24bit image receive buffer
     unsigned char *pImageBigBuffer[MAX_CAM_NUM];      // Big image
	 
     CString   m_strCM_ImageForamt[MAX_CAM_NUM];               // Image format	

     int       m_iCM_OffsetX[MAX_CAM_NUM];
	 int       m_iCM_OffsetY[MAX_CAM_NUM];
     int       m_iCM_Width[MAX_CAM_NUM];
	 int       m_iCM_Height[MAX_CAM_NUM];
	 int       m_iCM_reSizeWidth[MAX_CAM_NUM];

	 bool      m_bCaptureEnd[MAX_CAM_NUM];                        // capture finish flag
	 bool      m_bRemoveCamera[MAX_CAM_NUM];                      // 카메라 연결 상태 정보	 

     bool      m_bCamOpenFlag[MAX_CAM_NUM];                       // 카메라 open flag
	 bool      m_bCamConnectFlag[MAX_CAM_NUM];                    // 카메라 Connect flag
 
	 long      m_iSkippiedFrame[MAX_CAM_NUM];
	 long      m_iGrabbedFrame[MAX_CAM_NUM];
    
	 int        m_imgNjm;
	 
	// 멤버 함수 선언
    
	int FindCamera(char szCamName[MAX_CAM_NUM][100],char szCamSerialNumber[MAX_CAM_NUM][100],char szInterfacName[MAX_CAM_NUM][100],int *nCamNumber); // 완료(finish)
	int Open_Camera(int nCamIndex, int nPosition);
	int Close_Camera(int nCamIndex);
	int Connect_Camera(int nCamIndex,int nOffsetX,int nOffsetY,int nWidth,int nHeight,CString strImgFormat);
    int SingleGrab(int nCamIndex);
	int GrabLive(int nCamIndex,int nMode);

	int LiveStop(int nCamIndex,int nMode);
	
	int GetEnumeration(int nCamIndex, char *szValue, char *szNodeName);
	int SetEnumeration(int nCamIndex, char *szValue, char *szNodeName);
	int GetInteger(int nCamIndex, int *nValue, char *szNodeName);
	int SetInteger(int nCamIndex, int nValue, char *szNodeName);
    int GetIntegerMax(int nCamIndex, int *nValue, char *szNodeName);
	int GetIntegerMin(int nCamIndex, int *nValue, char *szNodeName);
	int GetBoolean(int nCamIndex, bool *bValue, char *szNodeName);
	int SetBoolean(int nCamIndex, bool bValue, char *szNodeName);
	int GetFloat(int nCamIndex, float *fValue, char *szNodeName);
	int SetFloat(int nCamIndex, float fValue, char *szNodeName);
	int SetCommand(int nCamIndex, char *szNodeName);	
	
	void ReadEnd(int nCamIndex);
	void CamWriteLog(int nCamIndex, CString strTemp1, CString strTemp2);

	bool CheckCaptureEnd(int nCamIndex);
	int SaveImage(int nFileFormat, unsigned char* pImage, char *filename,int nPixelType,int width, int height,int nColorband);

	static UINT LiveThread(void *lParam);

    virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
	virtual void OnImagesSkipped( CInstantCamera& camera, size_t countOfSkippedImages);
    virtual void OnCameraDeviceRemoved( CInstantCamera& camera);

};
