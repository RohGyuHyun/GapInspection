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
#include "stdafx.h"
#include "cvBlobFinder.h"

cvBlobFinder::cvBlobFinder()
{
	m_Blob = NULL;
	m_BlobCount = 0;
	m_Mask = NULL;
}

cvBlobFinder::~cvBlobFinder()
{
	Reset();
}

double cvBlobFinder::DegreeToRation(double deg)
{
	return deg * (M_PI / 180);
}
double cvBlobFinder::RatianToDegree(double rad)
{
	return rad * (180 / M_PI);
}
double cvBlobFinder::GetDistance(CvPoint2D32f p1, CvPoint2D32f p2)
{
	double xdf = p2.x - p1.x;
	double ydf = p2.y - p1.y;
	return sqrt(pow(xdf, 2) + pow(ydf, 2));
}
double cvBlobFinder::GetAngle(CvPoint2D32f p1, CvPoint2D32f p2)
{
	double xdf = p2.x - p1.x;
	double ydf = p2.y - p1.y;

	double ang = RatianToDegree(atan2(ydf, xdf));
	return ang + 90;
}

BOOL cvBlobFinder::SetBlobs(CvBlob1 *Blobs, int nBlobsCnt, IplImage* MaskImg, bool UseMask)
{
	BOOL rslt = TRUE;

	Reset();
	
	if(UseMask)
	{
		m_Mask = cvCreateImage(cvSize(MaskImg->width, MaskImg->height), IPL_DEPTH_8U, 1);
		cvZero(m_Mask);

		cvCopy(MaskImg, m_Mask);
	}

	m_BlobCount = nBlobsCnt;
	for(int i = 0; i < nBlobsCnt;i++)
	{
		m_Blob = (CvBlob1*)realloc((void*)m_Blob,sizeof(CvBlob1)*(i + 1)*(sizeof(CvPoint)*Blobs[i].blobBox.nEdgeCnt));
		m_Blob[m_BlobCount].blobBox.arPt = new CvPoint[Blobs[i].blobBox.nEdgeCnt];
		m_Blob[i] = Blobs[i];
	}

	return rslt;
}

void cvBlobFinder::SetBlobsSearchPara(CvBlobSearchPara BlobSearchPara)
{
	m_BlobSearchPara.ucThreshold = BlobSearchPara.ucThreshold;
	m_BlobSearchPara.dMinArea = BlobSearchPara.dMinArea;
	m_BlobSearchPara.dMinWidth = BlobSearchPara.dMinWidth;
	m_BlobSearchPara.dMinHeight = BlobSearchPara.dMinHeight;
	m_BlobSearchPara.dMaxArea = BlobSearchPara.dMaxArea;
	m_BlobSearchPara.dMaxWidth = BlobSearchPara.dMaxWidth;
	m_BlobSearchPara.dMaxHeight = BlobSearchPara.dMaxHeight;
	m_BlobSearchPara.nMode = BlobSearchPara.nMode;
	m_BlobSearchPara.bUseMask = BlobSearchPara.bUseMask;
	m_BlobSearchPara.nBlobMaxCnt = BlobSearchPara.nBlobMaxCnt;
}

