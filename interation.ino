#include <MsTimer2.h>

unsigned long t0 = 0;
unsigned long t = 0;

#include <Adafruit_NeoPixel.h>
#define BRIGHTNESS 64
#define LED_PIN 5
#define LED_COUNT 9
#define LED_PER_UNITS 1
#define UNITS 9
#define BRIGHTNESS_MAX 255
#define FADE_SPEED 10

int TRIG = 3;
int ECHO = 2;
double duration = 0;
double distance = 0;

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void D_measure() {
  
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);

  duration = duration / 2;
  distance = duration * 340 * 100 / 1000000;
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.print(" cm");
}

float increment() {
  float c = 0;
  if (distance < 10) {
    c = 0.04;
  } else if (distance < 100) {
    c = 0.02;
  } else {
    c = 0.01;
  }
  return c;
}

void setup() {
  Serial.begin(9600);

  pixels.begin();
  randomSeed(analogRead(0));
  pixels.setBrightness(BRIGHTNESS);
  pixels.show();

  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);

  randomSeed(analogRead(0));

  // 1000ms毎に
  MsTimer2::set(1000, D_measure);
  MsTimer2::start();
}

void loop() {
  t = millis();
  Serial.println(t);
  delay(10);

  if (duration > 0) {
    float tIncrement = 0.01;

    while (tIncrement <= t) {
      for (int i = 0; i < UNITS; i++) {
        fadeToBlack(i);
        pixels.show();
        float a[UNITS];
        a[i] = brignt_sin(tIncrement, i);
        pixels.setPixelColor(i, pixels.Color(abs(255 * a[i]), abs(50 * a[i]), abs(120 * a[i])));
        tIncrement += increment();
        delay(1);
      }
    }
  }
  TIMSK0 = 0;
}

void fadeToBlack(int i) {
  for (int j = 0; j < pixels.numPixels(); j++) {
    uint32_t color = pixels.getPixelColor(j);
    uint8_t r, g, b;
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;
    r = r * (255 - FADE_SPEED) / 255;
    g = g * (255 - FADE_SPEED) / 255;
    b = b * (255 - FADE_SPEED) / 255;
    pixels.setPixelColor(j, r, g, b);
  }
}


float brignt_sin(float t, int i) {
  float result;
  result = sin(t + 2 * i);
  return result;
}
