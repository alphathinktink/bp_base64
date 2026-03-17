//---------------------------------------------------------------------------
#ifndef ItsAMysteryUnitH
#define ItsAMysteryUnitH
//---------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include "AnsiString.h"
#include "UniString.h"
#include <vector>
#include <commdlg.h>
//---------------------------------------------------------------------------
#ifndef RB_AUTOSIZE
#define RB_AUTOSIZE (WM_USER + 26)
#endif
//---------------------------------------------------------------------------
class TBPIniFile
{
private:        //
        UniString FFile;
protected:      //
public:         //
        __fastcall TBPIniFile(const UniString AFile);
        __fastcall ~TBPIniFile(void);
        void __fastcall DeleteKey(const UniString Section, const UniString Ident);
        int __fastcall ReadInteger(const UniString Section, const UniString Ident, int Default);
        void __fastcall WriteInteger(const UniString Section, const UniString Ident, int Value);
        bool __fastcall ReadBool(const UniString Section, const UniString Ident, bool Default);
        void __fastcall WriteBool(const UniString Section, const UniString Ident, bool Value);
        UniString __fastcall ReadString(const UniString Section, const UniString Ident, const UniString Default);
        void __fastcall WriteString(const UniString Section, const UniString Ident, const UniString Value);
        UniString __fastcall ReadSections(void);
        UniString __fastcall ReadSection(const UniString Section);
        void __fastcall EraseSection(const UniString Section);
        UniString __fastcall GetFile(void);
};
//---------------------------------------------------------------------------
#pragma option push -b-
typedef struct _TOpenSaveDialogOptions
{
	bool ReadOnly;
	bool OverwritePrompt;
	bool HideReadOnly;
	bool NoChangeDir;
	bool ShowHelp;
	bool NoValidate;
	bool AllowMultiSelect;
	bool ExtensionDifferent;
	bool PathMustExist;
	bool FileMustExist;
	bool CreatePrompt;
	bool ShareAware;
	bool NoReadOnlyReturn;
	bool NoTestFileCreate;
	bool NoNetworkButton;
	bool NoLongNames;
	bool OldStyleDialog;
	bool NoDereferenceLinks;
	bool EnableIncludeNotify;
	bool EnableSizing;
} TOpenSaveDialogOptions;
#pragma option pop
//---------------------------------------------------------------------------
class TOpenSaveDialog;
typedef UINT_PTR (__fastcall *TOpenSaveDialogCallback)(TOpenSaveDialog *Sender,HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam);
class TOpenSaveDialog
{
private:	//--
	std::vector<wchar_t> FBuffer;      // for multiselect filenames
	std::vector<wchar_t> FFilterBuf;   // double-NUL filter buffer (built from FVclFilter)
	UniString FVclFilter;              // "Desc|pattern|Desc2|pattern2"
	UniString FTitle;
	UniString FInitialDir;
	UniString FDefaultExt;
	UniString FFileName;
	std::vector<UniString> FFiles;
	TOpenSaveDialogOptions FOptions;
	TOpenSaveDialogCallback FCallback;

	DWORD __fastcall BuildFlags(void) const;
	void __fastcall ResetOFN(HWND Owner);
	void __fastcall EnsureBuffer(int MinChars);
	void __fastcall ParseResult(void);

	// convert "A|*.c;*.cpp|B|*.*" -> L"A\0*.c;*.cpp\0B\0*.*\0\0"
	void __fastcall BuildFilterFromVCL(void);

	static UINT_PTR CALLBACK HookOldStyleProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam);
	static UINT_PTR CALLBACK HookProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam);
protected:	//--
	virtual bool __fastcall DoExecute(void)=0;
	OPENFILENAMEW OFN;

public:		//--
	__fastcall TOpenSaveDialog(void);
	__fastcall ~TOpenSaveDialog(void);

	bool __fastcall Execute(HWND Owner);
	
	

	TOpenSaveDialogOptions * __fastcall GetOptions(void);
	void __fastcall SetOptions(const TOpenSaveDialogOptions &Opts);

	void __fastcall SetTitle(const UniString &Title);
	void __fastcall SetInitialDir(const UniString &Dir);
	void __fastcall SetDefaultExt(const UniString &ExtNoDot);
	void __fastcall SetFileName(const UniString &FileName);

	// VCL-style filter setter: "Source (*.cpp;*.c)|*.cpp;*.c|All (*.*)|*.*"
	void __fastcall SetFilter(const UniString &VclStyle);

	const UniString & __fastcall GetFileName(void) const;
	int __fastcall GetFileCount(void) const;
	bool __fastcall GetFileAt(int Index,UniString &Out) const;
	const std::vector<UniString> & __fastcall GetFiles(void) const;
	
	void __fastcall SetCallback(TOpenSaveDialogCallback ACallback){FCallback=ACallback;}
	TOpenSaveDialogCallback GetCallback(void){return FCallback;}
};
//---------------------------------------------------------------------------
class TOpenDialog : public TOpenSaveDialog
{
protected:	//--
	virtual bool __fastcall DoExecute(void);
public:		//--
	__fastcall TOpenDialog(void);
};
//---------------------------------------------------------------------------
class TSaveDialog : public TOpenSaveDialog
{
protected:	//--
	virtual bool __fastcall DoExecute(void);
public:		//--
	__fastcall TSaveDialog(void);
};
//---------------------------------------------------------------------------
#pragma option push -b-
typedef struct _TFolderDialogOptions
{
	bool UseLegacy;       // force SHBrowseForFolderW
	bool ShowEditBox;     // legacy only (BIF_EDITBOX)
	bool NewFolderButton; // legacy only (BIF_NONEWFOLDERBUTTON = false)
	bool PathMustExist;   // IFileDialog: FOS_PATHMUSTEXIST
	bool ForceFileSystem; // IFileDialog: FOS_FORCEFILESYSTEM
} TFolderDialogOptions;
#pragma option pop
//---------------------------------------------------------------------------
class TSelectFolderDialog
{
private:	//--
	UniString FTitle;
	UniString FInitialDir;
	UniString FFolder;
	TFolderDialogOptions FOptions;

