#include <Wire.h>

#include "Multichannel_Gas_GMXXX.h"

// Settings
#define BTN_START           0                         // 1: press button to start, 0: loop
#define BTN_PIN             WIO_5S_PRESS              // Pin that button is connected to
#define SAMPLING_FREQ_HZ    4                         // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms)
#define NUM_SAMPLES         8                         // 8 samples at 4 Hz is 2 seconds

// Constants

// Global objects
GAS_GMXXX<TwoWire> gas;               // Multichannel gas sensor v2

void setup() {

  // Initialize button
  pinMode(BTN_PIN, INPUT_PULLUP);
  
  // Start serial
  Serial.begin(115200);

  // Initialize gas sensors
  gas.begin(Wire, 0x08);
}

void loop() {

  float gm_no2_v;
  float gm_eth_v;
  float gm_voc_v;
  float gm_co_v;
  unsigned long timestamp;

  // Wait for button press
#if BTN_START
  while (digitalRead(BTN_PIN) == 1);
#endif

  // Print header
  Serial.println("timestamp,co,voc,no2,eth");

  // Transmit samples over serial port
  for (int i = 0; i < NUM_SAMPLES; i++) {

    // Take timestamp so we can hit our target frequency
    timestamp = millis();

    // Read from GM-X02b sensors (multichannel gas)
    gm_no2_v = gas.calcVol(gas.getGM102B());
    gm_eth_v = gas.calcVol(gas.getGM302B());
    gm_voc_v = gas.calcVol(gas.getGM502B());
    gm_co_v = gas.calcVol(gas.getGM702B());
  
    // Print CSV data with timestamp
    Serial.print(timestamp);
    Serial.print(",");
    Serial.print(gm_voc_v);
    Serial.print(",");
    Serial.print(gm_no2_v);
    Serial.print(",");
    Serial.print(gm_eth_v);
    Serial.print(",");
    Serial.print(gm_co_v);
    Serial.println();

    // Wait just long enough for our sampling period
    while (millis() < timestamp + SAMPLING_PERIOD_MS);
  }

  // Print empty line to transmit termination of recording
  Serial.println();

  // Make sure the button has been released for a few milliseconds
#if BTN_START
  while (digitalRead(BTN_PIN) == 0);
  delay(100);
#endif
}