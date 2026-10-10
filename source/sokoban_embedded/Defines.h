#ifndef DEFINES_H
#define DEFINES_H

//the device comes first: the display library, SCREENBUFFER and IMAGESET are device settings, see
//PlatformESPboy.h / PlatformSDL.h
#include "PlatformDevice.h"

//1 = the art is read from a card while the game runs and none of it is in flash, see
//cardimages.h. It needs a device that can read one (PLATFORM_HAS_CARD in Platform.h) and the card
//file tools/mkcard.py writes. Every skin is then on the card in full RGB565 and the game can be
//asked for any of them, which is what flash could never hold
#ifndef CARDIMAGES
#define CARDIMAGES 0
#endif

//How much RAM a card build keeps its art in. A picture small enough to be worth it is read once
//and kept here, so drawing it again is a copy; a full screen one is read a row or a strip at a
//time and never kept. 0 is no arena at all. See the arena in cardimages.cpp
#ifndef CARDARENA
#define CARDARENA 2048
#endif

//1 = the full screen background is drawn as one plain colour (ColorWhite, which LoadGraphics
//sets per skin) instead of as a picture. Every row of this game's background is already one
//colour, so what is lost is the gradient and nothing else. It is for a card build: the
//background is 32768 bytes, too big to keep in the arena, and the board redraws strips of the
//screen, so it would be read off the card again and again. A skin whose background is a real
//picture must leave this off
#ifndef FLATBACKGROUND
#define FLATBACKGROUND 0
#endif

#define WINDOW_WIDTH 128
#define WINDOW_HEIGHT 128
#define HALFWINDOWWIDTH 64
#define HALFWINDOWHEIGHT 64
#define FPS 30
//1 = every frame waits until 1/FPS of a second has passed, 0 = a frame starts as soon
//as the last one is done, to see how fast the game can go. Movement, animation, input
//repeat and music all count frames, so without the lock they run faster as well.
//A build can set it itself
#ifndef FPSLOCK
#define FPSLOCK 1
#endif
//1 = the debug header (frame rate, free heap and stack) is always shown, Up + Down does not
//hide it. 0 = it starts hidden and Up + Down shows and hides it. A build can set it itself
#ifndef FORCEDEBUG
#define FORCEDEBUG 0
#endif
//1 = the colours of an image are spread over the ones the buffer can hold, so that a shade it
//has no colour for is a pattern of the two it does instead of the nearer of them. 0 = every
//colour becomes the nearest one there is, which shows as bands across anything that shades.
//An 8 bpp buffer is RGB332 and drops 2 bits of red, 3 of green and 3 of blue, and a 1 bpp buffer
//keeps only black and white, so both have something to spread. A 16 bpp buffer holds every colour
//of the image as it is and is left alone. A build can set this itself, see DitherSpread in
//Platform.h
#ifndef DITHERING
#define DITHERING 0
#endif

//1 = the floor tiles the player can reach are found with a floodfill every frame and
//drawn under the level, 0 = no floor is drawn and the floodfill, its bitmaps and its tile
//stack are left out of the build. A build can set it itself
#ifndef FLOODFILLFLOOR
#define FLOODFILLFLOOR 1
#endif
#define MAXSKINS 2
//the only skin a 1 bpp buffer can show
#define SKINBLACKWHITE 1

//FORCESKIN: -1 = every skin is built in and can be picked in the options, n = only skin n
//(0 default, 1 black & white) is built in and always used, which saves the flash of the others on a
//small device. A 1 bpp buffer has only two colours to show, so the
//black & white skin is the one it takes on its own. A build can still ask it for another one,
//whose shades then go through the brightness rule in SetBufferBit, and with DITHERING come out
//as a pattern of the two colours rather than as the nearer of them.
//Set by the device header or the build
//A card build names no skin either: every one of them is on the card in full RGB565 and the
//game is asked for one while it runs, see CardImages_UseSkin
#if !defined(FORCESKIN)
  #if (SCREENBUFFER == 1) && !CARDIMAGES
  #define FORCESKIN SKINBLACKWHITE
  #else
  #define FORCESKIN -1
  #endif
