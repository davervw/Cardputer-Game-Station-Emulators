/*
    main.cpp

    BLE GamePad Controller for Kano Pixel Kit (ESP-WROOM-32 + NeoPixel Matrix)
    by David R. Van Wagner davevw.com
    HISTORY: derived from my own https://github.com/davervw/c-simple-emu6502-cbm/tree/unified/src/BLE_commodore_keyboard_server
    Changes are open source, MIT License
    (Based on ESP32 BLE Arduino : BLE_server
    with BleGamepad library use added for standard BLE Gamepad HID support)

    Original comments:
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <Arduino.h>
#include <BleGamepad.h>
#include <Adafruit_NeoPixel.h>

// PIN CONFIGURATION
#define PIXEL_PIN 4
#define NUM_PIXELS 128 // 16x8 matrix
#define BUTTON_A 18
#define BUTTON_B 23
#define JOY_UP 35
#define JOY_DOWN 34
#define JOY_LEFT 26
#define JOY_RIGHT 25
#define JOY_CLICK 27

enum ButtonState
{
  UNKNOWN = -1,
  NONE = 0,
  A = 1,
  B = 2,
  UP = 4,
  DOWN = 8,
  LEFT = 16,
  RIGHT = 32,
  CENTER = 64
};

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
BleGamepad bleGamepad("Kano Pixel Kit Gamepad", "Arduino", 100);
BleGamepadConfiguration bleGamepadConfig;
ButtonState lastState = ButtonState::UNKNOWN;

const char *image[8] = {
    "                ",
    "  WWWWWWWWWWWW  ",
    " WW WWWWWWWWRWW ",
    "WWW WWWWWWWRRRWW",
    "W     WWWRWWRWWW",
    "WWW WWWWRRRWWWWW",
    " WW WWWWWRWWWWW ",
    "  WWWWWWWWWWWW  ",
};

void drawController()
{
  auto colorBlack = matrix.Color(48, 48, 48);
  auto colorGray = matrix.Color(96, 96, 96);
  auto colorWhite = matrix.Color(192, 192, 192);
  auto colorRed = matrix.Color(255, 0, 0);
  auto colorNothing = matrix.Color(0, 0, 0);

  int i = 0;
  for (int y = 0; y < 8; ++y)
  {
    for (int x = 0; x < 16; ++x)
    {
      uint32_t color;
      char c = image[y][x];
      if (c == ' ')
        color = colorBlack;
      else if (c == 'G')
        color = colorGray;
      else if (c == 'W')
        color = colorWhite;
      else if (c == 'R')
        color = colorRed;
      else
        color = colorNothing;
      matrix.setPixelColor(i++, color);
    }
  }
  // Push pixel updates to the matrix
  matrix.show();
}

class MyGamepadCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer) {
    // Serial.println(">> Callback: Device Connected!");
  };

  void onDisconnect(NimBLEServer *pServer)
  {
    // Serial.println(">> Callback: Device Disconnected! Restarting advertising...");
    //  Re-advertising is typically handled by the library, but you can trigger custom logic here
  }
};

void setup()
{
  // Initialize buttons as pullups (LOW when pressed)
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(JOY_UP, INPUT_PULLUP);
  pinMode(JOY_DOWN, INPUT_PULLUP);
  pinMode(JOY_LEFT, INPUT_PULLUP);
  pinMode(JOY_RIGHT, INPUT_PULLUP);

  matrix.begin();
  matrix.setBrightness(10); // Set moderate brightness
  drawController();

  // Serial.begin(115200);

  // --- HID Report Map Configuration ---
  bleGamepadConfig.setAutoReport(false); // Best practice: manual reports
  bleGamepadConfig.setButtonCount(3);    // Button 1 (A), 2 (B), 3 (D-pad Center)
  bleGamepadConfig.setHatSwitchCount(1); // One D-pad (Point of View Hat)

  // Parameters: (X, Y, Z, RX, RY, RZ, Slider1, Slider2)
  bleGamepadConfig.setWhichAxes(true, false, false, false, false, false, false, false);

  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);

  // Apply the configuration
  bleGamepad.begin(&bleGamepadConfig);

  matrix.setPixelColor(31, 0xFF8000); // orange on start (disconnected)

// Get the global advertising object
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising(); 

  // Add the HID Service UUID (0x1812) to the advertisement data
  pAdvertising->addServiceUUID(BLEUUID((uint16_t)0x1812));

  // Restart advertising to apply changes
  pAdvertising->start();

  // Serial.println("Started Custom BLE Gamepad Service");
}

// Function to set entire matrix to one color
void fillMatrix(uint32_t color)
{
  for (int i = 0; i < NUM_PIXELS; i++)
    matrix.setPixelColor(i, color);
  matrix.show();
}

void serviceLED()
{
  static long then = millis();
  static bool state = false;

  long now = millis();
  if ((now - then) >= 1000)
  {
    state = !state;
    if (state)
      matrix.setPixelColor(15, matrix.Color(0, 0, 255));
    else
      matrix.setPixelColor(15, matrix.Color(85, 85, 85));
    matrix.show();
    then = now;
  }
}

int buttonPins[] = {BUTTON_A, BUTTON_B, JOY_UP, JOY_DOWN, JOY_LEFT, JOY_RIGHT, JOY_CLICK};
const int buttonCount = sizeof(buttonPins) / sizeof(buttonPins[0]);

ButtonState ReadButtonState()
{
  auto state = 0;
  for (int i = 0; i < buttonCount; i++)
  {
    if (digitalRead(buttonPins[i]) == LOW)
      state |= (1 << i);
  }
  return static_cast<ButtonState>(state);
}

void DrawButtonState(ButtonState state)
{
  static const auto yellow = matrix.Color(255, 255, 0);
  static const auto red = matrix.Color(255, 0, 0);
  static const auto black = matrix.Color(0, 0, 0);
  static const auto purple = matrix.Color(127, 0, 255);
  static const auto cyan = matrix.Color(0, 255, 255);
  static const auto magenta = matrix.Color(255, 0, 255);
  static const auto pink = matrix.Color(255, 0, 127);
  int value = static_cast<int>(state);
  String s = "";

  matrix.setPixelColor(44, value & ButtonState::A ? yellow : red);
  matrix.setPixelColor(59, value & ButtonState::A ? yellow : red);
  matrix.setPixelColor(60, value & ButtonState::A ? yellow : red);
  matrix.setPixelColor(61, value & ButtonState::A ? yellow : red);
  matrix.setPixelColor(76, value & ButtonState::A ? yellow : red);

  matrix.setPixelColor(73, value & ButtonState::B ? cyan : red);
  matrix.setPixelColor(88, value & ButtonState::B ? cyan : red);
  matrix.setPixelColor(89, value & ButtonState::B ? cyan : red);
  matrix.setPixelColor(90, value & ButtonState::B ? cyan : red);
  matrix.setPixelColor(105, value & ButtonState::B ? cyan : red);

  matrix.setPixelColor(35, value & ButtonState::UP ? purple : black);
  matrix.setPixelColor(51, value & ButtonState::UP ? purple : black);

  matrix.setPixelColor(83, value & ButtonState::DOWN ? purple : black);
  matrix.setPixelColor(99, value & ButtonState::DOWN ? purple : black);

  matrix.setPixelColor(65, value & ButtonState::LEFT ? purple : black);
  matrix.setPixelColor(66, value & ButtonState::LEFT ? purple : black);

  matrix.setPixelColor(68, value & ButtonState::RIGHT ? purple : black);
  matrix.setPixelColor(69, value & ButtonState::RIGHT ? purple : black);

  matrix.setPixelColor(67, value & ButtonState::CENTER ? pink : black);

  matrix.show();
}

void checkForReset(ButtonState buttonState)
{
  static long whenReset = 0; // Time when reset first pressed

  bool isReset = ((buttonState & ButtonState::A) == ButtonState::A && (buttonState & ButtonState::B) == ButtonState::B && (buttonState & ButtonState::CENTER) == ButtonState::CENTER);

  if (!isReset)
    return;

  bool wasReset = ((lastState & ButtonState::A) == ButtonState::A && (lastState & ButtonState::B) == ButtonState::B && (lastState & ButtonState::CENTER) == ButtonState::CENTER);

  if (!wasReset)
    whenReset = millis();

  if (millis() - whenReset < 1000)
    return;

  fillMatrix(0x000000);

  while (ReadButtonState() != ButtonState::NONE)
    ;

  esp_restart();
}

void SendButtonState(ButtonState buttonState)
{
  const int updateInterval = 16; // ~60 updates per second
  static unsigned long lastUpdate = -updateInterval; // Initialize to allow immediate update on first run
  if (millis() - lastUpdate < updateInterval)
    return; // Skip update if interval hasn't passed
  lastUpdate = millis();

  // Send button states to the connected BLE client
  if ((buttonState & ButtonState::A) == ButtonState::A)
    bleGamepad.press(1);
  else
    bleGamepad.release(1);
  if ((buttonState & ButtonState::B) == ButtonState::B)
    bleGamepad.press(2);
  else
    bleGamepad.release(2);
  if ((buttonState & ButtonState::CENTER) == ButtonState::CENTER)
    bleGamepad.press(3);
  else
    bleGamepad.release(3);

  signed char hatValue = DPAD_CENTERED;

  // Define the masks for readability
  bool up = (buttonState & ButtonState::UP);
  bool down = (buttonState & ButtonState::DOWN);
  bool left = (buttonState & ButtonState::LEFT);
  bool right = (buttonState & ButtonState::RIGHT);

  // 1. Handle Diagonals (High Priority)
  if (up && right)
    hatValue = DPAD_UP_RIGHT;
  else if (down && right)
    hatValue = DPAD_DOWN_RIGHT;
  else if (down && left)
    hatValue = DPAD_DOWN_LEFT;
  else if (up && left)
    hatValue = DPAD_UP_LEFT;

  // 2. Handle Cardinal Directions (Medium Priority)
  else if (up)
    hatValue = DPAD_UP;
  else if (down)
    hatValue = DPAD_DOWN;
  else if (left)
    hatValue = DPAD_LEFT;
  else if (right)
    hatValue = DPAD_RIGHT;

  // 3. Neutral (Handled by initialization)

  int analogValue = analogRead(36);
  // Serial.print("Analog value: ");
  // Serial.println(analogValue);
  int mappedValue = map(analogValue, 0, 4095, 32767, -32768);
  // Serial.print("Mapped value: ");
  // Serial.println(mappedValue);

  // 4. Update the specific axis value based on the paddle position
  bleGamepad.setX(mappedValue);

  // Also update the hat switch value based on the D-pad state
  bleGamepad.setHat1(hatValue);

  lastState = buttonState; // used by checkForReset to detect new reset events
    
  bleGamepad.sendReport();
}

void serviceBLE()
{
  bool isConnected = bleGamepad.isConnected();
  static bool wasConnected = false;

  // Detect JUST CONNECTED
  if (isConnected && !wasConnected)
  {
    // Serial.println(">> EVENT: Gamepad Connected to Host!");
    matrix.setPixelColor(31, 0x00FF00); // Turn Green on connect
    matrix.show();
    wasConnected = true;
  }

  // Detect JUST DISCONNECTED
  if (!isConnected && wasConnected)
  {
    // Serial.println(">> EVENT: Gamepad Disconnected!");
    matrix.setPixelColor(31, 0xFF8000); // Turn Orange on disconnect
    matrix.show();
    wasConnected = false;
  }
}

void loop()
{
  serviceBLE();
  serviceLED();

  auto buttonState = ReadButtonState();
  checkForReset(buttonState);
  if (bleGamepad.isConnected())
    SendButtonState(buttonState); // note: checks for analog changes and rate limits internally
  if (buttonState != lastState)
    DrawButtonState(buttonState);

  // Serial.print("Button state changed: ");
  // Serial.println(buttonState);
  // Serial.println(s.c_str());
  delay(10);
}
