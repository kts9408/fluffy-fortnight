#include "../includes/win32_FluffyFortnight.h"

/******************************************************************************
 * Internal Methods 
 *****************************************************************************/
namespace {
    /**************************************************************************
     * Forward Declare Private Members/Functions
     *************************************************************************/
    win32_GfxBuffer backBuffer;
    LRESULT CALLBACK win32_MainWindowCallback(
        HWND window,            // handle to the window
        UINT msg,               // the message
        WPARAM wParam,          // additional message information dependent on the type of message
        LPARAM lParam           // additional message information dependent on the type of message
    );
    uint16_t win32_InitGfxBuffer(win32_GfxBuffer* buffer, int width, int height);
    uint16_t win32_InitWindow(
        HINSTANCE &hInstance,   // handle to current instance [input]
        HWND &window            // handle to main window [output]
    );

    inline win32_WindowDimension win32_GetWindowDimension(HWND window);
    inline uint16_t win32_GetLastWriteTime(char* filename, FILETIME* output);
    uint16_t win32_LoadGameCode(char* dllName, win32_GameCode* output);
    void win32_FreeGameCode(win32_GameCode* input);
    void win32_CopyBufferToWindow(
        HDC window,                             // Device Context for the destination window
        int windowWidth, int windowHeight,      // dimensions of the window
        win32_GfxBuffer* srcBuffer,             // source buffer
        int x, int y                            // coordinates to offset (TODO: Implement offsets)
    );      // End of Forward Declaration





    /**************************************************************************
     * Callback function that processes messages sent to the main window
     **************************************************************************/
    LRESULT CALLBACK win32_MainWindowCallback(
        HWND window,            // handle to the window
        UINT msg,               // the message
        WPARAM wParam,          // additional message information dependent on the type of message
        LPARAM lParam           // additional message information dependent on the type of message
    ) {
        LRESULT result = 0;
        switch(msg) {
            case WM_SIZE: {     // resize window message

            } break;

            case WM_CLOSE: {    // close window message

            } break;

            case WM_ACTIVATEAPP: {

            } break;

            case WM_PAINT: {    // repaint window message
                PAINTSTRUCT paint;
                HDC deviceContext = BeginPaint(window, &paint);

                int height = paint.rcPaint.bottom - paint.rcPaint.top;
                int width = paint.rcPaint.right - paint.rcPaint.left;
                int x = paint.rcPaint.left;
                int y = paint.rcPaint.top;
                win32_WindowDimension wd = win32_GetWindowDimension(window);
                win32_CopyBufferToWindow(deviceContext, wd.width, wd.height, &backBuffer, x, y);
                
            }
            default: {          // propogate any other messages up to the OS to process
                result = DefWindowProcA(window, msg, wParam, lParam);
            } break;
        }
    }
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
     * Initialize the Main Window
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
     * Helper function to return the dimensions of the main window
     *************************************************************************/
    inline win32_WindowDimension win32_GetWindowDimension(HWND window) {
        win32_WindowDimension result = {};
        RECT clientRect;

        GetClientRect(window, &clientRect);
        result.width = clientRect.right - clientRect.left;
        result.height = clientRect.bottom - clientRect.top;

        return result;
    }

    /**************************************************************************
     * Helper function to return the write timestamp on a given file.
     *************************************************************************/
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
    

    /**************************************************************************
     * Load game code from external library
     *************************************************************************/
    uint16_t win32_LoadGameCode(char* dllName, win32_GameCode* output) {
        // TODO: Get real path (ie. data not build)
        // TODO: swap to auto update instead of on demand

        uint16_t result = 0;        // default to general error
        output->isValid = false;

        char* tempDllName = "FluffyFortnight.temp.dll";         // set temporary file name

        CopyFileA(dllName, tempDllName, false);                 // make a working copy of dll
        output->dllGameCode = LoadLibraryA(tempDllName);         // deploy OS hooks to load working dll
        if(output->dllGameCode) {                               // load successful
            output->gameRender = (game_Render*)GetProcAddress(output->dllGameCode, "game_Render");
            win32_GetLastWriteTime(dllName, &(output->dllTimeStamp));
            output->isValid = (output->gameRender != nullptr);    // set initialized flag
        } else {
            result = 7;                 // ERROR: Failed to read library
        }

        if(output->isValid) {
            result = 1;                 // SUCCESS
        } else {
            result = 8;                 // ERROR: Failed to load render function
            output->gameRender = 0;   
        }

        return result;        
    }

