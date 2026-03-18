//---------------------------------------------------------------------------

#pragma hdrstop

#include "ItsAMysteryUnit.h"
#include "CommonFuncsUnit.h"
#include <windows.h>
#include <objbase.h> 
#include <shlobj.h>
#include <shobjidl.h>
#include <commdlg.h>
#include <wingdi.h>
//---------------------------------------------------------------------------
#define PROP_PARENTHWND TEXT("MyApp_Parent")
#define PROP_WNDPROC TEXT("MyApp_WndProc")
#define PROP_TOOLBAROBJ TEXT("MyApp_ToolBarObj")
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern HINSTANCE ThisInstance;
extern HINSTANCE hGlobalInstance;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TBPIniFile::TBPIniFile(const UniString AFile)
{
        FFile=AFile;
}
//---------------------------------------------------------------------------
__fastcall TBPIniFile::~TBPIniFile(void)
{
        ::WritePrivateProfileString(NULL,NULL,NULL,FFile.c_bstr());
}
//---------------------------------------------------------------------------
void __fastcall TBPIniFile::DeleteKey(const UniString Section, const UniString Ident)
{
        ::WritePrivateProfileStringW(
                Section.c_bstr(),
                Ident.c_bstr(),
                NULL,
                FFile.c_bstr()
        );
}
//---------------------------------------------------------------------------
int __fastcall TBPIniFile::ReadInteger(const UniString Section, const UniString Ident, int Default)
{
        UniString res=ReadString(Section,Ident,UniString(Default));
        return res.ToIntDef(Default);
}
//---------------------------------------------------------------------------
void __fastcall TBPIniFile::WriteInteger(const UniString Section, const UniString Ident, int Value)
{
        WriteString(Section,Ident,UniString(Value));
}
//---------------------------------------------------------------------------
bool __fastcall TBPIniFile::ReadBool(const UniString Section, const UniString Ident, bool Default)
{
        UniString res=ReadString(Section,Ident,UniString(Default?1:0));
        return res.ToIntDef(Default?1:0)!=0;
}
//---------------------------------------------------------------------------
void __fastcall TBPIniFile::WriteBool(const UniString Section, const UniString Ident, bool Value)
{
        WriteString(Section,Ident,UniString(Value?1:0));
}
//---------------------------------------------------------------------------
UniString __fastcall TBPIniFile::ReadString(const UniString Section, const UniString Ident, const UniString Default)
{
        UniString Res=Default;
        DWORD res;
        wchar_t *buffer=new wchar_t[65536];
        res=::GetPrivateProfileStringW(
                Section.c_bstr(),
                Ident.c_bstr(),
                Res.c_bstr(),
                buffer,
                65536-1,
                FFile.c_bstr()
        );
        if(res>0)
        {
                buffer[res]='\0';
                Res=UniString(buffer);
        }
        delete [] buffer;
        return Res;
}
//---------------------------------------------------------------------------
void __fastcall TBPIniFile::WriteString(const UniString Section, const UniString Ident, const UniString Value)
{
        ::WritePrivateProfileStringW(
                Section.c_bstr(),
                Ident.c_bstr(),
                Value.c_bstr(),
                FFile.c_bstr()
        );
}
//---------------------------------------------------------------------------
UniString __fastcall TBPIniFile::ReadSections(void)
{
        UniString Res;
        DWORD res;
        #define MASSIVE 65535*255
        wchar_t *buffer=new wchar_t[MASSIVE];
        res=::GetPrivateProfileStringW(
                NULL,
                NULL,
                L"",
                buffer,
                MASSIVE-1,
                FFile.c_bstr()
        );
        #undef MASSIVE
        if(res>0)
        {
                wchar_t *temp=buffer;
                UniString Temp=temp;
                while(Temp.Length()>0)
                {
                        if(Res!=L"")
                        {
                                Res+=L"\r\n";
                        }
                        Res+=Temp;
                        temp+=Temp.Length()+1;
                        Temp=temp;
                }
        }
        delete [] buffer;
        return Res;
}
//---------------------------------------------------------------------------
UniString __fastcall TBPIniFile::ReadSection(const UniString Section)
{
        UniString Res;
        UniString S=Section;
        DWORD res;
        #define MASSIVE 65535*255
        wchar_t *buffer=new wchar_t[MASSIVE];
        res=::GetPrivateProfileStringW(
                S.c_bstr(),
                NULL,
                L"",
                buffer,
                MASSIVE-1,
                FFile.c_bstr()
        );
        #undef MASSIVE
        if(res>0)
        {
                wchar_t *temp=buffer;
                UniString Temp=temp;
                while(Temp.Length()>0)
                {
                        if(Res!=L"")
                        {
                                Res+=L"\r\n";
                        }
                        Res+=Temp;
                        temp+=Temp.Length()+1;
                        Temp=temp;
                }
        }
        delete [] buffer;
        return Res;
}
//---------------------------------------------------------------------------
void __fastcall TBPIniFile::EraseSection(const UniString Section)
{
        UniString S=Section;
        ::WritePrivateProfileStringW(
                S.c_bstr(),
                NULL,
                L"",
                FFile.c_bstr()
        );
}
//---------------------------------------------------------------------------
UniString __fastcall TBPIniFile::GetFile(void)
{
        return FFile;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static bool IsSlash(wchar_t c){ return c==L'\\' || c==L'/'; }
//---------------------------------------------------------------------------
ATOM OpenSaveDialogPropAtom=GlobalAddAtom(TEXT("BPOpenSaveDialogData"));
TCHAR *OpenSaveDialogProp=MAKEINTATOM(OpenSaveDialogPropAtom);
void __fastcall SetMyOpenSaveDialogProp(HWND hwnd,TOpenSaveDialog *Win)
{
        if(OpenSaveDialogPropAtom==0)
        {
                OpenSaveDialogPropAtom=GlobalAddAtomA("BPOpenSaveDialogData");
                if(OpenSaveDialogPropAtom==0){throw "apecrap";}
                OpenSaveDialogProp=MAKEINTATOM(OpenSaveDialogPropAtom);
        }
        ::SetProp(hwnd,OpenSaveDialogProp,Win);
};
void __fastcall RemoveMyOpenSaveDialogProp(HWND hwnd)
{
        ::RemoveProp(hwnd,OpenSaveDialogProp);
};
#define GetMyOpenSaveDialogProp(hwnd) (TOpenSaveDialog *)::GetProp(hwnd,OpenSaveDialogProp)
//---------------------------------------------------------------------------
__fastcall TOpenSaveDialog::TOpenSaveDialog(void)
{
	memset(&OFN,0,sizeof(OFN));
	memset(&FOptions,0,sizeof(FOptions));
	
	FCallback=NULL;

	// Sensible defaults
	FOptions.EnableSizing=true;
	FOptions.OldStyleDialog=false; // use Explorer-style by default

	EnsureBuffer(32768); // ample room for multiselect lists
}
//---------------------------------------------------------------------------
__fastcall TOpenSaveDialog::~TOpenSaveDialog(void)
{
	
}
//---------------------------------------------------------------------------
UINT_PTR CALLBACK TOpenSaveDialog::HookOldStyleProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	TOpenSaveDialog *This=GetMyOpenSaveDialogProp(hwndDlg);
	switch(message)
	{
		case WM_INITDIALOG:
		if(lParam)
		{
			OPENFILENAMEW *OFN=(OPENFILENAMEW *)lParam;
			This=(TOpenSaveDialog *)(OFN->lCustData);
			if(This)
			{
				if(This->FCallback)
				{
					SetMyOpenSaveDialogProp(hwndDlg,This);
					return This->FCallback(This,hwndDlg,message,wParam,lParam);
				}
			}
		}
		break;
		case WM_NCDESTROY:
		if(This)
		{
			if(This->FCallback)
			{
				RemoveMyOpenSaveDialogProp(hwndDlg);
				return This->FCallback(This,hwndDlg,message,wParam,lParam);
			}
		}
		break;
	}
	if(This)
	{
		if(This->FCallback)
		{
			return This->FCallback(This,hwndDlg,message,wParam,lParam);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
UINT_PTR CALLBACK TOpenSaveDialog::HookProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	TOpenSaveDialog *This=GetMyOpenSaveDialogProp(hwndDlg);
	switch(message)
	{
		case WM_INITDIALOG:
		if(lParam)
		{
			OPENFILENAMEW *OFN=(OPENFILENAMEW *)lParam;
			This=(TOpenSaveDialog *)(OFN->lCustData);
			if(This)
			{
				if(This->FCallback)
				{
					SetMyOpenSaveDialogProp(hwndDlg,This);
					return This->FCallback(This,hwndDlg,message,wParam,lParam);
				}
			}
		}
		break;
		case WM_NCDESTROY:
		if(This)
		{
			if(This->FCallback)
			{
				RemoveMyOpenSaveDialogProp(hwndDlg);
				return This->FCallback(This,hwndDlg,message,wParam,lParam);
			}
		}
		break;
	}
	if(This)
	{
		if(This->FCallback)
		{
			return This->FCallback(This,hwndDlg,message,wParam,lParam);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::EnsureBuffer(int MinChars)
{
	if(MinChars<1024) MinChars=1024;
	if((int)FBuffer.size()<MinChars)
	{
		FBuffer.assign((size_t)MinChars,0);
	}
	else
	{
		// keep size, but clear leading char so no stale data
		if(!FBuffer.empty()) FBuffer[0]=0;
	}
}
//---------------------------------------------------------------------------
DWORD __fastcall TOpenSaveDialog::BuildFlags(void) const
{
	DWORD f=0;

	// Style
	if(!FOptions.OldStyleDialog) f|=OFN_EXPLORER;
	if(FOptions.EnableSizing)    f|=OFN_ENABLESIZING;

	// Behavior
	if(FOptions.ReadOnly)            f|=OFN_READONLY;
	if(FOptions.OverwritePrompt)     f|=OFN_OVERWRITEPROMPT;
	if(FOptions.HideReadOnly)        f|=OFN_HIDEREADONLY;
	if(FOptions.NoChangeDir)         f|=OFN_NOCHANGEDIR;
	if(FOptions.ShowHelp)            f|=OFN_SHOWHELP;
	if(FOptions.NoValidate)          f|=OFN_NOVALIDATE;
	if(FOptions.AllowMultiSelect)    f|=OFN_ALLOWMULTISELECT;
	if(FOptions.ExtensionDifferent)  f|=OFN_EXTENSIONDIFFERENT;
	if(FOptions.PathMustExist)       f|=OFN_PATHMUSTEXIST;
	if(FOptions.FileMustExist)       f|=OFN_FILEMUSTEXIST;
	if(FOptions.CreatePrompt)        f|=OFN_CREATEPROMPT;
	if(FOptions.ShareAware)          f|=OFN_SHAREAWARE;
	if(FOptions.NoReadOnlyReturn)    f|=OFN_NOREADONLYRETURN;
	if(FOptions.NoTestFileCreate)    f|=OFN_NOTESTFILECREATE;
	if(FOptions.NoNetworkButton)     f|=OFN_NONETWORKBUTTON;
	if(FOptions.NoLongNames)         f|=OFN_NOLONGNAMES;
	if(FOptions.NoDereferenceLinks)  f|=OFN_NODEREFERENCELINKS;
	if(FOptions.EnableIncludeNotify  && FCallback!=NULL) f|=OFN_ENABLEINCLUDENOTIFY;

	// Enforce Explorer style if multiselect is requested (old-style is messy)
	if(FOptions.AllowMultiSelect) f|=OFN_EXPLORER;

	return f;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::BuildFilterFromVCL(void)
{
	FFilterBuf.clear();

	const wchar_t *s=FVclFilter.c_str();
	if(s==NULL || s[0]==L'\0')
	{
		// No filter -> leave NULL (common dialogs default to *.*)
		return;
	}

	size_t len=wcslen(s);
	FFilterBuf.reserve(len+2);
	size_t segments=1;

	for(size_t i=0;i<len;i++)
	{
		if(s[i]==L'|')
		{
			FFilterBuf.push_back(L'\0');
			segments++;
		}
		else
		{
			FFilterBuf.push_back(s[i]);
		}
	}

	// Ensure even number of segments (pairs of Desc/Pattern)
	if((segments%2)!=0)
	{
		FFilterBuf.push_back(L'\0');
		const wchar_t *fallback=L"*.*";
		while(*fallback) { FFilterBuf.push_back(*fallback++); }
	}

	// Double-NUL terminate
	FFilterBuf.push_back(L'\0');
	FFilterBuf.push_back(L'\0');
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::ResetOFN(HWND Owner)
{
	memset(&OFN,0,sizeof(OFN));
	OFN.lStructSize=sizeof(OFN);
	OFN.hwndOwner=Owner;
	OFN.Flags=BuildFlags();

	// Filter (convert VCL-style on demand)
	BuildFilterFromVCL();
	OFN.lpstrFilter=FFilterBuf.empty()?NULL:&FFilterBuf[0];

	// Title / Dir / Default Ext
	OFN.lpstrTitle=FTitle.c_str();
	OFN.lpstrInitialDir=FInitialDir.c_str();
	OFN.lpstrDefExt=FDefaultExt.c_str(); // expect no leading '.'

	// Filename buffer (input/output)
	EnsureBuffer((FOptions.AllowMultiSelect)?32768:4096);
	if(!FFileName.IsEmpty())
	{
		// Preload suggested filename/path
		const wchar_t *src=FFileName.c_str();
		size_t n=wcslen(src);
		if(n>(size_t)FBuffer.size()-1) n=FBuffer.size()-1;
		memcpy(&FBuffer[0],src,n*sizeof(wchar_t));
		FBuffer[n]=0;
	}
	else
	{
		if(!FBuffer.empty()) FBuffer[0]=0;
	}
	OFN.lpstrFile=&FBuffer[0];
	OFN.nMaxFile=(DWORD)FBuffer.size();
	
	if(FCallback)
	{
		OFN.lCustData=(LPARAM)this;
		if(FOptions.OldStyleDialog)
		{
			OFN.lpfnHook=TOpenSaveDialog::HookOldStyleProc;
		}
		else
		{
			OFN.lpfnHook=TOpenSaveDialog::HookProc;
		}
		OFN.Flags|=OFN_ENABLEHOOK;
	}

	// We don't use lpstrFileTitle/lpstrCustomFilter; leave NULL
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::ParseResult(void)
{
	FFiles.clear();
	FFileName=L"";

	if(FBuffer.empty() || FBuffer[0]==0) return;

	// First token
	const wchar_t *p=&FBuffer[0];
	size_t len=wcslen(p);

	// If multiselect OFF: buffer is a single full path
	if(!FOptions.AllowMultiSelect)
	{
		FFileName=UniString(p);
		FFiles.push_back(FFileName);
		return;
	}

	// Multiselect ON (Explorer style):
	// Either:
	//  1) a single fully-qualified path (and next wchar is '\0'), or
	//  2) first token is the directory, followed by one-or-more filenames, ending with double-NUL
	const wchar_t *q=p+len+1;
	if(*q==0)
	{
		// Single full path even with multiselect flag
		FFileName=UniString(p);
		FFiles.push_back(FFileName);
		return;
	}

	// Case 2: directory + list of files
	UniString dir=p;
	// Ensure dir ends with backslash
	const wchar_t *ds=dir.c_str();
	int dlen=ds?((int)wcslen(ds)):0;
	bool needSlash=(dlen>0 && !IsSlash(ds[dlen-1]));

	for(const wchar_t *name=q; name && *name; name+=wcslen(name)+1)
	{
		UniString full=dir;
		if(needSlash) full+=L"\\";
		full+=UniString(name);
		if(FFiles.empty()) FFileName=full;
		FFiles.push_back(full);
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOpenSaveDialog::Execute(HWND Owner)
{
	ResetOFN(Owner);
	if(DoExecute())
	{
		ParseResult();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
// Setters / Getters
//---------------------------------------------------------------------------
TOpenSaveDialogOptions * __fastcall TOpenSaveDialog::GetOptions(void)
{
	return &FOptions;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetOptions(const TOpenSaveDialogOptions &Opts)
{
	FOptions=Opts;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetTitle(const UniString &Title)
{
	FTitle=Title;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetInitialDir(const UniString &Dir)
{
	FInitialDir=Dir;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetDefaultExt(const UniString &ExtNoDot)
{
	// Expect no leading '.'; if present, skip it
	const wchar_t *s=ExtNoDot.c_str();
	if(s && s[0]==L'.')
	{
		FDefaultExt=UniString(s+1);
	}
	else
	{
		FDefaultExt=ExtNoDot;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetFileName(const UniString &FileName)
{
	FFileName=FileName;
}
//---------------------------------------------------------------------------
void __fastcall TOpenSaveDialog::SetFilter(const UniString &VclStyle)
{
	FVclFilter=VclStyle;
	FFilterBuf.clear();
}
//---------------------------------------------------------------------------
const UniString & __fastcall TOpenSaveDialog::GetFileName(void) const
{
	return FFileName;
}
//---------------------------------------------------------------------------
int __fastcall TOpenSaveDialog::GetFileCount(void) const
{
	return (int)FFiles.size();
}
//---------------------------------------------------------------------------
bool __fastcall TOpenSaveDialog::GetFileAt(int Index,UniString &Out) const
{
	if(Index<0) return false;
	if((size_t)Index>=FFiles.size()) return false;
	Out=FFiles[(size_t)Index];
	return true;
}
//---------------------------------------------------------------------------
const std::vector<UniString> & __fastcall TOpenSaveDialog::GetFiles(void) const
{
	return FFiles;
}
//---------------------------------------------------------------------------
UINT_PTR CALLBACK HookOldStyleProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}
//---------------------------------------------------------------------------
UINT_PTR CALLBACK HookProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TOpenDialog::TOpenDialog(void)
	: TOpenSaveDialog()
{
	// Sensible defaults for "Open"
	TOpenSaveDialogOptions *o=GetOptions();
	o->FileMustExist=true;
	o->PathMustExist=true;
	o->OverwritePrompt=false;
	o->AllowMultiSelect=false; // caller can enable if desired
}
//---------------------------------------------------------------------------
bool __fastcall TOpenDialog::DoExecute(void)
{
	return ::GetOpenFileNameW(&OFN)!=FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TSaveDialog::TSaveDialog(void)
	: TOpenSaveDialog()
{
	// Sensible defaults for "Save"
	TOpenSaveDialogOptions *o=GetOptions();
	o->FileMustExist=false;
	o->PathMustExist=true;
	o->OverwritePrompt=true;   // prompt before overwriting
	o->AllowMultiSelect=false; // not supported by Save dialog
}
//---------------------------------------------------------------------------
bool __fastcall TSaveDialog::DoExecute(void)
{
	return ::GetSaveFileNameW(&OFN)!=FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TSelectFolderDialog::TSelectFolderDialog(void)
{
	FTitle=L"";
	FInitialDir=L"";
	FFolder=L"";
	memset(&FOptions,0,sizeof(FOptions));
	FOptions.PathMustExist=true;
	FOptions.ForceFileSystem=true;
	FOptions.NewFolderButton=true;
	FOptions.ShowEditBox=true;
	FOptions.UseLegacy=false;
}
//---------------------------------------------------------------------------
__fastcall TSelectFolderDialog::~TSelectFolderDialog(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TSelectFolderDialog::SetTitle(const UniString &Title)
{
	FTitle=Title;
}
//---------------------------------------------------------------------------
void __fastcall TSelectFolderDialog::SetInitialDir(const UniString &Dir)
{
	FInitialDir=Dir;
}
//---------------------------------------------------------------------------
const UniString & __fastcall TSelectFolderDialog::GetFolder(void) const
{
	return FFolder;
}
//---------------------------------------------------------------------------
TFolderDialogOptions * __fastcall TSelectFolderDialog::GetOptions(void)
{
	return &FOptions;
}
//---------------------------------------------------------------------------
void __fastcall TSelectFolderDialog::SetOptions(const TFolderDialogOptions &Opts)
{
	FOptions=Opts;
}
//---------------------------------------------------------------------------
bool __fastcall TSelectFolderDialog::Execute(HWND Owner)
{
	FFolder = L"";
	if(!FOptions.UseLegacy)
	{
		bool canceled = false;
		if(ExecuteVista(Owner, canceled))
			return true;

		if(canceled)
			return false; // user canceled -> do not show legacy dialog
	}
	return ExecuteLegacy(Owner); // only when Vista path unavailable / hard failure
}
//---------------------------------------------------------------------------
bool __fastcall TSelectFolderDialog::ExecuteVista(HWND Owner,bool &userCanceled)
{
	typedef HRESULT (WINAPI *TSHCreateItemFromParsingName)(PCWSTR,IBindCtx *,REFIID,void **);

	userCanceled=false;
	#if !defined(_WIN32_IE) || (_WIN32_IE) <= 0x0600
	#define NO_VISTA
	#endif
	
	#ifndef NO_VISTA
	#define VISTA
	#endif
	
	#ifdef NO_VISTA
	{
		bool Res=ExecuteLegacy(Owner);
		userCanceled=!Res;
		return Res;
	}
	#endif//VISTA

	#ifdef VISTA
	HRESULT hrCI = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileDialog *pdlg=NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pdlg));
	if(FAILED(hr) || !pdlg)
	{
		if(SUCCEEDED(hrCI)) CoUninitialize();
		return false;
	}

	// Options
	DWORD opts=0;
	if(SUCCEEDED(pdlg->GetOptions(&opts)))
	{
		opts |= FOS_PICKFOLDERS;
		if(FOptions.ForceFileSystem) opts |= FOS_FORCEFILESYSTEM;
		if(FOptions.PathMustExist)   opts |= FOS_PATHMUSTEXIST;
		opts |= FOS_NOCHANGEDIR; // keep process CWD stable (optional)
		pdlg->SetOptions(opts);
	}

	// Title
	if(!FTitle.IsEmpty())
	{
		pdlg->SetTitle(FTitle.c_str());
	}

	// Initial folder
	if(!FInitialDir.IsEmpty())
	{
		IShellItem *psi=NULL;
		HMODULE shell32 = ::GetModuleHandleW(L"shell32.dll");
		bool unloadShell32 = false;
		if(!shell32)
		{
			shell32 = ::LoadLibraryW(L"shell32.dll");
			unloadShell32 = (shell32!=NULL);
		}

		TSHCreateItemFromParsingName pSHCreateItemFromParsingName = NULL;
		if(shell32)
		{
			pSHCreateItemFromParsingName =
				(TSHCreateItemFromParsingName)::GetProcAddress(shell32,"SHCreateItemFromParsingName");
		}

		if(pSHCreateItemFromParsingName)
		{
			hr = pSHCreateItemFromParsingName(FInitialDir.c_str(), NULL, IID_PPV_ARGS(&psi));
		}
		else
		{
			hr = E_NOTIMPL;
		}

		if(SUCCEEDED(hr) && psi)
		{
			pdlg->SetFolder(psi);
			pdlg->SetDefaultFolder(psi);
			psi->Release();
		}

		if(unloadShell32)
		{
			::FreeLibrary(shell32);
		}
	}

	hr = pdlg->Show(Owner);
	if(FAILED(hr))
	{
		if(hr==HRESULT_FROM_WIN32(ERROR_CANCELLED)) userCanceled = true; // user canceled => no legacy fallback
		pdlg->Release();
		if(SUCCEEDED(hrCI)) CoUninitialize();
		return false;
	}

	IShellItem *result=NULL;
	hr = pdlg->GetResult(&result);
	if(SUCCEEDED(hr) && result)
	{
		LPWSTR psz=NULL;
		if(SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &psz)) && psz)
		{
			FFolder = UniString(psz);
			CoTaskMemFree(psz);
		}
		result->Release();
	}

	pdlg->Release();
	if(SUCCEEDED(hrCI)) CoUninitialize();

	return !FFolder.IsEmpty();
	#endif//VISTA
	
	#ifdef NO_VISTA
	#undef NO_VISTA
	#endif//NO_VISTA
	#ifdef VISTA
	#undef VISTA
	#endif//VISTA
}
//---------------------------------------------------------------------------
int __stdcall TSelectFolderDialog::BrowseCB(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData)
{
	if(uMsg==BFFM_INITIALIZED && lpData)
	{
		// lpData is a wchar_t* initial path
		::SendMessageW(hwnd,BFFM_SETSELECTIONW,(WPARAM)TRUE,(LPARAM)lpData);
	}
	return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TSelectFolderDialog::ExecuteLegacy(HWND Owner)
{
	wchar_t display[MAX_PATH+1]={0};
	HRESULT hrCI = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	bool hasComInit = SUCCEEDED(hrCI);

	BROWSEINFOW bi;
	memset(&bi,0,sizeof(bi));
	bi.hwndOwner=Owner;
	bi.pszDisplayName=display;
	bi.lpszTitle=FTitle.IsEmpty()?NULL:FTitle.c_str();

	// Flags
	UINT flags=BIF_RETURNONLYFSDIRS;
	// Use the newer dialog only when COM apartment initialization succeeded.
	if(hasComInit) flags|=BIF_NEWDIALOGSTYLE;
	if(FOptions.ShowEditBox)      flags|=BIF_EDITBOX;
	if(!FOptions.NewFolderButton) flags|=BIF_NONEWFOLDERBUTTON;
	bi.ulFlags=flags;

	// Initial folder via callback
	bi.lpfn=BrowseCB;
	bi.lParam=(LPARAM)FInitialDir.c_str();

	LPITEMIDLIST pidl=::SHBrowseForFolderW(&bi);
	if(pidl==NULL)
	{
		if(hasComInit) CoUninitialize();
		return false;
	}

	wchar_t path[MAX_PATH+1]={0};
	BOOL ok=::SHGetPathFromIDListW(pidl,path);
	IMalloc *pm=NULL;
	if(SUCCEEDED(::SHGetMalloc(&pm)) && pm){ pm->Free(pidl); pm->Release(); }
	if(hasComInit) CoUninitialize();

	if(!ok || path[0]==L'\0') return false;
	FFolder=UniString(path);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static UINT LastBandID=1;
__fastcall ToolBar::ToolBar(HWND AParent,const UniString &ACaption, UINT StripResID, int ImgW, int ImgH)
{
	FCaption   = ACaption;
	FBaseIndex = -1;
	FOwnsImageList=false;
	FParent=AParent;
	FHandle    = ::CreateWindowExW(
		0,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_NODIVIDER,
		0, 0, 0, 0,
		FParent,
		NULL,
		hGlobalInstance,
		NULL
	);
	if (!FHandle) return;
	SubclassToolBar();

	// Let the rebar fully control placement
	LONG_PTR style = ::GetWindowLongPtr(FHandle, GWL_STYLE);
	style |= CCS_NOPARENTALIGN;
	style &= ~CCS_NORESIZE;
	::SetWindowLongPtr(FHandle, GWL_STYLE, style);

	::SendMessage(FHandle, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	::SendMessage(FHandle, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS|TBSTYLE_EX_MIXEDBUTTONS);

	// 1) Create a masked imagelist at the cell size
	FImageList = ::ImageList_Create(ImgW, ImgH, ILC_COLOR24 | ILC_MASK, 10 /*init*/, 4 /*grow*/);
	FOwnsImageList=(FImageList!=NULL);
	if (!FImageList) return;

	// 2) Load your strip BMP (one bitmap with N images side-by-side)
	HBITMAP hBmp = (HBITMAP)LoadImageW(
		hGlobalInstance, MAKEINTRESOURCEW(StripResID),
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (!hBmp) return;

	// 3) Pick the mask color — top-left pixel is common
	COLORREF mask = GetTopLeftPixelColor(hBmp);

	// 4) Add the strip to the imagelist with that mask color
	//    ImageList_AddMasked splits the strip into cells automatically
	int first = ImageList_AddMasked(FImageList, hBmp, mask);

	// 5) Attach the imagelist to the toolbar (normal state list index 0)
	SendMessage(FHandle, TB_SETIMAGELIST, 0, (LPARAM)FImageList);

	// 6) No need to keep hBmp around after copying into the imagelist
	DeleteObject(hBmp);

	// 7) Base index for buttons is 0 with our imagelist
	FBaseIndex = 0;

	::SendMessage(FHandle, TB_AUTOSIZE, 0, 0);
	::UpdateWindow(FHandle);

	// --- Insert as a band in the rebar ---
	SIZE tbMax = {0};
	::SendMessage(FHandle, TB_GETMAXSIZE, 0, (LPARAM)&tbMax);
	if (tbMax.cx == 0 || tbMax.cy == 0)
	{
		RECT rc{}; ::GetWindowRect(FHandle, &rc);
		tbMax.cx = (rc.right - rc.left) ? (rc.right - rc.left) : 200;
		tbMax.cy = (rc.bottom - rc.top) ? (rc.bottom - rc.top) : 28;
	}

	int bandCount = (int)::SendMessage(FParent, RB_GETBANDCOUNT, 0, 0);

	REBARBANDINFO rbi;
	memset(&rbi, 0, sizeof(REBARBANDINFO));
	rbi.cbSize       = sizeof(rbi);
	rbi.fMask        = RBBIM_CHILD | RBBIM_LPARAM | RBBIM_ID | RBBIM_CHILDSIZE | RBBIM_STYLE;
	rbi.hwndChild    = FHandle;
	rbi.lParam       = (LPARAM)this;
	BandID           = ++LastBandID;
	rbi.wID          = BandID;
	rbi.cyMinChild   = tbMax.cy;
	rbi.cxMinChild   = 32;
	rbi.fStyle       = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
	if (bandCount > 0) rbi.fStyle |= RBBS_BREAK;

	#ifdef RBBIM_IDEALSIZE
	rbi.fMask      |= RBBIM_IDEALSIZE;
	rbi.cxIdeal     = tbMax.cx;   // preferred width (rebar can shrink and show chevron)
	#endif

	::SetLastError(0);
	LRESULT ok = ::SendMessage(FParent, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbi);
	if (!ok)
	{
		rbi.cbSize = REBARBANDINFO_V3_SIZE;
		::SetLastError(0);
		ok = ::SendMessage(FParent, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbi);
		if (!ok)
		{
			UniString Mess = TransGLEM(::GetLastError());
			::MessageBoxW(::GetAncestor(FParent,GA_ROOT),Mess.c_str(), L"Error", MB_OK);
		}
	}
	::SendMessage(FParent, RB_AUTOSIZE, 0, 0);
}
//---------------------------------------------------------------------------
__fastcall ToolBar::ToolBar(HWND AParent,const UniString &ACaption,UINT ResID,HINSTANCE hInstResource)
{
	FCaption=ACaption;
	FBaseIndex=0;
	FImageList=NULL;
	FOwnsImageList=false;
	FParent=AParent;
	FHandle=::CreateWindowExW(
		0,
		TOOLBARCLASSNAME,
		NULL,//FCaption.c_str(),
		WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS|TBSTYLE_FLAT|CCS_NODIVIDER,
		0,0,0,0,
		FParent,
		NULL,
		hGlobalInstance,
		NULL
	);
	if(!FHandle)return;
	SubclassToolBar();
	
	LONG_PTR style = ::GetWindowLongPtr(FHandle, GWL_STYLE);
	style |= CCS_NOPARENTALIGN;           // let rebar position it
	style &= ~CCS_NORESIZE;
	::SetWindowLongPtr(FHandle, GWL_STYLE, style);
	
	// Optional niceties:

	::SendMessage(FHandle,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
	::SendMessage(FHandle,TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS|TBSTYLE_EX_MIXEDBUTTONS);
	::SendMessage(FHandle,TB_LOADIMAGES,(WPARAM)ResID,(LPARAM)hInstResource);
	
	::SendMessage(FHandle,TB_AUTOSIZE,0,0);
	
	::UpdateWindow(FHandle);
	
	SIZE tbMax = {0};
	SendMessage(FHandle, TB_GETMAXSIZE, 0, (LPARAM)&tbMax);
	// Fallbacks if TB_GETMAXSIZE returns 0
	if(tbMax.cx == 0 || tbMax.cy == 0)
	{
		RECT rc{}; GetWindowRect(FHandle, &rc);
		tbMax.cx = (rc.right - rc.left) ? (rc.right - rc.left) : 200;
		tbMax.cy = (rc.bottom - rc.top) ? (rc.bottom - rc.top) : 28;
	}

	int bandCount=(int)::SendMessage(FParent, RB_GETBANDCOUNT, 0, 0);
	
	REBARBANDINFO rbi;
	memset(&rbi,0,sizeof(REBARBANDINFO));
	rbi.cbSize=sizeof(rbi);
	rbi.fMask|=RBBIM_CHILD;
	rbi.hwndChild=FHandle;
	//rbi.fMask|=RBBIM_TEXT;
	//rbi.lpText=FCaption.c_str();
	//rbi.cch=FCaption.Length();
	rbi.fMask|=RBBIM_LPARAM;
	rbi.lParam=(LPARAM)this;
	BandID=++LastBandID;
	rbi.fMask|=RBBIM_ID;
	rbi.wID=BandID;
	rbi.fMask|=RBBIM_CHILDSIZE;
	rbi.cyMinChild=tbMax.cy;
	rbi.cxMinChild=32;
	//rbi.fMask|=RBBIM_SIZE;
	//rbi.cx=tbMax.cx;   
	rbi.fMask|=RBBIM_STYLE;
	rbi.fStyle=RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
	//rbi.fStyle|=RBBS_USECHEVRON;
	if(bandCount>0)
	{
		rbi.fStyle|=RBBS_BREAK;
	}
	//rbi.fStyle|=RBBS_BREAK;
	#ifdef RBBIM_IDEALSIZE
	rbi.fMask|=RBBIM_IDEALSIZE;
	rbi.cxIdeal=tbMax.cx;  // preferred width
	#endif
	::SetLastError(0);
	LRESULT res=::SendMessage(FParent,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbi);
	if(!res)
	{
		rbi.cbSize = REBARBANDINFO_V3_SIZE;
		::SetLastError(0);
		res=::SendMessage(FParent,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbi);
		if(!res)
		{
			UniString Mess=TransGLEM(::GetLastError());
			::MessageBoxW(::GetAncestor(FParent,GA_ROOT),Mess.c_str(),L"Error",MB_OK);
		}
	}
	::SendMessage(FParent,RB_AUTOSIZE,0,0);
}
//---------------------------------------------------------------------------
__fastcall ToolBar::~ToolBar(void)
{
	FButtonToolTips.clear();
	::DestroyWindow(FHandle);
	if(FImageList && FOwnsImageList)
	{
		::ImageList_Destroy(FImageList);
	}
}
//---------------------------------------------------------------------------
COLORREF __fastcall ToolBar::GetTopLeftPixelColor(HBITMAP hBmp)
{
	if(!hBmp)
	{
		return CLR_INVALID;
	}
	HDC memDC=::CreateCompatibleDC(NULL);
	if(!memDC)
	{
		return CLR_INVALID;
	}
	HGDIOBJ old=::SelectObject(memDC,hBmp);
	if(old==NULL || old==HGDI_ERROR)
	{
		::DeleteDC(memDC);
		return CLR_INVALID;
	}
	COLORREF c=::GetPixel(memDC,0,0);
	::SelectObject(memDC,old);
	::DeleteDC(memDC);
	return c;
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::UpdateMetrics(void)
{
	// Recompute toolbar size
	::SendMessage(FHandle, TB_AUTOSIZE, 0, 0);

	SIZE max = {};
	::SendMessage(FHandle, TB_GETMAXSIZE, 0, (LPARAM)&max);
	if(max.cx==0 || max.cy==0){
		RECT rc{}; ::GetWindowRect(FHandle, &rc);
		max.cx = max.cx ? max.cx : (rc.right-rc.left);
		max.cy = max.cy ? max.cy : (rc.bottom-rc.top);
		if(max.cx==0) max.cx = 200;
		if(max.cy==0) max.cy = 28;
	}

	// Update the band with new constraints
	REBARBANDINFO bi = {};
	bi.cbSize = sizeof(bi);
	bi.fMask  = RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
	bi.cyMinChild  = max.cy;
	bi.cxMinChild  = 32;      // at least one button visible
	bi.cx          = max.cx+13;  // current width
	bi.cxIdeal     = max.cx+13;  // preferred width

	int index = (int)SendMessage(FParent, RB_IDTOINDEX, (WPARAM)BandID, 0);
	if(index >= 0) {
		SendMessage(FParent, RB_SETBANDINFO, (WPARAM)index, (LPARAM)&bi);
	}

	SendMessage(FParent, RB_AUTOSIZE, 0, 0);
	RepositionHosted();
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::RepositionHosted(void)
{
	for(auto &hc : FHosted)
	{
		RECT rcItem{};
		if(!::SendMessage(FHandle, TB_GETITEMRECT, (WPARAM)hc.Index, (LPARAM)&rcItem))
			continue;

		int x = rcItem.left;
		int y = rcItem.top;
		int h = rcItem.bottom - rcItem.top;

		// place label (if any)
		int used = 0;
		if(hc.hLabel)
		{
			RECT rLbl{}; ::GetWindowRect(hc.hLabel, &rLbl);
			int wLbl = rLbl.right - rLbl.left;
			::SetWindowPos(hc.hLabel, NULL, x, y + (h - (rLbl.bottom - rLbl.top))/2, wLbl, rLbl.bottom - rLbl.top,
			               SWP_NOZORDER | SWP_NOACTIVATE);
			used = wLbl + 6; // 6px gap after label
			::ShowWindow(hc.hLabel, SW_SHOW);
		}

		// place control
		RECT rCtl{}; ::GetWindowRect(hc.hCtrl, &rCtl);
		int wCtl = rCtl.right - rCtl.left;
		int hCtl = rCtl.bottom - rCtl.top;

		::SetWindowPos(hc.hCtrl, NULL,
		               x + used, y + (h - hCtl)/2, wCtl, hCtl,
		               SWP_NOZORDER | SWP_NOACTIVATE);
		::ShowWindow(hc.hCtrl, SW_SHOW);
	}
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::EnsureImageListForDynamicIcons(int RequiredW,int RequiredH)
{
	if(!::IsWindow(FHandle))return false;
	if(FImageList!=NULL)return true;

	HIMAGELIST existing=(HIMAGELIST)::SendMessage(FHandle,TB_GETIMAGELIST,0,0);
	if(existing)
	{
		FImageList=existing;
		FOwnsImageList=false;
		return true;
	}

	int w=RequiredW;
	int h=RequiredH;
	if(w<=0 || h<=0)
	{
		DWORD bs=(DWORD)::SendMessage(FHandle,TB_GETBUTTONSIZE,0,0);
		if(w<=0)w=(int)LOWORD(bs);
		if(h<=0)h=(int)HIWORD(bs);
	}
	if(w<=0)w=16;
	if(h<=0)h=16;

	FImageList=::ImageList_Create(w,h,ILC_COLOR32|ILC_MASK,4,4);
	if(!FImageList)
	{
		FImageList=::ImageList_Create(w,h,ILC_COLOR24|ILC_MASK,4,4);
	}
	if(!FImageList)return false;

	FOwnsImageList=true;
	::SendMessage(FHandle,TB_SETIMAGELIST,0,(LPARAM)FImageList);
	return true;
}
//---------------------------------------------------------------------------
static int __fastcall FindToolbarButtonIndexByActionID(HWND hToolbar,UINT ActionID)
{
	if(!::IsWindow(hToolbar))return -1;
	int count=(int)::SendMessage(hToolbar,TB_BUTTONCOUNT,0,0);
	for(int i=0;i<count;i++)
	{
		TBBUTTON b={0};
		if(::SendMessage(hToolbar,TB_GETBUTTON,(WPARAM)i,(LPARAM)&b))
		{
			if((UINT)b.dwData==ActionID)return i;
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::SetButtonToolTipText(UINT CommandID,const UniString &ToolTip)
{
	for(size_t i=0;i<FButtonToolTips.size();i++)
	{
		if(FButtonToolTips[i].CommandID==CommandID)
		{
			FButtonToolTips[i].ToolTip=ToolTip;
			return;
		}
	}
	TButtonToolTip t={0};
	t.CommandID=CommandID;
	t.ToolTip=ToolTip;
	FButtonToolTips.push_back(t);
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::RemoveButtonToolTipText(UINT CommandID)
{
	for(size_t i=0;i<FButtonToolTips.size();)
	{
		if(FButtonToolTips[i].CommandID==CommandID)
		{
			FButtonToolTips.erase(FButtonToolTips.begin()+i);
		}
		else
		{
			i++;
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::TryGetButtonToolTipText(UINT CommandID,UniString &OutToolTip) const
{
	for(size_t i=0;i<FButtonToolTips.size();i++)
	{
		if(FButtonToolTips[i].CommandID==CommandID)
		{
			OutToolTip=FButtonToolTips[i].ToolTip;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::AddButton(UINT ImgIdx,UINT ID,const UniString &ToolTip)
{
	SetButtonToolTipText(ID,ToolTip);
	TBBUTTON tbb={0};
	tbb.fsState=TBSTATE_ENABLED;
	tbb.fsStyle=TBSTYLE_BUTTON;
	tbb.iBitmap=FBaseIndex+ImgIdx;
	tbb.idCommand=ID;
	::SendMessage(FHandle,TB_ADDBUTTONS,1,(LPARAM)&tbb);
	UpdateMetrics();
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::AddDropdownButton(UINT ImgIdx,UINT ID,const UniString &ToolTip)
{
	SetButtonToolTipText(ID,ToolTip);
	TBBUTTON tbb={0};
	tbb.fsState=TBSTATE_ENABLED;
	tbb.fsStyle=TBSTYLE_BUTTON|TBSTYLE_DROPDOWN;
	tbb.iBitmap=FBaseIndex+ImgIdx;
	tbb.idCommand=ID;
	::SendMessage(FHandle,TB_ADDBUTTONS,1,(LPARAM)&tbb);
	UpdateMetrics();
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::AddDynamicButton(UINT ActionID,UINT ImgIdx,UINT CommandID,const UniString &ToolTip,bool IsDropdown)
{
	if(!::IsWindow(FHandle))return false;
	if(ActionID==0 || CommandID==0)return false;

	RemoveDynamicButton(ActionID);
	SetButtonToolTipText(CommandID,ToolTip);

	TBBUTTON tbb={0};
	tbb.fsState=TBSTATE_ENABLED;
	tbb.fsStyle=(BYTE)(TBSTYLE_BUTTON|(IsDropdown?TBSTYLE_DROPDOWN:0));
	tbb.iBitmap=FBaseIndex+ImgIdx;
	tbb.idCommand=CommandID;
	tbb.dwData=(DWORD_PTR)ActionID;
	LRESULT ok=::SendMessage(FHandle,TB_ADDBUTTONS,1,(LPARAM)&tbb);
	if(!ok)return false;

	FDynamicActionIDs.push_back(ActionID);
	UpdateMetrics();
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::AddDynamicButtonIcon(UINT ActionID,HICON AIcon,UINT CommandID,const UniString &ToolTip,bool IsDropdown)
{
	if(!::IsWindow(FHandle))return false;
	if(ActionID==0 || CommandID==0 || AIcon==NULL)return false;
	if(!EnsureImageListForDynamicIcons())return false;

	int img=::ImageList_ReplaceIcon(FImageList,-1,AIcon);
	if(img<0)return false;

	if(!AddDynamicButton(ActionID,(UINT)img,CommandID,ToolTip,IsDropdown))
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::RemoveDynamicButton(UINT ActionID)
{
	if(!::IsWindow(FHandle) || ActionID==0)return false;
	int idx=FindToolbarButtonIndexByActionID(FHandle,ActionID);
	if(idx<0)return false;
	TBBUTTON b={0};
	if(!::SendMessage(FHandle,TB_GETBUTTON,(WPARAM)idx,(LPARAM)&b))return false;
	LRESULT ok=::SendMessage(FHandle,TB_DELETEBUTTON,(WPARAM)idx,0);
	if(!ok)return false;
	RemoveButtonToolTipText((UINT)b.idCommand);
	for(size_t i=0;i<FDynamicActionIDs.size();)
	{
		if(FDynamicActionIDs[i]==ActionID)
		{
			FDynamicActionIDs.erase(FDynamicActionIDs.begin()+i);
		}
		else
		{
			i++;
		}
	}
	UpdateMetrics();
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::RemoveDynamicButtonByCommand(UINT CommandID)
{
	if(!::IsWindow(FHandle) || CommandID==0)return false;
	int idx=(int)::SendMessage(FHandle,TB_COMMANDTOINDEX,(WPARAM)CommandID,0);
	if(idx<0)return false;
	TBBUTTON b={0};
	if(!::SendMessage(FHandle,TB_GETBUTTON,(WPARAM)idx,(LPARAM)&b))return false;
	UINT actionID=(UINT)b.dwData;
	LRESULT ok=::SendMessage(FHandle,TB_DELETEBUTTON,(WPARAM)idx,0);
	if(!ok)return false;
	RemoveButtonToolTipText(CommandID);
	if(actionID!=0)
	{
		for(size_t i=0;i<FDynamicActionIDs.size();)
		{
			if(FDynamicActionIDs[i]==actionID)
			{
				FDynamicActionIDs.erase(FDynamicActionIDs.begin()+i);
			}
			else
			{
				i++;
			}
		}
	}
	UpdateMetrics();
	return true;
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::ClearDynamicButtons(void)
{
	if(!::IsWindow(FHandle))
	{
		FDynamicActionIDs.clear();
		FButtonToolTips.clear();
		return;
	}
	while(!FDynamicActionIDs.empty())
	{
		UINT id=FDynamicActionIDs.front();
		while(RemoveDynamicButton(id)){}
	}
	FDynamicActionIDs.clear();
}
//---------------------------------------------------------------------------
bool __fastcall ToolBar::HasDynamicButton(UINT ActionID) const
{
	for(size_t i=0;i<FDynamicActionIDs.size();i++)
	{
		if(FDynamicActionIDs[i]==ActionID)return true;
	}
	return false;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK ToolBar::ControlMessageProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HANDLE hParent=::GetProp(hwnd,PROP_PARENTHWND);
	HANDLE hProc=::GetProp(hwnd,PROP_WNDPROC);
	switch(msg)
	{
		case WM_NCDESTROY:
		::SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG_PTR)hProc);
		::RemovePropW(hwnd,PROP_WNDPROC);
		::RemovePropW(hwnd,PROP_PARENTHWND);
		break;
	}
	return ::CallWindowProc((WNDPROC)hProc,hwnd,msg,wParam,lParam);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK ToolBar::ToolBarMessageProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HANDLE hProc=::GetProp(hwnd,PROP_WNDPROC);
	ToolBar *This=(ToolBar *)::GetProp(hwnd,PROP_TOOLBAROBJ);
	switch(msg)
	{
		case WM_COMMAND:
		{
			HWND from=(HWND)lParam;
			if(::IsWindow(from))
			{
				HWND parent=(HWND)::GetProp(from,PROP_PARENTHWND);
				if(::IsWindow(parent))
				{
					return ::SendMessage(parent, WM_COMMAND, wParam, (LPARAM)from);
				}
			}
		}
		break;
		case WM_NOTIFY:
		{
			NMHDR *nm=(NMHDR *)lParam;
			if(nm && This && nm->code==TTN_GETDISPINFOW)
			{
				NMTTDISPINFOW *di=(NMTTDISPINFOW *)lParam;
				UINT cmd=(UINT)di->hdr.idFrom;
				if(di->uFlags & TTF_IDISHWND)
				{
					cmd=(UINT)::GetDlgCtrlID((HWND)di->hdr.idFrom);
				}
				UniString tip;
				if(This->TryGetButtonToolTipText(cmd,tip))
				{
					This->FToolTipScratch=tip;
					di->lpszText=(LPWSTR)This->FToolTipScratch.c_str();
					return TRUE;
				}
			}
			if(nm)
			{
				if(::IsWindow(nm->hwndFrom))
				{
					HWND parent=(HWND)::GetProp(nm->hwndFrom,PROP_PARENTHWND);
					if(::IsWindow(parent))
					{
						NMHDR nmn=*nm;
						nmn.idFrom=(UINT_PTR)::GetDlgCtrlID((HWND)nmn.hwndFrom);
						LRESULT RES=::SendMessage(parent,WM_NOTIFY,(WPARAM)nmn.idFrom,(LPARAM)&nmn);
						return RES;
					}
				}
			}
		}
		break;
		case WM_NCDESTROY:
		::SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG_PTR)hProc);
		::RemovePropW(hwnd,PROP_WNDPROC);
		::RemovePropW(hwnd,PROP_TOOLBAROBJ);
		break;
	}
	return ::CallWindowProc((WNDPROC)hProc,hwnd,msg,wParam,lParam);
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::SubclassToolBar(void)
{
	if(!::IsWindow(FHandle))return;
	LONG PROC=::GetWindowLongPtr(FHandle,GWLP_WNDPROC);
	::SetProp(FHandle,PROP_WNDPROC,(HANDLE)PROC);
	::SetProp(FHandle,PROP_TOOLBAROBJ,(HANDLE)this);
	LONG NEWPROC=(LONG)ToolBarMessageProc;
	::SetWindowLongPtr(FHandle,GWLP_WNDPROC,NEWPROC);
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::AddControl(const UniString &Caption, HWND hwnd)
{
	if(!::IsWindow(FHandle) || !::IsWindow(hwnd)) return;

	::SetProp(hwnd,PROP_PARENTHWND,::GetParent(hwnd));
	LONG PROC=::GetWindowLongPtr(hwnd,GWLP_WNDPROC);
	::SetProp(hwnd,PROP_WNDPROC,(HANDLE)PROC);
	LONG NEWPROC=(LONG)ControlMessageProc;
	::SetWindowLongPtr(hwnd,GWLP_WNDPROC,NEWPROC);
	
	// Make sure the control lives inside the toolbar
	::SetParent(hwnd, FHandle);
	::ShowWindow(hwnd, SW_SHOWNOACTIVATE);

	// Measure control size
	RECT rcCtl{}; ::GetWindowRect(hwnd, &rcCtl);
	int wCtl = rcCtl.right - rcCtl.left;
	int hCtl = rcCtl.bottom - rcCtl.top;

	// Optional label
	HWND hLbl = NULL;
	int  wLbl = 0;
	if(Caption.Length() > 0)
	{
		hLbl = ::CreateWindowExW(
			0, L"STATIC", Caption.c_str(),
			WS_CHILD | WS_VISIBLE,
			0,0,0,0,
			FHandle, NULL, hGlobalInstance, NULL
		);
		if(hLbl)
		{
			// Use toolbar font for size correctness
			HFONT hFont = (HFONT)::SendMessage(FHandle, WM_GETFONT, 0, 0);
			if(hFont) ::SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);

			RECT rcLbl{0,0,0,0};
			// Rough width: ask text extent via DC
			HDC hdc = ::GetDC(FHandle);
			HGDIOBJ old = NULL;
			if(hFont) old = ::SelectObject(hdc, hFont);
			SIZE sz{}; ::GetTextExtentPoint32W(hdc, Caption.c_str(), (int)Caption.Length(), &sz);
			if(old) ::SelectObject(hdc, old);
			::ReleaseDC(FHandle, hdc);

			wLbl = sz.cx; // height not critical; we'll center vertically
			if(wLbl < 4) wLbl = 4;
		}
	}

	const int gap = (hCtl > 0) ? 6 : 0; // gap between label and control
	const int totalWidth = wLbl + gap + wCtl;

	// Reserve space with a separator button
	TBBUTTON sep{}; 
	sep.fsStyle  = TBSTYLE_SEP;
	sep.iBitmap  = totalWidth;   // width, in pixels, for a horizontal toolbar
	sep.idCommand= 0;

	::SendMessage(FHandle, TB_ADDBUTTONS, 1, (LPARAM)&sep);
	::SendMessage(FHandle, TB_AUTOSIZE, 0, 0);

	// The separator is added at the end; get its index
	int count = (int)::SendMessage(FHandle, TB_BUTTONCOUNT, 0, 0);
	int sepIndex = count - 1;

	// Position label/control into that rectangle
	RECT rcItem{}; 
	::SendMessage(FHandle, TB_GETITEMRECT, (WPARAM)sepIndex, (LPARAM)&rcItem);

	int slotH = rcItem.bottom - rcItem.top;
	int x     = rcItem.left;
	int y     = rcItem.top;

	// place label
	if(hLbl)
	{
		RECT rLbl{}; ::GetWindowRect(hLbl, &rLbl);
		int hLblPx = rLbl.bottom - rLbl.top;
		if(hLblPx <= 0) hLblPx = slotH;
		::SetWindowPos(hLbl, NULL, x, y + (slotH - hLblPx)/2, wLbl, hLblPx,
		               SWP_NOZORDER | SWP_NOACTIVATE);
		::ShowWindow(hLbl, SW_SHOWNOACTIVATE);
		x += wLbl + gap;
	}

	// place control
	::SetWindowPos(hwnd, NULL, x, y + (slotH - hCtl)/2, wCtl, hCtl,
	               SWP_NOZORDER | SWP_NOACTIVATE);
	::ShowWindow(hwnd, SW_SHOWNOACTIVATE);

	// Remember so we can reposition later
	THostedCtrl hc{};
	hc.Index  = sepIndex;
	hc.hCtrl  = hwnd;
	hc.hLabel = hLbl;
	hc.Width  = totalWidth;
	FHosted.push_back(hc);

	// Let the rebar/toolbar recompute heights
	::SendMessage(FParent, RB_AUTOSIZE, 0, 0);
	UpdateMetrics();
}
//---------------------------------------------------------------------------
void __fastcall ToolBar::AddSeparator(void)
{
	TBBUTTON tbb={0};
	tbb.fsState=0;
	tbb.fsStyle=TBSTYLE_SEP;
	tbb.iBitmap=0;
	tbb.idCommand=-1;
	::SendMessage(FHandle,TB_ADDBUTTONS,1,(LPARAM)&tbb);
	UpdateMetrics();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Create a 32-bpp DIB HBITMAP from a horizontal bitmap strip resource.
// - ResId:        BITMAP resource id of the strip
// - ButtonIndex:  zero-based index of the cell within the strip
// - ButtonWidth:  width of a single cell
// - ButtonHeight: height of a single cell
// Transparency:
//   Uses the TOP-LEFT pixel of the ENTIRE source bitmap as the color key.
//   Any pixel matching that color in the selected cell gets alpha=0; others alpha=255.
// Notes:
//   Returns NULL on failure. Caller must DeleteObject() the returned HBITMAP.
//---------------------------------------------------------------------------
HBITMAP __fastcall CreateAlphaBitmapFromStrip(int ResId,int ButtonIndex,int ButtonWidth,int ButtonHeight)
{
	HINSTANCE hInst=::GetModuleHandleW(NULL);

	HBITMAP hSrcBmp=(HBITMAP)::LoadImageW(hInst,MAKEINTRESOURCEW(ResId),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	if(!hSrcBmp)
		return NULL;

	BITMAP bm={0};
	if(!::GetObjectW(hSrcBmp,sizeof(bm),&bm))
	{
		::DeleteObject(hSrcBmp);
		return NULL;
	}

	// Validate source bounds
	const int SrcLeft=ButtonIndex*ButtonWidth;
	const int SrcTop=0;
	if(ButtonWidth<=0 || ButtonHeight<=0 || SrcLeft<0 || (SrcLeft+ButtonWidth)>bm.bmWidth || ButtonHeight>bm.bmHeight)
	{
		::DeleteObject(hSrcBmp);
		return NULL;
	}

	// Create destination 32-bpp DIB section (BGRA)
	BITMAPINFO bi={0};
	bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth=ButtonWidth;
	bi.bmiHeader.biHeight=-ButtonHeight; // top-down
	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=32;
	bi.bmiHeader.biCompression=BI_RGB;

	void *DestBits=nullptr;
	HBITMAP hDstBmp=::CreateDIBSection(NULL,&bi,DIB_RGB_COLORS,&DestBits,NULL,0);
	if(!hDstBmp)
	{
		::DeleteObject(hSrcBmp);
		return NULL;
	}

	// Blit source cell into destination, then apply colorkey -> alpha
	HDC hdcSrc=::CreateCompatibleDC(NULL);
	HDC hdcDst=::CreateCompatibleDC(NULL);
	HGDIOBJ oldSrc=::SelectObject(hdcSrc,hSrcBmp);
	HGDIOBJ oldDst=::SelectObject(hdcDst,hDstBmp);

	// Grab transparent color from the top-left of the ENTIRE bitmap
	const COLORREF Key=::GetPixel(hdcSrc,0,0);

	// Copy the cell
	::BitBlt(hdcDst,0,0,ButtonWidth,ButtonHeight,hdcSrc,SrcLeft,SrcTop,SRCCOPY);

	// Walk pixels to set alpha (0 for key, 255 otherwise)
	UINT32 *p=(UINT32 *)DestBits;
	const int Count=ButtonWidth*ButtonHeight;
	const BYTE KeyB=GetBValue(Key);
	const BYTE KeyG=GetGValue(Key);
	const BYTE KeyR=GetRValue(Key);

	for(int i=0;i<Count;i++)
	{
		UINT32 px=p[i]; // BGRA in memory after BitBlt to 32-bpp DIB
		BYTE b=(BYTE)(px & 0xFF);
		BYTE g=(BYTE)((px >> 8) & 0xFF);
		BYTE r=(BYTE)((px >> 16) & 0xFF);
		BYTE a=(BYTE)((px >> 24) & 0xFF);

		if(r==KeyR && g==KeyG && b==KeyB)
			a=0;     // fully transparent
		else
			a=255;   // fully opaque

		p[i]=(UINT32)(b | (g<<8) | (r<<16) | (a<<24));
	}

	// Cleanup DCs and source (we made our own copy)
	::SelectObject(hdcSrc,oldSrc);
	::SelectObject(hdcDst,oldDst);
	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDst);
	::DeleteObject(hSrcBmp);

	return hDstBmp;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
