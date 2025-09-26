#include <FastLED.h>
#include "SimplexNoise.h"

#define NUM_LEDS 119   // Number of LEDs on your strip
#define DATA_PIN 6    // GPIO pin
#define BRIGHTNESS 200
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Variables for Simplex noise effect
float time_offset = 0.0;
float time_speed = 0.01; // speed of ripple motion

// Non-linear mapping function for "wave-like" brightness
uint8_t waveMap(uint8_t value) {
  float normalized = value / 255.0;           // 0.0 - 1.0
  normalized = pow(normalized, 0.5);          // square-root for smoother fade & higher crests
  return (uint8_t)(normalized * 255.0);
}

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  time_offset += time_speed;

  for (int i = 0; i < NUM_LEDS; i++) {
    // Generate 1D Simplex noise
    float noise_val = SimplexNoise::noise(i * 0.04, time_offset);

    // Amplify contrast for more dynamic range
    noise_val *= 2.0;
    if (noise_val > 1.0) noise_val = 1.0;
    if (noise_val < -1.0) noise_val = -1.0;

    // Map noise to 0-255
    uint8_t noise_value = (uint8_t)((noise_val + 1.0) * 127.5);

    // Apply wave-like non-linear mapping
    uint8_t mapped_value = waveMap(noise_value);

    // Ocean color mapping
    uint8_t blue_brightness  = map(mapped_value, 0, 255, 30, 255);  // visible base blue
    uint8_t green_brightness = map(mapped_value, 0, 255, 0, 200);   // stronger green variation

    leds[i] = CRGB(0, green_brightness, blue_brightness);
  }

  FastLED.show();
}
