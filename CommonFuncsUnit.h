//---------------------------------------------------------------------------

#ifndef CommonFuncsUnitH
#define CommonFuncsUnitH
#include "UniString.h"
#include <shlobj.h>
//---------------------------------------------------------------------------
namespace CommonFuncs {
//---------------------------------------------------------------------------
UniString __fastcall MBCSToUniString(const AnsiString &in);
AnsiString __fastcall UniStringToMBCS(const UniString &in);
AnsiString __fastcall EscapeXMLString(const AnsiString &in);
AnsiString __fastcall OmNomNomSD(AnsiString &Text,const AnsiString &Delim);
AnsiString __fastcall OmNomNom(AnsiString &Text);
UniString __fastcall OmNomNomSD(UniString &Text,const UniString &Delim);
UniString __fastcall OmNomNom(UniString &Text);
void __fastcall ChopToLen(UniString &Text,int nlen);
void __fastcall ChopToLen(AnsiString &Text,int nlen);
AnsiString FileUnicodeToAnsi(const wchar_t *file);
AnsiString PathUnicodeToAnsi(const wchar_t *path);
UniString __fastcall FileShortToLongW(const UniString &Short);
typedef struct TPathParts{
        UniString Exists;
        UniString Phantom;
}TPathParts;
TPathParts __fastcall PathThatExists(const UniString &APath);
AnsiString __fastcall Trim(const AnsiString &S);
UniString __fastcall Trim(const UniString &S);
AnsiString __fastcall IntToHex(int value,int digits);
AnsiString __fastcall PointerToString(void *obj);
AnsiString __fastcall ExcludeTrailingBackslash(const AnsiString &In);
AnsiString __fastcall IncludeTrailingBackslash(const AnsiString &In);
UniString __fastcall ExcludeTrailingBackslash(const UniString &In);
UniString __fastcall IncludeTrailingBackslash(const UniString &In);
AnsiString __fastcall ExtractFilePath(const AnsiString &In);
AnsiString __fastcall ExtractFileDir(const AnsiString &In);
AnsiString __fastcall ExtractFileExt(const AnsiString &In);
AnsiString __fastcall ExtractFileDrive(const AnsiString &In);
AnsiString __fastcall ExtractFileName(const AnsiString &In);
AnsiString __fastcall ChangeFileExt(const AnsiString &In,const AnsiString &Ext);
UniString __fastcall ExtractFilePath(const UniString &In);
UniString __fastcall ExtractFileDir(const UniString &In);
UniString __fastcall ExtractFileExt(const UniString &In);
UniString __fastcall ExtractFileDrive(const UniString &In);
UniString __fastcall ExtractFileName(const UniString &In);
UniString __fastcall ChangeFileExt(const UniString &In,const UniString &Ext);
AnsiString __fastcall GetCurrentDir(void);
bool __fastcall SetCurrentDir(const AnsiString &Dir);
AnsiString __fastcall ExpandFileName(const AnsiString &FileName);
UniString __fastcall ExpandFileName(const UniString &FileName);
bool __fastcall DeleteFileW(const UniString &File);
bool __fastcall CopyFileW(const UniString &Src,const UniString &Dest);
bool __fastcall RemoveDirW(const UniString &Directory);
bool __fastcall ForceDirectories(const AnsiString &ADir);
bool __fastcall ForceDirectoriesW(const UniString &ADir);
bool __fastcall IsURL(const AnsiString &Resource);
AnsiString __fastcall ReadClipboardTextA(void);
UniString __fastcall ReadClipboardTextW(void);
void __fastcall WriteClipboardUnicodeData(const wchar_t *pUnicodeData);
AnsiString __fastcall uriEncode(const AnsiString &Text);
AnsiString __fastcall uriDecode(const AnsiString &In,bool PlusAsSpace=false,bool *HadError=NULL);
UniString __fastcall TransGLEM(DWORD value);
bool __fastcall FileExists(const AnsiString &File);
bool __fastcall FileExistsW(const UniString &File);
bool __fastcall DirectoryExists(const AnsiString &File);
bool __fastcall DirectoryExistsW(const UniString &File);
#ifndef fmCreate
static const short fmCreate = 0xffff;
#endif
#ifndef fmOpenRead
static const short fmOpenRead = 0x0;
static const short fmOpenWrite = 0x1;
static const short fmOpenReadWrite = 0x2;
static const short fmShareCompat = 0x0;
static const short fmShareExclusive = 0x10;
static const short fmShareDenyWrite = 0x20;
static const short fmShareDenyRead = 0x30;
static const short fmShareDenyNone = 0x40;
#endif
int __fastcall FileCreateW(const UniString &FileName);
int __fastcall FileCreate(const AnsiString &FileName);
int __fastcall FileOpenW(const UniString &FileName,int Mode);
int __fastcall FileOpen(const AnsiString &FileName,int Mode);
int __fastcall FileRead(int Handle, void *Buffer, int Count);
int __fastcall FileWrite(int Handle, const void *Buffer, int Count);
int __fastcall FileSeek(int Handle, int Offset, int Origin);
void __fastcall FileClose(int Handle);
UniString __fastcall GetCurrentDirW(void);
bool __fastcall SetCurrentDirW(const UniString &Dir);
AnsiString __fastcall GetSpecialFolderPathA(int CSIDL);
UniString __fastcall GetSpecialFolderPathW(int CSIDL);
UniString __fastcall GetExeDirectoryW(void);
bool __fastcall IsDirectoryWritableW(const UniString &dir);
const int faReadOnly=0x00000001;// Read-only files
const int faHidden=0x00000002;// Hidden files
const int faSysFile=0x00000004;// System files
const int faVolumeID=0x00000008;// Volume ID files
const int faDirectory=0x00000010;// Directory files
const int faArchive=0x00000020;// Archive files
const int faAnyFile=0x0000003F;// Any file
struct TSearchRecW
{
	int Time;
	int Size;
	int Attr;
	UniString Name;
	int ExcludeAttr;
	unsigned FindHandle;
	_WIN32_FIND_DATAW FindData;
} ;
int __fastcall FindFirstW(const UniString &Path,int Attr,TSearchRecW &F);
int __fastcall FindNextW(TSearchRecW &F);
void __fastcall FindCloseW(TSearchRecW &F);
UniString __fastcall GetWndTxt(HWND hwnd);
void __fastcall SetCtrlCheckedState(HWND hwndDlg,int ID,bool Checked);
bool __fastcall GetCtrlCheckedState(HWND hwndDlg,int ID);
void __fastcall SetComboBoxSel(HWND hwnd,int idx);
int __fastcall GetComboBoxSel(HWND hwnd);
UniString __fastcall TrimTrailingSlashPreserveRoot(const UniString &p);
bool __fastcall IsAbsolutePathW(const UniString &Path);
UniString __fastcall NormalizeSeparatorsW(const UniString &Path);
UniString __fastcall CollapseDotsW(const UniString &Path); // resolves "." and ".."
UniString __fastcall NormalizePathTextW(const UniString &Path); // both of the above + trims trailing "\" (except root)
UniString __fastcall NormalizeAgainstBaseAbsW(const UniString &Path, const UniString &BaseDir);
bool __fastcall IsSameOrUnderW(const UniString &BaseDir, const UniString &Candidate, bool *IsSameOpt=nullptr);
bool __fastcall TryMakeRelativeToW(const UniString &BaseDir, const UniString &TargetPath, UniString &OutRelative);
UniString __fastcall NormalizeUnixNewlines(const UniString &Text);
AnsiString __fastcall NormalizeUnixNewlines(const AnsiString &Text);
void __fastcall ProcessMessages(void);
UniString __fastcall MakeRelativeIfInFolder(const UniString &In, const UniString &BasePath);
void __fastcall DisableRoundedCorners(HWND hDlg);
UniString __fastcall NormalizePathForCompare(const UniString &In,const UniString &BaseDir=L"");
bool __fastcall PathsReferToSameFile(const UniString &A,const UniString &B,const UniString &BaseDir=L"",bool RequireExistsForStrongCheck=true);
AnsiString __fastcall NormalizePathForCompare(const AnsiString &In,const AnsiString &BaseDir=AnsiString());
bool __fastcall PathsReferToSameFile(const AnsiString &A,const AnsiString &B,const AnsiString &BaseDir=AnsiString(),bool RequireExistsForStrongCheck=true);
UniString __fastcall GetPathFromIDListW(void *PIDL);
UniString __fastcall GetFullPathFromOFNotifyPIDL(void* psf, void* pidl);
//---------------------------------------------------------------------------
}; //namespace CommonFuncs
//---------------------------------------------------------------------------
using namespace CommonFuncs;
//---------------------------------------------------------------------------
#endif

