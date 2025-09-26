#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define NUM_LEDS 200

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(255);  // Set full brightness
  strip.show();              // Initialize all pixels to 'off'
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255)); // Full brightness blue
  }
  strip.show();
  delay(1000); // Keep blue on indefinitely (or adjust delay as needed)
}
