#include "device.h"

//edit this
#define SERIAL_RATE 4800
#define SERIAL_CONFIG SERIAL_8N1
#define SERIAL_TIMEOUT_MS 300
#define DEVICE_COUNT 2

//edit this
//for each device
#define TID1 1
#define HID1 2
#define INTERVAL_MS 2500
#define PIN1 2
#define TID2 3
#define HID2 4
#define PIN2 3

Device devices[DEVICE_COUNT];
PUART puart;

void setup()
{
 Serial.begin(SERIAL_RATE, SERIAL_CONFIG);
 Serial.setTimeout(SERIAL_TIMEOUT_MS);
  puart_init(&puart);
  
  //edit this
  //for each channel
  device_init(&devices[0], TID1, HID1, INTERVAL_MS, PIN1);
  device_init(&devices[1], TID2, HID2, INTERVAL_MS, PIN2);
}


void loop()
{
	for (int i = 0; i < DEVICE_COUNT; i++) {
	  device_control(&devices[i]);
	  puart_server(&puart, devices, DEVICE_COUNT );
	}
}
