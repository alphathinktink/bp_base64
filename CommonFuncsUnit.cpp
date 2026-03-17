//---------------------------------------------------------------------------

#include <windows.h>
#pragma hdrstop

#include "CommonFuncsUnit.h"
#include <vector>
#include <string>
#include <dwmapi.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
namespace CommonFuncs {
//---------------------------------------------------------------------------
UniString __fastcall MBCSToUniString(const AnsiString &in)
{
        int l,i=in.Length();
        if(i<1)
        {
                return L"";
        }
        l=i;
        i=::MultiByteToWideChar(
                CP_UTF8,
                0,
                in.c_str(),
                l,
                NULL,
                0
        );
        if(i<=0){return L"";}
        UniString Alpha;
        Alpha.SetLength(i);
        memset(Alpha.c_bstr(),0,(i+1)*sizeof(*(Alpha.c_bstr())));
        i=::MultiByteToWideChar(
                CP_UTF8,
                0,
                in.c_str(),
                l,
                Alpha.c_bstr(),
                i+1
        );
        if(i>0)
        {
                return Alpha;
        }
        else
        {
                return in;
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall UniStringToMBCS(const UniString &in)
{
        int i=in.Length();
        if(i<1)
        {
                return "";
        }
        int j=i;
        i=::WideCharToMultiByte(
                CP_UTF8,
                0,
                in.c_bstr(),
                i,
                NULL,
                0,
                NULL,
                NULL
        );
        if(i>0)
        {
                AnsiString Res;
                Res.SetLength(i);
                memset(Res.c_str(),0,i+1);
                i=::WideCharToMultiByte(
                        CP_UTF8,
                        0,
                        in.c_bstr(),
                        j,
                        Res.c_str(),
                        i+1,
                        NULL,
                        NULL
                );
                if(i>0)
                {
                        return Res;
                }
                else
                {
                        Res=in.c_bstr();
                        return Res;
                }
        }
        else
        {
                return AnsiString(in.c_bstr());
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall EscapeXMLString(const AnsiString &in)
{
        AnsiString Res,Temp=in;
        for(int i=1;i<=Temp.Length();i++)
        {
                char c=Temp[i];
                if(c=='<')
                {
                        Res+="&lt;";
                }
                else if(c=='>')
                {
                        Res+="&gt;";
                }
                else if(c=='"')
                {
                        Res+="&#34;";
                }
                else if(c=='\'')
                {
                        Res+="&#39;";
                }
                else if(c=='&')
                {
                        Res+="&amp;";
                }
                else Res+=c;
        }
        return Res;
}
//---------------------------------------------------------------------------
AnsiString __fastcall OmNomNomSD(AnsiString &Text,const AnsiString &Delim)
{
        AnsiString Result;
        int j=Delim.Length();
        if(Text=="")return "";
        if(j<1)
        {
                Result=Text;Result.Unique();
                Text="";
                return Result;
        }
        int i=Text.Pos(Delim);
        if(i==0)
        {
                i=Text.Length()+1;j=0;
        }
        Result=Text.SubString(1,i-1);
        Text.Delete(1,i+j-1);
        return Result;
};
//---------------------------------------------------------------------------
AnsiString __fastcall OmNomNom(AnsiString &Text)
{
        return OmNomNomSD(Text,"\r\n");
};
//---------------------------------------------------------------------------
UniString __fastcall OmNomNomSD(UniString &Text,const UniString &Delim)
{
        UniString Result;
        int j=Delim.Length();
        if(Text=="")return "";
        if(j<1)
        {
                Result=Text;Result.Unique();
                Text="";
                return Result;
        }
        int i=Text.Pos(Delim);
        if(i==0)
        {
                i=Text.Length()+1;j=0;
        }
        Result=Text.SubString(1,i-1);
        Text.Delete(1,i+j-1);
        return Result;
};
//---------------------------------------------------------------------------
UniString __fastcall OmNomNom(UniString &Text)
{
        return OmNomNomSD(Text,"\r\n");
};
//---------------------------------------------------------------------------
void __fastcall ChopToLen(UniString &Text,int nlen)
{
        int l=Text.Length();
        if(l>nlen)
        {
                Text.Delete(nlen+1,l-nlen+1);
        }
}
//---------------------------------------------------------------------------
void __fastcall ChopToLen(AnsiString &Text,int nlen)
{
        int l=Text.Length();
        if(l>nlen)
        {
                Text.Delete(nlen+1,l-nlen+1);
        }
}
//---------------------------------------------------------------------------
AnsiString FileUnicodeToAnsi(const wchar_t *file)
{
        DWORD l=::GetShortPathNameW(
                file,
                NULL,
                0
        );
        if(l<1)return UniStringToMBCS(file);
        wchar_t *out=new wchar_t[l+1];
        memset(out,0,sizeof(wchar_t)*(l+1));
        DWORD RES = ::GetShortPathNameW(
                file,
                out,
                l
        );
        if(RES==ERROR_INVALID_PARAMETER)
        {
                return UniStringToMBCS(file);
        }
        AnsiString Res=out;
        delete [] out;
        if(!FileExists(Res))
        {
                return UniStringToMBCS(file);
        }
        return Res;
}
//---------------------------------------------------------------------------
AnsiString PathUnicodeToAnsi(const wchar_t *path)
{
        DWORD l=::GetShortPathNameW(
                path,
                NULL,
                0
        );
        if(l<1)return UniStringToMBCS(path);
        wchar_t *out=new wchar_t[l+1];
        memset(out,0,sizeof(wchar_t)*(l+1));
        DWORD RES = ::GetShortPathNameW(
                path,
                out,
                l
        );
        if(RES==ERROR_INVALID_PARAMETER)
        {
                return UniStringToMBCS(path);
        }
        AnsiString Res=out;
        delete [] out;
        if(!DirectoryExists(Res))
        {
                return UniStringToMBCS(path);
        }
        return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall FileShortToLongW(const UniString &Short)
{
        TPathParts PP=PathThatExists(Short);
        wchar_t path[MAX_PATH+1];
        memset(path,0,sizeof(path));
        DWORD RES=::GetLongPathName(
                PP.Exists.c_str(),
                path,
                MAX_PATH
        );
        if(RES)
        {
                UniString Path(path);
                if(PP.Phantom!=L"")
                {
                        Path+=L"\\"+PP.Phantom;
                }
                return Path;
        }
        else
        {
                return Short;
        }
}
//---------------------------------------------------------------------------
//---------------------------------
TPathParts __fastcall PathThatExists(const UniString &APath)
{
        TPathParts Res;
        Res.Phantom=APath;
        UniString Temp,Temp2;
        while(Res.Phantom!=L"")
        {
                Temp=OmNomNomSD(Res.Phantom,L"\\");
                Temp2=Temp;
                if(Res.Exists!=L""){Temp2=Res.Exists+L"\\"+Temp;}
                if(DirectoryExistsW(Temp2))
                {
                        if(Res.Exists!=L"")
                        {
                                Res.Exists+=L"\\";
                        }
                        Res.Exists+=Temp;
                }
                else
                {
                        if(Res.Phantom!=L"")
                        {
                                Res.Phantom=L"\\"+Res.Phantom;
                        }
                        Res.Phantom=Temp+Res.Phantom;
                        break;
                }
        }
        return Res;
}
//---------------------------------
//---------------------------------------------------------------------------
AnsiString __fastcall Trim(const AnsiString &S)
{
        return S.Trim();
}
//---------------------------------------------------------------------------
UniString __fastcall Trim(const UniString &S)
{
        return S.Trim();
}
//---------------------------------------------------------------------------
AnsiString __fastcall IntToHex(int value,int digits)
{
        char ii[34];
        memset(ii,0,sizeof(ii));
        itoa(value,ii,16);
        AnsiString Res=(char *)ii;
        while(Res.Length()<digits)
        {
                Res="0"+Res;
        }
        return Res.UpperCase();
}
//---------------------------------------------------------------------------
AnsiString __fastcall PointerToString(void *obj)
{
        return "0x"+IntToHex((int)obj,8);
}
//---------------------------------------------------------------------------
bool __fastcall IsURL(const AnsiString &Resource)
{
AnsiString Res=Resource;
if(Res.Pos("file")==1)return false;
if(Res.Pos("//")==1)return false;
if(Res.Pos("\\\\")==1)return false;
int cpef=Res.Pos("://");
int cpeb=Res.Pos(":\\\\");
if((cpef>=1)||(cpeb>=1))return true;
return false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExcludeTrailingBackslash(const AnsiString &In)
{
        AnsiString Res=In;
	if(Res.Length()>0)
        {
                while(Res[Res.Length()]=='\\')
                {
                        Res.Delete(Res.Length(),1);
                }
        }
        return Res;
}
//---------------------------------------------------------------------------
AnsiString __fastcall IncludeTrailingBackslash(const AnsiString &In)
{
        AnsiString Res=In;
        if(Res.Length()>0)
        {
                if(Res[Res.Length()]!='\\')
                {
                        Res+="\\";
                }
        }
        return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall ExcludeTrailingBackslash(const UniString &In)
{
        return MBCSToUniString(ExcludeTrailingBackslash(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall IncludeTrailingBackslash(const UniString &In)
{
        return MBCSToUniString(IncludeTrailingBackslash(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExtractFilePath(const AnsiString &In)
{
UniString File=MBCSToUniString(In);
//if(!FileExistsW(File))return "";
int p=File.Pos(L"/");
while(p>0)
{
        File[p]=L'\\';
        p=File.Pos(L"/");
}
UniString Res=OmNomNomSD(File,L"\\");
while(File.Pos(L"\\")>0)
{
        Res+="\\";
        Res+=OmNomNomSD(File,L"\\");
}
return UniStringToMBCS(Res);
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExtractFileDir(const AnsiString &In)
{
        return ExtractFilePath(In);
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExtractFileExt(const AnsiString &In)
{
        UniString File=MBCSToUniString(In);
        UniString Dir=MBCSToUniString(ExtractFilePath(UniStringToMBCS(File)));
        if(File!=Dir)
                if(Dir==L"")return "";
        OmNomNomSD(File,".");
        while(File.Pos(L".")>0)
        {
                OmNomNomSD(File,".");
        }
        return AnsiString(".")+UniStringToMBCS(File);
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExtractFileDrive(const AnsiString &In)
{
        UniString File=MBCSToUniString(In);
        UniString Res;
        if(File.Pos(":")==2)
        {
                Res=File.SubString(1,2);
        }
        else if(File.Pos("\\\\")==1)
        {
                UniString Temp="\\\\";
                File=File.Delete(1,2);
                int c=0;
                while(c<2 && File!="")
                {
                        Temp+=OmNomNomSD(File,L"\\");
                        Temp+="\\";
                        c++;
                }
        }
        return UniStringToMBCS(Res);
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExtractFileName(const AnsiString &In)
{
        AnsiString Res=IncludeTrailingBackslash(ExtractFilePath(In));
        int l=Res.Length();
        Res=In;
        Res.Delete(1,l);
        return Res;
}
//---------------------------------------------------------------------------
AnsiString __fastcall ChangeFileExt(const AnsiString &In,const AnsiString &Ext)
{
        UniString FileName=MBCSToUniString(In);
        UniString NExt=MBCSToUniString(Ext);
        UniString Temp;
        while(FileName.Pos(L"\\")!=0 && FileName!=L"")
        {
                if(Temp!=L""){Temp+=L"\\";}
                Temp+=OmNomNomSD(FileName,"\\");
        }
        if(Temp.Length()>0)
                Temp+=L"\\";
        Temp+=OmNomNomSD(FileName,L".");
        Temp+=NExt;
        return UniStringToMBCS(Temp);
}
//---------------------------------------------------------------------------
UniString __fastcall ExtractFilePath(const UniString &In)
{
	return MBCSToUniString(ExtractFilePath(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall ExtractFileDir(const UniString &In)
{
	return MBCSToUniString(ExtractFileDir(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall ExtractFileExt(const UniString &In)
{
	return MBCSToUniString(ExtractFileExt(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall ExtractFileDrive(const UniString &In)
{
	return MBCSToUniString(ExtractFileDrive(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall ExtractFileName(const UniString &In)
{
	return MBCSToUniString(ExtractFileName(UniStringToMBCS(In)));
}
//---------------------------------------------------------------------------
UniString __fastcall ChangeFileExt(const UniString &In,const UniString &Ext)
{
	return MBCSToUniString(ChangeFileExt(UniStringToMBCS(In),UniStringToMBCS(Ext)));
}
//---------------------------------------------------------------------------
AnsiString __fastcall GetCurrentDir(void)
{
        AnsiString Res;
        wchar_t ii[1];
        ii[0]=L'\0';
        wchar_t *u=ii;
        DWORD LEN=0;
        LEN=::GetCurrentDirectoryW(
                LEN,
                u
        );
        if(LEN>0)
        {
                UniString Temp;
                Temp.SetLength(LEN);
                u=Temp.c_bstr();
                LEN=GetCurrentDirectoryW(
                        LEN,
                        u
                );
                if(LEN>0)
                {
                        Res=UniStringToMBCS(Temp);
                }
        }
        return Res;
}
//---------------------------------------------------------------------------
bool __fastcall SetCurrentDir(const AnsiString &Dir)
{
        return SetCurrentDirW(MBCSToUniString(Dir));
}
//---------------------------------------------------------------------------
AnsiString __fastcall ExpandFileName(const AnsiString &FileName)
{
        //GetFullPathName
        UniString WFile=MBCSToUniString(FileName);
        AnsiString Res;
        wchar_t *y=WFile.c_bstr();
        wchar_t ii[1];
        ii[0]=L'\0';
        wchar_t *u=ii;
        wchar_t *fp;
        DWORD LEN=::GetFullPathNameW(
                y,
                0,
                u,
                &fp
        );
        if(LEN>0)
        {
                UniString Temp;
                Temp.SetLength(LEN);
                u=Temp.c_bstr();
                LEN=::GetFullPathNameW(
                        y,
                        LEN,
                        u,
                        &fp
                );
                if(LEN>0)
                {
                        Res=UniStringToMBCS(Temp);
                }
        }
        return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall ExpandFileName(const UniString &FileName)
{
        return MBCSToUniString(ExpandFileName(UniStringToMBCS(FileName)));
}
//---------------------------------------------------------------------------
bool __fastcall DeleteFileW(const UniString &File)
{
        return ::DeleteFileW(File.c_bstr())!=FALSE;
}
//---------------------------------------------------------------------------
bool __fastcall CopyFileW(const UniString &Src,const UniString &Dest)
{
	BOOL SUC;
	SUC=::CopyFileW(Src.c_str(),Dest.c_str(),FALSE);
	return SUC!=0;
}
//---------------------------------------------------------------------------
bool __fastcall RemoveDirW(const UniString &Directory)
{
        BOOL SUC=::RemoveDirectory(
                Directory.c_bstr()
        );
        return SUC!=FALSE?true:false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall ReadClipboardTextA(void)
{
        AnsiString Res;
        if(::OpenClipboard(NULL))
        {
                if(::IsClipboardFormatAvailable(CF_TEXT))
                {
                        HANDLE hMem=::GetClipboardData(CF_TEXT);
                        if(hMem)
                        {
                                char *data=(char *)::GlobalLock(hMem);
                                if(data)
                                {
                                        Res=data;
                                        ::GlobalUnlock(hMem);
                                }
                        }
                }
                ::CloseClipboard();
        }
        return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall ReadClipboardTextW(void)
{
        UniString Res;
        if(::OpenClipboard(NULL))
        {
                if(::IsClipboardFormatAvailable(CF_UNICODETEXT))
                {
                        HANDLE hMem=::GetClipboardData(CF_UNICODETEXT);
                        if(hMem)
                        {
                                wchar_t *data=(wchar_t *)::GlobalLock(hMem);
                                if(data)
                                {
                                        Res=data;
                                        ::GlobalUnlock(hMem);
                                }
                        }
                }
                ::CloseClipboard();
        }
        return Res;
}
//---------------------------------------------------------------------------
void __fastcall WriteClipboardUnicodeData(const wchar_t *pUnicodeData)
{
	if (::OpenClipboard(NULL))
	{
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (wcslen(pUnicodeData) + 1) * 2) ;
		wchar_t* pClipboardData = (wchar_t*) GlobalLock(hGlobal) ;
		wcscpy(pClipboardData, pUnicodeData) ;
		::GlobalUnlock(hGlobal);
                ::EmptyClipboard();
		::SetClipboardData(CF_UNICODETEXT, hGlobal) ;
		::CloseClipboard() ;
	}
}
//---------------------------------------------------------------------------
AnsiString __fastcall uriEncode(const AnsiString &Text)
{
AnsiString Res,CText=Text;
char c;
for(int i=1;i<=CText.Length();i++)
{
        c=CText[i];
        if((c<'0' || (c>'9' && c<'A') || (c>'Z' && c<'a') || c>'z') && c!='$' && c!='-' && c!='_' && c!='.' && c!='+' && c!='!' && c!='*' && c!='\'' && c!='(' && c!=')' && c!=',' && c!='?' && c!='/' && c!='~')
        {
                __int64 narf=(unsigned char)(c);
                Res+="%"+IntToHex(narf,2);
        }
        else
        {
                Res+=c;
        }
}
return Res;
}
//---------------------------------------------------------------------------
static inline int __fastcall UriHexNibble(char ch)
{
	if(ch>='0'&&ch<='9')return ch-'0';
	if(ch>='A'&&ch<='F')return 10+(ch-'A');
	if(ch>='a'&&ch<='f')return 10+(ch-'a');
	return -1;
}
//---------------------------------------------------------------------------
AnsiString __fastcall uriDecode(const AnsiString &In,bool PlusAsSpace,bool *HadError)
{
	if(HadError)*HadError=false;

	const int len=In.Length();
	if(len<=0)return AnsiString("");

	const char *src=In.c_str();
	std::string out;
	out.reserve((size_t)len);

	for(int i=0;i<len;)
	{
		unsigned char c=(unsigned char)src[i];

		if(c=='%')
		{
			if(i+2<len)
			{
				int hi=UriHexNibble(src[i+1]);
				int lo=UriHexNibble(src[i+2]);
				if(hi>=0&&lo>=0)
				{
					unsigned char b=(unsigned char)((hi<<4)|lo);
					out.push_back((char)b);
					i+=3;
					continue;
				}
			}
			if(HadError)*HadError=true;
			out.push_back('%');
			++i;
			continue;
		}
		else if(c=='+')
		{
			out.push_back(PlusAsSpace?' ':'+');
			++i;
			continue;
		}
		else
		{
			out.push_back((char)c);
			++i;
			continue;
		}
	}

	return AnsiString(out.c_str());
}
//---------------------------------------------------------------------------
UniString __fastcall TransGLEM(DWORD value)
{
LPVOID lpMsgBuf;
FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,value,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),/* Default language*/(wchar_t *) &lpMsgBuf,0,NULL);
UniString EM=((wchar_t *)lpMsgBuf);
return EM;
};
//---------------------------------------------------------------------------
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#endif
bool __fastcall FileExists(const AnsiString &File)
{
        DWORD RES=::GetFileAttributesA(File.c_str());
        return (RES != INVALID_FILE_ATTRIBUTES && !(RES & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
bool __fastcall FileExistsW(const UniString &File)
{
        DWORD RES=::GetFileAttributesW(File.c_bstr());
        return (RES != INVALID_FILE_ATTRIBUTES && !(RES & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
bool __fastcall DirectoryExists(const AnsiString &File)
{
        DWORD RES=::GetFileAttributesA(File.c_str());
        return (RES != INVALID_FILE_ATTRIBUTES && (RES & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
bool __fastcall DirectoryExistsW(const UniString &File)
{
        DWORD RES=::GetFileAttributesW(File.c_bstr());
        return (RES != INVALID_FILE_ATTRIBUTES && (RES & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
int __fastcall FileCreateW(const UniString &FileName)
{
HANDLE FileHandle=::CreateFileW(
        FileName.c_bstr(),
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
);
if(FileHandle==INVALID_HANDLE_VALUE)return -1;else return (int)FileHandle;
}
//---------------------------------------------------------------------------
int __fastcall FileCreate(const AnsiString &FileName)
{
        return FileCreateW(MBCSToUniString(FileName));
}
//---------------------------------------------------------------------------
int __fastcall FileOpenW(const UniString &FileName,int Mode)
{
DWORD DA=GENERIC_READ|GENERIC_WRITE;
DWORD SM=FILE_SHARE_READ|FILE_SHARE_WRITE;
DWORD CD=CREATE_ALWAYS;
if(Mode!=0xFFFF)
{
        CD=OPEN_EXISTING;
        int HB=Mode&0xF0;
        int LB=Mode&0x0F;
        switch(LB)
        {
                case 0x0:
                DA=GENERIC_READ;
                break;
                case 0x1:
                DA=GENERIC_WRITE;
                break;
        };
        switch(HB)
        {
                case 0x10:
                SM=0;                
                break;
                case 0x20:
                SM=FILE_SHARE_READ;
                break;
                case 0x30:
                SM=FILE_SHARE_WRITE;
                break;
                case 0x40:
                SM=FILE_SHARE_WRITE|FILE_SHARE_READ;
                break;
        };
}
HANDLE FileHandle=::CreateFileW(
        FileName.c_bstr(),
        DA,
        SM,
        NULL,
        CD,
        FILE_ATTRIBUTE_NORMAL,
        NULL
);
if(FileHandle==INVALID_HANDLE_VALUE)return -1;else return (int)FileHandle;
}
//---------------------------------------------------------------------------
int __fastcall FileOpen(const AnsiString &FileName,int Mode)
{
        return FileOpenW(MBCSToUniString(FileName),Mode);
}
//---------------------------------------------------------------------------
int __fastcall FileRead(int Handle, void *Buffer, int Count)
{
        if(Count<0)return -1;
        DWORD ToWrite=Count,Written;
        BOOL RES=::ReadFile(
                (HANDLE)Handle,
                Buffer,
                ToWrite,
                &Written,
                NULL
        );
        if(RES==0)
        {
                return -1;
        }
        return Written;
}
//---------------------------------------------------------------------------
int __fastcall FileWrite(int Handle, const void *Buffer, int Count)
{
        if(Count<0)return -1;
        DWORD ToWrite=Count,Written;
        BOOL RES=::WriteFile(
                (HANDLE)Handle,
                Buffer,
                ToWrite,
                &Written,
                NULL
        );
        if(RES==0)
        {
                return -1;
        }
        return Written;
}
//---------------------------------------------------------------------------
int __fastcall FileSeek(int Handle, int Offset, int Origin)
{
        LONG Distance=Offset;
        DWORD MoveMethod;
        //LONG High=0;
        switch(Origin)
        {
                case 0://From beg
                MoveMethod=FILE_BEGIN;
                break;
                case 1://From cur
                MoveMethod=FILE_CURRENT;
                break;
                case 2://From end
                MoveMethod=FILE_END;
                break;
        }
        return ::SetFilePointer(
                (HANDLE)Handle,
                Distance,
                NULL,
                MoveMethod
        );
}
//---------------------------------------------------------------------------
void __fastcall FileClose(int Handle)
{
        ::CloseHandle((HANDLE)Handle);
}
//---------------------------------------------------------------------------
bool __fastcall ForceDirectories(const AnsiString &ADir)
{
if(ADir.Length()<1)return false;
AnsiString Dir=ADir;
int p=Dir.Pos("/");
while(p>0)
{
        Dir[p]='\\';
        p=Dir.Pos("/");
}
if(Dir[Dir.Length()]=='\\')
{
        Dir=Dir.Delete(Dir.Length(),1);
}
if(Dir.Length()<1)return false;
if(Dir.Pos(":")<1)return false;
AnsiString Temp=Dir;
AnsiString Temp2=OmNomNomSD(Temp,"\\");
do
{
        AnsiString Narf=Temp2;
        if(!DirectoryExists(Narf))
        {
                BOOL RES=::CreateDirectoryA(
                        Narf.c_str(),
                        NULL
                );
                if(RES==0)
                {
                        return false;
                }
        }
        if(Temp=="")break;
        Temp2+=AnsiString("\\")+OmNomNomSD(Temp,"\\");
}
while(true);
return true;
}
//---------------------------------------------------------------------------
bool __fastcall ForceDirectoriesW(const UniString &ADir)
{
if(ADir.Length()<1)return false;
UniString Dir=ADir;
int p=Dir.Pos(L"/");
while(p>0)
{
        Dir[p]=L'\\';
        p=Dir.Pos("/");
}
if(Dir[Dir.Length()]==L'\\')
{
        Dir=Dir.Delete(Dir.Length(),1);
}
if(Dir.Length()<1)return false;
if(Dir.Pos(L":")<1)return false;
UniString Temp=Dir;
UniString Temp2=OmNomNomSD(Temp,L"\\");
do
{
        UniString Narf=Temp2;
        if(!DirectoryExistsW(Narf))
        {
                BOOL RES=::CreateDirectoryW(
                        Narf.c_bstr(),
                        NULL
                );
                if(RES==0)
                {
                        return false;
                }
        }
        if(Temp=="")break;
        Temp2+="\\"+OmNomNomSD(Temp,L"\\");
}
while(true);
return true;
}
//---------------------------------------------------------------------------
UniString __fastcall GetCurrentDirW(void)
{
wchar_t curdir[MAX_PATH+1];
memset(curdir,0,sizeof(curdir));
DWORD RES=::GetCurrentDirectoryW(
        MAX_PATH+1,
        curdir
);
if(RES>0)
{
        return UniString(curdir);
}
return L"";
}
//---------------------------------------------------------------------------
bool __fastcall SetCurrentDirW(const UniString &Dir)
{
return (::SetCurrentDirectoryW(Dir.c_bstr())!=0);
}
//---------------------------------------------------------------------------
typedef HRESULT (WINAPI *_SHGetFolderPathA)(HWND hwnd,int csidl,HANDLE hToken,DWORD dwFlags,LPSTR pszPath);
typedef HRESULT (WINAPI *_SHGetFolderPathW)(HWND hwnd,int csidl,HANDLE hToken,DWORD dwFlags,LPWSTR pszPath);
//---------------------------------------------------------------------------
AnsiString __fastcall GetSpecialFolderPathA(int CSIDL)
{
	AnsiString Res;
	char PATH[MAX_PATH+1];
	HMODULE hLib=::LoadLibraryA("shfolder.dll");
	if(hLib)
	{
		FARPROC Proc=::GetProcAddress(hLib,"SHGetFolderPathA");
		if(Proc)
		{
			_SHGetFolderPathA SHGetFolderPathA=(_SHGetFolderPathA)Proc;
			memset(PATH,0,(sizeof(char))*(MAX_PATH+1));
			HRESULT RES=SHGetFolderPathA(
				NULL,
				CSIDL,
				NULL,
				CSIDL_FLAG_CREATE,
				PATH
			);
			if(SUCCEEDED(RES))
			{
				Res=(char *)PATH;
			}
		}
		::FreeLibrary(hLib);
	}
	return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall GetSpecialFolderPathW(int CSIDL)
{
	UniString Res;
	wchar_t PATH[MAX_PATH+1];
	HMODULE hLib=::LoadLibraryA("shfolder.dll");
	if(hLib)
	{
		FARPROC Proc=::GetProcAddress(hLib,"SHGetFolderPathW");
		if(Proc)
		{
			_SHGetFolderPathW SHGetFolderPathW=(_SHGetFolderPathW)Proc;
			memset(PATH,0,(sizeof(wchar_t))*(MAX_PATH+1));
			HRESULT RES=SHGetFolderPathW(
				NULL,
				CSIDL,
				NULL,
				CSIDL_FLAG_CREATE,
				PATH
			);
			if(SUCCEEDED(RES))
			{
				Res=(wchar_t *)PATH;
			}
		}
		::FreeLibrary(hLib);
	}
	if(Res==L"")Res=MBCSToUniString(GetSpecialFolderPathA(CSIDL));
	return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall GetModulePathW_Wrapper(void)
{
	OSVERSIONINFOA vi={0};
	vi.dwOSVersionInfoSize=sizeof(vi);
	::GetVersionExA(&vi);

	if(vi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) // Win9x
	{
		char aPath[MAX_PATH]={0};
		if(::GetModuleFileNameA(NULL,aPath,MAX_PATH)>0)
		{
			return MBCSToUniString(aPath);
		}
		return L"";
	}
	else // NT-class
	{
		wchar_t wPath[MAX_PATH]={0};
		if(::GetModuleFileNameW(NULL,wPath,MAX_PATH)>0)
		{
			return (wchar_t *)wPath;
		}
		return L"";
	}
}
//------------------------------------------------------------------------------------------
UniString __fastcall GetExeDirectoryW(void)
{
	UniString p=GetModulePathW_Wrapper();
	if(p==L"") return L"";

	// Find last slash using 1-based indexing
	int slashPos=0; // 1..Length if found, else 0
	for(int i=p.Length(); i>=1; --i)
	{
		wchar_t c=p[i]; // 1-based char access
		if(c==L'\\' || c==L'/')
		{
			slashPos=i;
			break;
		}
	}

	if(slashPos>0)
	{
		// SubString(start,count) → include the slash
		return p.SubString(1,slashPos);
	}
	return L"";
}
//------------------------------------------------------------------------------------------
bool __fastcall IsDirectoryWritableW(const UniString &dir)
{
	if(dir==L"") return false;
	UniString test=IncludeTrailingBackslash(dir)+L".__wtest$.tmp";
	HANDLE h=::CreateFileW(
		test.c_bstr(),
		GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,
		NULL
	);
	if(h==INVALID_HANDLE_VALUE) return false;
	::CloseHandle(h);
	// file is auto-deleted on close because of DELETE_ON_CLOSE
	return true;
}
//------------------------------------------------------------------------------------------
int __fastcall FindMatchingFileW(TSearchRecW &F)
{
        int Result=0;
        //TFileTime LocalFileTime;
        while((F.FindData.dwFileAttributes&F.ExcludeAttr)!=0)
        {
                if(!::FindNextFileW((HANDLE)(F.FindHandle),&(F.FindData)))
                {
                        Result=::GetLastError();
                        break;
                }
        }
        //FileTimeToLocalFileTime(&(F.FindData.ftLastWriteTime),&LocalFileTime);
        //FileTimeToDosDateTime(&LocalFileTime,&(((LongRec*)(&F.Time))->Hi),&(((LongRec*)(&F.Time))->Lo));
        F.Size=F.FindData.nFileSizeLow;
        F.Attr=F.FindData.dwFileAttributes;
        F.Name=F.FindData.cFileName;
        return Result;
}
//---------------------------------------------------------------------------
int __fastcall FindFirstW(const UniString &Path,int Attr,TSearchRecW &F)
{
int Res;
const int faSpecial = faHidden | faSysFile | faVolumeID | faDirectory;
F.ExcludeAttr=(~Attr)&faSpecial;
F.FindHandle=(unsigned int)::FindFirstFileW(Path.c_bstr(), &(F.FindData));
if(F.FindHandle != (unsigned int)INVALID_HANDLE_VALUE)
{
        Res=FindMatchingFileW(F);
        if(Res!=0)
        {
                FindCloseW(F);
        }
}
else Res=::GetLastError();
return Res;
}
//---------------------------------------------------------------------------
int __fastcall FindNextW(TSearchRecW &F)
{
int Res;
if(::FindNextFileW((HANDLE)F.FindHandle,&(F.FindData)))
{
        Res=FindMatchingFileW(F);
}
else
{
        Res=::GetLastError();
}
return Res;
}
//---------------------------------------------------------------------------
void __fastcall FindCloseW(TSearchRecW &F)
{
if(F.FindHandle!=(unsigned)INVALID_HANDLE_VALUE)
{
        ::FindClose((HANDLE)(F.FindHandle));
        F.FindHandle=(unsigned)INVALID_HANDLE_VALUE;
}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifndef _SHLOBJ_H_
typedef struct _SHITEMID        // mkid
{
    USHORT      cb;             // Size of the ID (including cb itself)
    BYTE        abID[1];        // The item ID (variable length)
} SHITEMID;
typedef UNALIGNED SHITEMID *LPSHITEMID;
typedef const UNALIGNED SHITEMID *LPCSHITEMID;
typedef struct _ITEMIDLIST      // idl
{
    SHITEMID    mkid;
} ITEMIDLIST;
typedef UNALIGNED ITEMIDLIST * LPITEMIDLIST;
typedef const UNALIGNED ITEMIDLIST * LPCITEMIDLIST;
#endif


//------
typedef HRESULT (WINAPI *SHGetSpecialFolderLocationProc)(HWND hwnd, int csidl, LPITEMIDLIST *ppidl);
#define CSIDL_DESKTOP                   0x0000        // <desktop>
#define CSIDL_INTERNET                  0x0001        // Internet Explorer (icon on desktop)
#define CSIDL_PROGRAMS                  0x0002        // Start Menu\Programs
#define CSIDL_CONTROLS                  0x0003        // My Computer\Control Panel
#define CSIDL_PRINTERS                  0x0004        // My Computer\Printers
#define CSIDL_PERSONAL                  0x0005        // My Documents
#define CSIDL_FAVORITES                 0x0006        // <user name>\Favorites
#define CSIDL_STARTUP                   0x0007        // Start Menu\Programs\Startup
#define CSIDL_RECENT                    0x0008        // <user name>\Recent
#define CSIDL_SENDTO                    0x0009        // <user name>\SendTo
#define CSIDL_BITBUCKET                 0x000a        // <desktop>\Recycle Bin
#define CSIDL_STARTMENU                 0x000b        // <user name>\Start Menu
#define CSIDL_DESKTOPDIRECTORY          0x0010        // <user name>\Desktop
#define CSIDL_DRIVES                    0x0011        // My Computer
#define CSIDL_NETWORK                   0x0012        // Network Neighborhood
#define CSIDL_NETHOOD                   0x0013        // <user name>\nethood
#define CSIDL_FONTS                     0x0014        // windows\fonts
#define CSIDL_TEMPLATES                 0x0015
#define CSIDL_COMMON_STARTMENU          0x0016        // All Users\Start Menu
#ifndef CSIDL_COMMON_PROGRAMS
#define CSIDL_COMMON_PROGRAMS           0X0017        // All Users\Programs
#endif//CSIDL_COMMON_PROGRAMS
#define CSIDL_COMMON_STARTUP            0x0018        // All Users\Startup
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019        // All Users\Desktop
#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
#define CSIDL_PRINTHOOD                 0x001b        // <user name>\PrintHood
#define CSIDL_LOCAL_APPDATA             0x001c        // <user name>\Local Settings\Applicaiton Data (non roaming)
#define CSIDL_ALTSTARTUP                0x001d        // non localized startup
#define CSIDL_COMMON_ALTSTARTUP         0x001e        // non localized common startup
#define CSIDL_COMMON_FAVORITES          0x001f
#define CSIDL_INTERNET_CACHE            0x0020
#define CSIDL_COOKIES                   0x0021
#define CSIDL_HISTORY                   0x0022
#define CSIDL_COMMON_APPDATA            0x0023        // All Users\Application Data
#define CSIDL_WINDOWS                   0x0024        // GetWindowsDirectory()
#define CSIDL_SYSTEM                    0x0025        // GetSystemDirectory()
#define CSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
#define CSIDL_MYPICTURES                0x0027        // C:\Program Files\My Pictures
#define CSIDL_PROFILE                   0x0028        // USERPROFILE
#define CSIDL_SYSTEMX86                 0x0029        // x86 system directory on RISC
#define CSIDL_PROGRAM_FILESX86          0x002a        // x86 C:\Program Files on RISC
#define CSIDL_PROGRAM_FILES_COMMON      0x002b        // C:\Program Files\Common
#define CSIDL_PROGRAM_FILES_COMMONX86   0x002c        // x86 Program Files\Common on RISC
#define CSIDL_COMMON_TEMPLATES          0x002d        // All Users\Templates
#define CSIDL_COMMON_DOCUMENTS          0x002e        // All Users\Documents
#define CSIDL_COMMON_ADMINTOOLS         0x002f        // All Users\Start Menu\Programs\Administrative Tools
#define CSIDL_ADMINTOOLS                0x0030        // <user name>\Start Menu\Programs\Administrative Tools
#define CSIDL_CONNECTIONS               0x0031        // Network and Dial-up Connections

#define CSIDL_FLAG_CREATE               0x8000        // combine with CSIDL_ value to force folder creation in SHGetFolderPath()
#define CSIDL_FLAG_DONT_VERIFY          0x4000        // combine with CSIDL_ value to return an unverified folder path
#ifndef CSIDL_FLAG_MASK
#define CSIDL_FLAG_MASK                 0xFF00        // mask for all possible flag values
#endif//CSIDL_FLAG_MASK
typedef BOOL (WINAPI *SHGetPathFromIDListWProc)(LPCITEMIDLIST pidl, LPWSTR pszPath);
typedef HRESULT (WINAPI *SHGetMallocProc)(LPMALLOC * ppMalloc);
//------


static UniString LogDirectory;
//---------------------------------------------------------------------------
UniString __fastcall GetLogDirectoryEx(void)
{
	if(LogDirectory.Length()>0)return LogDirectory;
	UniString Res;
	ITEMIDLIST* pidl;
	wchar_t lpPath[MAX_PATH+1];
	HMODULE hmod=LoadLibraryA("Shell32.dll");
	FARPROC Proc1=GetProcAddress(hmod,"SHGetSpecialFolderLocation");
	FARPROC Proc2=GetProcAddress(hmod,"SHGetPathFromIDListW");
	FARPROC Proc3=GetProcAddress(hmod,"SHGetMalloc");
	SHGetSpecialFolderLocationProc SHGetSpecialFolderLocation=(SHGetSpecialFolderLocationProc)Proc1;
	SHGetPathFromIDListWProc SHGetPathFromIDListW=(SHGetPathFromIDListWProc)Proc2;
	SHGetMallocProc SHGetMalloc=(SHGetMallocProc)Proc3;
	HRESULT hRes = SHGetSpecialFolderLocation( NULL, CSIDL_PERSONAL, &pidl );
	if(hRes==NOERROR)
	{
			SHGetPathFromIDListW( pidl, lpPath );
			Res=lpPath;
	}
	IMalloc* palloc;
	hRes = SHGetMalloc(&palloc);
	if(hRes==NOERROR)
	{
			palloc->Free( (void*)pidl );
			palloc->Release();
	}
	FreeLibrary(hmod);
	if(Res.Length()>0)
	{
			if(Res[Res.Length()]!=L'\\')
			{
					Res+=L"\\";
			}
	}
	LogDirectory=Res;
	return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall GetLogDirectory(void)
{
        UniString Narf=GetLogDirectoryEx();
        if(Narf.Length()==0)return L"";
        if(Narf[Narf.Length()]!=L'\\')
        {
                Narf+=L"\\";
        }
        Narf+=L"BogProg support for Opus\\";
        wchar_t EXE[MAX_PATH+1];
        memset(EXE,0,sizeof(EXE));
        DWORD EXELEN=::GetModuleFileNameW(
                NULL,
                EXE,
                MAX_PATH
        );
        if(EXELEN>5)
        {
                UniString WinampFileName(EXE);
                for(int i=1;i<=WinampFileName.Length();i++)
                {
                        if(WinampFileName[i]==L'/')
                        {
                                WinampFileName[i]=L'\\';
                        }
                }
                AnsiString Temp=UniStringToMBCS(WinampFileName);
                AnsiString Temp2;
                while(Temp.Pos("\\")>0)
                {
                        Temp2=OmNomNomSD(Temp,"\\");
                }
                if(Temp2!="" && Temp2.Pos(":")==0)
                {
                        Narf+=MBCSToUniString(Temp2);
                        return Narf;
                }
        }
        return L"";
}
//---------------------------------------------------------------------------
UniString __fastcall GetWndTxt(HWND hwnd)
{
        UniString Res;
        int c=::GetWindowTextLengthW(hwnd);
        if(c>0)
        {
                c++;
                Res.SetLength(c);
                c=::GetWindowTextW(hwnd,Res.c_bstr(),c);
                if(c==0)
                {
                        Res=L"";
                }
        }
        return Res;
}
//---------------------------------------------------------------------------
void __fastcall SetCtrlCheckedState(HWND hwndDlg,int ID,bool Checked)
{
        ::CheckDlgButton(hwndDlg,ID,Checked?BST_CHECKED:BST_UNCHECKED);
}
//---------------------------------------------------------------------------
bool __fastcall GetCtrlCheckedState(HWND hwndDlg,int ID)
{
        UINT RES=::IsDlgButtonChecked(hwndDlg,ID);
        return ((RES&BST_CHECKED)!=0);
}
//---------------------------------------------------------------------------
void __fastcall SetComboBoxSel(HWND hwnd,int idx)
{
        ::SendMessage(hwnd,CB_SETCURSEL,(WPARAM)idx,0);
}
//---------------------------------------------------------------------------
int __fastcall GetComboBoxSel(HWND hwnd)
{
        LRESULT RES=::SendMessage(hwnd,CB_GETCURSEL,0,0);
        return *((int *)(&RES));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Helper: case-insensitive string compare for first n chars
static int __fastcall wcsnicmp_local(const wchar_t* a, const wchar_t* b, size_t n)
{
    for(size_t i=0;i<n;i++)
    {
        wchar_t ca = towlower(a[i]);
        wchar_t cb = towlower(b[i]);
        if(ca!=cb || ca==0 || cb==0) return (int)(ca - cb);
    }
    return 0;
}
// Root length of path: "C:\" => 3, UNC "\\server\share\" => len of that root, "\" => 1, else 0
static size_t __fastcall RootLen(const wchar_t *s)
{
    if(!s) return 0;
    // UNC?
    if(s[0]==L'\\' && s[1]==L'\\')
    {
        // \\server\share\...
        const wchar_t *p=s+2;
        while(*p && *p!=L'\\') ++p;            // server
        if(*p!=L'\\') return 2;                // just "\\server"
        ++p;
        while(*p && *p!=L'\\') ++p;            // share
        return (size_t)(*p==L'\\' ? (p-s+1) : (p-s));
    }
    // Drive-root? "C:\" or "C:"
    if(s[0] && s[1]==L':' )
    {
        if(s[2]==L'\\' || s[2]==L'/') return 3;
        return 2;
    }
    // Leading slash root
    if(s[0]==L'\\' || s[0]==L'/') return 1;
    return 0;
}
// Trim trailing '\' or '/' except keep a single root slash
//---------------------------------------------------------------------------
UniString __fastcall TrimTrailingSlashPreserveRoot(const UniString &p)
{
    const wchar_t *s=p.c_str();
    if(!s) return p;
    size_t n=wcslen(s);
    if(n==0) return p;
    size_t rl=RootLen(s);
    while(n>rl && (s[n-1]==L'\\' || s[n-1]==L'/')) --n;
    return UniString(UniString(p).SubString(1,(int)n));
}
//---------------------------------------------------------------------------
// Public helpers
//---------------------------------------------------------------------------
bool __fastcall IsAbsolutePathW(const UniString &Path)
{
    const wchar_t *p=Path.c_str();
    if(!p || !p[0]) return false;
    // UNC
    if(p[0]==L'\\' && p[1]==L'\\') return true;
    // Drive letter
    if(p[1]==L':') return true;
    return false;
}
//---------------------------------------------------------------------------
UniString __fastcall NormalizeSeparatorsW(const UniString &Path)
{
    const wchar_t *s=Path.c_str();
    if(!s) return Path;
    UniString out=Path;
    wchar_t *d=out.c_str();
    for(size_t i=0, n=wcslen(d); i<n; ++i)
        if(d[i]==L'/') d[i]=L'\\';
    return out;
}
//---------------------------------------------------------------------------
// Collapse "." and ".." segments; preserve root; no filesystem access
UniString __fastcall CollapseDotsW(const UniString &Path)
{
    UniString norm = NormalizeSeparatorsW(Path);
    const wchar_t *s = norm.c_str();
    if(!s) return norm;

    size_t rl = RootLen(s);
    std::vector<UniString> segs;
    // collect segments
    size_t i=rl, start=rl, len=wcslen(s);
    while(i<=len)
    {
        if(i==len || s[i]==L'\\')
        {
            if(i>start)
            {
                UniString seg = UniString(UniString(norm).SubString((int)start+1,(int)(i-start)));
                if(seg==L".")
                {
                    // skip
                }
                else if(seg==L"..")
                {
                    if(!segs.empty()) segs.pop_back(); // back up a segment
                    else
                    {
                        // keep leading ".." for relative paths with no root
                        if(rl==0) segs.push_back(seg);
                        // else ignore for rooted paths
                    }
                }
                else segs.push_back(seg);
            }
            start=i+1;
        }
        ++i;
    }
    // rebuild
    UniString out;
    if(rl>0)
    {
        out = UniString(norm.SubString(1,(int)rl)); // copy root verbatim
        // Normalize drive letter to upper if like "c:"
        if(rl>=2 && out.c_str()[1]==L':')
        {
            UniString tmp=out;
            wchar_t *p=tmp.c_str();
            p[0]=(wchar_t)towupper(p[0]);
            out=tmp;
        }
    }
    for(size_t k=0;k<segs.size();k++)
    {
        if(!(out.IsEmpty()) && out.c_str()[wcslen(out.c_str())-1]!=L'\\') out+=L"\\";
        out+=segs[k];
    }
    return TrimTrailingSlashPreserveRoot(out);
}
//---------------------------------------------------------------------------
UniString __fastcall NormalizePathTextW(const UniString &Path)
{
    return CollapseDotsW(Path);
}
//---------------------------------------------------------------------------
// Make absolute path from Path against BaseDir (both normalized text-wise)
// If Path is absolute, just normalize it; if relative, combine and normalize.
UniString __fastcall NormalizeAgainstBaseAbsW(const UniString &Path, const UniString &BaseDir)
{
    if(IsAbsolutePathW(Path))
        return NormalizePathTextW(Path);

    // Combine: BaseDir + '\' + Path, then collapse
    UniString base = NormalizePathTextW(BaseDir);
    if(base.IsEmpty()) return NormalizePathTextW(Path);
    // ensure base ends with '\'
    const wchar_t *b=base.c_str();
    if(b[wcslen(b)-1]!=L'\\') base+=L"\\";
    return NormalizePathTextW(base + Path);
}
//---------------------------------------------------------------------------
// true if Candidate is the same directory or a child of BaseDir
bool __fastcall IsSameOrUnderW(const UniString &BaseDir, const UniString &Candidate, bool *IsSameOpt)
{
    UniString baseAbs = NormalizeAgainstBaseAbsW(BaseDir, L"");
    UniString candAbs = NormalizeAgainstBaseAbsW(Candidate, BaseDir);

    baseAbs = TrimTrailingSlashPreserveRoot(baseAbs);
    candAbs = TrimTrailingSlashPreserveRoot(candAbs);

    const wchar_t *b=baseAbs.c_str();
    const wchar_t *c=candAbs.c_str();
    if(!b || !c) { if(IsSameOpt) *IsSameOpt=false; return false; }

    size_t bl=wcslen(b);
    size_t cl=wcslen(c);
    if(bl==0 || cl<bl) { if(IsSameOpt) *IsSameOpt=false; return false; }

    if(wcsnicmp_local(b,c,bl)!=0) { if(IsSameOpt) *IsSameOpt=false; return false; }

    if(cl==bl) { if(IsSameOpt) *IsSameOpt=true; return true; }
    // must be followed by a separator to be "under"
    if(c[bl]==L'\\') { if(IsSameOpt) *IsSameOpt=false; return true; }
    if(IsSameOpt) *IsSameOpt=false;
    return false;
}
//---------------------------------------------------------------------------
// If target is the same as or under base, OutRelative = "." or "sub\...\target", returns true.
// Otherwise returns false and leaves OutRelative unchanged.
bool __fastcall TryMakeRelativeToW(const UniString &BaseDir, const UniString &TargetPath, UniString &OutRelative)
{
    UniString baseAbs = TrimTrailingSlashPreserveRoot( NormalizeAgainstBaseAbsW(BaseDir, L"") );
    UniString targAbs = TrimTrailingSlashPreserveRoot( NormalizeAgainstBaseAbsW(TargetPath, BaseDir) );

    const wchar_t *b=baseAbs.c_str();
    const wchar_t *t=targAbs.c_str();
    if(!b || !t) return false;

    size_t bl=wcslen(b);
    size_t tl=wcslen(t);
    if(bl==0 || tl<bl) return false;
    if(wcsnicmp_local(b,t,bl)!=0) return false;

    if(tl==bl)
    {
        OutRelative = L".";
        return true;
    }
    if(t[bl]!=L'\\') return false;

    OutRelative = UniString(targAbs.SubString((int)bl+2, (int)(tl - (bl+1))));
    return true;
}
//---------------------------------------------------------------------------
UniString __fastcall NormalizeUnixNewlines(const UniString &Text)
{
	UniString Result;
	const int Len=Text.Length();
	const wchar_t *s=Text.c_bstr();

	for(int i=0;i<Len;i++)
	{
		wchar_t c=s[i];
		if(c==L'\n')
		{
			if((i+1)<Len && s[i+1]==L'\r')
			{
				Result+=L"\r\n";
				i++;
			}
			else
			{
				Result+=L"\r\n";
			}
		}
		else if(c==L'\r')
		{
			if((i+1)<Len && s[i+1]==L'\n')
			{
				Result+=L"\r\n";
				i++;
			}
			else
			{
				Result+=L'\r';
			}
		}
		else
		{
			Result+=c;
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
AnsiString __fastcall NormalizeUnixNewlines(const AnsiString &Text)
{
	AnsiString Result;
	const int Len=Text.Length();
	const char *s=Text.c_str();

	for(int i=0;i<Len;i++)
	{
		char c=s[i];
		if(c=='\n')
		{
			if((i+1)<Len && s[i+1]=='\r')
			{
				Result+="\r\n";
				i++;
			}
			else
			{
				Result+="\r\n";
			}
		}
		else if(c=='\r')
		{
			if((i+1)<Len && s[i+1]=='\n')
			{
				Result+="\r\n";
				i++;
			}
			else
			{
				Result+='\r';
			}
		}
		else
		{
			Result+=c;
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall ProcessMessages(void)
{
	MSG msg;
	while(::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		if(msg.message==WM_QUIT)
		{
			// Preserve quit semantics for the outer loop
			::PostQuitMessage((int)msg.wParam);
			break;
		}
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}
//---------------------------------------------------------------------------
// Make "In" relative to "BasePath" if (and only if) it's inside BasePath.
// Returns a cleaned relative path (no leading ".\"). Otherwise returns a
// normalized absolute path.
UniString __fastcall MakeRelativeIfInFolder(const UniString &In, const UniString &BasePath)
{
    if(In==L"") return In;

    // Normalize base to absolute, trim trailing slash
    UniString base = BasePath;
    if(!IsAbsolutePathW(base))
        base = NormalizeAgainstBaseAbsW(base, GetCurrentDirW());
    base = TrimTrailingSlashPreserveRoot(ExcludeTrailingBackslash(base));

    // Normalize input to absolute (using base for relative inputs)
    UniString absIn = In;
    if(!IsAbsolutePathW(absIn))
        absIn = NormalizeAgainstBaseAbsW(absIn, base);
    else
        absIn = NormalizeAgainstBaseAbsW(absIn, absIn); // canonicalize absolute
    absIn = TrimTrailingSlashPreserveRoot(absIn);

    // If inside base, make it relative
    UniString rel;
    if(IsSameOrUnderW(base, absIn) && TryMakeRelativeToW(base, absIn, rel))
    {
        // Remove leading ".\" if present
        if(rel.Length() >= 2 && rel[1]==L'.' && (rel[2]==L'\\' || rel[2]==L'/'))
            rel = rel.SubString(3, rel.Length()-2);

        // Normalize slashes to backslashes for the toolchain/Windows
        for(int i=1;i<=rel.Length();++i)
            if(rel[i]==L'/') rel[i]=L'\\';

        return rel;
    }

    // Outside project folder: return absolute, canonical form
    return absIn;
}
//---------------------------------------------------------------------------
// Older SDKs won't have these; define them with the correct values.
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWCP_DEFAULT
#define DWMWCP_DEFAULT      0
#define DWMWCP_DONOTROUND   1
#define DWMWCP_ROUND        2
#define DWMWCP_ROUNDSMALL   3
#endif

// Signature of DwmSetWindowAttribute (avoids needing dwmapi.h)
typedef HRESULT (WINAPI *PFN_DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);

//------------------------------------------------------------------------------------------
void __fastcall DisableRoundedCorners(HWND hWnd)
{
	// Load dwmapi.dll only if available (Win11/10 with DWM features)
	HMODULE hDwm=::LoadLibraryW(L"dwmapi.dll");
	if(!hDwm)
	{
		// Not available on older systems (e.g., Win98/XP). Nothing to do.
		return;
	}

	PFN_DwmSetWindowAttribute pDwmSetWindowAttribute=
		(PFN_DwmSetWindowAttribute)::GetProcAddress(hDwm,"DwmSetWindowAttribute");

	if(pDwmSetWindowAttribute)
	{
		const DWORD attr=DWMWA_WINDOW_CORNER_PREFERENCE;
		const DWORD pref=DWMWCP_DONOTROUND; // request square corners
		pDwmSetWindowAttribute(hWnd,attr,&pref,sizeof(pref));
	}

	::FreeLibrary(hDwm);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static UniString __fastcall StripLongPathPrefix(const UniString &Path)
{
	// \\?\C:\...  => C:\...
	// \\?\UNC\server\share\... => \\server\share\...
	const wchar_t *s=Path.c_str();
	if(wcsncmp(s,L"\\\\?\\UNC\\",8)==0)
	{
		return UniString(L"\\\\")+UniString(s+8);
	}
	if(wcsncmp(s,L"\\\\?\\",4)==0)
	{
		return UniString(s+4);
	}
	return Path;
}
//---------------------------------------------------------------------------
static UniString __fastcall CollapseDotDot(const UniString &In)
{
	UniString Result=L"";
	const int n=In.Length();
	const wchar_t *s=In.c_str(); // read-only view

	// Detect root
	bool isUNC=(n>=2 && s[0]==L'\\' && s[1]==L'\\');
	int pos=1;
	UniString root=L"";

	if(isUNC)
	{
		// \\server\share\  -> keep first two components
		int parts=0;
		int start=1;
		for(int i=1;i<=n;i++)
		{
			wchar_t ch=(i<=n)?s[i-1]:L'\\';
			if(ch==L'\\')
			{
				int len=i-start;
				if(len>0)
				{
					UniString part=In.SubString(start,len);
					if(root!=L"")root+=L"\\";
					root+=part;
					parts++;
					if(parts==2)
					{
						pos=i+1;
						break;
					}
				}
				start=i+1;
			}
		}
		if(root==L"")
		{
			root=L"\\\\";
		}
	}
	else
	{
		// Drive root like "C:\"
		if(n>=3 && s[1]==L':' && s[2]==L'\\')
		{
			root=In.SubString(1,3);
			pos=4;
		}
		else if(n>=1 && s[0]==L'\\')
		{
			root=L"\\";
			pos=2;
		}
	}

	// Component stack
	UniString stack[512];
	int top=0;

	int start=pos;
	for(int i=pos;i<=n;i++)
	{
		wchar_t ch=(i<=n)?s[i-1]:L'\\';
		if(ch==L'\\')
		{
			int len=i-start;
			UniString part=len>0?In.SubString(start,len):L"";
			start=i+1;

			if(part==L"" || part==L".")
			{
				continue;
			}
			if(part==L"..")
			{
				if(top>0) top--;
			}
			else
			{
				if(top<(int)(sizeof(stack)/sizeof(stack[0])))
				{
					stack[top++]=part;
				}
			}
		}
	}

	// Reassemble
	Result=root;
	for(int i=0;i<top;i++)
	{
		if(Result!=L"" && Result[Result.Length()]!=L'\\')
		{
			Result+=L"\\";
		}
		Result+=stack[i];
	}

	// Trim trailing backslash except for roots
	if(Result.Length()>0)
	{
		bool isDriveRoot=(Result.Length()==3 && Result[2]==L':' && Result[3]==L'\\');
		if(Result!=L"\\" && !isDriveRoot)
		{
			if(Result[Result.Length()]==L'\\')
			{
				Result=Result.SubString(1,Result.Length()-1);
			}
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
static UniString __fastcall ToAbsolutePath(const UniString &In,const UniString &BaseDir)
{
	// Normalize slashes first
	UniString P=In;
	for(int i=1;i<=P.Length();i++)
	{
		if(P[i]==L'/')P[i]=L'\\';
	}

	// Build the candidate we pass to GetFullPathNameW:
	// - If already absolute (drive or UNC or \\?\), use as-is.
	// - Otherwise join with BaseDir (or CWD).
	bool isAbs=false;
	const wchar_t *s=P.c_str();
	int n=P.Length();
	if(n>=3 && s[1]==L':' && s[2]==L'\\') isAbs=true;					// "C:\..."
	else if(n>=2 && s[0]==L'\\' && s[1]==L'\\') isAbs=true;			// UNC
	else if(n>=4 && wcsncmp(s,L"\\\\?\\",4)==0) isAbs=true;			// \\?\

	UniString Candidate;
	if(isAbs)
	{
		Candidate=P;
	}
	else
	{
		UniString base=BaseDir;
		if(base==L"")
		{
			DWORD need=::GetCurrentDirectoryW(0,NULL);
			if(need>0)
			{
				base.SetLength(need);
				DWORD got=::GetCurrentDirectoryW(need,(LPWSTR)base.c_str());
				if(got>0) base.SetLength(got);
			}
		}
		if(base.Length()>0 && base[base.Length()]!=L'\\') base+=L"\\";
		Candidate=base+P;
	}

	// Canonicalize to absolute via GetFullPathNameW
	DWORD need=::GetFullPathNameW(Candidate.c_str(),0,NULL,NULL);
	if(need==0)
	{
		// Fallback: just return what we have; later stages still normalize case/slashes.
		return Candidate;
	}
	UniString Full;
	Full.SetLength(need);
	DWORD got=::GetFullPathNameW(Candidate.c_str(),need,(LPWSTR)Full.c_str(),NULL);
	if(got>0) Full.SetLength(got);

	return Full;
}
//---------------------------------------------------------------------------
static UniString __fastcall TryGetLongPath(const UniString &AbsPath)
{
	// Optional: improve casing / 8.3 vs long names
	typedef DWORD (WINAPI *TGetLongPathNameW)(LPCWSTR,LPWSTR,DWORD);
	HMODULE k32=::GetModuleHandleW(L"kernel32.dll");
	TGetLongPathNameW p=(TGetLongPathNameW)(k32?::GetProcAddress(k32,"GetLongPathNameW"):NULL);
	if(!p) return AbsPath;

	DWORD need=p(AbsPath.c_str(),NULL,0);
	if(need==0) return AbsPath;

	UniString out; out.SetLength(need);
	DWORD got=p(AbsPath.c_str(),(LPWSTR)out.c_str(),need);
	if(got>0) out.SetLength(got);
	return out;
}
//---------------------------------------------------------------------------
UniString __fastcall NormalizePathForCompare(const UniString &In,const UniString &BaseDir)
{
	if(In==L"") return L"";

	// Absolute, canonical
	UniString abs=ToAbsolutePath(In,BaseDir);

	// If it exists, try to promote to long/cased variant
	DWORD attrs=::GetFileAttributesW(abs.c_str());
	if(attrs!=INVALID_FILE_ATTRIBUTES)
	{
		abs=TryGetLongPath(abs);
	}

	// Normalize slashes and force lowercase (case-insensitive filesystem)
	for(int i=1;i<=abs.Length();i++)
	{
		wchar_t c=abs[i];
		if(c==L'/') abs[i]=L'\\';
		else if(c>=L'A' && c<=L'Z') abs[i]=(wchar_t)(c+32);
	}

	// Drop a trailing backslash except for roots ("c:\" or "\\server\share")
	if(abs.Length()>0)
	{
		const wchar_t *s=abs.c_str();
		int n=abs.Length();

		bool isDriveRoot=(n==3 && s[1]==L':' && s[2]==L'\\');
		bool isUncRoot=false;
		if(n>=5 && s[0]==L'\\' && s[1]==L'\\')
		{
			// \\server\share
			int slashes=0;
			for(int i=0;i<n;i++) if(s[i]==L'\\') slashes++;
			isUncRoot=(slashes==3); // exactly two names -> 3 backslashes total
		}
		if(!isDriveRoot && !isUncRoot)
		{
			if(s[n-1]==L'\\')
			{
				abs=abs.SubString(1,n-1);
			}
		}
	}
	return abs;
}
//---------------------------------------------------------------------------
static bool __fastcall SameFileByHandle(const UniString &A,const UniString &B)
{
	// Open both with minimal access; handle directories via FILE_FLAG_BACKUP_SEMANTICS
	auto OpenForMeta=[](const UniString &p)->HANDLE
	{
		DWORD attrs=::GetFileAttributesW(p.c_str());
		if(attrs==INVALID_FILE_ATTRIBUTES)return INVALID_HANDLE_VALUE;
		DWORD flags=((attrs&FILE_ATTRIBUTE_DIRECTORY)?FILE_FLAG_BACKUP_SEMANTICS:0);
		return ::CreateFileW(
			p.c_str(),
			FILE_READ_ATTRIBUTES,
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			flags,
			NULL
		);
	};

	HANDLE hA=OpenForMeta(A);
	if(hA==INVALID_HANDLE_VALUE)return false;
	HANDLE hB=OpenForMeta(B);
	if(hB==INVALID_HANDLE_VALUE){::CloseHandle(hA);return false;}

	BY_HANDLE_FILE_INFORMATION ia={0},ib={0};
	BOOL ra=::GetFileInformationByHandle(hA,&ia);
	BOOL rb=::GetFileInformationByHandle(hB,&ib);

	::CloseHandle(hA);
	::CloseHandle(hB);

	if(!ra || !rb)return false;

	return ia.dwVolumeSerialNumber==ib.dwVolumeSerialNumber
		&& ia.nFileIndexHigh==ib.nFileIndexHigh
		&& ia.nFileIndexLow==ib.nFileIndexLow;
}
//---------------------------------------------------------------------------
bool __fastcall PathsReferToSameFile(const UniString &A,const UniString &B,const UniString &BaseDir,bool RequireExistsForStrongCheck)
{
	if(A==L"" && B==L"")return true;
	if(A==L"" || B==L"")return false;

	UniString na=NormalizePathForCompare(A,BaseDir);
	UniString nb=NormalizePathForCompare(B,BaseDir);

	// Fast path: identical normalized strings
	if(na==nb)return true;

	// Strong path: if both exist (or required), compare by file ID
	DWORD aa=::GetFileAttributesW(na.c_str());
	DWORD bb=::GetFileAttributesW(nb.c_str());

	bool aExists=(aa!=INVALID_FILE_ATTRIBUTES);
	bool bExists=(bb!=INVALID_FILE_ATTRIBUTES);

	if((RequireExistsForStrongCheck && aExists && bExists)
	|| (!RequireExistsForStrongCheck && aExists && bExists))
	{
		if(SameFileByHandle(na,nb))return true;
	}

	// Fallback: relaxed string compare already failed -> not the same file
	return false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall NormalizePathForCompare(const AnsiString &In,const AnsiString &BaseDir)
{
	UniString uIn=In;
	UniString uBase=BaseDir;
	return AnsiString(NormalizePathForCompare(uIn,uBase).c_str());
}
//---------------------------------------------------------------------------
bool __fastcall PathsReferToSameFile(const AnsiString &A,const AnsiString &B,const AnsiString &BaseDir,bool RequireExistsForStrongCheck)
{
	UniString uA=A;
	UniString uB=B;
	UniString uBase=BaseDir;
	return PathsReferToSameFile(uA,uB,uBase,RequireExistsForStrongCheck);
}
//---------------------------------------------------------------------------
UniString __fastcall GetPathFromIDListW(void *PIDL)
{
	UniString Res;
	ITEMIDLIST* pidl=(ITEMIDLIST *)PIDL;
	wchar_t lpPath[MAX_PATH+1];
	lpPath[MAX_PATH+1]=0;
	HMODULE hmod=LoadLibraryA("Shell32.dll");
	FARPROC Proc2=GetProcAddress(hmod,"SHGetPathFromIDListW");
	SHGetPathFromIDListWProc SHGetPathFromIDListW=(SHGetPathFromIDListWProc)Proc2;
	HRESULT hRes=SHGetPathFromIDListW( pidl, lpPath );
	if(hRes==NOERROR)
	{
		Res=lpPath;
	}
	FreeLibrary(hmod);
	return Res;
}
//---------------------------------------------------------------------------
// Minimal Shell defs (no extra includes)
//---------------------------------------------------------------------------
#ifndef _MIN_STRRET_DEFINED
#define _MIN_STRRET_DEFINED
typedef struct _ITEMIDLIST ITEMIDLIST;
typedef const ITEMIDLIST* LPCITEMIDLIST;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// STRRET types
#ifndef STRRET_WSTR
#define STRRET_WSTR   0
#endif
#ifndef STRRET_OFFSET
#define STRRET_OFFSET 1
#endif
#ifndef STRRET_CSTR
#define STRRET_CSTR   2
#endif

typedef struct tagSTRRET
{
	UINT uType;
	union
	{
		LPWSTR pOleStr;          // STRRET_WSTR
		UINT   uOffset;          // STRRET_OFFSET
		char   cStr[MAX_PATH];   // STRRET_CSTR
	};
} STRRET, *LPSTRRET;

// SHGDN flags we need
#ifndef SHGDN_FORPARSING
#define SHGDN_FORPARSING 0x8000
#endif
#endif//_MIN_STRRET_DEFINED

//---------------------------------------------------------------------------
// Minimal IShellFolder vtable (only members we touch need exact signatures; others are loose)
//---------------------------------------------------------------------------
#ifndef _MIN_ISHELLFOLDER_DEFINED
#define _MIN_ISHELLFOLDER_DEFINED

struct IShellFolder;

typedef struct IShellFolderVtbl
{
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IShellFolder*, REFIID, void**);
	ULONG   (STDMETHODCALLTYPE *AddRef)(IShellFolder*);
	ULONG   (STDMETHODCALLTYPE *Release)(IShellFolder*);

	// IShellFolder
	HRESULT (STDMETHODCALLTYPE *ParseDisplayName)(IShellFolder*, HWND, void* /*IBindCtx**/,
		LPWSTR, ULONG*, LPCITEMIDLIST*, ULONG*);
	HRESULT (STDMETHODCALLTYPE *EnumObjects)(IShellFolder*, HWND, DWORD, void** /*IEnumIDList**/);
	HRESULT (STDMETHODCALLTYPE *BindToObject)(IShellFolder*, LPCITEMIDLIST, void* /*IBindCtx**/, REFIID, void**);
	HRESULT (STDMETHODCALLTYPE *BindToStorage)(IShellFolder*, LPCITEMIDLIST, void* /*IBindCtx**/, REFIID, void**);
	HRESULT (STDMETHODCALLTYPE *CompareIDs)(IShellFolder*, LPARAM, LPCITEMIDLIST, LPCITEMIDLIST);
	HRESULT (STDMETHODCALLTYPE *CreateViewObject)(IShellFolder*, HWND, REFIID, void**);
	HRESULT (STDMETHODCALLTYPE *GetAttributesOf)(IShellFolder*, UINT, LPCITEMIDLIST*, ULONG*);
	HRESULT (STDMETHODCALLTYPE *GetUIObjectOf)(IShellFolder*, HWND, UINT, LPCITEMIDLIST*, REFIID, UINT*, void**);

	// The one we actually need:
	HRESULT (STDMETHODCALLTYPE *GetDisplayNameOf)(IShellFolder*, LPCITEMIDLIST, DWORD, LPSTRRET);

	HRESULT (STDMETHODCALLTYPE *SetNameOf)(IShellFolder*, HWND, LPCITEMIDLIST, LPCWSTR, DWORD, LPCITEMIDLIST*);
} IShellFolderVtbl;

struct IShellFolder
{
	const IShellFolderVtbl* lpVtbl;
};

#endif//_MIN_ISHELLFOLDER_DEFINED

//---------------------------------------------------------------------------
// Local typedef for StrRetToBufW so we can load it dynamically
//---------------------------------------------------------------------------
typedef HRESULT (WINAPI *PFN_StrRetToBufW)(const STRRET*, LPCITEMIDLIST, LPWSTR, UINT);

//---------------------------------------------------------------------------
// Helper: quick filesystem-ish check (drive path, UNC, or \\?\ extended)
//---------------------------------------------------------------------------
static bool __fastcall LooksLikeFilesystemPath(const wchar_t* s)
{
	if(!s||!s[0])return false;
	bool isDrive=(s[0]!=0 && s[1]==L':' && (s[2]==L'\\' || s[2]==L'/'));
	bool isUNC=(s[0]==L'\\' && s[1]==L'\\' && s[2]!=0);
	bool isExt=(wcsncmp(s,L"\\\\?\\",4)==0) || (wcsncmp(s,L"\\\\.\\",4)==0);
	return isDrive || isUNC || isExt;
}

//---------------------------------------------------------------------------
// Returns a filesystem full path from OFNOTIFYEX::psf / ::pidl.
// - psf: IShellFolder* (passed in as void* to avoid headers)
// - pidl: PCUITEMID_CHILD (passed in as void*; treated as LPCITEMIDLIST here)
// For virtual items (no filesystem path), returns L"".
//---------------------------------------------------------------------------
UniString __fastcall GetFullPathFromOFNotifyPIDL(void* psf, void* pidl)
{
	UniString Result=L"";
	if(!psf || !pidl)return Result;

	IShellFolder* pSF=reinterpret_cast<IShellFolder*>(psf);
	LPCITEMIDLIST pidlChild=reinterpret_cast<LPCITEMIDLIST>(pidl);

	STRRET sr={};
	HRESULT hr=pSF->lpVtbl->GetDisplayNameOf(pSF,pidlChild,SHGDN_FORPARSING,&sr);
	if(FAILED(hr))
	{
		return Result;
	}

	wchar_t buf[32768];
	buf[0]=0;

	// Try StrRetToBufW from Shlwapi.dll
	HMODULE hShlwapi=LoadLibraryA("Shlwapi.dll");
	PFN_StrRetToBufW pStrRetToBufW=nullptr;
	if(hShlwapi)
	{
		pStrRetToBufW=(PFN_StrRetToBufW)GetProcAddress(hShlwapi,"StrRetToBufW");
	}

	if(pStrRetToBufW)
	{
		if(SUCCEEDED(pStrRetToBufW(&sr,pidlChild,buf,(UINT)(sizeof(buf)/sizeof(buf[0])))) && buf[0])
		{
			if(LooksLikeFilesystemPath(buf))
			{
				Result=buf;
			}
		}
	}
	else
	{
		// Manual fallback if StrRetToBufW isn't available
		switch(sr.uType)
		{
			case STRRET_WSTR:
			{
				if(sr.pOleStr && sr.pOleStr[0] && LooksLikeFilesystemPath(sr.pOleStr))
				{
					Result=sr.pOleStr;
				}
				// Optional: dynamically load Ole32!CoTaskMemFree to free sr.pOleStr
				// if you want to avoid a potential small leak in this rare path.
				break;
			}
			case STRRET_CSTR:
			{
				if(sr.cStr[0])
				{
					int n=MultiByteToWideChar(CP_ACP,0,sr.cStr,-1,buf,(int)(sizeof(buf)/sizeof(buf[0])));
					if(n>0 && LooksLikeFilesystemPath(buf))
					{
						Result=buf;
					}
				}
				break;
			}
			case STRRET_OFFSET:
			default:
				// OFFSET form not handled in this minimal path.
				break;
		}
	}

	if(hShlwapi)
	{
		FreeLibrary(hShlwapi);
	}

	return Result;
}
//---------------------------------------------------------------------------



}; //namespace CommonFuncs
//---------------------------------------------------------------------------

