//------------------------------------------------------------------------------------------
#include <windows.h>
#include "MainWindowUnit.h"
#include "EncodeDecodeThreadUnit.h"
#include "UniString.h"

/******************************************************************************************\
**** This is supposed to be a Win32 Unicode full GUI application with a main window.********
*********Not a console app or a dll or COM store.*******************************************
************************Please utilize the provided AnsiString.h and UniString.h files.*****
*****For your convienience, CommonFuncsUnit and ItsAMysteryUnit are also available.*********
\******************************************************************************************/
HINSTANCE hGlobalInstance=NULL;
//------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	hGlobalInstance=hInstance;
	return 0;
}
//------------------------------------------------------------------------------------------
