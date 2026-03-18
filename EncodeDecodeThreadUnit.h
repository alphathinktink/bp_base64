//---------------------------------------------------------------------------
#ifndef EncodeDecodeThreadUnitH
#define EncodeDecodeThreadUnitH
//---------------------------------------------------------------------------

#include <windows.h>
#include "UniString.h"

const UINT WM_BP_THREAD_DONE=WM_APP+100;

namespace EncodeDecodeThread {

enum TOperation
{
        opEncodeFileToBase64=1,
        opDecodeBase64ToFile=2,
        opEncodeFileToDataUri=3
};

typedef struct TRequest
{
        TOperation Operation;
        UniString FileName;
        AnsiString InputText;
}TRequest;

typedef struct TResult
{
        bool Success;
        UniString Message;
        AnsiString OutputText;
}TResult;

class TWorker
{
private:
        HWND FNotifyWindow;
        HANDLE FThread;
        DWORD FThreadID;
        TRequest FRequest;

        static DWORD WINAPI ThreadProc(LPVOID Param);
        void __fastcall Execute(TResult &Result);
public:
        __fastcall TWorker(HWND NotifyWindow,const TRequest &Request);
        __fastcall ~TWorker(void);

        bool __fastcall Start(void);
        HANDLE __fastcall GetHandle(void) const;
};

};

//---------------------------------------------------------------------------
#endif//EncodeDecodeThreadUnitH
//---------------------------------------------------------------------------
