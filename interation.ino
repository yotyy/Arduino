#include <MsTimer2.h>
#include <Adafruit_NeoPixel.h>

//2つの圧力センサーを5VとA0に対して、並列繋ぎで繋ぐ
//https://arduino-lesson.blogspot.com/2013/09/arduinoprocessing_6.html

unsigned long t0 = 0;
unsigned long t = 0;

#define BRIGHTNESS 64
#define LED_PIN 6
#define LED_COUNT 52
#define LED_PER_UNITS 1
#define UNITS 52
#define BRIGHTNESS_MAX 255
#define FADE_SPEED 10

const int pressureSensorPin = A0;
int currentPressure = 0;
int previousPressure = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long eventtime = 0;  // eventtimeの宣言を追加
int difference = 0;
float c = 0.01; // 初期値を設定

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  pixels.begin();
  randomSeed(analogRead(0));
  pixels.setBrightness(BRIGHTNESS);
  pixels.show();

  eventtime = millis();  // eventtimeの初期化を追加

  // 1000ms毎に
  MsTimer2::set(200, Pressure);
  MsTimer2::start();
}

float increment() {
    if((currentMillis - eventtime)/1000 < 2) {
    c=0.017;
    delay(1);
    }else if((currentMillis - eventtime)/1000 < 4) {
    c = 0.013;
    delay(1);
    }else if((currentMillis - eventtime)/1000 < 6) {
    c = 0.011;
    delay(1);
    }else if((currentMillis - eventtime)/1000 < 8) {
    c=0.009;
    delay(1);
    }else if((currentMillis - eventtime)/1000 < 10) {
    c=0.005;
    delay(1);
    }else if((currentMillis - eventtime)/1000 < 12) {
    c=0.003;
    delay(1);
    }else{
    c=0.002;
    delay(1);
    }
  return c;
}

void Pressure() {
  currentMillis = millis();
  currentPressure = analogRead(pressureSensorPin);

  if (abs(difference) > 10) {
    // differenceの絶対値が10より大きく、かつeventtimeが初期値の場合
    eventtime = currentMillis;
  }

    difference = currentPressure - previousPressure;

  Serial.print("time:");
  Serial.print(currentMillis / 1000);
  Serial.print(" weight:");
  Serial.print(currentPressure);
  Serial.print(" difference:");
  Serial.print(difference);
  Serial.print(" eventtime:");
  Serial.print(eventtime/1000);
  Serial.print(" E:");
  Serial.print((currentMillis - eventtime)/1000);
  Serial.print(" c:");
  Serial.println(c*100);//確認用(serial.printで表示されるのが小数第三位までなので、100倍にしている。)

  // 1秒前の圧力を更新
  previousPressure = currentPressure;

  // 時間を更新
  previousMillis = currentMillis;
  delay(5);
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
  return sin(t + (0.4) * i);
}

void loop() {
  t = millis();
  currentMillis = millis();

  // 現在の圧力を取得
  float tIncrement = 0.01;

  while (tIncrement <= t) {
    for (int i = 0; i < UNITS; i++) {
      pixels.show();
      float a[UNITS];
      a[i] = brignt_sin(tIncrement, i);
      pixels.setPixelColor(i, pixels.Color(abs(255 * a[i]), abs(50 * a[i]), abs(120 * a[i])));
      tIncrement += increment();
      delay(1);
    }
  }
}
