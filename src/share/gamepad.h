// gamepad.h - interfaces to gamepad.cpp
//
// gamepad_test - test Bluetooth/BLE controllers - targeting gamepads (8-position dpad or joystick, plus a couple buttons)
// Copyright (c) 2026 David Van Wagner
//
// Classic Bluetooth supported only on classic ESP32 (e.g. not CoreS3, not ESP32-C4)
// BLE (Bluetooth Low Energy) should be supported on any wireless capable ESP32
//
// Dependencies, derivations include
// Bluepad32 example Copyright 2016-2024 Ricardo Quesada 
// Bluepad32 library Copyright 2016-2024 Ricardo Quesada
// BTStack library Copyright (C) 2009, 2017 BlueKitchen GmbH
//
// Open source for individual, non-commercial use (BlueKitchen restriction)
// see LICENSE for full details

class MyGamepad
{
public:
    void begin();
    void check();
    void (*onUpdate)(int dpad, int buttons) = 0;
    void (*onDisconnect)() = 0;
};

extern MyGamepad MyController;
