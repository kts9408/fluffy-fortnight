#include "../includes/win32_FluffyFortnight.h"

/******************************************************************************
 * Internal Methods 
 *****************************************************************************/
namespace {

    /**************************************************************************
     * Initialize a Screen Buffer
     *************************************************************************/
    uint16_t win32_InitGfxBuffer(win32_GfxBuffer* buffer, int width, int height) {
        if(buffer->memory) {
            VirtualFree(buffer->memory, 0, MEM_RELEASE);    // release existing buffer before re-allocating
        }

        buffer->channelCount = 4;
        buffer->pitch = width * buffer->channelCount;

        buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
        buffer->info.bmiHeader.biWidth = width;
        buffer->info.bmiHeader.biHeight = height;
        buffer->info.bmiHeader.biPlanes = 1;
        buffer->info.bmiHeader.biBitCount = 32;                     // Force 32-bit color to force DWORD aligned
        buffer->info.bmiHeader.biCompression = BI_RGB;               // use No Compression

        int memSz = width * height * buffer->channelCount;
        buffer->memory = VirtualAlloc(
            NULL,                               // Starting memory address (NULL so system determines)
            memSz,                              // Size of region (in Bytes)
            MEM_RESERVE|MEM_COMMIT,             // Reserve Page files and Commit the to memory
            PAGE_READWRITE);                    // Allow Read/Write access
    }



    /**************************************************************************
     * Initialize the mainWindow
     *************************************************************************/
    uint16_t win32_InitWindow(
        HINSTANCE &hInstance,   // handle to current instance [input]
        HWND &window            // handle to main window [output]
    ) {   
        uint16_t result = 0;    // default to general failure

        // initialize the window structure with appropriate class attributes
        WNDCLASSA windowClass = {};                                  // declare window template
        windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;          // repaint entire window on resize
        windowClass.lpfnWndProc = win32_MainWindowCallback;          // set message processing callback
        windowClass.hInstance = hInstance;                           // set the instance handle
        windowClass.lpszClassName = "FluffyFortnightWindowClass";

        if (RegisterClassA(&windowClass)) {         // register window class to use CreateWindow
            window =
                CreateWindowExA(
                    0,                              // default style
                    windowClass.lpszClassName,      // registered window class name
                    "Fluffy Fortnight",             // Window Title
                    WS_OVERLAPPEDWINDOW|WS_VISIBLE, // Allow overlap and show window
                    CW_USEDEFAULT, CW_USEDEFAULT,   // use default coordinates (upper left)
                    CW_USEDEFAULT, CW_USEDEFAULT,   // use default size (windows fullscreen)
                    NULL,                           // handle to parent (none because top level)
                    NULL,                           // handle to menu (none because top level)
                    hInstance,                      // handle to application instance
                    NULL);                          // optional parameters (none)

            if (window) {  
                // Success
                result = 1;
            } else {
                // (ERROR: Failed to create main window)
                result = 3;
            }

        } else {
            // (ERROR: Failed to register main window class)
            result = 2;
        }

        return(result);
    }

    /**************************************************************************
     * Callback function that processes messages sent to the main window
     **************************************************************************/
    LRESULT CALLBACK win32_MainWindowCallback(
        HWND window,            // handle to the window
        UINT msg,               // the message
        WPARAM wParam,          // additional message information dependent on the type of message
        LPARAM lParam           // additional message information dependent on the type of message
    ) {

    }

    inline uint16_t win32_GetLastWriteTime(char* filename, FILETIME* output) {
        uint16_t result = 0;
        WIN32_FILE_ATTRIBUTE_DATA data;

        if(GetFileAttributesEx((LPCWSTR)filename, GetFileExInfoStandard, &data)) {      // attempt to get file info
            output = &(data.ftLastWriteTime);           // grab last write timestamp
            result = 1;         // SUCCESS
        } else {
            result = 6;         // ERROR: Failed to Read timestamp
        }

        return result;
    }

    win32_GameCode win32_LoadGameCode(char* dllName) {
        win32_GameCode result = {};

        
    }

}       // End of Internal Methods


/******************************************************************************
 * Entry Point
 *****************************************************************************/
int CALLBACK WinMain(
    HINSTANCE hInstance,        // handle to current instance
    HINSTANCE hPrevInstance,    // handle to previous instance (Always NULL)
    LPSTR lpCmdLine,            // char * containing command line args
    int showCmd                 // show state of window
) {
    HWND mainWindow;
    HDC deviceContext;
    uint16_t errorCode;
    win32_GfxBuffer backBuffer = {};    // create the backbuffer
    bool running = false;

    errorCode = win32_InitWindow(hInstance, mainWindow);
    if(errorCode == 1) {
        running = true;
    } else {
        // TODO: Error Logging
    }


    while(running) {



    }
}