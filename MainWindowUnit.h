//---------------------------------------------------------------------------
#ifndef MainWindowUnitH
#define MainWindowUnitH
//---------------------------------------------------------------------------

#include <windows.h>
#include <shellapi.h>
#include "UniString.h"

namespace EncodeDecodeThread { class TWorker; }

class TMainWindow
{
private:
        HWND FHandle;
        HWND FEditBase64;
        HWND FEditEncodeFile;
        HWND FEditDecodeFile;
        HWND FBtnDecode;
        HWND FBtnEncode;
        HWND FBtnUriEncode;
        HWND FBtnEncodeBrowse;
        HWND FBtnDecodeBrowse;
        HWND FBtnQuit;
        WNDPROC FEncodeFileEditOrigProc;

        EncodeDecodeThread::TWorker *FWorker;

        void __fastcall CacheControls(void);
        void __fastcall SetBusy(bool Busy);
        void __fastcall StartEncode(bool AsDataUri);
        void __fastcall StartDecode(void);
        void __fastcall BrowseForEncodeFile(void);
        void __fastcall BrowseForDecodeFile(void);
        void __fastcall InitDragAndDrop(void);
        void __fastcall HandleEncodeFileDrop(HDROP hDrop);

        static LRESULT CALLBACK EncodeFileEditProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

        UniString __fastcall GetEncodeFilePath(void) const;
        UniString __fastcall GetDecodeFilePath(void) const;
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
