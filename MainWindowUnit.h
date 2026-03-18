//---------------------------------------------------------------------------
#ifndef MainWindowUnitH
#define MainWindowUnitH
//---------------------------------------------------------------------------

#include <windows.h>
#include "UniString.h"

namespace EncodeDecodeThread { class TWorker; }

class TMainWindow
{
private:
        HWND FHandle;
        HWND FEditBase64;
        HWND FEditFile;
        HWND FBtnDecode;
        HWND FBtnEncode;
        HWND FBtnUriEncode;
        HWND FBtnBrowse;
        HWND FBtnQuit;

        EncodeDecodeThread::TWorker *FWorker;

        void __fastcall CacheControls(void);
        void __fastcall SetBusy(bool Busy);
        void __fastcall StartEncode(bool AsDataUri);
        void __fastcall StartDecode(void);
        void __fastcall BrowseForFile(void);

        UniString __fastcall GetFilePath(void) const;
        AnsiString __fastcall GetBase64Text(void) const;
        void __fastcall SetBase64Text(const AnsiString &Text);

        static INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
        INT_PTR __fastcall HandleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
        __fastcall TMainWindow(void);
        __fastcall ~TMainWindow(void);

        bool __fastcall Create(HINSTANCE hInst,int nCmdShow);
        int __fastcall Run(void);
};

#endif//MainWindowUnitH
//---------------------------------------------------------------------------
