/*
    main.cpp

    BLE GamePad Server
    by David R. Van Wagner davevw.com
    HISTORY: derived from my own https://github.com/davervw/c-simple-emu6502-cbm/tree/unified/src/BLE_commodore_keyboard_server
    Changes are open source, MIT License
    (Based on ESP32 BLE Arduino : BLE_server)

    Original comments:
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Custom BLE Gamepad Service
#define SERVICE_UUID        "b496c097-3364-43e6-b3ee-b59d1d4d9e34"

// Custom 64/128 BLE Gamepad Scan Characteristic
#define CHARACTERISTIC_UUID "050c1c21-cc9f-4281-ac9c-242f1dbb67e8"

BLECharacteristic *pCharacteristic;

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

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const char *image[8] = {
  "GGGGGGGGGGGGGGGG",
  "GGWWWGGGGGGGGWGG",
  "GWW WWGGGGGGWRWG",
  "WWW WWWGGGWWRRRW",
  "W     WGGWRWWRWG",
  "WWW WWWGWRRRWWGG",
  "GWW WWGGGWRWGGGG",
  "GGWWWGGGGGWGGGGG",
};

void drawController()
{
  auto colorBlack = matrix.Color(48, 48, 48);
  auto colorGray = matrix.Color(96, 96, 96);
  auto colorWhite = matrix.Color(192, 192, 192);
  auto colorRed = matrix.Color(255, 0, 0);
  auto colorNothing = matrix.Color(0, 0, 0);

  int i = 0;
  for (int y=0; y<8; ++y)
  {
    for (int x=0; x<16; ++x)
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
  // Push pixel updates to the strip/matrix
  matrix.show();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      // Stage: Connected
      matrix.setPixelColor(31, 0x00FF00); // Turn Green on connect
      matrix.show();
      //Serial.println("Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      // Stage: Disconnected
      matrix.setPixelColor(31, 0xFF8000); // Turn Orange on disconnect
      matrix.show();
      //Serial.println("Disconnected");
      
      // Stage: Advertising (Restart so it can be found again)
      BLEDevice::startAdvertising();
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
  matrix.show();
  drawController();

  //Serial.begin(115200);

  //Serial.println("Starting Commodore Emulator BLE Keyboard Service for Kano Pixel Kit");
  BLEDevice::init("Commodore 64/128 BLE Keyboard Service");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->setValue("");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMaxPreferred(0x12);

  BLEDevice::startAdvertising();

  matrix.setPixelColor(31, 0xFF8000); // orange on start (disconnected)
  matrix.show();

  //Serial.println("Started Commodore 64/128 BLE Keyboard Service");
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
  if ((now - then) >= 1000) {
    state = !state;
    if (state)
      matrix.setPixelColor(15, matrix.Color(0, 0, 255));
    else
      matrix.setPixelColor(15, matrix.Color(85, 85, 85));
    matrix.show();
    then = now;
  }
}

enum ButtonState {
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

ButtonState lastState = ButtonState::UNKNOWN;

void Append (String &s, const char *value)
{
  // if (!s.isEmpty())
  //   s += ",";
  s += value;
}

String ButtonStateToString(ButtonState state)
{
  int value = static_cast<int>(state);
  String s = "";
  if (value & ButtonState::A) Append(s, "l");
  if (value & ButtonState::B) Append(s, "k");
  if (value & ButtonState::UP) Append(s, "e");
  if (value & ButtonState::DOWN) Append(s, "s");
  if (value & ButtonState::LEFT) Append(s, "a");
  if (value & ButtonState::RIGHT) Append(s, "d");
  //if (value & ButtonState::CENTER) Append(s, "");
  // if (s.isEmpty())
  //   s = "64"; // No buttons/keys pressed
  s += "\n"; // Newline for easier parsing on client side
  return s;
}

void checkForReset(ButtonState buttonState)
{
  static long whenReset = 0; // Time when reset first pressed

  bool isReset = (
    (buttonState & ButtonState::A) == ButtonState::A
    && (buttonState & ButtonState::B) == ButtonState::B
    && (buttonState & ButtonState::CENTER) == ButtonState::CENTER
  );

  if (!isReset)
    return;

  bool wasReset = (
    (lastState & ButtonState::A) == ButtonState::A
    && (lastState & ButtonState::B) == ButtonState::B
    && (lastState & ButtonState::CENTER) == ButtonState::CENTER
  );

  if (!wasReset)
    whenReset = millis();

  if (millis() - whenReset < 1000)
    return;
  
  fillMatrix(0);

  while (ReadButtonState() != ButtonState::NONE);

  esp_restart();
}

void SendButtonState(String s)
{
  pCharacteristic->setValue(s.c_str());
  pCharacteristic->notify();
}

void loop() {
  serviceLED();

  auto buttonState = ReadButtonState();
  checkForReset(buttonState);
  if (buttonState == lastState)
    return;
  lastState = buttonState;
  auto s = ButtonStateToString(buttonState);
  SendButtonState(s);

  //Serial.print("Button state changed: ");
  //Serial.println(buttonState);
  //Serial.println(s.c_str());
}
