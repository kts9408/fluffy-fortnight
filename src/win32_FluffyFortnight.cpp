#include "../includes/win32_FluffyFortnight.h"
namespace {
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
    void win32_FreeGameCode(win32_GameCode* input);
    void win32_ProcessPendingMessages(
        game_ControllerInput* keyboardController
    );
    void win32_ProcessKeyboardMessages(
        MSG* msg,
        game_ControllerInput* keyboard
    );
    void win32_ProcessKeyboardInput(
        bool isDown,
        game_ButtonState* out
    );
    void win32_CopyBufferToWindow(
        HDC window,                             // Device Context for the destination window
        int windowWidth, int windowHeight,      // dimensions of the window
        win32_GfxBuffer* srcBuffer,             // source buffer
        int x, int y                            // coordinates to offset (TODO: Implement offsets)
    );

    uint16_t win32_InitXAudio2(win32_SoundEngine* out);
    uint16_t win32_ProcessGameSound(
        game_SoundBuffer* in,
        XAUDIO2_BUFFER* out
    );


    // End of Forward Declaration
    /**************************************************************************
    * Stubs to prevent crash from lack of XInput Supported Controller
    **************************************************************************/
    XINPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
    XINPUT_GET_STATE(XInputGetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }

    /**************************************************************************
     * Internal Members
     *************************************************************************/
    win32_GfxBuffer frameBuffer;
    win32_SoundEngine audioEngine;
    bool running = true;
    xinput_set_state* _XInputSetState = XInputSetStateStub;
    xinput_get_state* _XInputGetState = XInputGetStateStub;



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
    uint16_t win32_InitGfxBuffer(
        win32_GfxBuffer* buffer,
        int width,
        int height
    ){
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
        buffer->info.bmiHeader.biCompression = BI_RGB;              // use No Compression

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
    ){   
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
                    DEFAULT_GFX_BUFFER_WIDTH,
                    DEFAULT_GFX_BUFFER_HEIGHT,   
                    NULL,                           // handle to parent (none because top level)
                    NULL,                           // handle to menu (none because top level)
                    hInstance,                      // handle to application instance
                    NULL                            // optional parameters (none)
                );

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