	bool __fastcall ExecuteVista(HWND Owner,bool &userCanceled);
	bool __fastcall ExecuteLegacy(HWND Owner);

	// Legacy callback to set initial folder
	static int __stdcall BrowseCB(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

public:		//--
	__fastcall TSelectFolderDialog(void);
	__fastcall ~TSelectFolderDialog(void);

	bool __fastcall Execute(HWND Owner);

	void __fastcall SetTitle(const UniString &Title);
	void __fastcall SetInitialDir(const UniString &Dir);

	const UniString & __fastcall GetFolder(void) const;

	TFolderDialogOptions * __fastcall GetOptions(void);
	void __fastcall SetOptions(const TFolderDialogOptions &Opts);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ToolBar
{
	struct THostedCtrl
	{
		int Index;          // toolbar button index of the separator "slot"
		HWND hCtrl;         // the hosted control (e.g., combo box)
		HWND hLabel;        // optional static label (or NULL)
		int Width;          // total reserved width (label + gap + control)
	};	
private:
	HWND FHandle;
	HWND FParent;
	UniString FCaption;
	UINT BandID;
	int FBaseIndex;
	HIMAGELIST FImageList;
	bool FOwnsImageList;
	struct TButtonToolTip
	{
		UINT CommandID;
		UniString ToolTip;
	};
	std::vector<THostedCtrl> FHosted;
	std::vector<UINT> FDynamicActionIDs;
	std::vector<TButtonToolTip> FButtonToolTips;
	UniString FToolTipScratch;
	void __fastcall RepositionHosted(void);
	bool __fastcall EnsureImageListForDynamicIcons(int RequiredW=0,int RequiredH=0);
	void __fastcall SetButtonToolTipText(UINT CommandID,const UniString &ToolTip);
	void __fastcall RemoveButtonToolTipText(UINT CommandID);
	bool __fastcall TryGetButtonToolTipText(UINT CommandID,UniString &OutToolTip) const;
	void __fastcall SubclassToolBar(void);
	static LRESULT CALLBACK ControlMessageProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK ToolBarMessageProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
protected:
public:
	__fastcall ToolBar(HWND AParent,const UniString &ACaption, UINT StripResID, int ImgW, int ImgH);
	__fastcall ToolBar(HWND AParent,const UniString &ACaption,UINT ResID,HINSTANCE hInstResource);
	__fastcall ~ToolBar(void);
	void __fastcall UpdateMetrics(void);
	static COLORREF __fastcall GetTopLeftPixelColor(HBITMAP hBmp);
	void __fastcall AddButton();
	HWND __inline __fastcall GetHandle(void){return FHandle;};
	UniString __inline __fastcall GetCaption(void){return FCaption;};
	void __fastcall AddButton(UINT ImgIdx,UINT ID,const UniString &ToolTip);
	void __fastcall AddDropdownButton(UINT ImgIdx,UINT ID,const UniString &ToolTip);
	bool __fastcall AddDynamicButton(UINT ActionID,UINT ImgIdx,UINT CommandID,const UniString &ToolTip,bool IsDropdown=false);
	bool __fastcall AddDynamicButtonIcon(UINT ActionID,HICON AIcon,UINT CommandID,const UniString &ToolTip,bool IsDropdown=false);
	bool __fastcall RemoveDynamicButton(UINT ActionID);
	bool __fastcall RemoveDynamicButtonByCommand(UINT CommandID);
	void __fastcall ClearDynamicButtons(void);
	bool __fastcall HasDynamicButton(UINT ActionID) const;
	void __fastcall AddControl(const UniString &Caption,HWND hwnd);
	void __fastcall AddSeparator(void);
};
//---------------------------------------------------------------------------
HBITMAP __fastcall CreateAlphaBitmapFromStrip(int ResId,int ButtonIndex,int ButtonWidth,int ButtonHeight);
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
