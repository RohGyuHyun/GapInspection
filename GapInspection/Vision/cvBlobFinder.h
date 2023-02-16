/******************************************************************************
Copyright (C) 2005 Matteo Lucarelli 
                   Angelo Moro

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

// WINDOWS NEED: -lcv -lhighgui -lcxcore
// LINUX NEED: -lcv -lhighgui -lcxcore

#if !defined(CVBLOBFINDER_H_INCLUDED)
#define CVBLOBFINDER_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define M_PI									3.14159265

#define SHAP_TRIANGLE							0
#define SHAP_RECTANGLE							1
#define SHAP_PENTAGON							2
#define SHAP_HEXAGON							3
#define SHAP_DECAGON							4
#define SHAP_CIRCLE								5

#define SHAP_STR_TRIANGLE						_T("TRIANGLE")
#define SHAP_STR_RECTANGLE						_T("RECTANGLE")
#define SHAP_STR_PENTAGON						_T("PENTAGON")
#define SHAP_STR_HEXAGON						_T("HEXAGON")
#define SHAP_STR_DECAGON						_T("DECAGON")
#define SHAP_STR_CIRCLE							_T("CIRCLE")

#define BLOB_BOX_POINT_IDX_LEFT_TOP				0
#define BLOB_BOX_POINT_IDX_LEFT_BOTTOM			1
#define BLOB_BOX_POINT_IDX_RIGHT_TOP			2
#define BLOB_BOX_POINT_IDX_RIGHT_BOTTOM			3

typedef struct CvBlobBox
{
	CvPoint2D32f LeftTop;
	CvPoint2D32f LeftBottom;
	CvPoint2D32f RightTop;
	CvPoint2D32f RightBottom;
	CvPoint2D32f center;
	float angle;
	double longLength;
	double shortLength;
	long long blobArea;
	CvPoint2D32f anchor;
	CvPoint2D32f vect1;
	CvPoint2D32f vect2;
	int nShapIdx;//0:triangle, 1:rectangle, 2:pentagon, 3:hexagon; 4: decagon, 5:circle
	char strShap[12];
	int nEdgeCnt;
	CvPoint* arPt;
}CvBlobBox;

typedef struct CvBlob1
{
	int x;
	int y;
	int width;
	int height;
	double area;
	
	CvBlobBox blobBox;
}CvBlob1;

typedef struct BlobSearch
{
	char ucThreshold;
	double dMinArea;
	double dMinWidth;
	double dMinHeight;
	double dMaxArea;
	double dMaxWidth;
	double dMaxHeight;
	int nMode;
	BOOL bUseMask;
	int nBlobMaxCnt;
}CvBlobSearchPara;

 #define CV_UNREFERENCED(arg)
CV_INLINE void cvMinAreaRect( CvPoint* points, int n,
                              int CV_UNREFERENCED(left), int CV_UNREFERENCED(bottom),
                              int CV_UNREFERENCED(right), int CV_UNREFERENCED(top),
                              CvPoint2D32f* anchor,
                              CvPoint2D32f* vect1,
                              CvPoint2D32f* vect2 )
{
    CvMat mat = cvMat( 1, n, CV_32SC2, points );
    CvBox2D box = cvMinAreaRect2( &mat, 0 );
    CvPoint2D32f pt[4];

    cvBoxPoints( box, pt );
    *anchor = pt[0];
    vect1->x = pt[1].x - pt[0].x;
    vect1->y = pt[1].y - pt[0].y;
    vect2->x = pt[3].x - pt[0].x;
    vect2->y = pt[3].y - pt[0].y;

    CV_UNREFERENCED( (left, bottom, right, top) );
}

class cvBlobFinder
{
private:
	
	IplImage* m_Mask;
	long m_BlobCount;
	CvBlob1* m_Blob;
	CvBlobSearchPara m_BlobSearchPara;
	void Reset();
public:

	cvBlobFinder();
	virtual ~cvBlobFinder();

	// img = 8 bit unsigned grayscale image
	// threshold = limit value
	// minarea = minimum blobs area
	// mode = 0:blob=threshold, 1:blob>=threshold, -1:blob<=threshold
	// UseMask = don't modify image (blobs are draws on Mask)
	long FindBlobs(IplImage* img, CvBlobSearchPara BlobSearchPara, bool SaveMask = false, CString strPath = _T(""));
	
	CvBlob1* GetBlobs();	// NULL-teminated array of found blobs
	CvBlob1 GetEachBlob(int nIdx){return m_Blob[nIdx];};
	CvPoint GetEachBlobEdgePoint(int nIdx, int nEdgeIdx){return m_Blob[nIdx].blobBox.arPt[nEdgeIdx];};
	int GetEachBlobEdgeCnt(int nIdx){return m_Blob[nIdx].blobBox.nEdgeCnt;};
	long GetBlobCount();
	IplImage* GetMask();
	BOOL SetBlobs(CvBlob1 *Blobs, int nBlobsCnt, IplImage* MaskImg = NULL, bool UseMask = false);
	void SetBlobsSearchPara(CvBlobSearchPara BlobSearchPara);

	void GetEachBlobPoint(int nBlobIdx, CvPoint2D32f *pt);//pt idx = 0:lefttop, 1:leftbottom, 2:righttop, 3:rightbottom
	CvPoint2D32f GetEachBlobCenter(int nBlobIdx){return m_Blob[nBlobIdx].blobBox.center;};
	double GetEachBlobAngle(int nBlobIdx){return (double)m_Blob[nBlobIdx].blobBox.angle;};
	double GetEachBlobLongLength(int nBlobIdx){return m_Blob[nBlobIdx].blobBox.longLength;};
	double GetEachBlobShortLength(int nBlobIdx){return m_Blob[nBlobIdx].blobBox.shortLength;};
	CvBlobBox GetEachBlobBox(int nBlobIdx){return m_Blob[nBlobIdx].blobBox;};

	void BlobErosion(int nEroSize, bool SaveImage = false, CString strPath = _T(""));
	void BlobDilation(int nDilSize, bool SaveImage = false, CString strPath = _T(""));
	void BlobOpening(int nOpenSize, bool SaveImage = false, CString strPath = _T(""));
	void BlobClosing(int nCloseSize, bool SaveImage = false, CString strPath = _T(""));

	uchar GetSrcEachBlobAvgVal(IplImage *src, int nBlobIdx);

protected:

	double DegreeToRation(double deg);
	double RatianToDegree(double rad);
	double GetDistance(CvPoint2D32f p1, CvPoint2D32f p2);
	double GetAngle(CvPoint2D32f p1, CvPoint2D32f p2);

	int ContourToShap(CvSeq *contour, char *strShap);//strShap[10]
};

#endif // !defined(CVBLOBFINDER_H_INCLUDED)



