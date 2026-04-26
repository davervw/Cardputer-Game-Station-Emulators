#include <Arduino.h>
#include "ble_input.h"

#include "BLEDevice.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("b496c097-3364-43e6-b3ee-b59d1d4d9e34"); // Commodore 64/128 BLE Keyboard Service
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("050c1c21-cc9f-4281-ac9c-242f1dbb67e8"); // Commodore 64/128 BLE Keyboard Scan Characteristic

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static String keysPressed = "";
// static bool keysChanged = false;

// class KeyboardQueue
// {
// private:
//   int queue_size = 5;
//   int head;
//   int tail;
//   String* queue;

// public:
//   KeyboardQueue()
//   {
//     head = tail = 0;
//     queue = new String[queue_size];
//   }

//   ~KeyboardQueue()
//   {
//     delete [] queue;
//   }

//   bool Enqueue(String s)
//   {
//     int next_head = (head + 1) % queue_size;
//     if (next_head == tail)
//       return false;
//     queue[head] = s;
//     head = next_head;
//     return true;
//   }

//   bool Dequeue(String &s)
//   {
//     if (head == tail)
//       return false;
//     s = queue[tail];
//     tail = (tail + 1) % queue_size;
//     return true;
//   }
// };

// KeyboardQueue* kbdqueue = new KeyboardQueue();

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    // printf("BLE received: ");
    // for (int i=0; i<length; ++i)
    //     printf("%c", pData[i]);
    // if (length < 1 || pData[length-1] != '\n')
    //   printf("\n");

    //kbdqueue->Enqueue(String(pData, length));
    keysPressed = String(pData, length);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    //printf("%s\n", "onDisconnect");
  }
};

static bool connectToServer() {
    // printf("Forming a connection to ");
    // printf("%s\n", myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    // printf("%s\n", " - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    // printf("%s\n", " - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      // printf("Failed to find our service UUID: ");
      // printf("%s\n", serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    // printf("%s\n", " - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      // printf("Failed to find our characteristic UUID: ");
      // printf("%s\n", charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    // printf("%s\n", " - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      // printf("The characteristic value was: ");
      // printf("%s\n", value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    printf("BLE Advertised Device found: ");
    printf("%s\n", advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


BleInput::BleInput()
{
  printf("BleInput constructor\n");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void ServiceConnection(BleInput* bleInput, bool restart) {
  static unsigned long timer_then = micros();
  const unsigned long timeout = 1000000;

  unsigned long timer_now = micros();
  unsigned long elapsed_micros = micros() - timer_then;
  if (elapsed_micros < timeout)
    return;
  timer_then = timer_now;

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      printf("%s\n", "We are now connected to the BLE Server.");
    } else {
      printf("%s\n", "We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
    // // Set the characteristic's value to be the array of bytes that is actually a string.
    // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  } else if (restart) {
    BLEDevice::getScan()->start(5, false);
  }
} // End of loop

void BleInput::handle()
{
    ServiceConnection(this, false);
    // String s;
    // if (kbdqueue->Dequeue(s))
    // {
    //     keysChanged = !s.equals(keysPressed);
    //     if (!keysChanged)
    //         keysPressed = s;
    // }
}

// bool BleInput::isChange()
// {
//     return keysChanged;
// }

bool BleInput::isKeyPressed(char key)
{
    return (keysPressed.indexOf(key) >= 0);
}
