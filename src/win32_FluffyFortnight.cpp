#include "../includes/win32_FluffyFortnight.h"

/******************************************************************************
 * Internal Methods 
 *****************************************************************************/
namespace {

    win32_GfxBuffer frameBuffer;
    bool running = true;
    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/
    LRESULT CALLBACK win32_MainWindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    uint16_t win32_InitGfxBuffer(win32_GfxBuffer* buffer, int width, int height);
    uint16_t win32_InitWindow(HINSTANCE &hInstance, HWND &window);
    inline win32_WindowDimension win32_GetWindowDimension(HWND window);
    inline uint16_t win32_GetLastWriteTime(char* filename, FILETIME* output);
    uint16_t win32_LoadGameCode(char* dllName, win32_GameCode* output);
    void win32_ProcessPendingMessages();
    void win32_ProcessKeyboardInput(MSG* msg);
    void win32_FreeGameCode(win32_GameCode* input);
    void win32_CopyBufferToWindow(HDC window, int windowWidth, int windowHeight, win32_GfxBuffer* srcBuffer, int x, int y);      
    void win32_InitXAudio2();

    // End of Forward Declaration

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

            case WM_DESTROY: {
                running = false;
            }
            case WM_QUIT: {
                running = false;
            } break;

            case WM_CLOSE: {    // close window message
                running = false;
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
                win32_CopyBufferToWindow(deviceContext, wd.width, wd.height, &frameBuffer, x, y);
                EndPaint(window, &paint);
            } break;

            default: {          // propogate any other messages up to the OS to process
                result = DefWindowProcA(window, msg, wParam, lParam);
            } break;
        }
        return result;
    }
    /**************************************************************************
     * Initialize a Screen Buffer
     *************************************************************************/
    uint16_t win32_InitGfxBuffer(win32_GfxBuffer* buffer, int width, int height) {
        uint16_t result = 0;                // default to general error
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

        if(buffer->memory) {
            result = 1;                         // SUCCESS!
        } else {
            result = 5;                         // ERROR: Failed to allocate buffer memory!
        }

        return result;
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

        if(GetFileAttributesExA(filename, GetFileExInfoStandard, &data)) {      // attempt to get file info
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
            output->gameRender = (game_Render*)GetProcAddress(output->dllGameCode, "renderGame");
            output->gameInit = (game_Init*)GetProcAddress(output->dllGameCode, "initGame");
            win32_GetLastWriteTime(dllName, &(output->dllTimeStamp));
            output->isValid = (output->gameRender != nullptr);    // set initialized flag
        } else {
            result = 7;                 // ERROR: Failed to read library
        }

        if(output->isValid) {
            result = 1;                 // SUCCESS
        } else {
            result = 8;                 // ERROR: Failed to load render function
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
        input->gameInit = 0;
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

    /**************************************************************************
     * 
     *************************************************************************/
    void win32_ProcessPendingMessages(HWND window) {
        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {      // Poll Windows Message Queue
            switch(msg.message) {
                case WM_SYSKEYDOWN:		                    
			    case WM_SYSKEYUP:		
			    case WM_KEYDOWN:
			    case WM_KEYUP: {                            // On Keyboard Event Message
                    win32_ProcessKeyboardInput(&msg);
                } break;
                default: {
                    TranslateMessage(&msg);                 // Propogate up to OS
                    DispatchMessage(&msg);
                }break;
            }

        }

    }

    /**************************************************************************
     * 
     *************************************************************************/
    void win32_ProcessKeyboardInput(MSG* msg) {
        uint32_t vkCode = (uint32_t)msg->wParam;            // Unpack key
        bool wasDown = ((msg->lParam & (1 << 30)) != 0);    // Unpack state
        bool isDown = ((msg->lParam & (1 << 31)) != 0);
        bool altKeyWasDown = (msg->lParam & (1 << 29));     // Unpack modifiers

        running = !((vkCode == VK_F4) && (altKeyWasDown));  // Check for Alt + F4
    }

    
    /**************************************************************************
     * AudioCallback for XAudio2 Stub
     *************************************************************************/
    DWORD WINAPI win32_AudioCallback(LPVOID params) {
        return 1;
    }

    /**************************************************************************
     * Safely Truncates an unsigned 64-bit value to a 32-bit value
     *************************************************************************/ 
    inline uint32_t win32_TruncateUInt64(uint64_t value) {
        ASSERT(value <= 0xffffffff);
        uint32_t result = (uint32_t)value;
        return result;
    }

    /**************************************************************************
     * 
     *************************************************************************/
    void win32_InitXAudio2() {
//        CoInitialize(NULL, COINIT_MULTITHREADED);

        IXAudio2* xAudio2                       = 0;                // Pointer to audio engine
        IXAudio2SourceVoice* srcVoice           = 0;                // Pointer to source voice
        IXAudio2MasteringVoice* masterVoice     = 0;                // Pointer to master voice
        
        // Initialize the Sound Format Parameters (Default config) TODO: Read these settings from a persistant config file.
        WAVEFORMATEX waveFormat                 = {};
        waveFormat.wFormatTag                   = WAVE_FORMAT_PCM;
        waveFormat.nChannels                    = 2;
        waveFormat.wBitsPerSample               = 16;
        waveFormat.nSamplesPerSec               = 48000;
        waveFormat.nBlockAlign                  = waveFormat.nSamplesPerSec * waveFormat.wBitsPerSample / 8;
        waveFormat.nAvgBytesPerSec              = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveFormat.cbSize                       = 0;

        XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        xAudio2->CreateMasteringVoice(&masterVoice);
        xAudio2->CreateSourceVoice(&srcVoice, waveFormat);

    }



}       // End of Internal Methods

/******************************************************************************
 * Public Methods
 *****************************************************************************/
#if PROTOTYPE       // Prototyping code not for actual release
    /**************************************************************************
     * Reads an entire file from disk and stores the data in memory (PROTOTYPING)
     *************************************************************************/ 
    uint16_t win32_ReadFromDisk(char* filename, void* out) {
        uint16_t result = 0;            // initialize error code to general failure
        HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,  // Get a Handle to the File, Shared for Reading
            0, OPEN_EXISTING, 0, 0);    // Throw error if it doesn't exist
        LARGE_INTEGER fileSize;         // 64-bit integer to hold the file size (NTFS/EXT4 safe)
        uint32_t fileSize32;            // 32-bit integer to pass into ReadFile (Legacy)
        DWORD bytesRead = 0;
        GetFileSizeEx(file, &fileSize); // 64-bit size of the file
        fileSize32 = win32_TruncateUInt64(fileSize.QuadPart);   // safe cast to 32-bit
        out = VirtualAlloc(0, fileSize.QuadPart,
            MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);             // Allocate memory to store data
        if(out) {                       // Memory successfuly allocated
            if((ReadFile(file, out, fileSize32, &bytesRead, 0)) && (fileSize32 == bytesRead)) {
                result = 1;             // Success!
            } else {                    // Failed to Read or could not read entire file
                result = 0xa;           // ERROR: Failed to Read File!
                win32_FreeFileMemory(out);  // on failed read release memeory
            }
        } else {
            result = 9;             // ERROR: Could not allocate memory for file!
        }
        CloseHandle(file);          // Close the File Handle
        return result;              // return error code
    }

    /**************************************************************************
     * Writes a value from memory out to disk (PROTOTYPING)
     *************************************************************************/
    bool win32_WriteToDisk(char* filename, uint32_t memorySize, void* memory) {
        uint16_t result = 0;        // initialize error code to general failure
        HANDLE file = CreateFileA(filename, GENERIC_WRITE, 0,   
            0, CREATE_ALWAYS, 0, 0);    // Get a handle to the file
        LARGE_INTEGER fileSize;     // 64-bit int to store filesize (NTFS/EXT4 safe)
        uint32_t fileSize32;        // 32-bit int to pass to WriteFile
        DWORD bytesWritten = 0;
        GetFileSizeEx(file, &fileSize); // Get filesize
        fileSize32 = win32_TruncateUInt64(fileSize.QuadPart);   // safe cast to 32-bit
    
        if((WriteFile(file, memory, fileSize32, &bytesWritten, 0)) && (fileSize32 == bytesWritten)) {
            result = 1;         // Success!
        } else {
            result = 0;         // ERROR: General Failure (for boolean logic)
        }
    
        return result;
    }

    /**************************************************************************
     * Free memory allocated to store file data. (PROTOTYPING)
     *************************************************************************/
    void win32_FreeFileMemory(void* memory) {
        if(memory) {
            VirtualFree(memory, 0, MEM_RELEASE);
        }
    }   
