//---------------------------------------------------------------------------
#include "MainWindowUnit.h"
#include "MainWindowResource.h"
#include "EncodeDecodeThreadUnit.h"
#include "ItsAMysteryUnit.h"
#include "CommonFuncsUnit.h"
#include <winreg.h>

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

static bool StartsWithNoCase(const AnsiString &Text,const char *Prefix)
{
        int i=1;
        while(*Prefix)
        {
                if(i>Text.Length()) return false;
                char a=Text[i];
                char b=*Prefix;
                if(a>='A' && a<='Z') a=(char)(a-'A'+'a');
                if(b>='A' && b<='Z') b=(char)(b-'A'+'a');
                if(a!=b) return false;
                ++i;
                ++Prefix;
        }
        return true;
}

static UniString TrimQuotes(const UniString &Text)
{
        UniString out=CommonFuncs::Trim(Text);
        while(out.Length()>=2 && out.SubString(1,1)==L"\"" && out.SubString(out.Length(),1)==L"\"")
        {
                out=CommonFuncs::Trim(out.SubString(2,out.Length()-2));
        }
        return out;
}

static bool QueryMimeRegistryString(const UniString &MimeType,const wchar_t *ValueName,UniString &Value)
{
        if(MimeType==L"") return false;

        UniString keyPath=L"MIME\\Database\\Content Type\\"+MimeType;
        HKEY hKey=NULL;
        if(::RegOpenKeyExW(HKEY_CLASSES_ROOT,keyPath.c_bstr(),0,KEY_QUERY_VALUE,&hKey)!=ERROR_SUCCESS)
        {
                return false;
        }

        wchar_t buffer[260];
        DWORD type=0;
        DWORD size=sizeof(buffer);
        LONG rc=::RegQueryValueExW(hKey,ValueName,NULL,&type,(LPBYTE)buffer,&size);
        ::RegCloseKey(hKey);
        if(rc!=ERROR_SUCCESS || (type!=REG_SZ && type!=REG_EXPAND_SZ)) return false;

        buffer[(sizeof(buffer)/sizeof(buffer[0]))-1]=0;
        Value=UniString(buffer);
        return Value!=L"";
}

static UniString ExtensionFromMimeType(const UniString &MimeType)
{
        UniString ext;
        if(QueryMimeRegistryString(MimeType,L"Extension",ext))
        {
                ext=CommonFuncs::Trim(ext);
                if(ext!=L"")
                {
                        if(ext.SubString(1,1)!=L".") ext=L"."+ext;
                        return ext.LowerCase();
                }
        }
        return L"";
}

static bool ParseDataUriMimeType(const AnsiString &Input,UniString &MimeType)
{
        MimeType=L"";
        if(!StartsWithNoCase(Input,"data:")) return false;

        int commaPos=Input.Pos(",");
        if(commaPos<=5) return false;

        AnsiString meta=Input.SubString(6,commaPos-6);
        if(meta=="") return false;

        int semiPos=meta.Pos(";");
        AnsiString mime=(semiPos>0)?meta.SubString(1,semiPos-1):meta;
        mime=CommonFuncs::Trim(mime);
        if(mime=="") return false;

        MimeType=CommonFuncs::Trim(CommonFuncs::MBCSToUniString(mime)).LowerCase();
        return MimeType!=L"";
}

static UniString BuildDecodeFilter(const UniString &Extension,const UniString &MimeType)
{
        if(Extension==L"") return L"All Files (*.*)|*.*";

        UniString description=(MimeType!=L"")?MimeType.UpperCase()+L" Files":Extension.UpperCase()+L" Files";
        return description+L" (*"+Extension+L")|*"+Extension+L"|All Files (*.*)|*.*";
}

static UniString BuildSuggestedDecodePath(const UniString &ExistingPath,const UniString &Extension)
{
        UniString path=CommonFuncs::Trim(ExistingPath);
        if(path!=L"")
        {
                if(Extension!=L"")
                {
                        UniString currentExt=CommonFuncs::ExtractFileExt(path).LowerCase();
                        if(currentExt==L"") return path+Extension;
                }
                return path;
        }

        if(Extension!=L"") return L"decoded"+Extension;
        return L"decoded.bin";
}

