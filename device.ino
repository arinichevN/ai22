#include "device.h"

void device_init(Device *item, int tid, int hid, int interval_ms, int pin) {
  item->pin = pin;
  item->temperature.id = tid;
  item->humidity.id = hid;
  ton_setInterval(&item->tmr, interval_ms);
  item->temperature.selected = 0;
  item->humidity.selected = 0;
  item->state = INIT;
}

#define DHT22_MAXTIMINGS 80
#define DHT22_MAX_VAL 80
#define DHT22_TIMEOUT -1
int dht22_waitPulse(int pin, int level) {
  int count = 0;
  while (digitalRead(pin) == level) {
    if (count++ >= DHT22_MAX_VAL) {
      return DHT22_TIMEOUT; // Exceeded timeout, fail.
    }
  }
  return count;
}

int dht22_read(int pin, double *t, double *h) {
  pinMode(pin, INPUT);
  delay(1);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(1100); // data sheet says "at least 1ms"
  pinMode(pin, INPUT);
  delayMicroseconds(55);

  if (dht22_waitPulse(pin, LOW) == DHT22_TIMEOUT) {
  //  Serial.println("failed wl");
    return 0;
  }
  if (dht22_waitPulse(pin, HIGH) == DHT22_TIMEOUT) {
  //  Serial.println("failed wh");
    return 0;
  }


  int cycles[DHT22_MAXTIMINGS];
  for (int i = 0; i < DHT22_MAXTIMINGS; i += 2) {
    cycles[i] = dht22_waitPulse(pin, LOW);
    cycles[i + 1] = dht22_waitPulse(pin, HIGH);
  }
//for (int i = 0; i < DHT22_MAXTIMINGS; i++ ) {
  //  Serial.print(cycles[i]); Serial.print(".");
  //}
  //Serial.println(" ");
  uint8_t data[5] = {0, 0, 0, 0, 0};
  for (int i = 0; i < 40; ++i) {
    uint32_t lowCycles  = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];
    if ((lowCycles == DHT22_TIMEOUT) || (highCycles == DHT22_TIMEOUT)) {
      return 0;
    }
    data[i / 8] <<= 1;
    if (highCycles > lowCycles) {
      data[i / 8] |= 1;
    }
  }


  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    *h = ( double ) data[0] * 256 + ( double ) data[1];
    *h /= 10;
    *t = ( double ) ( data[2] & 0x7F ) * 256 + ( double ) data[3];
    *t /= 10.0;
    if ( ( data[2] & 0x80 ) != 0 ) {
      *t *= -1;
    }
    return 1;
  }  else {
   // Serial.println("bad data crc");
    return 0;
  }

}

int device_control(Device *item) {
  switch (item->state) {
    case RUN:
      if (ton(&item->tmr)) {
        ton_reset(&item->tmr);
        double tv = 0.0, hv = 0.0;
        int r = dht22_read(item->pin, &tv, &hv);
        item->temperature.output.state = item->humidity.output.state = r;
        if (r) {
          item->temperature.output.value = tv;
          item->humidity.output.value = hv;
        }
        item->temperature.output.tm = item->humidity.output.tm = millis();
      }
      break;
    case OFF:
      break;
    case INIT:
      ton_reset(&item->tmr);
      item->temperature.output.state = item->humidity.output.state = 0;
      item->temperature.output.tm = item->humidity.output.tm = millis();
      pinMode(item->pin, INPUT);
      item->state = RUN;
      break;
    default:
      break;
  }
  return item->state;
}
