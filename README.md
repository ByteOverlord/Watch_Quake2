![Logo](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Watch_Quake2_Logo_01.png)
03.09.2023
<p align="center">
  <img src="https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Watch_Quake2_demo1_00.gif" width="35%" />
</p>

Watch_Quake2 is a Quake 2 port specifically designed for Apple Watch, incorporating software rendering and working video and audio playback capabilities.

## Contents

[**About**](#about)  
[**Changes by ByteOverlord**](#changes)  
[**Features**](#features)  
[**Changelog**](#changelog)  
[**Game Size**](#size)  
[**Battery Usage**](#battery)  
[**Benchmark**](#benchmark)  
[**Known Issues**](#issues)  
[**Building Requirements**](#requirements)  
[**Building the Watch_Quake2**](#building)  
[**Adding music to Watch_Quake2 (Optional)**](#music)  
[**Credits**](#credits)  
[**License**](#license)  

## <a name="about"></a>About


<p align="center">
  <img src="https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/wquake2_ico_00.png" width="15%" " />
</p>

Following the successful release of the Quake 1 port for Apple Watch, our team set an ambitious objective: to bring Quake 2 to this portable device. However, porting the 1997 release of Quake posed significant challenges. We encountered initial setbacks that considerably delayed the release schedule for several months. ByteOverlord made commendable progress early in the porting process, but crashing issues persisted. After a brief hiatus, we resumed our efforts and focused on improving the stability of the game port, ultimately achieving a more reliable and robust build.

The primary objective of this project was to demonstrate the remarkable capabilities of the Apple Watch, even when operating in software rendering mode. This project serves as a glimpse into the untapped potential that could be unlocked if we were granted access to the device's GPU. Just envision the possibilities that could be achieved with such resources at our disposal.

Previous port:  
Quake 1 for Apple Watch https://github.com/ByteOverlord/Watch_Quake 18.11.2022

![Photo](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/WQ2_Photo_2.jpg)

## <a name="changes"></a>Changes by ByteOverlord

* Swift for UI drawing, audio callback and input processing
* Music playback support added. Audio file format “Mono, 22050 Hz, WAV, IMA ADPCM” preferred for lower processing and minimal space required
* Cheats an maps menus added. Custom graphics for these menus included with pak3.pak
* Resolution setting is automatic with hud, menu and console notification placements affected by resolution
* Transparent surface drawing fixed for 64bit
* Game pausing through Swift UI when application enters background state and saves settings. Mutes audio when inactive state
* Saving and loading rewritten for 64bit (referenced Yamagi Quake 2). Might still have some bugs
* Video playback’s audio sample rate changed to match Apple Watch’s sample rate
* Video cinematic’s audio volume control added to options
* Initial game loading fixed so that game and audio loops start after the Swift UI tells the app is active (≥ WatchOS 9.2). When content view updates the game starts (< WatchOS 9.2)
* Touch input handling through Swift UI. Customized touch control layout for movement, camera, interaction and menu controls
* Benchmark mode that logs average, low and high frame times. Uses the Quake’s DEMO1 and DEMO2 playbacks

![Photo](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/WQ2_Photo_3.jpg)

## <a name="features"></a>Features

* pak0, pak1 and pak2 loading
* Menu screen
* Save and load game
* Autosaving of options
* Map quick select screen
* Cheat activation screen
* Tweaked controls for watch interface
* Video and Music playback
* Benchmark mode
* Automatic native resolution (Series 4 40mm -> Ultra)

![Controls](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Menu_Controls.jpg)

![Controls](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/In_Game_Controls.jpg)

## <a name="changelog"></a>Changelog

**08.10.2023**

* **<em>Game view position fixed for watchOS 10</em>**

<em>06.09.2023</em>

* <em>Cheats added to the menu (noclip and notarget)</em>
* <em>Menu graphics updated (pak3.pak)</em>

## <a name="size"></a>Game size 
 
Complete game with optional video and music takes ~ 403,4 MB on the watch.

```
pak0.pak is 184 MB
pak1.pak is 13 MB
pak2.pak is 45 KB
Video Files 163,7 MB
Music Tracks are 38,7 MB (converted with Audacity (Mono, 22050 Hz, WAV, IMA ADPCM))
```

## <a name="battery"></a>Battery Usage
 
 Usage listed as % the charge dropped during the test.
 
Model  | Battery Health  | 30 min  | 60 min  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | -  | -  | -  |
S4 44mm  | -  | -  | -  |
S5 40mm  | -  | -  | -  |
S5 44mm  | 95%  | 17%  | 33%  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | -  | -  | -  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

<em>Results are only indicative.</em> 

## <a name="benchmark"></a>Benchmark

#### Reference table:  

Good  | Moderate  | Bad
------------- | ------------- |-------------
≤ 5 ms  | 6-12 ms  | > 16 ms  |

#### Results: 

DEMO1  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 6.80  | 2.85  | 12.96  |
S4 44mm  | 7.12  | 2.90  | 14.08  |
S5 40mm  | 6.95  | 2.55  | 16.72  |
S5 44mm  | 7.28  | 3.19  | 14.89  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | 7.71  | 2.57  | 14.29  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

DEMO2  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 7.10  | 2.93  | 15.36  |
S4 44mm  | 7.58  | 4.31  | 16.79  |
S5 40mm  | 7.23  | 3.35  | 14.50  |
S5 44mm  | 7.20  | 4.17  | 14.43  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | 7.96  | 3.45  | 18.13  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

<em>Benchmarks are only indicative.</em> 

## <a name="issues"></a>Known Issues

* Player camera jumps after saving or loading a game
* Mods are not supported
* Multiplayer is not supported
* Audio will not transmit through Bluetooth
* Sound does not playback on app start (Put the game to the dock and open it from there again.)
* App does not start anymore (Build the app to the watch from Xcode). There is a 7 day testing period on a 'Free Provisioning' profile.

## <a name="requirements"></a>Building Requirements

* Apple Watch Series 4 or later with watchOS 9.1 or later
* iPhone 8 or later with iOS 16 or later
* Mac with macOS 12.5 or later
* Xcode 14.1 or later with command-line tools
* Homebrew
* Innoextract
* bchunk (only for music)
* Audacity (only for music)

## <a name="building"></a>Building the Watch_Quake2

1) Install Xcode through Apple AppStore and install command-line tools when Xcode asks for it.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_0.jpg)

2) Download the Watch_Quake2 from GitHub. (<https://github.com/ByteOverlord/Watch_Quake2>)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00.jpg)

3) Get Quake II (Original) through gog.com

4) Download the Quake II (Original) installer from your gog.com profile:  
 `"gog.com Profile" -> Games -> Quake II (Original) -> View Downloads -> Download Offline Backup Game Installers -> Quake II (Original)`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_01_A.jpg)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_01.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_02.jpg)

5) Install Homebrew from terminal:   
`ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/homebrew/go/install)"`
	
6) Install innoextract from Homebrew:

```
brew update
brew install innoextract
```

7) In terminal extract the Quake 2 game files with innoextract:   
`innoextract “downloaded setup_quake2_quad_damage_2.0.0.3.exe file location”`

(innoextract will extract an “app” and "tmp" folders in current working directory.)	
"app" folder has the needed PAK files (app/baseq2/), optional video (app/baseq2/video/) and optional audio (app/music/).

8) Copy pak0.pak, pak1.pak and pak2.pak to WatchQuake2 project (pak names have to be lowercase (case sensitive))  

`/WatchQuake2 Watch App/Resources/baseq2/`

Optional video files can be placed in:
`/WatchQuake2 Watch App/Resources/baseq2/video/`

9) Open the GitHub WatchQuake Xcode project.

10) In WatchQuake2 project file go to:  
`WatchQuake2 -> Targets -> WatchQuake2 -> Signing & Capabilities`

11) Select your "Team" and write a unique “Bundle Identifier”.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_03.jpg)