#endif
//1 when the images of skin n are part of the build
#define SKINBUILT(n) ((FORCESKIN < 0) || (FORCESKIN == (n)))

//1 when the black & white skin is in the build, whose pictures are packed one bit a pixel
//by tools/onebit.py and drawn by the routines in onebitimage.cpp rather than as RGB565. It
//shows two colours, and keeping each of them in sixteen bits costs both flash and the work
//of writing a colour per pixel. Every skin can be in the build here and picked in the
//options, so which kind a picture is cannot be known at build time: skinImagesOneBit says
//A card build has none of them: every skin is on the card in full RGB565, so there is no
//reduced form to read and nothing of the one bit paths is built
#define ONEBITIMAGES (!CARDIMAGES && SKINBUILT(SKINBLACKWHITE))

//1 when the black & white skin is the only one in the build. Every picture is then one bit a pixel
//and the paths that read RGB565 are dead: a build that is only ever going to draw one bit pictures
//need not carry the index the run length encoded background is read through, which is a row table
//the width of the screen
#define ONEBITONLY (ONEBITIMAGES && (FORCESKIN == SKINBLACKWHITE))

//image headers to build with: 1 = images (16x16 tiles), 2 = images2 (same images at half size, 8x8 tiles)
//set by the device header (PlatformESPboy.h / PlatformSDL.h) or by the build
#ifndef IMAGESET
#error "the device header has to define IMAGESET"
#endif

#if IMAGESET == 2
#define IMAGES_DIR images2
#define TileWidth 8
#define TileHeight 8
#define GameMoveSpeed 2					//dec if fps increases
#define PlayerAnimDelay 3				//inc if fps increases
#define ViewportMove 2					//dec if fps increases
#else
#define IMAGES_DIR images
#define TileWidth 16
#define TileHeight 16
#define GameMoveSpeed 4					//dec if fps increases
#define PlayerAnimDelay 3				//inc if fps increases
#define ViewportMove 4					//dec if fps increases
#endif

//path of an image header in the selected folder: #include GAME_IMAGE(box_RGB565_LE.h)
#define GAME_IMAGE_STR(x) #x
#define GAME_IMAGE_PATH(dir, file) GAME_IMAGE_STR(dir/file)
#define GAME_IMAGE(file) GAME_IMAGE_PATH(IMAGES_DIR, file)
#define NrOfRows 16
#define NrOfCols 25
#define NrOfColsVisible (WINDOW_WIDTH / TileWidth)
#define NrOfRowsVisible ((WINDOW_HEIGHT / TileHeight))
#define MaxHistory 25
#define ZEmpty 6
#define ZPlayer 5
#define ZBox 4
#define ZWall 3
#define ZSpot 2
#define ZFloor 1
#define IDPlayer 1
#define IDBox 2
#define IDWall 3
#define IDSpot 4
#define IDEmpty 5
#define IDFloor 6

//The pool of world parts, which is the largest single thing the game asks the heap for. A level
//can hold one part per playfield tile, so a full grid takes any level that could be written. The
//device header may ask for fewer, which a device with little ram has to: it can size itself from
//LEVELPACKMAXPARTS, the busiest level of the packs it actually ships, which is checked below
//where that is known
#ifndef MAXWORLDPARTS
#define MAXWORLDPARTS ((NrOfCols * NrOfRows)+2)
#endif
//only the parts that actually moved this frame get redrawn on top, that is the
//active player plus whatever it is pushing
#define MAXMOVEABLEWORLDPARTS 8
//>>> written by tools/convert_levelpacks.py from assets/levelpacks, do not edit by hand
//how many packs there are in all, which is what the saved unlocks are sized by
#define MaxLevelPacks 19

