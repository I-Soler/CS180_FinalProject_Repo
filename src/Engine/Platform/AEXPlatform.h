#include "AEXWindow.h"
#include "AEXInput.h"
#include "AEXTime.h"
#include "AEXFilePath.h"
#include "AEXOpenSaveFile.h"

// Global Platform API
namespace AEX
{
    namespace Platform
    {
        // Console Management
        void ShowConsole();
        void DestroyConsole();
        void ResetConsole();
        void HideConsole();

        // Monitor Size 
        void GetScreenSize(int& sW, int& sH);  // Primary monitor
    }
}