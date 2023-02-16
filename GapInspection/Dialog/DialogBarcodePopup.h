#pragma once
#include "afxwin.h"


// CDialogBarcodePopup ��ȭ �����Դϴ�.

class CDialogBarcodePopup : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogBarcodePopup)

public:
	CDialogBarcodePopup(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogBarcodePopup();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BARCODE_POPUP };

private:
	CString m_strBarcode;
	CEdit m_edit_Barcode;
	int m_nMainTapIdx;
	CBtnenhctrl m_ctrBarcode;
	BOOL m_bInputBarcode;
	CRect m_ShowRect;
	//20100315 ngh
	BOOL m_bSeqProgress;
	CString m_strWaitBarcode;
	BOOL m_bWaitBarcode;
	CString m_strBarCap;
	BOOL m_bNoBarcode;
public:
	CString GetBarcode(){m_bInputBarcode = FALSE; return m_strBarcode;};
	void SetMainTapIdx(int nTapIdx);
	BOOL GetBarcodeInput(){return m_bInputBarcode;};
	void SetShowRect(CRect *rect);
	//20190315 ngh
	void SetSeqProgress(BOOL isProgress);
	BOOL GetWaitBarcode(){return m_bWaitBarcode;};
	CString GetWaitBarcodeString(){return m_strWaitBarcode;};

	void SetNoBarcode(CString strBarcode);
protected:


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditBarcodePopupString();
	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