//LEVELPACKS: the level packs that are built in, an LP_ bit each. The size is what the pack
//takes in flash, which is its text run length encoded, see build_header. All of them unless
//the device header or the build picks fewer; a pack that is left out takes no flash and is
//not offered in the game
#define LP_696                     (1ul <<  0)    //696.sok                      59568 bytes
#define LP_Cosmonotes              (1ul <<  1)    //Cosmonotes.sok                4216 bytes
#define LP_Cosmopoly               (1ul <<  2)    //Cosmopoly.sok                 4776 bytes
#define LP_Erim_Sever_Collection   (1ul <<  3)    //Erim Sever Collection.sok    29969 bytes
#define LP_GRIGoRusha_2001         (1ul <<  4)    //GRIGoRusha 2001.sok          15778 bytes
#define LP_GRIGoRusha_2002         (1ul <<  5)    //GRIGoRusha 2002.sok           7089 bytes
#define LP_GRIGoRusha_Remodel_Club (1ul <<  6)    //GRIGoRusha Remodel Club.sok  33497 bytes
#define LP_GRIGoRusha_Special      (1ul <<  7)    //GRIGoRusha Special.sok        7402 bytes
#define LP_GRIGoRusha_Star         (1ul <<  8)    //GRIGoRusha Star.sok           5625 bytes
#define LP_GRIGoRusha_Sun          (1ul <<  9)    //GRIGoRusha Sun.sok            2073 bytes
#define LP_LOMA                    (1ul << 10)    //LOMA.sok                     12963 bytes
#define LP_Microcosmos             (1ul << 11)    //Microcosmos.sok               7578 bytes
#define LP_Minicosmos              (1ul << 12)    //Minicosmos.sok                7358 bytes
#define LP_Myriocosmos             (1ul << 13)    //Myriocosmos.sok               3471 bytes
#define LP_Nabokosmos              (1ul << 14)    //Nabokosmos.sok                7607 bytes
#define LP_Picokosmos              (1ul << 15)    //Picokosmos.sok                4251 bytes
#define LP_SokEvo                  (1ul << 16)    //SokEvo.sok                   12290 bytes
#define LP_SokHard                 (1ul << 17)    //SokHard.sok                  24971 bytes
#define LP_SokWhole                (1ul << 18)    //SokWhole.sok                 11550 bytes
#define LP_ALL ((1ul << 19) - 1)
#ifndef LEVELPACKS
#define LEVELPACKS LP_ALL
#endif
#if (LEVELPACKS & LP_ALL) == 0
#error "LEVELPACKS has to leave at least one level pack in"
#endif
//how many of them this build takes, which is how many the game lists
#define LEVELPACKCOUNT (((LEVELPACKS & LP_696) != 0) + ((LEVELPACKS & LP_Cosmonotes) != 0) + ((LEVELPACKS & LP_Cosmopoly) != 0) + ((LEVELPACKS & LP_Erim_Sever_Collection) != 0) + ((LEVELPACKS & LP_GRIGoRusha_2001) != 0) + ((LEVELPACKS & LP_GRIGoRusha_2002) != 0) + ((LEVELPACKS & LP_GRIGoRusha_Remodel_Club) != 0) + ((LEVELPACKS & LP_GRIGoRusha_Special) != 0) + ((LEVELPACKS & LP_GRIGoRusha_Star) != 0) + ((LEVELPACKS & LP_GRIGoRusha_Sun) != 0) + ((LEVELPACKS & LP_LOMA) != 0) + ((LEVELPACKS & LP_Microcosmos) != 0) + ((LEVELPACKS & LP_Minicosmos) != 0) + ((LEVELPACKS & LP_Myriocosmos) != 0) + ((LEVELPACKS & LP_Nabokosmos) != 0) + ((LEVELPACKS & LP_Picokosmos) != 0) + ((LEVELPACKS & LP_SokEvo) != 0) + ((LEVELPACKS & LP_SokHard) != 0) + ((LEVELPACKS & LP_SokWhole) != 0))

