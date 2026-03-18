//---------------------------------------------------------------------------
#include "EncodeDecodeThreadUnit.h"
#include "CommonFuncsUnit.h"
#include "base64.h"

#include <vector>
#include <algorithm>
#include <Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

using namespace EncodeDecodeThread;

namespace {

bool ReadWholeFile(const UniString &FileName,std::vector<unsigned char> &Out,UniString &Err)
{
        HANDLE h=::CreateFileW(FileName.c_bstr(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if(h==INVALID_HANDLE_VALUE)
        {
                Err=L"Unable to open file for reading.";
                return false;
        }

        LARGE_INTEGER li;
        if(!::GetFileSizeEx(h,&li) || li.QuadPart<0)
        {
                ::CloseHandle(h);
                Err=L"Unable to obtain file size.";
                return false;
        }

        if(li.QuadPart>0x7FFFFFFF)
        {
                ::CloseHandle(h);
                Err=L"File is too large for this build.";
                return false;
        }

        Out.resize((size_t)li.QuadPart);
        DWORD totalRead=0;
        while(totalRead<Out.size())
        {
                DWORD chunk=0;
                DWORD toRead=(DWORD)std::min<size_t>(Out.size()-totalRead,0x400000);
                if(!::ReadFile(h,&Out[totalRead],toRead,&chunk,NULL))
                {
                        ::CloseHandle(h);
                        Err=L"Failed while reading file.";
                        return false;
                }
                if(chunk==0) break;
                totalRead+=chunk;
        }
        ::CloseHandle(h);
        if(totalRead!=Out.size())
        {
                Err=L"Unexpected end of file while reading.";
                return false;
        }
        return true;
}

bool WriteWholeFile(const UniString &FileName,const std::vector<unsigned char> &Data,UniString &Err)
{
        HANDLE h=::CreateFileW(FileName.c_bstr(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(h==INVALID_HANDLE_VALUE)
        {
                Err=L"Unable to open output file for writing.";
                return false;
        }

        DWORD totalWritten=0;
        while(totalWritten<Data.size())
        {
                DWORD chunk=0;
                DWORD toWrite=(DWORD)std::min<size_t>(Data.size()-totalWritten,0x400000);
                if(!::WriteFile(h,&Data[totalWritten],toWrite,&chunk,NULL))
                {
                        ::CloseHandle(h);
                        Err=L"Failed while writing file.";
                        return false;
                }
                if(chunk==0) break;
                totalWritten+=chunk;
        }

        ::CloseHandle(h);
        if(totalWritten!=Data.size())
        {
                Err=L"Failed to write complete output file.";
                return false;
        }
        return true;
}

AnsiString NormalizeBase64(const AnsiString &Input)
{
        AnsiString Work=Input;
        int commaPos=Work.Pos(",");
        if(commaPos>0)
        {
                AnsiString left=Work.SubString(1,commaPos-1).LowerCase();
                if(left.Pos(";base64")>0)
                {
                        Work=Work.SubString(commaPos+1,Work.Length()-commaPos);
                }
        }

        AnsiString Out;
        for(int i=1;i<=Work.Length();i++)
        {
                char c=Work[i];
                if(c=='\r' || c=='\n' || c=='\t' || c==' ') continue;
                Out+=c;
        }
        return Out;
}

AnsiString MimeTypeFromFileName(const UniString &FileName)
{
        wchar_t buf[256];
        DWORD cch=sizeof(buf)/sizeof(buf[0]);
        UniString ext=CommonFuncs::ExtractFileExt(FileName).LowerCase();
        if(ext!=L"")
        {
                if(::AssocQueryStringW(0,ASSOCSTR_CONTENTTYPE,ext.c_bstr(),NULL,buf,&cch)==S_OK)
                {
                        return CommonFuncs::UniStringToMBCS(UniString(buf));
                }
        }
        return "application/octet-stream";
}

};

__fastcall TWorker::TWorker(HWND NotifyWindow,const TRequest &Request)
: FNotifyWindow(NotifyWindow),FThread(NULL),FThreadID(0),FRequest(Request)
{
}
//---------------------------------------------------------------------------
__fastcall TWorker::~TWorker(void)
{
        if(FThread)
        {
                ::CloseHandle(FThread);
                FThread=NULL;
        }
}
//---------------------------------------------------------------------------
bool __fastcall TWorker::Start(void)
{
        FThread=::CreateThread(NULL,0,&TWorker::ThreadProc,this,0,&FThreadID);
        return FThread!=NULL;
}
//---------------------------------------------------------------------------
HANDLE __fastcall TWorker::GetHandle(void) const
{
        return FThread;
}
//---------------------------------------------------------------------------
DWORD WINAPI TWorker::ThreadProc(LPVOID Param)
{
        TWorker *self=(TWorker *)Param;
        TResult *res=new TResult;
        res->Success=false;
        res->Message=L"Unknown error.";
        self->Execute(*res);
        ::PostMessageW(self->FNotifyWindow,WM_BP_THREAD_DONE,0,(LPARAM)res);
        return 0;
}
//---------------------------------------------------------------------------
void __fastcall TWorker::Execute(TResult &Result)
{
        std::vector<unsigned char> bytes;
        UniString err;

        switch(FRequest.Operation)
        {
                case opEncodeFileToBase64:
                {
                        if(!ReadWholeFile(FRequest.FileName,bytes,err))
                        {
                                Result.Message=err;
                                return;
                        }

                        size_t outLen=0;
                        char *encoded=AAbase64::base64_encode(bytes.empty()?NULL:&bytes[0],bytes.size(),&outLen);
                        if(!encoded)
                        {
                                Result.Message=L"Base64 encoding failed.";
                                return;
                        }
                        AnsiString tmp;
                        //for(size_t i=0;i<outLen;i++) tmp+=encoded[i];
						tmp.SetLength(outLen);
						tmp.c_str()[outLen]='\0';
						memcpy(tmp.c_str(),encoded,outLen);
                        Result.OutputText=tmp;
                        free(encoded);
                        Result.Success=true;
                        Result.Message=L"File encoded to base64.";
                        return;
                }
                case opEncodeFileToDataUri:
                {
                        if(!ReadWholeFile(FRequest.FileName,bytes,err))
                        {
                                Result.Message=err;
                                return;
                        }

                        size_t outLen=0;
                        char *encoded=AAbase64::base64_encode(bytes.empty()?NULL:&bytes[0],bytes.size(),&outLen);
                        if(!encoded)
                        {
                                Result.Message=L"Base64 encoding failed.";
                                return;
                        }
                        AnsiString mime=MimeTypeFromFileName(FRequest.FileName);
                        AnsiString tmp;
                        //for(size_t i=0;i<outLen;i++) tmp+=encoded[i];
						tmp.SetLength(outLen);
						tmp.c_str()[outLen]='\0';
						memcpy(tmp.c_str(),encoded,outLen);
                        Result.OutputText="data:"+mime+";base64,"+tmp;
                        free(encoded);
                        Result.Success=true;
                        Result.Message=L"File encoded to Data URI.";
                        return;
                }
                case opDecodeBase64ToFile:
                {
                        AnsiString clean=NormalizeBase64(FRequest.InputText);
                        if(clean=="")
                        {
                                Result.Message=L"No base64 text to decode.";
                                return;
                        }
                        if((clean.Length()%4)!=0)
                        {
                                Result.Message=L"Base64 text length is invalid.";
                                return;
                        }

                        size_t outLen=0;
                        unsigned char *decoded=AAbase64::base64_decode(clean.c_str(),clean.Length(),&outLen);
                        if(!decoded)
                        {
                                Result.Message=L"Base64 decoding failed.";
                                return;
                        }

                        std::vector<unsigned char> out;
                        out.assign(decoded,decoded+outLen);
                        free(decoded);

                        if(!WriteWholeFile(FRequest.FileName,out,err))
                        {
                                Result.Message=err;
                                return;
                        }

                        Result.Success=true;
                        Result.Message=L"Base64 decoded and file written.";
                        return;
                }
                default:
                        Result.Message=L"Unknown worker request.";
                        return;
        }
}
//---------------------------------------------------------------------------
