# Cardputer Game Station w/ BLE controllers

This version is a fork from [geo-tp/Cardputer-Game-Station-Emulators v1.2](https://github.com/geo-tp/Cardputer-Game-Station-Emulators) for the sole purpose of adding standard BLE gamepad support.  I am not taking credit for any of the work done before me.

## !!! ADDED BLE SUPPORT !!!

![xbox one controller](images/xbox_ble.jpg)

Tested with the following controllers

* Xbox One Controller (must be BLE, e.g. updated edition has share button)
* [M5MiniJoyC](https://github.com/davervw/m5_minijoystickc_gamepad) (attached to M5Stick, using my custom BLE firmware)
* [ble_gamepad_KanoPixelKit](https://github.com/davervw/ble_gamepad_KanoPixelKit)

Note: Classic Bluetooth controllers are not supported by ESP32-S3, so only BLE controllers are supported. See [gamepad_test](https://github.com/davervw/gamepad_test) for testing BLE controller pairing with ESP32.

Unfortunately only a subset of consoles and games are supported, included in this version, as due to resource constraints.  See original (link at top of file) for more consoles.

* Atari 2600
* Atari 7800
* Gameboy
* Gameboy Color
* NES

![ble_gamepad_KanoPixelKit](images/KanoPixelKit_GamePad.jpg)

## Original README continues... 
WARNING: (though lightly edited - removed consoles from table - still mentions non-supported consoles, emulators, etc.)

![NES emulator screen captures on the M5Stack Cardputer](images/nes_emulator_s.jpg)
![GBC emulator screen captures on the M5Stack Cardputer](images/gbc_emulator_s.jpg)
![SMS emulator screen captures on the M5Stack Cardputer](images/sms_emulator_s.jpg)
![NGP emulator screen captures on the M5Stack Cardputer](images/ngp_emulator_s.jpg)
![Megadrive emulator screen captures on the M5Stack Cardputer](images/megadrive_emulator_s.jpg)

Powered by [Nofrendo](https://github.com/moononournation/arduino-nofrendo), [Snes9x](https://github.com/snes9xgit/snes9x), [Smsplus](https://github.com/ducalex/retro-go/tree/master/retro-core/components/smsplus), [Race](https://github.com/libretro/RACE), [Gwenesis](https://github.com/bzhxx/gwenesis), [Oswan](https://github.com/alekmaul/oswan), [GnuBoy](https://github.com/rofl0r/gnuboy), [Handy](https://github.com/libretro/libretro-handy), [PCE-GO](https://github.com/ducalex/retro-go/tree/master/retro-core/components/pce-go), [Stella](https://stella-emu.github.io/), [Pro-System](https://github.com/libretro/prosystem-libretro), [fMSX](https://fms.komkon.org/fMSX/) and [Arnold](https://github.com/gameblabla/arnold_gcw0). 

All cores were optimized to run within a memory footprint of **under 256 KB of RAM**, with particular emphasis on **minimizing both RAM and flash usage**. All 13 cores are packed **into just 2.5 MB of flash**, and no RAM is allocated for a core until it is actualloy launched.

 Console           | Sound | Video | Save | Speed | All Games  | Notes |
|-------------------|--------|--------|---------------|-------------|-------------------|--------|
| **NES**           | ✅ | ✅ | ✅  | ✅ | ✅ | Few mappers issues in some games |
| **Game Boy**      | ✅ | ✅ | ✅  | ✅ | ✅ | Mono/Color support, Fully compatible |
| **Atari 2600**    | ✅ | ✅ | N/A | ✅ | ✅ | Few cart mappers not supported |
| **Atari 7800**    | ✅ | ✅ | N/A | ⚠️ | ✅ | Not full speed in some games |

It runs **`.nes` `.gb` `.gbc` `.a26` `.a78` ROM files from the SD**.

> **Make sure your ROMs are uncompressed** (not .zip, .7z, or .rar).

## Controls

The built-in **Cardputer keyboard** is used for all controls: 

| Function | Cardputer Key | Description |
|---------------|---------------|-------------|
| 🕹️ Up | **E** | Move up |
| 🕹️ Down | **S** | Move down |
| 🕹️ Left | **A** | Move left |
| 🕹️ Right | **D** | Move right |
| 🅰️ Button A | **K** | Primary action / confirm |
| 🅱️ Button B | **L** | Secondary action / cancel |
| ▶️ Start | **1** | Start / pause |
| ⏸️ Select | **2** | Select / menu |
| 💡 Brightness + | **]** | Increase LCD brightness |
| 💡 Brightness − | **[** | Decrease LCD brightness |
| 🔊 Volume + | **+** | Increase audio volume |
| 🔊 Volume − | **-** | Decrease audio volume |
| 🖥️ Screen Mode | **\\** | Toggle screen display mode |
| 🔍 Zoom − | **Fn + ←** | Zoom out |
| 🔍 Zoom + | **Fn + →**| Zoom in |
| 🔘 Quit Game | **G0 (hold 1 s)** | Go back to menu |

> The `j` key is also bound as Button A to allow an alternative layout for player preference.

> The `z` key is also bound as Arrow Down to allow the use of a D-PAD.

## M5Stack Joystick

You can alternatively use the M5Stack Joystick v1.1 (U024-C) or Joystick2 (U024-V2), **just plug it in before launching a game** and it will work automatically.

<img src="images/m5stack_joysticks.jpg" alt="A photo of the M5Stack Joysticks" width="800" height="400">

## D-Pad 3D Model

[Cardputer-Accessories repo](https://github.com/AndreiVladescu/Cardputer-Accessories) to get the 3D model for D-Pad that you can put on the Cardputer's keys. (Thanks to @AndreiVladescu)

[![A render of the 3D DPAD model](images/cardputer_gamepad_render.jpg)](https://github.com/AndreiVladescu/Cardputer-Accessories)

## Zoom Mode

The Zoom Mode allows you to **dynamically adjust the display scale of games** on the Cardputer’s screen.

By pressing `\` (above the `OK` key), you can toggle between **multiple zoom levels (100 to 150%), fullscreen or 4/3**. This flexibility ensures that each game looks its best on the Cardputer’s compact display. 

> In the SNES emulator, pressing `\` toggles between adaptive interlaced and lower-resolution modes for better performance.

> in the GameBoy (not color) emulator, pressing `\` toggles between different color palettes.

You can precisely adjust the display zoom level with `fn` + `arrows left/right`.

## About Games

You can place the **ROM uncompressed files** anywhere on your SD card and select them. The firmware allows running ROMs up to 5.5 MB.

> **⚠️ Avoid having more than 512 ROMs per folder** to prevent loading times.

When browsing your game list, you can **type the first few letters of a game’s name** to jump directly to it. This makes it much faster to find a specific title, especially when your library contains dozens of entries.

## About Saves

Save files are created automatically and organized into separate folders per console on your SD card. **Each save is linked to the game’s filename**.

> **⚠️ The autosave system writes to the SD card in the background at regular intervals.**

The chance of corrupting a save by resetting the device exactly at the moment a write occurs is low. However, to completely eliminate this risk, it is recommended to exit games properly.

Hold the **GO button for 1 second to quit safely** and ensure no save corruption.

## Launcher

For [Launcher](https://github.com/bmorcelli/Launcher)'s users, you can now use the **“Game Station” partition** scheme to load ROMs larger than 1MB.

> In the Launcher main menu, Go to **CFG → Partition Change, and select Game Station.**

The firmware can also automatically switch the device to the “Game Station” partition scheme in order to load ROMs larger than 1 MB.

When you try to run a ROM that needs more space (up to 4 MB):

- The firmware checks that it is running under the Launcher.
- If needed, it asks to flash the Game Station partition table.
- The device reboots once to apply the new layout.

After the reboot, you can load larger ROMs normally, without any extra steps.
