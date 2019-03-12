#ifndef FLUFFYFORTNIGHT_H
#define FLUFFYFORTNIGHT_H
/******************************************************************************
 * PROTOTYPE
 * 1    - Prototype build
 * 0    - Release build
 *****************************************************************************/
#define PROTOTYPE 1


/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <stdint.h>

// PROTOTYPING - TODO: Replace math functions with more efficent versions
#if PROTOTYPE

#include "FluffyFortnightIntrinsic.h"            
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
#define MAP_LOCATION_SHIFT 0x10
#define MAP_LOCATION_MASK (1 << MAP_LOCATION_SHIFT) - 1
#define DEFAULT_GFX_BUFFER_WIDTH 1920
#define DEFAULT_GFX_BUFFER_HEIGHT 1080
#define MAX_INPUT_COUNT 2
#define KEYBOARD_INPUT 0
#define GAMEPAD_INPUT 1
#define MAX_BUTTON_COUNT 12
#define PI32 3.14159265359f
#define TILE_PAGE_FRAME_SIZE 9
#define TILE_PAGE_FRAME_WIDTH 3

uint8_t TILE_DATA00[144] = {
    0,0,0,1, 1,1,1,2, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA01[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA02[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA10[144] = {
    0,0,0,1, 1,1,1,2, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA11[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA12[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA20[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

uint8_t TILE_DATA21[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};
uint8_t TILE_DATA22[144] = {
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1,
    1,1,1,1, 1,1,1,3, 1,1,1,1, 1,1,1,1
};

/******************************************************************************
 * STRUCTS
 *****************************************************************************/
// Struct containing temporary data used to describe a location on the world
// map.
struct game_WorkingPosition {  
    // Coordinates describing a location on a TileMap in TilePages
    uint16_t    TileMapX;       
    uint16_t    TileMapY;

    // Coordinates describing a location on a TilePage in Tiles
    uint16_t    TilePageX;
    uint16_t    TilePageY;

    // Vector components relative to the Bottom-Left corner of the Tile describing
    // a Location in Tiles
    // TODO: Encapsulate this into an actual vector struct
    float       TileX;      // X(i) component
    float       TileY;      // Y(j) component
    // Vector components relative to the Bottom-Left corner of the Tile describing
    // a Location in Units
    // TODO: Encapsulate this into an actual vector struct
    float       UnitX;      // X(i) in meters
    float       UnitY;      // Y(j) in meters
};

// Struct containing data used to describe an absolute location on the world
// map.
// 16-bits      | 16-bits
//------------------------------
// TileMapIndex | TilePageIndex
// MAX SIZE 65,535 TilePages @ 65,535 Tiles in 1 direction
struct game_UnifiedPosition {
    uint32_t    TileMapX;       // The absolute X coordinate of a position on the Tile Map
    uint32_t    TileMapY;       // The absolute Y coordinate of a position on the Tile Map

    // Vector components relative to the Bottom-Left corner of the Tile
    // TODO: Encapsulate this into an actual vector struct
    float       TileX;
    float       TileY;
};

// struct representing a tile on a tilemap
// TODO: Change to include Sub-tiles (NES era tiles)
// TODO: Replace color rectangles with bitmaps
struct game_Tile {
    float width;        // Width of a Tile in UNITS
    float height;       // Height of a Tile in UNITS
    
    //TODO: Fill out with other tile properties.
};

// struct representing a page of the tile map. Map data is paged
// into memory, TilePage at a time to avoid wasting memory.
// TODO: Store in Compressed Sparse Row or Compressed Sparse Column
struct game_TilePage {
    uint16_t    width;      // number of horizontal tiles in the TilePage
    uint16_t    height;     // number of vertical tiles in the TilePage

    // TODO: Move these into the Tile struct
    float       tileWidth;
    float       tileHeight;

    // TODO: Make these Tiles instead of chars
    uint8_t*    data;      // Array of Tile data
};

// struct representing an entire game map.
struct game_TileMap {
    uint16_t    width;      // width in TilePages
    uint16_t    height;     // height in TilePages
    uint8_t     style;      // streaming map or discrete

    // TODO: implement this
    game_TilePage*   data;
};

// struct containing the state of the game persistant from frame to frame
struct game_State {
    float*          t;          // persistent time
    game_UnifiedPosition*  playerPosition;
    float*          playerY;
    uint16_t*       currentMap;
    uint16_t*       inputContext;
    game_TileMap*   map;
};

// struct containing the different memory partitions
// TODO: implement a custom allocator to manage the working memory
// TODO: combine parts of GameState into tempStorage
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



// struct representing a platform independent sound buffer for the game to work with.
// TODO: adding information for mixing (i.e. Volume)
// TODO: add ENUM for underlying different underlying libraries
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
            // Face Buttons
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
#define GAME_UPDATE(name) void name(game_ControllerInput* controllerInput, game_GfxBuffer* gfxBuffer)
// TODO: Add additional bindings here

/******************************************************************************
 * The game_render function takes a reference to a game_GfxBuffer struct and 
 * renders the game screen to it.
 * @param gfxBuffer - The game_GfxBuffer struct to modify.
 *****************************************************************************/
typedef GAME_INIT(game_Init);
typedef GAME_RENDER_AUDIO(game_RenderAudio);
typedef GAME_UPDATE(game_Update);

/******************************************************************************
 * Public Functions
 *****************************************************************************/


#endif