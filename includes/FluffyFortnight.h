#ifndef FLUFFYFORTNIGHT_H
#define FLUFFYFORTNIGHT_H
/******************************************************************************
 * PROTOTYPE
 * 1    - Prototype build
 * 0    - Release build
 *****************************************************************************/
#define PROTOTYPE 1
#define MAX_INPUT_COUNT 2
#define KEYBOARD_INPUT 0
#define GAMEPAD_INPUT 1
#define MAX_BUTTON_COUNT 12

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <stdint.h>

// PROTOTYPING - TODO: Replace math functions with more efficent versions
#if PROTOTYPE
#include <math.h>              
#include <Windows.h>
#endif

/******************************************************************************
 * MACROS
 *****************************************************************************/
#define KILOBYTES(value) ((value) * 1024LL)
#define MEGABYTES(value) (KILOBYTES(value) * 1024LL)
#define GIGABYTES(value) (MEGABYTES(value) * 1024LL)
#define TERABYTES(value) (GIGABYTES(value) * 1024LL)
#define ASSERT(exp) \
    if(!(exp)) { *(uint32_t *)0 = 0; }

/******************************************************************************
 * CONST
 *****************************************************************************/
const int DEFAULT_GFX_BUFFER_WIDTH = 1920;
const int DEFAULT_GFX_BUFFER_HEIGHT = 1080;
const float PI32 = 3.14159265359f;
uint8_t TILE_DATA00[9][16] = {
    { 0,0,0,1, 1,1,1,2, 1,1,1,1, 1,1,1,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 }
};

uint8_t TILE_DATA01[9][16] = {
    { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1 }
};
uint8_t TILE_DATA10[9][16] = {
    { 0,0,0,1, 1,1,1,2, 1,1,1,1, 1,1,1,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 }
};

uint8_t TILE_DATA11[9][16] = {
    { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 },
    { 1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1 }
};


/******************************************************************************
 * STRUCTS
 *****************************************************************************/
// Struct representing a location in the game world coordinates
struct game_UnifiedPosition {  
    uint32_t    MapX;       // Upper 16 bits = MapX; Lower 16 bits TileX
    uint32_t    MapY;       // Upper 16 bits = MapY; Lower 16 bits TileY
    uint32_t    MapZ;       // Upper 16 bits = WorldZ; Lower 16 bits MapSetZ
};

// struct representing a map
// TODO: Store in Compressed Sparse Row or Compressed Sparse Column
struct game_TileMap {
    float   offsetX;    // offset for the entire map
    float   offsetY;

    int     CountX;     // number of horizontal tiles
    int     CountY;     // number of vertical tiles

    float   tileWidth;  // width of a tile
    float   tileHeight; // height of a tile

    uint8_t* data ;
};

struct game_MapSet {
    int             CountX;
    int             CountY;

    game_TileMap*   data;
};

// struct containing the state of the game persistant from frame to frame
struct game_State {
    float*          t;          // persistent time
    game_UnifiedPosition*  playerPosition;
    float*          playerX;    
    float*          playerY;
    uint16_t*       currentMap;
    uint16_t*       inputContext;
    game_TileMap*   Level[2];
};

// struct containing the different memory partitions
// TODO: implement a custom allocator to manage the working memory
struct game_Memory {
    bool            isInitialized;                  
    
    uint64_t        persistentStorageSize;           // size of the persistent partition
    void*           persistentStorage;               // next available byte of persistent memory
    void*           persistentStorageHead;           // start of persistent memory

    uint64_t        tempStorageSize;                // size of the temporary partition
    void*           tempStorageHead;                // start of working memory
    void*           tempStorage;                    // next available byte of working memory
};


// struct representing a platform independent graphics buffer
// for the game to work with.
struct game_GfxBuffer {
    void*       memory;
    int         width;
    int         height;
    int         channelCount;
    int         pitch;
};

struct game_Color {
    float       red;
    float       green;
    float       blue;
    float       alpha;
};

// struct representing a tile on a tilemap
// TODO: Change to include Sub-tiles (NES era tiles)
// TODO: Replace color rectangles with bitmaps
struct game_Tile {
    game_Color* color;
    uint8_t     isPassable;
};

// struct representing a platform independent sound buffer for the game to work with.
// TODO: adding information for mixing (i.e. Volume)
// TODO: add ENUM for underlying different underlying engines
struct game_SoundBuffer {
    bool            isInitialized;
    int             samplesPerSecond;
    int             sampleCount;
    int             bufferSize;
    uint16_t*       samples;
};

// struct encapsulating a thread for the game to use.
struct game_Thread {
    uint32_t*       threadId;
    void*           threadCallBack;
};

// struct encapsulating the state of a button
struct game_ButtonState {
    uint16_t    stateChangeCount;       // used for sub-frame updates
    bool        isDown;
};

struct game_ControllerInput {
    // Analog components of a controller
    bool            isAnalog;
    // Left Analog Stick State
    float           avgLX;
    float           avgLY;
    // Right Analog Stick State
    float           avgRX;
    float           avgRY;
    // Controller Vibration
    uint16_t        LeftVibration;      // Output
    uint16_t        RightVibration;     // Output
    // digital components of a controller
    union {     
        game_ButtonState        Buttons[MAX_BUTTON_COUNT];
        struct {    
            // face buttons
            game_ButtonState    Top;
            game_ButtonState    Bottom;
            game_ButtonState    Left;
            game_ButtonState    Right;
            game_ButtonState    Start;
            game_ButtonState    Select;
            game_ButtonState    LeftShoulder;
            game_ButtonState    RightShoulder;
            // D-Pad
            game_ButtonState    North;
            game_ButtonState    South;
            game_ButtonState    West;
            game_ButtonState    East;
        };
    };
};

// struct encapsulating all sources of input used by the game layer
struct game_Input {
    // TODO: Insert clock values        
    union {
        game_ControllerInput    Controllers[MAX_INPUT_COUNT];
        struct {
            game_ControllerInput    KeyboardController;
            game_ControllerInput    GamePadController;
        };
    };   
};

/******************************************************************************
 * External Bindings
 *****************************************************************************/
// PROTOTYPING CODE
#if PROTOTYPE
    void* win32_ReadFromDisk(char* filename);
    bool win32_WriteToDisk(char* filename, uint32_t memorySize, void* memory);
    void win32_FreeFileMemory(void* memory);
#endif

#define GAME_RENDER_GFX(name) void name(game_GfxBuffer* gfxBuffer)
#define GAME_INIT(name) void name(void* memory)
#define GAME_RENDER_AUDIO(name) void name(game_SoundBuffer* soundBuffer)
#define GAME_UPDATE(name) void name(game_ControllerInput* controllerInput)
// TODO: Add additional bindings here

/******************************************************************************
 * The game_render function takes a reference to a game_GfxBuffer struct and 
 * renders the game screen to it.
 * @param gfxBuffer - The game_GfxBuffer struct to modify.
 *****************************************************************************/
typedef GAME_RENDER_GFX(game_RenderGfx);
typedef GAME_INIT(game_Init);
typedef GAME_RENDER_AUDIO(game_RenderAudio);
typedef GAME_UPDATE(game_Update);

/******************************************************************************
 * Public Functions
 *****************************************************************************/


#endif