long cvBlobFinder::FindBlobs(IplImage* img, CvBlobSearchPara BlobSearchPara, bool SaveMask, CString strPath)
{
	CvPoint seed;
	CvConnectedComp lastblob;
	uchar val;
	int flags;
	CvBlob1 *Blobs = NULL;
	int BlobCount = 0;
	CvScalar new_val, lo_diff, up_diff;
	CvMemStorage* cvStorage = cvCreateMemStorage(0);
	IplImage *m_Mask2, *m_Mask3, *m_Mask4, *m_Mask5;
	m_Mask3 = NULL;

	if(BlobSearchPara.dMaxArea == 0)
	{
		BlobSearchPara.dMaxArea = img->imageSize;
	}

	if(BlobSearchPara.dMaxWidth == 0)
	{
		BlobSearchPara.dMaxWidth = img->width;
	}

	if(BlobSearchPara.dMaxHeight == 0)
	{
		BlobSearchPara.dMaxHeight = img->height;
	}

	m_BlobSearchPara = BlobSearchPara;

	Reset();
	
	if(m_BlobSearchPara.bUseMask)
	{
		m_Mask = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 1);
		cvZero(m_Mask);
		m_Mask2 = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 1);
		cvZero(m_Mask2);
		m_Mask3 = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 3);
		cvZero(m_Mask3);
		m_Mask4 = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 1);
		cvZero(m_Mask4);
		m_Mask5 = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 3);
		cvZero(m_Mask5);
		flags = (255 << 8) + CV_FLOODFILL_MASK_ONLY + CV_FLOODFILL_FIXED_RANGE + 4;
	}
	else
	{
		flags = CV_FLOODFILL_FIXED_RANGE + 4;
	}
	
	for(seed.x = 0; seed.x < img->width; seed.x++)
	{
		for (seed.y = 0; seed.y < img->height; seed.y++)
		{
			if (m_BlobSearchPara.nMode && (*(cvPtr2D(m_Mask2, seed.y + 1, seed.x + 1)) != 0)) continue;
				
			val = *(cvPtr2D(img, seed.y, seed.x));
			
			if(((m_BlobSearchPara.nMode == 1) && (val >= m_BlobSearchPara.ucThreshold)) ||
			   ((m_BlobSearchPara.nMode == 0) && (val == m_BlobSearchPara.ucThreshold)) ||
			   ((m_BlobSearchPara.nMode == -1) && (val <= m_BlobSearchPara.ucThreshold)))
			{
				if(m_BlobSearchPara.nMode == 1) 
				{
					new_val = cvScalarAll(0);
					lo_diff = cvScalarAll(val - m_BlobSearchPara.ucThreshold);
					up_diff = cvScalarAll(255-val);
				}
				else if(m_BlobSearchPara.nMode == 0) 
				{
					new_val = cvScalarAll(!m_BlobSearchPara.ucThreshold);
					lo_diff = cvScalarAll(0);
					up_diff = cvScalarAll(0);	
				}
				else if(m_BlobSearchPara.nMode == -1) 
				{
					new_val = cvScalarAll(255);
					lo_diff = cvScalarAll(val);
					up_diff = cvScalarAll(m_BlobSearchPara.ucThreshold - val);	
				}

				cvFloodFill(img, seed, new_val, lo_diff, up_diff, &lastblob, flags, m_Mask2);

				if((m_BlobSearchPara.dMaxArea >= lastblob.area && lastblob.area >= m_BlobSearchPara.dMinArea) &&
					(m_BlobSearchPara.dMaxWidth >= lastblob.rect.width && lastblob.rect.width >= m_BlobSearchPara.dMinWidth) &&
					(m_BlobSearchPara.dMaxHeight >= lastblob.rect.height && lastblob.rect.height >= m_BlobSearchPara.dMinHeight))
				{
					if(BlobCount > m_BlobSearchPara.nBlobMaxCnt)
						break;

					Blobs = (CvBlob1*)realloc((void*)Blobs,sizeof(CvBlob1)*(BlobCount + 1));
					Blobs[BlobCount].x = lastblob.rect.x;
					Blobs[BlobCount].y = lastblob.rect.y;
					Blobs[BlobCount].width = lastblob.rect.width;
					Blobs[BlobCount].height = lastblob.rect.height;  
					Blobs[BlobCount].area = lastblob.area;
					BlobCount++;
				}	
			}
		}
	} 

	cvFindContours(m_Mask2, cvStorage, &lastblob.contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	//cvFindContours(m_Mask2, cvStorage, &lastblob.contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	
	CvRect rc2d;
	int nEdgeCnt = 0;
	for(CvSeq* c = lastblob.contour; c!= NULL; c = c->h_next) 
	{
		rc2d = cvBoundingRect(c);

		for(int i = 0; i < BlobCount; i++)//minarea, double minwidth, double minheight
		{
			if((rc2d.width == Blobs[i].width) && (rc2d.height == Blobs[i].height))
			{
				CvPoint* arPt = new CvPoint[c->total];
				CvMat ptMat = cvMat(1, c->total, CV_32SC2, arPt);

				for(int j = 0; j < c->total; j++)
				{
					CvPoint* p = (CvPoint*)cvGetSeqElem(c, j);
					arPt[j].x = p->x;
					arPt[j].y = p->y;
				}

				CvPoint2D32f anchor, vect1, vect2;
				CvPoint2D32f pt[4];
				CvMat mat = cvMat( 1, c->total, CV_32SC2, arPt );
				CvBox2D box = cvMinAreaRect2( &mat, 0 );
				cvMinAreaRect(arPt, c->total, 0, rc2d.height, rc2d.width, 0, &anchor, &vect1, &vect2);
				cvBoxPoints( box, pt );

				cvFillConvexPoly(m_Mask3, arPt, c->total, CV_RGB(255, 255, 255));

				cvCvtColor(m_Mask3, m_Mask4, CV_BGR2GRAY);

				long long nCnt = 0;
				for(int h = rc2d.y; h < (rc2d.y + rc2d.height) - 1; h++)
				{
					for(int w = rc2d.x; w < (rc2d.x + rc2d.width) - 1; w++)
					{
						if(m_Mask4->imageData[w + (h * m_Mask4->widthStep)] != 0)
						{
							nCnt++;
						}
					}
				}

				float nXSize, nYSize, x1, x2, y1, y2;

				x1 = min(pt[0].x, pt[1].x);
				for(int nXcnt = 2; nXcnt < 4; nXcnt++)
				{
					x1 = min(x1, pt[nXcnt].x);
				}
				x2 = max(pt[0].x, pt[1].x);
				for(int nXcnt = 2; nXcnt < 4; nXcnt++)
				{
					x2 = max(x2, pt[nXcnt].x);
				}

				y1 = min(pt[0].y, pt[1].y);
				for(int nYcnt = 2; nYcnt < 4; nYcnt++)
				{
					y1 = min(y1, pt[nYcnt].y);
				}
				y2 = max(pt[0].y, pt[1].y);
				for(int nYcnt = 2; nYcnt < 4; nYcnt++)
				{
					y2 = max(y2, pt[nYcnt].y);
				}

				nXSize = x2 - x1;
				nYSize = y2 - y1;

				if((m_BlobSearchPara.dMaxArea >= nCnt && nCnt >= m_BlobSearchPara.dMinArea) &&
					(m_BlobSearchPara.dMaxWidth >= nXSize && nXSize >= m_BlobSearchPara.dMinWidth) &&
					(m_BlobSearchPara.dMaxHeight >= nYSize && nYSize >= m_BlobSearchPara.dMinHeight))
				{
					m_Blob = (CvBlob1*)realloc((void*)m_Blob,sizeof(CvBlob1)*(m_BlobCount + 1)*(sizeof(CvPoint)*c->total));
					m_Blob[m_BlobCount].blobBox.arPt = new CvPoint[c->total];
					for(int j = 0; j < c->total; j++)
					{
						CvPoint* p = (CvPoint*)cvGetSeqElem(c, j);
						m_Blob[m_BlobCount].blobBox.arPt[j].x = p->x;
						m_Blob[m_BlobCount].blobBox.arPt[j].y = p->y;
					}
					m_Blob[m_BlobCount].blobBox.nEdgeCnt = c->total;
					nEdgeCnt = c->total;
					m_Blob[m_BlobCount].blobBox.center = box.center;

					double dTemp = 0.;
					dTemp = max(box.size.width, box.size.height);
					m_Blob[m_BlobCount].blobBox.longLength = dTemp;

					dTemp = min(box.size.width, box.size.height);
					m_Blob[m_BlobCount].blobBox.shortLength = dTemp;

					m_Blob[m_BlobCount].blobBox.angle = box.angle;

					m_Blob[m_BlobCount].blobBox.blobArea = nCnt;
					m_Blob[m_BlobCount].blobBox.anchor = anchor;
					m_Blob[m_BlobCount].blobBox.vect1 = vect1;
					m_Blob[m_BlobCount].blobBox.vect2 = vect2;

					CvPoint2D32f TempPt;
					TempPt = CvPoint2D32f(x1, y1);
					m_Blob[m_BlobCount].blobBox.LeftTop = TempPt;
					TempPt = CvPoint2D32f(x2, y1);
					m_Blob[m_BlobCount].blobBox.RightTop = TempPt;
					TempPt = CvPoint2D32f(x2, y2);
					m_Blob[m_BlobCount].blobBox.RightBottom = TempPt;
					TempPt = CvPoint2D32f(x1, y2);
					m_Blob[m_BlobCount].blobBox.LeftBottom = TempPt;

					m_Blob[m_BlobCount].x = Blobs[i].x;
					m_Blob[m_BlobCount].y = Blobs[i].y;
					m_Blob[m_BlobCount].width = Blobs[i].width;
					m_Blob[m_BlobCount].height = Blobs[i].height;  
					m_Blob[m_BlobCount].area = Blobs[i].area;

					//cvFillConvexPoly(m_Mask5, m_Blob[m_BlobCount].blobBox.arPt, c->total, CV_RGB(255, 255, 255), CV_AA);
					cvDrawContours(m_Mask5, c, CV_RGB(255, 255, 255), CV_RGB(0,0,0), 255, -1);
					m_Blob[m_BlobCount].blobBox.nShapIdx = ContourToShap(c, m_Blob[m_BlobCount].blobBox.strShap);

					m_BlobCount++;
				}

				delete arPt;
			}
		}
	}
	free(Blobs);
	Blobs = NULL;

	cvCvtColor(m_Mask5, m_Mask, CV_BGR2GRAY);

	if (m_Mask2 != NULL) cvReleaseImage(&m_Mask2);
	m_Mask2 = NULL;
	if (m_Mask3 != NULL) cvReleaseImage(&m_Mask3);
	m_Mask3 = NULL;
	if (m_Mask4 != NULL) cvReleaseImage(&m_Mask4);
	m_Mask4 = NULL;
	if (m_Mask5 != NULL) cvReleaseImage(&m_Mask5);
	m_Mask5 = NULL;
	if(cvStorage)	cvReleaseMemStorage(&cvStorage);

	if(SaveMask && m_BlobSearchPara.nMode)
	{
		if(!strPath.IsEmpty())
		{
			SaveIplImage(m_Mask, strPath);
		}
	}

	return m_BlobCount;	
}

