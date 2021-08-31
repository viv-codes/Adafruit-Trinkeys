#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include "Adafruit_FreeTouch.h"
#include "HID-Project.h"  // https://github.com/NicoHood/HID

// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
int16_t neo_brightness = 20; // initialize with 20 brightness (out of 255)

RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B, RotaryEncoder::LatchMode::FOUR3);
// This interrupt will do our encoder reading/checking!
void checkPosition() {
  encoder.tick(); // just call tick() to check the state.
}

uint8_t wheel_offset = 99;
int last_rotary = 0;
bool last_button = false;

void setup() {
  Serial.begin(115200);
  //while (!Serial);
  delay(100);
  
  Serial.println("Rotary Trinkey Surface Dial");
  
  // start neopixels
  strip.begin();
  strip.setBrightness(neo_brightness);
  strip.show(); // Initialize all pixels to 'off'

  attachInterrupt(PIN_ENCODER_A, checkPosition, CHANGE);
  attachInterrupt(PIN_ENCODER_B, checkPosition, CHANGE);
  
  // set up the encoder switch, which is separate from the encoder
  pinMode(PIN_ENCODER_SWITCH, INPUT_PULLDOWN);

  // Sends a clean report to the host. This is important on any Arduino type.
  SurfaceDial.begin();
}


void loop() {
  // read encoder
  int curr_rotary = encoder.getPosition();
  RotaryEncoder::Direction direction = encoder.getDirection();
  // read switch
  bool curr_button = !digitalRead(PIN_ENCODER_SWITCH);
  
  if (direction != RotaryEncoder::Direction::NOROTATION) {
    Serial.print("Encoder value: ");
    Serial.print(curr_rotary);
    Serial.print(" direction: ");
    Serial.print((int)direction);

    if (direction == RotaryEncoder::Direction::CLOCKWISE) {
      Serial.println(" Rotate+");
      SurfaceDial.rotate(40);
      wheel_offset++;
    }
    if (direction == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
      Serial.println(" Rotate-");
      SurfaceDial.rotate(-40);
      wheel_offset--;
    }

    last_rotary = curr_rotary;
    strip.setBrightness(neo_brightness);
    strip.setPixelColor(0, Wheel(wheel_offset));
    strip.show();
  }

  if (curr_button && !last_button) { // switch pressed!
    Serial.println("Press");
    SurfaceDial.press();
  }
  if (!curr_button && last_button) { // switch released!
    Serial.println("Release");
    SurfaceDial.release();
  }
  last_button = curr_button;

  delay(10); // debounce
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 51) {
   return strip.Color(WheelPos * 5, 255 - WheelPos * 5, 0);
  } else if(WheelPos < 102) {
   WheelPos -= 102;
   return strip.Color(255 - WheelPos * 5, 0, WheelPos * 5);
  } else if(WheelPos < 153) {
   WheelPos -= 153;
   return strip.Color(0, WheelPos * 5, 255 - WheelPos * 5);
  } else if(WheelPos < 204) {
   WheelPos -= 204;
   return strip.Color(0, WheelPos * 5, 255 - WheelPos * 5);
  } else {
   WheelPos -= 255;
   return strip.Color(0, WheelPos * 5, 255 - WheelPos * 5);
  }
}

/*
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
} */