__fastcall TMainWindow::TMainWindow(void)
: FHandle(NULL),FEditBase64(NULL),FEditEncodeFile(NULL),FEditDecodeFile(NULL),FBtnDecode(NULL),FBtnEncode(NULL),FBtnUriEncode(NULL),FBtnEncodeBrowse(NULL),FBtnDecodeBrowse(NULL),FBtnQuit(NULL),FWorker(NULL)
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
        FEditEncodeFile=::GetDlgItem(FHandle,IDC_FILEENCODEFULLPATH_EDIT);
        FEditDecodeFile=::GetDlgItem(FHandle,IDC_FILEDECODEFULLPATH_EDIT);
        FBtnDecode=::GetDlgItem(FHandle,IDC_DECODE_BUTTON);
        FBtnEncode=::GetDlgItem(FHandle,IDC_ENCODE_BUTTON);
        FBtnUriEncode=::GetDlgItem(FHandle,IDC_URIENCODE_BUTTON);
        FBtnEncodeBrowse=::GetDlgItem(FHandle,IDC_FILEENCODEBROWSE_BUTTON);
        FBtnDecodeBrowse=::GetDlgItem(FHandle,IDC_FILEDECODEBROWSE_BUTTON);
        FBtnQuit=::GetDlgItem(FHandle,IDC_QUIT_BUTTON);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SetBusy(bool Busy)
{
        BOOL enabled=Busy?FALSE:TRUE;
        ::EnableWindow(FEditBase64,enabled);
        ::EnableWindow(FEditEncodeFile,enabled);
        ::EnableWindow(FEditDecodeFile,enabled);
        ::EnableWindow(FBtnDecode,enabled);
        ::EnableWindow(FBtnEncode,enabled);
        ::EnableWindow(FBtnUriEncode,enabled);
        ::EnableWindow(FBtnEncodeBrowse,enabled);
        ::EnableWindow(FBtnDecodeBrowse,enabled);
        ::EnableWindow(FBtnQuit,enabled);
        ::SetCursor(::LoadCursor(NULL,Busy?IDC_WAIT:IDC_ARROW));
        ::SetWindowTextW(FHandle,Busy?L"Base64 - Processing...":L"Base64");
}
//---------------------------------------------------------------------------
UniString __fastcall TMainWindow::GetEncodeFilePath(void) const
{
        return GetWindowTextWString(FEditEncodeFile);
}
//---------------------------------------------------------------------------
UniString __fastcall TMainWindow::GetDecodeFilePath(void) const
{
        return GetWindowTextWString(FEditDecodeFile);
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
void __fastcall TMainWindow::BrowseForEncodeFile(void)
{
        TOpenDialog dlg;
        TOpenSaveDialogOptions *o=dlg.GetOptions();
        o->FileMustExist=true;
        o->PathMustExist=true;
        o->NoChangeDir=true;
        dlg.SetTitle(L"Choose a file to encode");
        dlg.SetFilter(L"All Files (*.*)|*.*");
        dlg.SetFileName(TrimQuotes(GetEncodeFilePath()));
        if(dlg.Execute(FHandle))
        {
                ::SetWindowTextW(FEditEncodeFile,dlg.GetFileName().c_bstr());
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::BrowseForDecodeFile(void)
{
        AnsiString base64=GetBase64Text();
        UniString mimeType;
        UniString extension;
        if(ParseDataUriMimeType(base64,mimeType))
        {
                extension=ExtensionFromMimeType(mimeType);
        }

        TSaveDialog dlg;
        TOpenSaveDialogOptions *o=dlg.GetOptions();
        o->PathMustExist=true;
        o->NoChangeDir=true;
        dlg.SetTitle(L"Choose where to save decoded bytes");
        dlg.SetFilter(BuildDecodeFilter(extension,mimeType));
        if(extension!=L"") dlg.SetDefaultExt(extension);
        dlg.SetFileName(BuildSuggestedDecodePath(TrimQuotes(GetDecodeFilePath()),extension));
        if(dlg.Execute(FHandle))
        {
                ::SetWindowTextW(FEditDecodeFile,dlg.GetFileName().c_bstr());
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::StartEncode(bool AsDataUri)
{
        UniString filePath=CommonFuncs::Trim(GetEncodeFilePath());
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
        UniString filePath=CommonFuncs::Trim(GetDecodeFilePath());
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
                                case IDC_FILEENCODEBROWSE_BUTTON: BrowseForEncodeFile(); return TRUE;
                                case IDC_FILEDECODEBROWSE_BUTTON: BrowseForDecodeFile(); return TRUE;
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