int cvBlobFinder::ContourToShap(CvSeq *contour, char *strShap)
{
	CvSeq *rslt;
	int nRslt = -1;
	int nLen = 0;
	if(contour->total == 0)
	{
		return nRslt;
	}
	rslt = cvApproxPoly(contour, contour->header_size, NULL, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.02, 0);

	switch(rslt->total)
	{
	case 3:
		nRslt = SHAP_TRIANGLE;
		nLen = lstrlen(SHAP_STR_TRIANGLE) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_TRIANGLE);
		break;
	case 4:
		nRslt = SHAP_RECTANGLE;
		nLen = lstrlen(SHAP_STR_RECTANGLE) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_RECTANGLE);
		break;
	case 5:
		nRslt = SHAP_PENTAGON;
		nLen = lstrlen(SHAP_STR_PENTAGON) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_PENTAGON);
		break;
	case 6:
		nRslt = SHAP_HEXAGON;
		nLen = lstrlen(SHAP_STR_HEXAGON) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_HEXAGON);
		break;
	case 10:
		nRslt = SHAP_DECAGON;
		nLen = lstrlen(SHAP_STR_DECAGON) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_DECAGON);
		break;
	default:
		nRslt = SHAP_CIRCLE;
		nLen = lstrlen(SHAP_STR_CIRCLE) + 1;
		sprintf_s(strShap, nLen, "%S", SHAP_STR_CIRCLE);
		break;
	}

	return nRslt;
}

