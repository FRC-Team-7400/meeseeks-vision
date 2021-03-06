
// MeeseeksVisionDlg.h : header file
//

#pragma once
#include <thread>
#include "ZoomDlg.h"
#include "AlgorithmDlg.h"
#include "CameraDlg.h"
#include "TapeDlg.h"

// CMeeseeksVisionDlg dialog
class CMeeseeksVisionDlg : public CDialogEx
{
// Construction
public:
	CMeeseeksVisionDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEESEEKSVISION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
   HANDLE m_hStopThread;
   CZoomDlg     *m_pZoomDlg;
   CString       m_serverAddress;
   CSliderCtrl   m_jpgQualitySlider;
   CComboBox     m_paramFilesComboBox;
   int           m_serverPort;
   bool          m_bHoldOffUpdate;
   CTabCtrl      m_tab;
   CAlgorithmDlg *m_pAlgorithmDlg;
   CCameraDlg    *m_pCameraDlg;
   CTapeDlg      *m_pTapeDlg;

   std::thread m_socketClientThread;

	// Generated message map functions
	virtual BOOL OnInitDialog();
   virtual BOOL PreTranslateMessage(MSG *pMsg);

   void SetScrollBars();
   void SizeTabWindows();

	afx_msg void OnPaint();
   afx_msg void OnDestroy();
   afx_msg void OnZoomWindow();
   afx_msg void OnStartStop();
   afx_msg void OnLoadFile();
   afx_msg void OnSaveFile();
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnPropButtonClicked();
   afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg HCURSOR OnQueryDragIcon();

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   LRESULT OnNewParams(WPARAM wParam, LPARAM lParam);
   LRESULT OnFiles(WPARAM wParam, LPARAM lParam);
   LRESULT OnStats(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
