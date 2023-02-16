#include "StdAfx.h"
#include "BaslerManage.h"



CBaslerManage::CBaslerManage()
{
}
CBaslerManage::CBaslerManage(int _w, int _h)
{
	m_hDevice = NULL;
	m_nPayloadSize = 0;
	m_pImageBuf = NULL;

	if(_w != 0 && _h != 0)
	{
		m_pImageBuf = new BYTE[_w*_h];
		memset(m_pImageBuf, NULL, sizeof(BYTE)*(_w*_h));
	}
}


CBaslerManage::~CBaslerManage()
{
	if(NULL != m_pImageBuf)
	{
		delete[] m_pImageBuf;
		m_pImageBuf = NULL;
	}
}




int CBaslerManage::InitCamera(int heartbeat_ms)
{
	GENAPIC_RESULT res;
	size_t numDevices;

	BOOL isAvail;

	//PylonInitialize();
	res = PylonEnumerateDevices( &numDevices );

	//20161019 ht 카메라 상태에 따라 return값을 설정해줘서 체크
	if ( 0 == numDevices )
	{
		return ERROR_CAMERA_NOT_OPEN;
		/*fprintf( stderr, "No devices found!\n" );
		PylonTerminate();
		exit(EXIT_FAILURE);*/
	}

	res = PylonCreateDeviceByIndex( 0, &m_hDevice );
	res = PylonDeviceOpen( m_hDevice, PYLONC_ACCESS_MODE_CONTROL | PYLONC_ACCESS_MODE_STREAM );

	isAvail = PylonDeviceFeatureIsAvailable(m_hDevice, "EnumEntry_PixelFormat_Mono8");
	//20161019 ht 카메라 상태에 따라 return값을 설정해줘서 체크
	if ( !isAvail )
	{
		return ERROR_CAMERA_AVAILABLE;
		//fprintf(stderr, "Device doesn't support the Mono8 pixel format.");
		//PylonTerminate();
		//exit (EXIT_FAILURE);
	}

	res = PylonDeviceFeatureFromString(m_hDevice, "PixelFormat", "Mono8" );

	isAvail = PylonDeviceFeatureIsAvailable( m_hDevice, "EnumEntry_TriggerSelector_AcquisitionStart");
	if (isAvail)
	{
		res = PylonDeviceFeatureFromString( m_hDevice, "TriggerSelector", "AcquisitionStart");
		res = PylonDeviceFeatureFromString( m_hDevice, "TriggerMode", "Off");
	}

	isAvail = PylonDeviceFeatureIsAvailable( m_hDevice, "EnumEntry_TriggerSelector_FrameBurstStart");
	if (isAvail)
	{
		res = PylonDeviceFeatureFromString( m_hDevice, "TriggerSelector", "FrameBurstStart");
		res = PylonDeviceFeatureFromString( m_hDevice, "TriggerMode", "Off");
	}

	isAvail = PylonDeviceFeatureIsAvailable( m_hDevice, "EnumEntry_TriggerSelector_FrameStart");
	if (isAvail)
	{
		PylonDeviceFeatureFromString( m_hDevice, "TriggerSelector", "FrameStart");
		res = PylonDeviceFeatureFromString( m_hDevice, "TriggerMode", "Off");
	}


	isAvail = PylonDeviceFeatureIsWritable(m_hDevice, "GevSCPSPacketSize");
	if ( isAvail )
	{
		res = PylonDeviceSetIntegerFeature( m_hDevice, "GevSCPSPacketSize", 1500 );
	}

	res = PylonDeviceGetIntegerFeatureInt32( m_hDevice, "PayloadSize", &m_nPayloadSize );

	res = setHeartbeatTimeout(heartbeat_ms);




	return SUCCESS_CAMERA;
}


BOOL CBaslerManage::Grab(BOOL disp)
{
	GENAPIC_RESULT          res;           /* Return value of pylon methods. */
	bool bufferReady;
	BOOL rslt = TRUE;

	res = PylonDeviceGrabSingleFrame( m_hDevice, 0, (BYTE*)m_pImageBuf, m_nPayloadSize, &m_struct_GrabResult, &bufferReady, 500 );

	if ( GENAPI_E_OK == res && !bufferReady )
	{
		rslt = FALSE;
	}

	if(disp == TRUE){
		//DISPALY TEST	
	}
	//CHECK(res);

	if(m_struct_GrabResult.Status == Grabbed)
	{
		rslt = TRUE;
	}
	else
	{
		rslt = FALSE;
	}

	return rslt;
}



void CBaslerManage::Release()
{
	GENAPIC_RESULT          res; 

	res = PylonDeviceClose( m_hDevice );
	res = PylonDestroyDevice ( m_hDevice );

	PylonTerminate();

}



GENAPIC_RESULT CBaslerManage::setHeartbeatTimeout(int timeout_ms )
{

	NODEMAP_HANDLE              hNodemap;   /* Handle to the node map */
	NODE_HANDLE                 hNode;      /* Handle to a node, i.e., a feature */
	GENAPIC_RESULT              res;        /* Return value */
	int64_t                     oldTimeout; /* The current timeout value */

	/* Get the node map for the transport layer parameters. */
	res = PylonDeviceGetTLNodeMap( m_hDevice, &hNodemap );

	if ( GENAPIC_INVALID_HANDLE == hNodemap )
	{
		/* The device doesn't provide a transport layer node map. Nothing to do. */
		return -1;
	}
	/* Get the node for the heartbeat timeout parameter. */
	res = GenApiNodeMapGetNode( hNodemap, "HeartbeatTimeout", &hNode );

	if ( GENAPIC_INVALID_HANDLE == hNode )
	{
		/* There is no heartbeat timeout parameter. Nothing to do. */
		return -1;
	}

	/* Get the current value. */
	res = GenApiIntegerGetValue( hNode, &oldTimeout );


	/* Set the new value. */
	res = GenApiIntegerSetValue( hNode, timeout_ms );


	/* Return the old value. */
	return res;
}

