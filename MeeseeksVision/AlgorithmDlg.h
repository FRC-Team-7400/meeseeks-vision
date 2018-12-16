#pragma once
// CMeeseeksVisionDlg dialog
class CAlgorithmDlg : public CDialogEx
{
   // Construction
public:
   CAlgorithmDlg(CWnd* pParent = nullptr);	// standard constructor

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
   CSliderCtrl	  m_minRedSlider, m_maxRedSlider, m_minGreenSlider, m_maxGreenSlider, m_minBlueSlider, m_maxBlueSlider;
   CSliderCtrl   m_blurSlider, m_erodeSlider, m_dilateSlider;
   bool          m_bHoldOffUpdate;
   CStatic       m_redStatic, m_greenStatic, m_blueStatic;

   // Generated message map functions
   virtual BOOL OnInitDialog();
   virtual BOOL PreTranslateMessage(MSG *pMsg);

   void SetScrollBars();
   void SetRGBHSV();

   afx_msg void OnPaint();
   afx_msg void OnDestroy();
   afx_msg void OnPropButtonClicked();
   afx_msg void OnPropButtonRGBClicked();
   afx_msg void OnPropButtonHSVClicked();


   afx_msg HCURSOR OnQueryDragIcon();

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   LRESULT OnNewParams(WPARAM wParam, LPARAM lParam);

   DECLARE_MESSAGE_MAP()
};


