#pragma once
// CMeeseeksVisionDlg dialog
class CCameraDlg : public CDialogEx
{
   // Construction
public:
   CCameraDlg(CWnd* pParent = nullptr);	// standard constructor
   void NewParams();
                                             // Dialog Data
#ifdef AFX_DESIGN_TIME
   enum { IDD = IDD_MEESEEKSVISION_DIALOG };
#endif

protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   virtual void PostNcDestroy();


   // Implementation
protected:

   void SetScrollBars();

   // Generated message map functions
   virtual BOOL OnInitDialog();
   virtual BOOL PreTranslateMessage(MSG *pMsg);

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnExposureManual();
   afx_msg void OnExposureAperturePriority();
   afx_msg void OnPropButtonClicked();


   int  m_autoExposure;
   bool m_bHoldOffUpdate;
   CSliderCtrl m_zoomSlider, m_brightnessSlider, m_contrastSlider, m_exposureSlider;
   CSliderCtrl m_saturationSlider, m_gainSlider, m_sharpnessSlider, m_whiteBalanceSlider, m_focusSlider;


   DECLARE_MESSAGE_MAP()
};


#pragma once