void cvBlobFinder::GetEachBlobPoint(int nBlobIdx, CvPoint2D32f *pt)
{
	pt[0] = m_Blob[nBlobIdx].blobBox.LeftTop;
	pt[1] = m_Blob[nBlobIdx].blobBox.RightTop;
	pt[2] = m_Blob[nBlobIdx].blobBox.RightBottom;
	pt[3] = m_Blob[nBlobIdx].blobBox.LeftBottom;
}

CvBlob1* cvBlobFinder::GetBlobs()
{
	return m_Blob;	
}

IplImage* cvBlobFinder::GetMask()
{
	return m_Mask;	
}

long cvBlobFinder::GetBlobCount()
{
	return m_BlobCount;
}

void cvBlobFinder::Reset()
{
	if (m_Mask != NULL) cvReleaseImage(&m_Mask);
	m_Mask = NULL;

	for(int i = 0; i < m_BlobCount; i++)
	{
		if(m_Blob[i].blobBox.nEdgeCnt > 0)
		{
			delete m_Blob[i].blobBox.arPt;
		}
	}

	if (m_Blob != NULL) free(m_Blob);
	m_Blob = NULL;
	m_BlobCount = 0;
}

void cvBlobFinder::BlobErosion(int nEroSize, bool SaveImage, CString strPath)
{
	IplImage *dst, *src, *dst2;
	dst = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	dst2 = cvCreateImage(cvSize(m_Mask->width - 2, m_Mask->height - 2), IPL_DEPTH_8U, 1);
	src = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);	
	
	cvCopy(m_Mask, src);

	IplConvKernel *element = cvCreateStructuringElementEx(nEroSize, nEroSize, 1, 1, CV_SHAPE_RECT, NULL);

	cvErode(src, dst, element);

	for(int h = 0; h < dst2->height; h++)
	{
		for(int w = 0; w < dst2->width; w++)
		{
			dst2->imageData[w + (h * dst2->widthStep)] = dst->imageData[w + (h * dst->widthStep)];
		}
	}

	FindBlobs(dst2, m_BlobSearchPara, SaveImage, strPath);

	if (dst != NULL) cvReleaseImage(&dst);
	dst = NULL;
	if (src != NULL) cvReleaseImage(&src);
	src = NULL;
	if (dst2 != NULL) cvReleaseImage(&dst2);
	dst2 = NULL;

	if(element != NULL)	cvReleaseStructuringElement(&element);
	element = NULL;
}