    /**************************************************************************
     * Free game code loaded
     *************************************************************************/
    void win32_FreeGameCode(win32_GameCode* input) {
        if (input->dllGameCode) {
            FreeLibrary(input->dllGameCode);
        }

        input->gameRender = 0;
        input->isValid = false;
    }


    /**************************************************************************
     * Copy Screen Buffer to a window (Flip).
     *************************************************************************/
    void win32_CopyBufferToWindow(
        HDC window,                             // Device Context for the destination window
        int windowWidth, int windowHeight,      // dimensions of the window
        win32_GfxBuffer* srcBuffer,             // source buffer
        int x, int y                            // coordinates to offset (TODO: Implement offsets)
    ) {
        // TODO: Maintain aspect ratio
        StretchDIBits(
            window,                             // destination
            0, 0, windowWidth, windowHeight,    // destination starting coordinates/dimensions to copy to
            0, 0,                               // source starting coordinates
            srcBuffer->info.bmiHeader.biWidth, srcBuffer->info.bmiHeader.biHeight,  //source copy dimensions
            srcBuffer->memory,                  // data to copy/stretch
            &(srcBuffer->info),                 // header for data
            DIB_RGB_COLORS, SRCCOPY             // direct copy (non-palettized)
        );
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
    
    char* dllName = "FluffyFortnight.dll";
    win32_GameCode gameCode = {};                           // create the external code library
    FILETIME dllWriteTime;
    errorCode = win32_LoadGameCode(dllName, &gameCode);     // load the game code      
    if(errorCode != 1) {    
        // TODO: Error Logging
    }
    
    bool unloadCode = false;
    errorCode = win32_InitWindow(hInstance, mainWindow);    // create the Main Window
    if(errorCode != 1) {    
        // TODO: Error Logging
    }

    // win32_GfxBuffer backBuffer = {};                        // create the back buffer
    backBuffer = {};                        
    errorCode = win32_InitGfxBuffer(                        // initialize back buffer
        &backBuffer,
        DEFAULT_GFX_BUFFER_WIDTH,
        DEFAULT_GFX_BUFFER_HEIGHT
    );
    if(errorCode != 1) {     
        // TODO: Error Logging
    }

    bool running = true;
    while((running) && (errorCode == 1)) {

        // update loaded code if needed
        win32_GetLastWriteTime(dllName, &dllWriteTime);
        if(CompareFileTime(&dllWriteTime, &(gameCode.dllTimeStamp)) != 0) { 
            win32_FreeGameCode(&gameCode);
            errorCode = win32_LoadGameCode(dllName, &gameCode);
        }

        // create and initialize a working buffer for the game code to manipulate
        game_GfxBuffer buffer = {};
        buffer.memory = backBuffer.memory;
        buffer.width = backBuffer.info.bmiHeader.biWidth;
        buffer.height = backBuffer.info.bmiHeader.biHeight;
        buffer.pitch = backBuffer.pitch;
        buffer.channelCount = backBuffer.channelCount;

        gameCode.gameRender(&buffer);
        deviceContext = GetDC(mainWindow);
        win32_CopyBufferToWindow(
            deviceContext,
            DEFAULT_GFX_BUFFER_WIDTH, DEFAULT_GFX_BUFFER_HEIGHT,
            &backBuffer, 0, 0
        );
    }
}