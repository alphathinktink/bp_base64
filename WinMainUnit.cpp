//------------------------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
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

        INITCOMMONCONTROLSEX icc;
        icc.dwSize=sizeof(icc);
        icc.dwICC=ICC_WIN95_CLASSES|ICC_STANDARD_CLASSES;
        InitCommonControlsEx(&icc);

        TMainWindow mainWindow;
        if(!mainWindow.Create(hInstance,nCmdShow))
        {
                ::MessageBoxW(NULL,L"Unable to create Base64 main window.",L"Base64",MB_ICONERROR|MB_OK);
                return 1;
        }

        return mainWindow.Run();
}
//------------------------------------------------------------------------------------------