void cvBlobFinder::BlobDilation(int nDilSize, bool SaveImage, CString strPath)
{
	IplImage *dst, *src, *dst2;
	dst = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	dst2 = cvCreateImage(cvSize(m_Mask->width - 2, m_Mask->height - 2), IPL_DEPTH_8U, 1);
	src = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	cvCopy(m_Mask, src);
	IplConvKernel *element = cvCreateStructuringElementEx(nDilSize, nDilSize, 1, 1, CV_SHAPE_RECT, NULL);

	cvDilate(src, dst, element);

	for(int h = 0; h < dst2->height; h++)
	{
		for(int w = 0; w < dst2->width; w++)
		{
			dst2->imageData[w + (h * dst2->widthStep)] = dst->imageData[w + (h * dst->widthStep)];
		}
	}

	FindBlobs(dst2, m_BlobSearchPara, SaveImage, strPath);

	if (dst != NULL) cvReleaseImage(&dst);
	dst = NULL;
	if (src != NULL) cvReleaseImage(&src);
	src = NULL;
	if (dst2 != NULL) cvReleaseImage(&dst2);
	dst2 = NULL;

	if(element != NULL)	cvReleaseStructuringElement(&element);
	element = NULL;
}

void cvBlobFinder::BlobOpening(int nOpenSize, bool SaveImage, CString strPath)
{
	IplImage *dst, *src;
	dst = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	src = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	cvCopy(m_Mask, src);
	IplConvKernel *element = cvCreateStructuringElementEx(nOpenSize, nOpenSize, 1, 1, CV_SHAPE_RECT, NULL);

	cvMorphologyEx(src, dst, NULL, element, CV_MOP_OPEN, 1); 

	FindBlobs(dst, m_BlobSearchPara, SaveImage, strPath);

	if (dst != NULL) cvReleaseImage(&dst);
	dst = NULL;
	if (src != NULL) cvReleaseImage(&src);
	src = NULL;

	if(element != NULL)	cvReleaseStructuringElement(&element);
	element = NULL;
}