//The busiest level each pack has. The pool of world parts is the largest thing the game
//asks the heap for and no level fills the whole playfield, so a build wants no more slots
//than the packs it holds can fill, see MAXWORLDPARTS in the device header
#define LP_PARTS_696                       79
#define LP_PARTS_Cosmonotes                60
#define LP_PARTS_Cosmopoly                 59
#define LP_PARTS_Erim_Sever_Collection    161
#define LP_PARTS_GRIGoRusha_2001          192
#define LP_PARTS_GRIGoRusha_2002          122
#define LP_PARTS_GRIGoRusha_Remodel_Club  205
#define LP_PARTS_GRIGoRusha_Special        80
#define LP_PARTS_GRIGoRusha_Star           68
#define LP_PARTS_GRIGoRusha_Sun            71
#define LP_PARTS_LOMA                      59
#define LP_PARTS_Microcosmos               63
#define LP_PARTS_Minicosmos                49
#define LP_PARTS_Myriocosmos               90
#define LP_PARTS_Nabokosmos                53
#define LP_PARTS_Picokosmos                60
#define LP_PARTS_SokEvo                    60
#define LP_PARTS_SokHard                  222
#define LP_PARTS_SokWhole                  72

//how many parts the busiest level of the packs this build holds has. A pack that is
//left out counts for nothing, so the count follows what LEVELPACKS says
//One comparison a pack. LP_PARTS_MAX is a function and not a macro on purpose: a macro
//naming its first argument twice doubles the text at every step, which with nineteen
//packs put the compiler out of memory. constexpr keeps it usable where a constant is
//wanted, such as the static_assert below and the size of the pool
static inline constexpr int LP_PARTS_MAX(int a, int b) { return (a > b) ? a : b; }
#define LP_PARTS_OF(p) (((LEVELPACKS & LP_##p) != 0) ? LP_PARTS_##p : 0)
#define LEVELPACKMAXPARTS LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(LP_PARTS_MAX(0, LP_PARTS_OF(696)), LP_PARTS_OF(Cosmonotes)), LP_PARTS_OF(Cosmopoly)), LP_PARTS_OF(Erim_Sever_Collection)), LP_PARTS_OF(GRIGoRusha_2001)), LP_PARTS_OF(GRIGoRusha_2002)), LP_PARTS_OF(GRIGoRusha_Remodel_Club)), LP_PARTS_OF(GRIGoRusha_Special)), LP_PARTS_OF(GRIGoRusha_Star)), LP_PARTS_OF(GRIGoRusha_Sun)), LP_PARTS_OF(LOMA)), LP_PARTS_OF(Microcosmos)), LP_PARTS_OF(Minicosmos)), LP_PARTS_OF(Myriocosmos)), LP_PARTS_OF(Nabokosmos)), LP_PARTS_OF(Picokosmos)), LP_PARTS_OF(SokEvo)), LP_PARTS_OF(SokHard)), LP_PARTS_OF(SokWhole))
//<<<

//The pool has to take the busiest level of the packs this build ships. A build that takes every
//pack wants a full grid, which is what MAXWORLDPARTS is by default
static_assert(MAXWORLDPARTS >= LEVELPACKMAXPARTS, "a level of a pack in this build would not fit the pool");
#define InputDelay 16
#define MaxLevelPackNameLength 50

#define GSTitleScreen 1 
#define GSCredits 2
#define GSGame 3 
#define GSStageSelect 4
#define GSOptions 5

#define GSTitleScreenInit 51
#define GSCreditsInit 52
#define GSGameInit 53
#define GSStageSelectInit 54
#define GSOptionsInit 55

#define IDSolvedLevelNextUnlocked 2
#define IDSolvedLastLevel 3
#define IDSolvedEarlierLevel 4
#define IDNoPlayer 5
#define IDMoreSpotsThanBoxes 6
#define IDNoLevelsInPack 7
#define IDCurrentLevelNotSaved 8
#define IDRestartLevel 9
#define IDDeleteAllParts 10
#define IDDeleteLevelPack 11
#define IDDeleteLevel 12
#define IDLevelNotUnlocked 13
#define IDLevelInfo 14
#define IDQuitPlaying 15


#define MenuUpdateTicks 10
//frames between repeats while a direction is held in the level selector
#define LevelSelectUpdateTicks 5

#endif