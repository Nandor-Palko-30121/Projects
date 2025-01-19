#include <Wire.h>

#include "Multichannel_Gas_GMXXX.h"
#include "Seeed_BME280.h"
#include "TFT_eSPI.h"                                 // Comes with Wio Terminal package

#include "AiNose_2.0_inferencing.h"                     // Name of Edge Impulse library

// Settings
#define DEBUG               1                         // 1 to print out debugging info
#define DEBUG_NN            false                     // Print out EI debugging info
#define ANOMALY_THRESHOLD   0.3                       // Scores above this are an "anomaly"
#define SAMPLING_FREQ_HZ    4                         // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms)
#define NUM_SAMPLES         EI_CLASSIFIER_RAW_SAMPLE_COUNT  // 4 samples at 4 Hz
#define READINGS_PER_SAMPLE EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME // 8

// Constants
#define BME680_I2C_ADDR     uint8_t(0x76)             // I2C address of BME680
#define PA_IN_KPA           1000.0                    // Convert Pa to KPa

// Global objects
GAS_GMXXX<TwoWire> gas;               // Multichannel gas sensor v2
BME280 bme280;                        // Environmental sensor
TFT_eSPI tft;                         // Wio Terminal LCD

void setup() {
  
  // Start serial
  Serial.begin(115200);

  // Configure LCD
  tft.begin();
  tft.setRotation(3);
  tft.setFreeFont(&FreeSansBoldOblique24pt7b);
  tft.fillScreen(TFT_BLACK);

  // Initialize gas sensors
  gas.begin(Wire, 0x08);
}

void loop() {
  
  float gm_no2_v;
  float gm_eth_v;
  float gm_voc_v;
  float gm_co_v;
  unsigned long timestamp;
  static float raw_buf[NUM_SAMPLES * READINGS_PER_SAMPLE];
  static signal_t signal;
  float temp;
  int max_idx = 0;
  float max_val = 0.0;
  char str_buf[40];
  
  // Collect samples and perform inference
  for (int i = 0; i < NUM_SAMPLES; i++) {

    // Take timestamp so we can hit our target frequency
    timestamp = millis();

    // Read from GM-X02b sensors (multichannel gas)
    gm_no2_v = gas.calcVol(gas.getGM102B());
    gm_eth_v = gas.calcVol(gas.getGM302B());
    gm_voc_v = gas.calcVol(gas.getGM502B());
    gm_co_v = gas.calcVol(gas.getGM702B());
  

    // Store raw data into the buffer
    //raw_buf[(i * READINGS_PER_SAMPLE) + 0] = bme280.getTemperature();
    //raw_buf[(i * READINGS_PER_SAMPLE) + 1] = bme280.getHumidity();
    raw_buf[(i * READINGS_PER_SAMPLE) + 0] = gm_voc_v;
    raw_buf[(i * READINGS_PER_SAMPLE) + 1] = gm_no2_v;
    raw_buf[(i * READINGS_PER_SAMPLE) + 2] = gm_eth_v;
    raw_buf[(i * READINGS_PER_SAMPLE) + 3] = gm_co_v;

  // Print out our raw buffer
#if DEBUG
  for (int i = 0; i < NUM_SAMPLES * READINGS_PER_SAMPLE; i++) {
    Serial.print(raw_buf[i]);
    if (i < (NUM_SAMPLES * READINGS_PER_SAMPLE) - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
#endif

  // Turn the raw buffer in a signal which we can the classify
  int err = numpy::signal_from_buffer(raw_buf, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  if (err != 0) {
      ei_printf("ERROR: Failed to create signal from buffer (%d)\r\n", err);
      return;
  }

  // Run inference
  ei_impulse_result_t result = {0};
  err = run_classifier(&signal, &result, DEBUG_NN);
  if (err != EI_IMPULSE_OK) {
      ei_printf("ERROR: Failed to run classifier (%d)\r\n", err);
      return;
  }

  // Print the predictions
  ei_printf("Predictions ");
  ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)\r\n",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    ei_printf("\t%s: %.3f\r\n", 
              result.classification[i].label, 
              result.classification[i].value);
  }
  delay(1000);
  // Print anomaly detection score
#if EI_CLASSIFIER_HAS_ANOMALY == 1
  ei_printf("\tanomaly acore: %.3f\r\n", result.anomaly);
#endif

  // Find maximum prediction
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > max_val) {
      max_val = result.classification[i].value;
      max_idx = i;
    }
  }

  // Optional

  // Print predicted label and value to LCD if not anomalous
  tft.fillScreen(TFT_BLACK);
  if (result.anomaly < ANOMALY_THRESHOLD) {
    tft.drawString(result.classification[max_idx].label, 20, 60);
    sprintf(str_buf, "%.3f", max_val);
    tft.drawString(str_buf, 60, 120);
  } else {
    tft.drawString("Unknown", 20, 60);
    sprintf(str_buf, "%.3f", result.anomaly);
    tft.drawString(str_buf, 60, 120);
  }
}
}
