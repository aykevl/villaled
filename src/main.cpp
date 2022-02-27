#include <Arduino.h>
#include <FastLED.h>
#include <RotaryEncoder.h>
#include "pins.h"
#include "parameter.h"

#define numPalettes 5
static CRGBPalette16 palettes[numPalettes] = {
  PartyColors_p,
  RainbowColors_p,
  HeatColors_p,
  ForestColors_p,
  OceanColors_p,
};

#define numParams 4
static Parameter animationSpeed(3, 7);
static Parameter animationSpread(6, 10);
static Parameter palette(0, numPalettes-1, true);
static Parameter brightness(2, 4);

static Parameter *parameters[numParams] = {&animationSpeed, &animationSpread, &palette, &brightness};
static Parameter currentParameter(0, numParams-1, true);

// There are 150 LEDs, but they are controlled in groups of 3 (WS2811).
#define NUM_LEDS 50
static CRGB barleds[NUM_LEDS];

// Rotary encoder, can be rotated and pressed like a regular button.
static RotaryEncoder knob(ROTARY_PIN_A, ROTARY_PIN_B, RotaryEncoder::LatchMode::FOUR3);
static bool knobButtonPressed;

static bool isPressed(int button, bool &wasPressed);

// Called on rotary encoder pin changes.
static void updateKnob() {
  knob.tick();
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");

  FastLED.addLeds<WS2811, LED_PIN, BRG>(barleds, NUM_LEDS);

  pinMode(ROTARY_BUTTON, INPUT_PULLUP);
  knob.setPosition(10);

  // Update the rotary encoder when one of the pins changes state.
  attachInterrupt(ROTARY_PIN_A, updateKnob, CHANGE);
  attachInterrupt(ROTARY_PIN_B, updateKnob, CHANGE);
}

void loop() {
  // Handle pressing of the button on the rotary knob.
  if (isPressed(ROTARY_BUTTON, knobButtonPressed)) {
    Serial.println("rotary button press");
    currentParameter.increment();
  }

  // Handle rotary encoder input.
  // Keep the rotary encoder around 10.
  long position = knob.getPosition();
  Parameter *param = parameters[currentParameter.value];
  if (position >= 11) {
    Serial.println("+");
    knob.setPosition(position - 1);
    param->increment();
  } else if (position <= 9) {
    Serial.println("-");
    knob.setPosition(position + 1);
    param->decrement();
  }

  // Calculate new time value, which increments at a rate set by animationSpeed.
  static uint32_t t = 0;
  static unsigned long lastMicros = 0;
  unsigned long now = micros();
  unsigned long diff = now - lastMicros;
  lastMicros = now;
  if (animationSpeed.value != 0) {
    t += diff * (1 << (animationSpeed.value-1)) / 8192;
  }

  // Calculate spread value that's used in the noise function.
  uint32_t spread = 2 << animationSpread.value;
  if (animationSpread.value == 0) {
    spread = 0;
  }

  // Update LED strip.
  CRGBPalette16 currentPalette = palettes[palette.value];
  for (int i = 0; i < NUM_LEDS; i++) {
    // For simplex noise, see: https://github.com/FastLED/FastLED/pull/1252
    // Using Perlin noise instead, which can have rather visible artifacts.
    //uint16_t val = snoise16(t, i * spread) - 0x8000;
    uint16_t val = inoise16(t * 32, i * spread * 32) - 0x8000;
    barleds[i] = ColorFromPalette(currentPalette, val >> 8, (1 << (brightness.value + 4)) - 1, LINEARBLEND);
  }
  FastLED.show();
  delay(10);
}

static bool isPressed(int button, bool &wasPressed) {
  bool pressed = !digitalRead(button);
  bool result = pressed && !wasPressed;
  wasPressed = pressed;
  return result;
}
