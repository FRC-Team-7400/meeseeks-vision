#include "stdafx.h"
#include "ZoomDlg.h"
#include "resource.h"
#include "Magick++.h"
#include "VideoCapture.h"
#include <vector>

CRITICAL_SECTION g_bufferLock;

std::vector<VideoBuffer> g_buffers;

char g_moviePathAndFile[_MAX_PATH];

void PutFrame(BYTE *pBuffer, int size)
{   
   VideoBuffer buffer;
 
   buffer.pBuffer = pBuffer;
   buffer.byteCount = size;
   
   ::EnterCriticalSection(&g_bufferLock);
 
   g_buffers.push_back(buffer);
 
   ::LeaveCriticalSection(&g_bufferLock);
}

bool GetFrame(VideoBuffer &buffer)
{
   bool bStatus = false;

   ::EnterCriticalSection(&g_bufferLock);

   if (g_buffers.size()) {
      buffer = g_buffers[0];   
      g_buffers.erase(g_buffers.begin());
      bStatus = true;
   }

   ::LeaveCriticalSection(&g_bufferLock);

   return bStatus;
}

BEGIN_MESSAGE_MAP(CZoomDlg, CDialog)

   ON_MESSAGE(WM_USER_NEW_REAR_FRAME, OnNewRearCameraFrame)
   ON_BN_CLICKED(IDC_START_STOP_RECORD, OnStartStopRecording)

END_MESSAGE_MAP()

CZoomDlg::CZoomDlg(CWnd *pParemt)
{
   m_bRecording = false;

   ::InitializeCriticalSection(&g_bufferLock);

   char exeFolder[_MAX_PATH];

   ::GetModuleFileNameA(NULL, exeFolder, _MAX_PATH);

   ::PathRemoveFileSpecA(exeFolder);

   ::PathCombineA(m_movieFolder.GetBuffer(_MAX_PATH), exeFolder, "Movies");

   m_hStopRecordingThread = ::CreateEvent(NULL, TRUE, FALSE, L"StopRecordingThread");

   Create(IDD_ZOOM_OUT);
}

void CZoomDlg::PostNcDestroy()
{
   StopRecording();
   
   delete this;

   CDialog::PostNcDestroy();

   ::CloseHandle(m_hStopRecordingThread);
   ::DeleteCriticalSection(&g_bufferLock);
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_ZOOM, m_camera);
   DDX_Control(pDX, IDC_START_STOP_RECORD, m_startStopRecordingButton);
}

BOOL CZoomDlg::OnInitDialog()
{
   BOOL bStatus = CDialog::OnInitDialog();

   m_camera.SetSize(1280, 720);

   m_camera.SetWindowPos(NULL, 0, 0, 1280, 720, SWP_NOZORDER | SWP_NOMOVE);

   CRect rect, frameRect;
   
   m_camera.GetWindowRect(rect);

   GetWindowRect(frameRect);

   int edgeDelta = rect.left - frameRect.left;

   int windowWidth = edgeDelta * 2 + rect.Width();

   int topDelta = rect.top - frameRect.top;
   int windowHeight = topDelta + edgeDelta + rect.Height();

   SetWindowPos(NULL, 0, 0, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOMOVE);

   return bStatus;
}


LRESULT CZoomDlg::OnNewRearCameraFrame(WPARAM wParam, LPARAM lParam)
{
   BYTE *pBuffer = (BYTE *)wParam;
   int count = lParam;

   m_camera.SetBuffer(pBuffer, count);

   if (m_bRecording)
      PutFrame(pBuffer, count);  
   else
      delete[]pBuffer;

   return 0;
}

void RecordingThread()
{
   HANDLE hStopEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"StopRecordingThread");

   if (!hStopEvent)
      return;

   BYTE yuvFrame[640*360*3];

   VideoCapture vc;

   vc.Init(640, 360, 30, 40000);

   do
   {
      VideoBuffer buffer;

      if (GetFrame(buffer)) {
         Magick::Image image;
         Magick::Blob blob;
         blob.updateNoCopy(buffer.pBuffer, buffer.byteCount);
         image.magick("jpg");
         image.read(blob);
         image.magick("RGB");
         image.write(&blob);
         int length = blob.length();

         vc.AddFrame((uint8_t *) blob.data());

         if (length == 123)
         {
            OutputDebugString(L"here");
         }
      }

   } while (::WaitForSingleObject(hStopEvent, 10) != WAIT_OBJECT_0);

   vc.Finish(g_moviePathAndFile);

   ::Sleep(1000);

   ::CloseHandle(hStopEvent);
}

void CZoomDlg::StartRecording()
{
   m_bRecording = true;

   m_recordingThread = std::thread(RecordingThread);
}

void CZoomDlg::StopRecording()
{
   CWaitCursor waitCursor;

   COleDateTime timeNow = COleDateTime::GetCurrentTime();

   _bstr_t bstrFileName = timeNow.Format(L"%Y-%m-%d_%H_%M_%S.mp4");

   ::PathCombineA(g_moviePathAndFile, m_movieFolder, (const char *) bstrFileName);
 
   if (m_recordingThread.joinable()) {
      ::SetEvent(m_hStopRecordingThread);
      m_recordingThread.join();
      ::ResetEvent(m_hStopRecordingThread);
   }

   m_bRecording = false;
}

void CZoomDlg::OnStartStopRecording()
{
   m_startStopRecordingButton.SetWindowText(m_bRecording ? L"Start Recording" : L"Stop Recording");

   if (m_bRecording) 
      StopRecording();
   else
      StartRecording();
}