#endif      // end of Prototyping

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
    char* dllName                   = "FluffyFortnight.dll";
    game_GfxBuffer pushBuffer       = {};                         // create the push buffer
    win32_GameCode gameCode         = {};                         // create the external code library
    FILETIME dllWriteTime;
    uint16_t errorCode;
    // HANDLE threadPool[4];
    void* audioParams               = 0;
    LPDWORD audioThreadId           = 0;
    HANDLE audioThread              = CreateThread(NULL, NULL, win32_AudioCallback, audioParams, DELAY_THREAD_START, audioThreadId);

    // TODO: Enumerate HW and determine appropriate memory footprint
    void* memory                    = VirtualAlloc(0, (SIZE_T)((uint64_t)MEGABYTES(64) + (uint64_t)GIGABYTES(4)), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    for(int i = 0; i < 4; i++) {
        // TODO: Create Threads
    }

    errorCode = win32_LoadGameCode(dllName, &gameCode);     // load the game code      
    errorCode = win32_InitWindow(hInstance, mainWindow);    // create the Main Window
    frameBuffer = {};                        
    errorCode = win32_InitGfxBuffer(                        // initialize back buffer
        &frameBuffer,
        DEFAULT_GFX_BUFFER_WIDTH,
        DEFAULT_GFX_BUFFER_HEIGHT
    );

    if(errorCode != 1) {     
        running = false;
        // TODO: Error Logging
    }

    while((running)) {
        gameCode.gameInit(memory);

        // update loaded code if needed
        win32_GetLastWriteTime(dllName, &dllWriteTime);
        if(CompareFileTime(&dllWriteTime, &(gameCode.dllTimeStamp)) != 0) { 
            win32_FreeGameCode(&gameCode);
            errorCode = win32_LoadGameCode(dllName, &gameCode);
        }

        // create and initialize a working buffer for the game code to manipulate
        
        pushBuffer.memory = frameBuffer.memory;
        pushBuffer.width = frameBuffer.info.bmiHeader.biWidth;
        pushBuffer.height = frameBuffer.info.bmiHeader.biHeight;
        pushBuffer.pitch = frameBuffer.pitch;
        pushBuffer.channelCount = frameBuffer.channelCount;
		if (gameCode.gameRender) {

			gameCode.gameRender(&pushBuffer);
		}
        deviceContext = GetDC(mainWindow);
        win32_CopyBufferToWindow(
            deviceContext,
            DEFAULT_GFX_BUFFER_WIDTH, DEFAULT_GFX_BUFFER_HEIGHT,
            &frameBuffer, 0, 0
        );

        win32_ProcessPendingMessages(mainWindow);

    }
}