12) Then in WatchQuake2 project file go to:  
 `WatchQuake2 -> Targets -> WatchQuake2 Watch App -> Signing & Capabilities`

13) Select your "Team" and use the same “Bundle Identifier” as previously but with “.watchkitapp” added at the end.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_04.jpg)

If the project “Frameworks” folder shows the framework texts in red continue with step 14.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_05A.jpg)

If the project “Frameworks” folder shows the framework texts in white jump to step 17.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_05AB.jpg)
	
14) In WatchQuake2 project file go to:  
`WatchQuake2 -> Targets -> WatchQuake2 -> Build Phases -> Link Binary With Libraries`

15) Add these frameworks:

```
AVFoundation.framework
CoreGraphics.framework
Foundation.framework
SwiftUI.framework
UIKit.framework
WatchKit.framework
```

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_05B.jpg)

16)  And after adding them delete the frameworks with red text.

17) Check that the project is on release setting.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_06.jpg)

18) Test the build first with a simulator (Series 4 Simulator or newer (watchOS Simulator))

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_06A.jpg)

After building, the game will start and playback demo1.

![Simulator](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Simulator_0.png)

Click the game view to go to main menu.

![Simulator](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Simulator_1.png)

19) After testing connect iPhone to the Mac with a cable and make sure Apple Watch has a connection to iPhone.