void cvBlobFinder::BlobClosing(int nCloseSize, bool SaveImage, CString strPath)
{
	IplImage *dst, *src;
	dst = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	src = cvCreateImage(cvSize(m_Mask->width, m_Mask->height), IPL_DEPTH_8U, 1);
	cvCopy(m_Mask, src);
	IplConvKernel *element = cvCreateStructuringElementEx(nCloseSize, nCloseSize, 1, 1, CV_SHAPE_RECT, NULL);

	cvMorphologyEx(src, dst, NULL, element, CV_MOP_CLOSE, 1); 

	FindBlobs(dst, m_BlobSearchPara, SaveImage, strPath);

	if (dst != NULL) cvReleaseImage(&dst);
	dst = NULL;
	if (src != NULL) cvReleaseImage(&src);
	src = NULL;

	if(element != NULL)	cvReleaseStructuringElement(&element);
	element = NULL;
}

uchar cvBlobFinder::GetSrcEachBlobAvgVal(IplImage *src, int nBlobIdx)
{
	uchar rslt = 0;
	BOOL isSearch = FALSE;
	IplImage *temp1, *temp2, *temp3;
	CvMemStorage* cvStorage = cvCreateMemStorage(0);
	CvSeq *seq = 0;
	CvRect rect, rc2d;
	long long lSumVal = 0;
	BYTE *by_src;
	by_src = new BYTE[src->width * src->height];
	memcpy(by_src, src->imageData, src->width * src->height * sizeof(BYTE));

	rect.x = m_Blob[nBlobIdx].x;
	rect.y = m_Blob[nBlobIdx].y;
	rect.width = m_Blob[nBlobIdx].width;
	rect.height = m_Blob[nBlobIdx].height;

	temp1 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	cvZero(temp1);
	temp2 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	cvZero(temp2);
	temp3 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 3);
	cvZero(temp3);
	cvCopy(m_Mask, temp1);
	
	cvSetImageROI(temp1, rect);

	cvFindContours(temp1, cvStorage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

	for(CvSeq* c = seq; c!= NULL; c = c->h_next) 
	{
		rc2d = cvBoundingRect(c);

		if(((m_Blob[nBlobIdx].width + 2 >= rc2d.width) && (rc2d.width >= m_Blob[nBlobIdx].width - 2)) &&
			((m_Blob[nBlobIdx].height + 2 >= rc2d.height) && (rc2d.height >= m_Blob[nBlobIdx].height - 2)))
		{
			CvPoint* arPt = new CvPoint[c->total];
			CvMat ptMat = cvMat(1, c->total, CV_32SC2, arPt);

			for(int j = 0; j < c->total; j++)
			{
				CvPoint* p = (CvPoint*)cvGetSeqElem(c, j);
				arPt[j].x = p->x + rect.x;
				arPt[j].y = p->y + rect.y;
			}

			CvPoint2D32f anchor, vect1, vect2;
			CvPoint2D32f pt[4];
			CvMat mat = cvMat( 1, c->total, CV_32SC2, arPt );
			CvBox2D box = cvMinAreaRect2( &mat, 0 );
			cvMinAreaRect(arPt, c->total, 0, rc2d.height, rc2d.width, 0, &anchor, &vect1, &vect2);
			cvBoxPoints( box, pt );

			cvFillConvexPoly(temp3, arPt, c->total, CV_RGB(255, 255, 255));

			cvCvtColor(temp3, temp2, CV_BGR2GRAY);

			delete arPt;
			isSearch = TRUE;
		}

		if(isSearch)
		{
			break;
		}
	}

	for(int h = rect.y; h < (rect.y + rect.height); h++)
	{
		for(int w = rect.x; w < (rect.x + rect.width); w++)
		{
			if(temp2->imageData[w + (h * temp2->widthStep)] != 0)
			{
				lSumVal += by_src[w + (h * src->widthStep)];
			}
		}
	}

	rslt = lSumVal / m_Blob[nBlobIdx].blobBox.blobArea;

	if (temp1 != NULL) cvReleaseImage(&temp1);
	temp1 = NULL;
	if (temp2 != NULL) cvReleaseImage(&temp2);
	temp2 = NULL;
	if (temp3 != NULL) cvReleaseImage(&temp3);
	temp3 = NULL;

	if(cvStorage)	cvReleaseMemStorage(&cvStorage);

	delete by_src;

	return rslt;
}