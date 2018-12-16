#pragma once
#include "JPEGDisplay.h"
#include <thread>

typedef struct
{
   int byteCount;
   BYTE *pBuffer;
} VideoBuffer;

class CZoomDlg : public CDialog
{
public: CZoomDlg(CWnd *pParemt);
protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   virtual BOOL OnInitDialog();
   virtual void PostNcDestroy();

   void StartRecording();
   void StopRecording();

   afx_msg void OnStartStopRecording();

   LRESULT OnNewRearCameraFrame(WPARAM wParam, LPARAM lParam);

   CJPEGDisplay  m_camera;
   CButton       m_startStopRecordingButton;
   CStringA      m_movieFolder;
   bool          m_bRecording;
   std::thread   m_recordingThread;
   HANDLE        m_hStopRecordingThread;

   DECLARE_MESSAGE_MAP()
};