20) Set the iPhone and Apple Watch in to "Developer" mode.

On iPhone:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_A.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_B.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_C.jpg)

On Apple Watch:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_D.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_E.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_00_F.jpg)

After the devices have restarted.  

21) Select the Apple Watch as target.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_07.jpg)

22) From Xcode build the Watch_Quake2 to the watch.  

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_08.jpg)

Transfer times:  
2 min (iPhone X & S4 40mm)  
2 min (iPhone X & S4 44mm)  
7 min (game, music), 10 min (game, videos, music), (iPhone X & S5 44mm))  
15 min (iPhone 12 mini & S5 40mm)    

![Watch](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Watch_1.png)

## <a name="music"></a>Adding music to Watch_Quake2 (Optional)

1) Locate the music tracks (track02 - track21.ogg) that where extracted from the Quake 2 installer (app/music/).

2) Open Audacity and drag the tracks to it. Edit the tracks to Mono.  
`Select -> All`  
`Tracks -> Mix -> Mix Stereo Down to Mono`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Music_Install_Guide_00.jpg)  

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Music_Install_Guide_00A.jpg)  

3) Set the “Project Rate (Hz)” to 22050.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Music_Install_Guide_01.jpg)

4) Export tracks with “Export Multiple” in “WAV” format with “IMA ADPCM” encoding.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Music_Install_Guide_02.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Music_Install_Guide_03.jpg)

5) Place the exported audio files (track02 - track21.wav) inside the WatchQuake2 project:   
`“WatchQuake2 Watch App/Resources/baseq2/music/”`

6) From Xcode build the Watch_Quake2 to the watch.  

![Screenshot](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/Game_Install_Guide_08.jpg)

Transfer times:  
2 min (iPhone X & S4 40mm)  
2 min (iPhone X & S4 44mm)  
7 min (game, music), 10 min (game, videos, music), (iPhone X & S5 44mm))  
15 min (iPhone 12 mini & S5 40mm)   

![Photo](https://github.com/ByteOverlord/Watch_Quake2/raw/main/README_Images/WQ2_Photo_1.jpg)

## <a name="credits"></a>Credits

quake\_watch <https://github.com/MyOwnClone/quake_watch>  
Quake 2 source code by Id Software. <https://github.com/id-Software/Quake-2>  
Yamagi Quake II (game saving) <https://github.com/yquake2/yquake2>  
App icon, images and documentation by IdeaVoid

## <a name="license"></a>License
This software is released under the terms of the GNU General Public License v2.
