//---------------------------------------------------------------------------
#include "MainWindowUnit.h"
#include "MainWindowResource.h"
#include "EncodeDecodeThreadUnit.h"
#include "ItsAMysteryUnit.h"
#include "CommonFuncsUnit.h"

using namespace EncodeDecodeThread;

static UniString GetWindowTextWString(HWND hwnd)
{
        int len=::GetWindowTextLengthW(hwnd);
        if(len<=0) return L"";
        wchar_t *buf=new wchar_t[len+2];
        buf[0]=0;
        ::GetWindowTextW(hwnd,buf,len+1);
        UniString out(buf);
        delete [] buf;
        return out;
}

static AnsiString ToAnsiNoLoss(const UniString &in)
{
        return CommonFuncs::UniStringToMBCS(in);
}

__fastcall TMainWindow::TMainWindow(void)
: FHandle(NULL),FEditBase64(NULL),FEditFile(NULL),FBtnDecode(NULL),FBtnEncode(NULL),FBtnUriEncode(NULL),FBtnBrowse(NULL),FBtnQuit(NULL),FWorker(NULL)
{
}
//---------------------------------------------------------------------------
__fastcall TMainWindow::~TMainWindow(void)
{
        if(FWorker)
        {
                delete FWorker;
                FWorker=NULL;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CacheControls(void)
{
        FEditBase64=::GetDlgItem(FHandle,IDC_BASE64_MULTILINE);
        FEditFile=::GetDlgItem(FHandle,IDC_FILEFULLPATH_EDIT);
        FBtnDecode=::GetDlgItem(FHandle,IDC_DECODE_BUTTON);
        FBtnEncode=::GetDlgItem(FHandle,IDC_ENCODE_BUTTON);
        FBtnUriEncode=::GetDlgItem(FHandle,IDC_URIENCODE_BUTTON);
        FBtnBrowse=::GetDlgItem(FHandle,IDC_FILEBROWSE_BUTTON);
        FBtnQuit=::GetDlgItem(FHandle,IDC_QUIT_BUTTON);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SetBusy(bool Busy)
{
        BOOL enabled=Busy?FALSE:TRUE;
        ::EnableWindow(FEditBase64,enabled);
        ::EnableWindow(FEditFile,enabled);
        ::EnableWindow(FBtnDecode,enabled);
        ::EnableWindow(FBtnEncode,enabled);
        ::EnableWindow(FBtnUriEncode,enabled);
        ::EnableWindow(FBtnBrowse,enabled);
        ::EnableWindow(FBtnQuit,enabled);
        ::SetCursor(::LoadCursor(NULL,Busy?IDC_WAIT:IDC_ARROW));
        ::SetWindowTextW(FHandle,Busy?L"Base64 - Processing...":L"Base64");
}
//---------------------------------------------------------------------------
UniString __fastcall TMainWindow::GetFilePath(void) const
{
        return GetWindowTextWString(FEditFile);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::GetBase64Text(void) const
{
        UniString s=GetWindowTextWString(FEditBase64);
        return ToAnsiNoLoss(s);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SetBase64Text(const AnsiString &Text)
{
        UniString u=CommonFuncs::MBCSToUniString(Text);
        ::SetWindowTextW(FEditBase64,u.c_bstr());
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::BrowseForFile(void)
{
        TOpenDialog dlg;
        TOpenSaveDialogOptions *o=dlg.GetOptions();
        o->FileMustExist=true;
        o->PathMustExist=true;
        o->NoChangeDir=true;
        dlg.SetTitle(L"Choose a file");
        dlg.SetFilter(L"All Files (*.*)|*.*");
        if(dlg.Execute(FHandle))
        {
                ::SetWindowTextW(FEditFile,dlg.GetFileName().c_bstr());
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::StartEncode(bool AsDataUri)
{
        UniString filePath=CommonFuncs::Trim(GetFilePath());
        if(filePath==L"")
        {
                ::MessageBoxW(FHandle,L"Please choose a file path first.",L"Base64",MB_ICONWARNING|MB_OK);
                return;
        }

        TRequest req;
        req.Operation=AsDataUri?opEncodeFileToDataUri:opEncodeFileToBase64;
        req.FileName=filePath;
        req.InputText="";

        FWorker=new TWorker(FHandle,req);
        if(!FWorker->Start())
        {
                delete FWorker;
                FWorker=NULL;
                ::MessageBoxW(FHandle,L"Failed to start worker thread.",L"Base64",MB_ICONERROR|MB_OK);
                return;
        }

        SetBusy(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::StartDecode(void)
{
        UniString filePath=CommonFuncs::Trim(GetFilePath());
        if(filePath==L"")
        {
                ::MessageBoxW(FHandle,L"Please provide an output file path for decoded bytes.",L"Base64",MB_ICONWARNING|MB_OK);
                return;
        }

        AnsiString base64=GetBase64Text();
        if(base64=="")
        {
                ::MessageBoxW(FHandle,L"Please enter/paste base64 text first.",L"Base64",MB_ICONWARNING|MB_OK);
                return;
        }

        TRequest req;
        req.Operation=opDecodeBase64ToFile;
        req.FileName=filePath;
        req.InputText=base64;

        FWorker=new TWorker(FHandle,req);
        if(!FWorker->Start())
        {
                delete FWorker;
                FWorker=NULL;
                ::MessageBoxW(FHandle,L"Failed to start worker thread.",L"Base64",MB_ICONERROR|MB_OK);
                return;
        }

        SetBusy(true);
}
//---------------------------------------------------------------------------
INT_PTR CALLBACK TMainWindow::DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
        TMainWindow *self=(TMainWindow *)::GetWindowLongPtrW(hwndDlg,DWLP_USER);
        if(uMsg==WM_INITDIALOG)
        {
                self=(TMainWindow *)lParam;
                ::SetWindowLongPtrW(hwndDlg,DWLP_USER,(LONG_PTR)self);
                self->FHandle=hwndDlg;
                self->CacheControls();
                return TRUE;
        }
        if(self) return self->HandleMessage(uMsg,wParam,lParam);
        return FALSE;
}
//---------------------------------------------------------------------------
INT_PTR __fastcall TMainWindow::HandleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
        switch(uMsg)
        {
                case WM_COMMAND:
                {
                        switch(LOWORD(wParam))
                        {
                                case IDC_FILEBROWSE_BUTTON: BrowseForFile(); return TRUE;
                                case IDC_ENCODE_BUTTON: StartEncode(false); return TRUE;
                                case IDC_URIENCODE_BUTTON: StartEncode(true); return TRUE;
                                case IDC_DECODE_BUTTON: StartDecode(); return TRUE;
                                case IDC_QUIT_BUTTON: ::DestroyWindow(FHandle); return TRUE;
                        }
                        break;
                }
                case WM_CLOSE:
                        if(FWorker)
                        {
                                ::MessageBoxW(FHandle,L"An operation is still running. Please wait for it to complete.",L"Base64",MB_ICONINFORMATION|MB_OK);
                                return TRUE;
                        }
                        ::DestroyWindow(FHandle);
                        return TRUE;
                case WM_DESTROY:
                        ::PostQuitMessage(0);
                        return TRUE;
                case WM_BP_THREAD_DONE:
                {
                        TResult *res=(TResult *)lParam;
                        SetBusy(false);
                        if(res)
                        {
                                if(res->Success)
                                {
                                        if(res->OutputText!="") SetBase64Text(res->OutputText);
                                        ::MessageBoxW(FHandle,res->Message.c_bstr(),L"Base64",MB_ICONINFORMATION|MB_OK);
                                }
                                else
                                {
                                        ::MessageBoxW(FHandle,res->Message.c_bstr(),L"Base64",MB_ICONERROR|MB_OK);
                                }
                                delete res;
                        }
                        if(FWorker)
                        {
                                delete FWorker;
                                FWorker=NULL;
                        }
                        return TRUE;
                }
        }
        return FALSE;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::Create(HINSTANCE hInst,int nCmdShow)
{
        HWND hwnd=::CreateDialogParamW(hInst,MAKEINTRESOURCEW(IDD_MAIN_WINDOW),NULL,&TMainWindow::DialogProc,(LPARAM)this);
        if(!hwnd) return false;
        ::ShowWindow(hwnd,nCmdShow);
        ::UpdateWindow(hwnd);
        return true;
}
//---------------------------------------------------------------------------
int __fastcall TMainWindow::Run(void)
{
        MSG msg;
        while(::GetMessageW(&msg,NULL,0,0)>0)
        {
                if(!::IsDialogMessageW(FHandle,&msg))
                {
                        ::TranslateMessage(&msg);
                        ::DispatchMessageW(&msg);
                }
        }
        return (int)msg.wParam;
}
//---------------------------------------------------------------------------
