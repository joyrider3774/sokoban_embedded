# Sokoban Embedded Version
![DownloadCountTotal](https://img.shields.io/github/downloads/joyrider3774/sokoban_embedded/total?label=total%20downloads&style=plastic) ![DownloadCountLatest](https://img.shields.io/github/downloads/joyrider3774/sokoban_embedded/latest/total?style=plastic) ![LatestVersion](https://img.shields.io/github/v/tag/joyrider3774/sokoban_embedded?label=Latest%20version&style=plastic) ![License](https://img.shields.io/github/license/joyrider3774/sokoban_embedded?style=plastic)

Sokoban is a remake of the classic sokoban game. 

You as the player need to push boxes to specific locations in the level. 
Once all boxes are pushed to the correct spots the level is finished. 

The game comes with 19 built in level packs, made from ascii (.sok) level sets, with a total amount of nearly 2000 levels.
The game also has an undo system for the last 25 moves. 

## Devices
Every [release](https://github.com/joyrider3774/sokoban_embedded/releases) has a build for every device. `releases/` is where a build of your own puts them, it is not part of the repository:

| Device | File | How to install |
| ------ | ---- | -------------- |
| [ESPboy](https://www.espboy.com/) | ESPboy_Sokoban.bin | flash it, the board is a LOLIN(WEMOS) D1 mini |
| [Gamebuino META](https://gamebuino.com/gamebuino-meta) | GamebuinoMeta_Sokoban.bin | copy it into a folder on the SD card, the .hex is for flashing it directly |
| [Adafruit PyBadge](https://www.adafruit.com/product/4200) | PyBadge_Sokoban.uf2 | double press reset and copy it onto the drive that appears |
| [Adafruit PyGamer](https://www.adafruit.com/product/4242) | PyGamer_Sokoban.uf2 | same as the PyBadge |
| [Pimoroni PicoSystem](https://shop.pimoroni.com/products/picosystem) | PicoSystem_Sokoban.uf2 | hold X while switching on and copy it onto the drive that appears |
| [Pimoroni Explorer](https://shop.pimoroni.com/products/explorer?variant=42092697845843) | Explorer_Sokoban.uf2 | hold BOOT while pressing RESET and copy it onto the drive that appears |
| [Pimoroni Tufty 2350](https://shop.pimoroni.com/products/tufty-2350?variant=55811986227579) | Tufty_Sokoban.uf2 | hold HOME while pressing RESET and copy it onto the drive that appears |
| [TinyCircuits Thumby Color](https://tinycircuits.com/products/thumby-color) | ThumbyColor_Sokoban.uf2 | put it into bootloader mode and copy it onto the RPI-RP2 drive that appears |
| [Playdate](https://play.date/) | Playdate_Sokoban.pdx.zip | unzip it and sideload Sokoban.pdx, the same pdx runs in the Playdate simulator |
| [Libretro / RetroArch](https://www.retroarch.com/) | Libretro_Sokoban.zip | copy sokoban_libretro.dll into RetroArch's cores folder and sokoban_libretro.info into its info folder, then Load Core and Start Core |
| [Game Boy Advance](https://en.wikipedia.org/wiki/Game_Boy_Advance) | GBA_Sokoban.gba | put it on a flash cart or open it in an emulator, the progress is saved in the cartridge's SRAM |
| [Nintendo DS](https://en.wikipedia.org/wiki/Nintendo_DS) | NDS_Sokoban.nds | put it on a flash card or open it in an emulator, the progress is saved next to it in Sokoban.sav |
| [Nintendo 3DS](https://en.wikipedia.org/wiki/Nintendo_3DS) | 3DS_Sokoban.3dsx | copy it into /3ds/ on the SD card and start it from the Homebrew Launcher, or open it in an emulator, the progress is saved in sdmc:/3ds/Sokoban/ |
| [Nintendo 64](https://en.wikipedia.org/wiki/Nintendo_64) | N64_Sokoban.z64 | put it on a flash cart or open it in an emulator, the progress is saved in the cartridge EEPROM |
| [PlayStation](https://en.wikipedia.org/wiki/PlayStation_(console)) | PSX_Sokoban.exe | open it in an emulator or send it to a console that runs unsigned code, the progress is not saved yet |
| [PlayStation Portable](https://en.wikipedia.org/wiki/PlayStation_Portable) | PSP_Sokoban.PBP | rename it to EBOOT.PBP and put it in ms0:/PSP/GAME/Sokoban/ on the memory stick, or open it in PPSSPP |
| [PlayStation Vita](https://en.wikipedia.org/wiki/PlayStation_Vita) | Vita_Sokoban.vpk | install it with VitaShell on a Vita with homebrew enabled, or open it in Vita3K |
| Windows | Windows_Sokoban.exe | runs on its own, the progress is saved next to it in Sokoban.sav |
| MS-DOS | DOS_Sokoban.zip | unzip SOKOBAN.EXE onto a DOS machine or into DOSBox and run it, the progress is saved next to it in SOKOBAN.SAV |
| Browser | Web_Sokoban.zip | upload it to an itch.io HTML project, or unzip it and open index.html from a web server, the progress is saved in the browser |

`python tools/build_releases.py` builds all of them, `python tools/convert_skins.py` turns the images in `assets/skins` and `assets/skins2` into the headers the game includes and `python tools/convert_levelpacks.py` does the same for the level packs in `assets/levelpacks`. The Playdate build also needs the Playdate SDK, see `playdate/CMakeLists.txt`, the libretro core libretro-common, see `libretro/CMakeLists.txt`, the Game Boy Advance build devkitARM and libgba, see `gba/CMakeLists.txt`, the Nintendo DS build devkitARM, libnds and calico, see `nds/CMakeLists.txt`, the Nintendo 3DS build devkitARM and libctru, see `3ds/CMakeLists.txt`, the PlayStation build PSn00bSDK, see `psx/CMakeLists.txt`, the Nintendo 64 build the mips64-elf toolchain and libdragon, see `n64/CMakeLists.txt`, the PSP build the pspdev toolchain, see `psp/CMakeLists.txt` (pspdev has no Windows build, so on Windows it is built from WSL), and the Vita build VitaSDK, see `vita/CMakeLists.txt`, the browser build Emscripten, see `web/CMakeLists.txt`, and the MS-DOS build DJGPP, see `dos/CMakeLists.txt`.

### Buttons
The game's buttons on every device:

| Device | D-pad | A | B | L | R |
| ------ | ----- | - | - | - | - |
| ESPboy | d-pad | ACT | ESC | LFT | RGT |
| Gamebuino META | d-pad | A | B | MENU | HOME |
| Adafruit PyBadge | d-pad | A | B | SELECT | START |
| Adafruit PyGamer | joystick | A | B | SELECT | START |
| Pimoroni PicoSystem | d-pad | A | B | Y | X |
| Pimoroni Explorer | A up, C down, B left, Y right | X | Z | BOOT + B | BOOT + Y |
| Pimoroni Tufty 2350 | UP up, DOWN down, A left, C right | B | HOME | HOME + A | HOME + C |
| TinyCircuits Thumby Color | d-pad | A | B | left bumper | right bumper |
| Playdate | d-pad | A | B | menu: restart | menu: free view |
| Libretro | d-pad | A | B | L | R |
| Game Boy Advance | d-pad | A | B | L | R |
| Nintendo DS | d-pad | A | B | L | R |
| Nintendo 3DS | d-pad or circle pad | A | B | L | R |
| Nintendo 64 | d-pad | A | B | L | R |
| PlayStation | d-pad | Cross | Circle | L1 | R1 |
| PlayStation Portable | d-pad or the analog stick | Cross | Circle | L | R |
| PlayStation Vita | d-pad or the left stick | Cross | Circle | L | R |
| Windows | arrow keys | X | C | S | D |
| MS-DOS | arrow keys | X | C | S | D |
| Browser | arrow keys | X | C | S | D |

On the Explorer BOOT is held as a shift, the direction pressed with it is not sent.

On the Tufty 2350 a tap of HOME is B when it is let go, held HOME is a shift like BOOT on the Explorer. It has no speaker, the game is silent there. Holding RESET until the rear LEDs are dark puts it to sleep, a front button wakes it up again, with UP and DOWN held as well it goes into shipping mode instead.

The Thumby Color's display is 128x128, the game's own size, so it is shown 1:1 over the whole screen. That build has not been tried on the device itself yet.

The Playdate shows the black & white skin, scaled up in the middle of its display. It has no side buttons, L and R are the restart and free view entries of its system menu.

The Game Boy Advance shows the game scaled to 160x160 in the middle of its screen, with black bars at the sides. Scrolling around a level that is bigger than the screen is slow there: the game engine draws the whole screen again while the view moves.

On the Nintendo DS the game is scaled to 192x192 in the middle of the top screen, with black bars at the sides, and the bottom screen stays dark. What the game saves goes into Sokoban.sav on the card it was started from, so a card that libfat can not write to (or an emulator without one) plays the game but forgets it afterwards. Its tones are square waves played as a sample: the DS's own tone channels count their frequency in a 16 bit timer and can not go below about 256 Hz.

On the Nintendo 3DS the game is scaled to 240x240 in the middle of the top screen, with black bars at the sides, and the bottom screen stays dark. What the game saves goes into sdmc:/3ds/Sokoban/Sokoban.sav. Its tones play through the console's DSP when the DSP firmware has been dumped to the SD card (sdmc:/3ds/dspfirm.cdc), and through CSND when it has not: on hardware either one plays, in an emulator only the DSP one does.

On MS-DOS the game runs in VGA mode X, 320x240 in 256 colours, blown up to 240x240 in the middle of the screen with black bars at the sides. That mode rather than the usual 320x200 one because its pixels are square, where 320x200 is stretched over the same screen and would show the game a fifth too tall. The 256 colours are set to the RGB332 cube, which is exactly what the game's 8 bpp screen buffer holds, so a frame reaches the card without a colour being worked out. Its tones are a square wave on the PC speaker, the progress is saved next to the program in SOKOBAN.SAV, and Escape quits. The program is 32 bit and carries the CWSDPMI host inside it, so it needs nothing beside it on the disk.

In a browser the game is drawn into a canvas of its own 128x128 pixels, which the page stretches to whatever room it is given while keeping it square and keeping the pixels sharp. The progress is saved in the browser's localStorage under the game's name, so a private window plays it but forgets it afterwards. The zip holds index.html, index.js and index.wasm and is what an itch.io HTML project takes as it is.

On the Nintendo 64 the game is drawn into memory in the colours the RDP takes and the RDP shows it scaled to 240x240 in the middle of its 320x240 screen, with black bars at the sides. Its tones are a square wave written into the buffers the sound hardware plays from. The progress is saved in the cartridge EEPROM, which the ROM says it has, so a cartridge or an emulator without one plays the game but forgets it afterwards.

On the PlayStation the game is drawn into memory in the colours the GPU takes, handed to it as a texture and shown scaled to 240x240 in the middle of its 320x240 screen, with black bars at the sides. Its tones are a square wave the SPU plays from a single looping block. The memory card is not written yet, so what the game saves is gone when the console is switched off.

On the PlayStation Portable the game is doubled to 256x256 in the middle of the display, and the high scores are saved next to the EBOOT.PBP in Sokoban.sav.

On the PlayStation Vita the game is blown up four times to 512x512 in the middle of the display, and the high scores are saved in ux0:data/Sokoban/Sokoban.sav.

On the Gamebuino META holding HOME for a second goes back to its loader.

## Game Features:
- 19 level packs with about 2000 levels
- Max 25x16 level size, levels that are too big are left out
- Undo System for the last 25 moves
- 2 skins to choose from (default and black & white)
- Sound can be switched on or off
- Free view mode to look around levels that are bigger than the screen
- Autosaves progress per level pack

## Playing the Game:
Sokoban will start up at a main menu, here you can choose the level pack you wish to play by using left and right when the level pack name is selected (it's between "<>" signs).
If you are new to Sokoban i suggest that you play the "SokWhole" level pack first since these are not so hard to solve. 
When a level pack is loaded the level selector will show up. Here you select what level you wish to play, if it has been unlocked yet.
After you solved a level one extra level will be unlocked. you always have to solve to last unlocked level in order to unlock the next one.

### Adding level packs
Level packs are built into the game. To add one, put its .sok file in `assets/levelpacks`, run `python tools/convert_levelpacks.py` and add the pack to the level pack table in the source before building the game again.

## Controls

| Button | Action |
| ------ | ------ |
| DPAD | Select menu's, options, level packs or levels. Move the player in game, pan around the level in free view |
| A | Confirm in menus, level selector and questions asked. Hold it in game to undo moves |
| B | Back in options and level selector, ask to quit to the level selector in game, leave free view |
| L | Ask to restart the level |
| R | Enter or leave free view |
| (A) + Left + Down | Show or hide the debug info |

## Credits
Game is an adaptation from the gp2x version i (willems davy, aka joyrider3774) initially made many years ago

### Graphics
- wall (adapted): [1001.com](https://opengameart.org/content/sokoban-pack) - [Attribution-ShareAlike 3.0 Unported](https://creativecommons.org/licenses/by-sa/3.0/)
- floor (adapted): [Kenney Sokoban tiles](https://opengameart.org/content/sokoban-100-tiles) - [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/)
- player (adapted): [Kenney Sokoban tiles](https://opengameart.org/content/sokoban-100-tiles) - [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/)
- box (adapted): [SpriteAttack boxes and crates](https://opengameart.org/content/boxes-and-crates-svg-and-pngs) - [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/)

### Level (pack) Authors
The Game uses Levels created by different Authors almost all levelpacks are loaded through level set sok files. 

I used the sok files available for download on http://sokobano.de/en/levels.php as they contained the Authors as well as the title of the levels and comments in the files.
I implemented a parser for these levels so they can now be used with the game.

Back when i initially created the game for the gp2x i had my own format and this is still used when people create their own levels (inside the game). One such level pack that
is still available like this is the "pimpernel" level pack.

Below you can find all authors and details for the different .sok files that is supplied with this game.

#### 696.sok
- Author: Dries de Clercq

#### Minicosmos.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Microcosmos.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Nabokosmos.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Picokosmos.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Cosmopoly.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Myriocosmos.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### Cosmonotes.sok
- Author: Aymeric du Peloux
- Homepage:  https://aymericdupeloux.wixsite.com/sokoban

#### GRIGoRusha 2001.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### GRIGoRusha 2002.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### GRIGoRusha Remodel Club.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### GRIGoRusha Special.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### GRIGoRusha Star.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### GRIGoRusha Sun.sok
- Author: Evgeniy Grigoriev (GRIGoRusha)
- Homepage:  http://grigr.narod.ru/

#### SokEvo.sok
- Author: Lee J Haywood
- Homepage:  https://ljhaywood.uk/games/sokoban/

#### SokHard.sok
- Author: Lee J Haywood
- Homepage:  https://ljhaywood.uk/games/sokoban/

#### SokWhole.sok
- Author: Lee J Haywood
- Homepage:  https://ljhaywood.uk/games/sokoban/

#### LOMA.sok
- Author: Lee J Haywood
- Homepage:  https://ljhaywood.uk/games/sokoban/

#### Erim Sever Collection.sok
- Author: Erim Sever
- Homepage: https://web.archive.org/web/20191029215423/http://www.erimsever.com/e_sokoban.htm