        return result;
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
    inline uint16_t win32_GetLastWriteTime(
        char* filename,
        FILETIME* output)
    {
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
    uint16_t win32_LoadGameCode(
        char* dllName,
        win32_GameCode* output
    ){
        // TODO: Get real path (ie. data not build)
        // TODO: swap to auto update instead of on demand

        uint16_t result = 0;        // default to general error
        output->isValid = false;

        char* tempDllName = "FluffyFortnight.temp.dll";         // set temporary file name

        CopyFileA(dllName, tempDllName, false);                 // make a working copy of dll
        output->dllGameCode = LoadLibraryA(tempDllName);         // deploy OS hooks to load working dll
        if(output->dllGameCode) {                               // load successful
            output->gameRenderGfx = (game_RenderGfx*)GetProcAddress(output->dllGameCode, "renderGameGfx");
            output->gameInit = (game_Init*)GetProcAddress(output->dllGameCode, "initGame");
            output->gameRenderAudio = (game_RenderAudio*)GetProcAddress(output->dllGameCode, "renderGameAudio");
            output->gameUpdate = (game_Update*)GetProcAddress(output->dllGameCode, "updateGame");
            win32_GetLastWriteTime(dllName, &(output->dllTimeStamp));
            output->isValid = (output->gameRenderGfx != nullptr);    // set initialized flag
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

        input->gameRenderGfx        = 0;
        input->gameRenderAudio      = 0;
        input->gameInit             = 0;
        input->isValid              = false;
    }

    /**************************************************************************
     * Copy Screen Buffer to a window (Flip).
     *************************************************************************/
    void win32_CopyBufferToWindow(
        HDC window,                             // Device Context for the destination window
        int windowWidth, int windowHeight,      // dimensions of the window
        win32_GfxBuffer* srcBuffer,             // source buffer
        int offsetX, int offsetY                // coordinates to offset (TODO: Implement offsets)
    ) {
        // TODO: Implement this
        offsetX = 0;
        offsetY = 0;
        
        // clear gutter to black
        // TODO: Bad Code clean up
        PatBlt(
            window,                             // destination context
            0, 0,                               // starting coordinates
            offsetX, windowHeight,              // width of clear box
            BLACKNESS                           // clear to black
        );
        PatBlt(
            window,                             
            0, 0,                               
            windowWidth, offsetY,               
            BLACKNESS                           
        );
        PatBlt(
            window,                             
            offsetX + srcBuffer->info.bmiHeader.biWidth, 0,
            windowWidth, windowHeight,          
            BLACKNESS                           
        );
        PatBlt(
            window,                             
            0, offsetY + srcBuffer->info.bmiHeader.biHeight,
            windowWidth, windowHeight,          
            BLACKNESS                           
        );
        // End bad code

        // TODO: Maintain aspect ratio
        StretchDIBits(
            window,                             // destination
            offsetX, offsetY,                   // destination starting coordinates
            srcBuffer->info.bmiHeader.biWidth,  // destination dimensions (1:1 for debugging purposes)
            srcBuffer->info.bmiHeader.biHeight,    
            0, 0,                               // source starting coordinates
            srcBuffer->info.bmiHeader.biWidth,  // source copy dimensions
            srcBuffer->info.bmiHeader.biHeight,  
            srcBuffer->memory,                  // data to copy
            &(srcBuffer->info),                 // header for data
            DIB_RGB_COLORS, SRCCOPY             // direct copy (non-palettized)
        );
    }

    /**************************************************************************
     * 
     *************************************************************************/
    void win32_ProcessPendingMessages(
        game_ControllerInput* keyboardController
    ) {
        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {      // Poll Windows Message Queue
            switch(msg.message) {
                case WM_SYSKEYDOWN:		                    
			    case WM_SYSKEYUP:		
			    case WM_KEYDOWN:
			    case WM_KEYUP: {                            // On Keyboard Event Message
                    win32_ProcessKeyboardMessages(
                        &msg,
                        keyboardController
                    );
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
    inline void win32_ProcessKeyboardInput(
        bool isDown,
        game_ButtonState* out
    ) {
        out->isDown = &isDown;
	    out->stateChangeCount++;
    }

    /**************************************************************************
     * 
     *************************************************************************/
    void win32_ProcessKeyboardMessages(
        MSG* msg,
        game_ControllerInput* keyboard
    ) {
        uint32_t vkCode = (uint32_t)msg->wParam;            // Unpack key
        bool wasDown = ((msg->lParam & (1 << 30)) != 0);    // Unpack state
        bool isDown = ((msg->lParam & (1 << 31)) != 0);
        bool altKeyWasDown = (msg->lParam & (1 << 29));     // Unpack modifiers
        if(wasDown != isDown) {
            // TODO: Collapse this into a for...loop???
            switch(vkCode) {
                case 'Q': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->LeftShoulder);
                } break;
                case 'W': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->RightShoulder);
                } break;
                case 'A': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Left);
                } break;
                case 'S': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Top);
                } break;
                case 'Z': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Bottom);
                } break;
                case 'X': {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Right);
                } break;
                case VK_RETURN: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Start);
                } break;
                case VK_LSHIFT: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->Select);
                } break;
                case VK_UP: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->North);
                } break;
                case VK_DOWN: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->South);
                } break;
                case VK_LEFT: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->West);
                } break;
                case VK_RIGHT: {
                    win32_ProcessKeyboardInput(isDown, &keyboard->East);
                } break;
            }
        }
           
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
        ASSERT(!(value > 0xffffffff));
        uint32_t result = (uint32_t)value;
        return result;
    }

    /**************************************************************************
     * 
     *************************************************************************/
    uint16_t win32_InitXAudio2(win32_SoundEngine* out) {
        uint16_t result                         = 0;                // initialize to general failure
        HMODULE xAudioLib                       = LoadLibraryA("XAUDIO2_9.DLL");
        WAVEFORMATEX* waveFormat                = &out->waveFormat;
        XAUDIO2_BUFFER* soundBuffer             = &out->soundBuffer;                // Pointer to XAudio2 sound buffer
        
        // Initialize the Sound Format Parameters (Default config)
        // TODO: Read these settings from a persistant config file.
        waveFormat->wFormatTag                  = WAVE_FORMAT_PCM;
        waveFormat->nChannels                   = 2;
        waveFormat->wBitsPerSample              = 16;
        waveFormat->nSamplesPerSec              = 48000;
        waveFormat->nBlockAlign                 = 4;
        // waveFormat->nBlockAlign                 = ((uint16_t)waveFormat->nSamplesPerSec) * waveFormat->wBitsPerSample / 8;
        waveFormat->nAvgBytesPerSec             = waveFormat->nSamplesPerSec * waveFormat->nBlockAlign;
        waveFormat->cbSize                      = 0;

        // Initalize the Sound Buffer Parameters (Default config)
        // TODO: Read these settings from a persistant config file.
        soundBuffer->Flags          = 0;                
        soundBuffer->AudioBytes     = 384000;
        soundBuffer->pAudioData     = 0;
        soundBuffer->PlayBegin      = 0;
        soundBuffer->PlayLength     = 0;
        soundBuffer->LoopBegin      = 0;
        soundBuffer->LoopLength     = 0;
        soundBuffer->LoopCount      = XAUDIO2_LOOP_INFINITE;
        soundBuffer->pContext       = 0;
        // CoInitialize(NULL, COINIT_MULTITHREADED);

        if(xAudioLib) {
            xaudio_Create* XAudio2Create        = (xaudio_Create*)GetProcAddress(xAudioLib, "XAudio2Create");
            XAudio2Create(&out->xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
            if(out->xAudio) {
                out->xAudio->CreateMasteringVoice(&out->masterVoice);
                result      = 1;        // Success!
            } else {
                // ERROR: Failed to Start Audio Engine
                result      = 0xb;   
            }
        } else {
            // ERROR: Failed to Start Audio Engine
            result      = 0xb;   
        }
        return result;
    }
    /**************************************************************************
     * Process Game Sound with XAudio
     *************************************************************************/
    uint16_t win32_ProcessGameSound(
        game_SoundBuffer* in,
        XAUDIO2_BUFFER* out
    ){
        uint16_t result = 0;                    // initialize to general failure
        if(in->isInitialized) {
            out->pAudioData = (uint8_t*)in->samples;       // cast/dereference the audio buffer data to BYTE (required by XAudio2)
            out->AudioBytes = in->bufferSize;
            audioEngine.xAudio->CreateSourceVoice(&audioEngine.sourceVoice, (WAVEFORMATEX*)&audioEngine.waveFormat);
            audioEngine.sourceVoice->SubmitSourceBuffer(out);
            audioEngine.sourceVoice->Start();
            in->isInitialized   = false;
            result = 1;                         // SUCCESS!
        } else {
            result = 0xc;                      // ERROR:  Audio buffer not initialized
        }
        return result;
    }
    /**************************************************************************
     * 
     *************************************************************************/ 
    uint16_t win32_InitXInput(void) {
        // TODO: Test on Win8
        uint16_t result = 0;
        HMODULE XInputLib   = LoadLibraryA("xinput1_4.dll");    // load XInput1.4
        if(!XInputLib) {
            XInputLib   = LoadLibraryA("xinput1_3.dll");    // fallback to 1.3
        }

        if(XInputLib) {
            XInputGetState  = (xinput_get_state*)GetProcAddress(XInputLib, "XInputGetState");
            XInputSetState  = (xinput_set_state*)GetProcAddress(XInputLib, "XInputSetState");
            result  = 1;    // Success!
        } else {
            result  = 0xd;  // ERROR: Failed to Load XInput
        }

        return result;
    }
    /**************************************************************************
     * Takes the stick value reported by XINPUT and normalizes it to a float
     * value between -1 and 1 after compensating for the deadzone.
     *************************************************************************/ 
    float win32_NormalizeAnalogStick(
        int16_t in,       // SHORT value reported by XINPUT
        int16_t deadzone  // deadzone value
    ) {

        float result = 0.0f;
        if(in < -deadzone) {
            result = (float)in/32768.0f;
        } else if(in > deadzone) {
            result = (float)in/32767.0f;
        }

        return result;
    }

    /**************************************************************************
     * 
     *************************************************************************/
    inline void win32_ProcessDigitalButton(
        DWORD in,        //  Button State from XInput
        DWORD mask,      //  Bit-Mask for XInput
        game_ButtonState* oldButton,    //  Previous Button State
        game_ButtonState* out       // Current Button State
    ) {
        out->isDown = ((in & mask) == mask);    // use bitmask to determine state of button
        // TODO: change to += to track sub-frame transitions once this start polling more than once per frame.
        out->stateChangeCount = (oldButton->isDown != out->isDown) ? 1 : 0;    // increment on state change
    }

    /**************************************************************************
     * 
     *************************************************************************/ 
    void win32_CreateGameController(
        XINPUT_GAMEPAD in,
        game_ControllerInput* old,
        game_ControllerInput* out
    ) {
        for(int buttonIndex = 0; buttonIndex < MAX_BUTTON_COUNT; buttonIndex++) {
            win32_ProcessDigitalButton(
                in.wButtons,
                XINPUT_BITMASKS[buttonIndex],
                &old->Buttons[buttonIndex],
                &out->Buttons[buttonIndex]
            );
        }
        // TODO: Possibly change this to calculate avg stick position based stick starting and ending position
        out->avgLX  = win32_NormalizeAnalogStick(in.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        out->avgLY  = win32_NormalizeAnalogStick(in.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        out->avgRX  = win32_NormalizeAnalogStick(in.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        out->avgRY  = win32_NormalizeAnalogStick(in.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        // TODO: Map Analog Stick input to D-Pad
    }


}       // End of Internal Methods

/******************************************************************************
 * Public Methods
 *****************************************************************************/
#if PROTOTYPE       // Prototyping code not for actual release
/******************************************************************************
 * Reads an entire file from disk and stores the data in memory (PROTOTYPING)
 *****************************************************************************/ 
uint16_t win32_ReadFromDisk(
    char* filename,
    void* out
){
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
bool win32_WriteToDisk(
    char* filename,
    uint32_t memorySize,
    void* memory
){
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
 * ~~~~~ Entry Point ~~~~~
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
    game_GfxBuffer gfxPushBuffer    = {};                         // create the push buffer
    win32_GameCode gameCode         = {};                         // create the external code library
    
    
    uint16_t errorCode;
    // HANDLE threadPool[4];
    void* audioParams               = 0;
    LPDWORD audioThreadId           = 0;
    //HANDLE audioThread              = CreateThread(NULL, NULL, win32_AudioCallback, audioParams, DELAY_THREAD_START, audioThreadId);
    //game_Memory memory              = {};
    LPVOID baseAddress              = 0;

    // TODO: Enumerate HW and determine appropriate memory footprint
    baseAddress                     = (LPVOID)TERABYTES(2);         // set base address for game memory so pointers are the same between runs DEBUGGING
    void* memory                    = VirtualAlloc(baseAddress, ((uint64_t)MEGABYTES(64) + (uint64_t)GIGABYTES(4)), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    for(int i = 0; i < 4; i++) {
        // TODO: Create Threads
    }

    errorCode = win32_LoadGameCode(dllName, &gameCode);     // load the game code      
    errorCode = win32_InitWindow(hInstance, mainWindow);    // create the Main Window
    frameBuffer = { };                        
    errorCode = win32_InitGfxBuffer(                        // initialize back buffer
        &frameBuffer,
        DEFAULT_GFX_BUFFER_WIDTH,
        DEFAULT_GFX_BUFFER_HEIGHT
    );

    // initialize a working sound buffer for the game code to manipulate
    // TODO: Load Sound settings from ini file
    game_SoundBuffer soundPushBuffer    = {};
    soundPushBuffer.isInitialized       = true;
    soundPushBuffer.samplesPerSecond    = 48000;
    soundPushBuffer.sampleCount         = 48000;
    soundPushBuffer.bufferSize          = 48000 * sizeof(uint16_t) * 2;
    soundPushBuffer.samples             = (uint16_t*)VirtualAlloc(0, soundPushBuffer.bufferSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    soundPushBuffer.isInitialized       = true;
    win32_InitXAudio2(&audioEngine);
    if(errorCode != 1) {     
        running = false;
        // TODO: Error Logging
    }

    gameCode.gameInit(&memory);
    game_Input Controllers[2];
    game_Input* inputA = &Controllers[0];
    game_Input* inputB = &Controllers[1];
    game_Input* tempController = inputA;
    

    // initialize keyboard controller to always be digital
    inputA->KeyboardController = { 0 };
    inputA->KeyboardController.isAnalog = false;
    inputB->KeyboardController = { 0 };
    inputB->KeyboardController.isAnalog = false;
    
    XINPUT_STATE controllerState;

    while((running)) {

        // update loaded code if needed
        FILETIME dllWriteTime = {};
        win32_GetLastWriteTime(dllName, &dllWriteTime);
        if(CompareFileTime(&dllWriteTime, &gameCode.dllTimeStamp) != 0) { 

            errorCode = win32_LoadGameCode(dllName, &gameCode);
        }

        // initialize a working buffer for the game code to manipulate
        gfxPushBuffer.memory = frameBuffer.memory;
        gfxPushBuffer.width = frameBuffer.info.bmiHeader.biWidth;
        gfxPushBuffer.height = frameBuffer.info.bmiHeader.biHeight;
        gfxPushBuffer.pitch = frameBuffer.pitch;
        gfxPushBuffer.channelCount = frameBuffer.channelCount;

        // update input pointers to reflect frame change
        tempController->GamePadController = inputA->GamePadController;
        inputA->GamePadController = inputB->GamePadController;
        inputB->GamePadController = tempController->GamePadController;

        // grab input from controller
        if(XInputGetState(0, &controllerState) == ERROR_SUCCESS) {
            win32_CreateGameController(
                controllerState.Gamepad,
                &inputB->GamePadController,
                &inputA->GamePadController
            );
            XINPUT_VIBRATION controllerOutput = {
                inputB->GamePadController.LeftVibration,
                inputB->GamePadController.RightVibration
            };
            XInputSetState(0, &controllerOutput);
        } else {    // Controller Unavailable

        }
        // end of input

		if (gameCode.isValid) {
            gameCode.gameUpdate(&inputA->KeyboardController);
            gameCode.gameRenderAudio(&soundPushBuffer);     // Call the game to fill an audio buffer
			gameCode.gameRenderGfx(&gfxPushBuffer);         // Call the game to fill a graphics buffer
		}
       /* win32_ProcessGameSound(
            &soundPushBuffer,
            &audioEngine.soundBuffer);          // push the game audio to the XAudio2 buffer
        */
        // audioEngine.srcVoice->Start(0);      // Start playing the buffer
        deviceContext = GetDC(mainWindow);      // Get a device context to the window
        win32_CopyBufferToWindow(               // FLIP
            deviceContext,
            DEFAULT_GFX_BUFFER_WIDTH, DEFAULT_GFX_BUFFER_HEIGHT,
            &frameBuffer, 0, 0
        );

        // update keyboard input pointers to reflect frame change
        inputA->KeyboardController = inputB->KeyboardController;
        win32_ProcessPendingMessages(       // check message queue
            &inputA->KeyboardController
        );              

    }
}