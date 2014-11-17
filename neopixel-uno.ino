#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUMPIXELS 60
#define LEVEL 64

struct Adafruit {

  Adafruit(Adafruit_NeoPixel *pixels) {
    m_pixels = pixels;
    m_pixels->begin();
  }

protected:
  Adafruit_NeoPixel *m_pixels;

  void setPixel(int idx, int r, int g, int b) {
    m_pixels->setPixelColor(idx, r, g, b);
  }

  void show() {
    m_pixels->show();
  }

  void setup() {
    m_pixels->begin();
  }
};

struct Pattern : Adafruit {
  Pattern(Adafruit_NeoPixel *pixels) : Adafruit(pixels) { }

  virtual void apply() = 0;
};

struct Random : Pattern {
  Random(Adafruit_NeoPixel *pixels) : Pattern(pixels) { }

  virtual void apply() {
    int maxval = 16;
    for(int idx = 0; idx <= NUMPIXELS; idx++) {
      //setPixel(idx, random(maxval), random(maxval), random(maxval));
      m_pixels->setPixelColor(idx, random(maxval), random(maxval), random(maxval));
    }
    m_pixels->show();
    delay(50);
  }
};

struct SerialStrobe : Pattern {
  SerialStrobe(Adafruit_NeoPixel *pixels) : Pattern(pixels) { }

  virtual void apply() {
    for(int idx = 0; idx < NUMPIXELS; idx++) {
      setPixel(idx, LEVEL, 0, 0);
      show();
      delay(50);
      setPixel(idx, 0, LEVEL, 0);
      show();
      delay(50);
      setPixel(idx, 0, 0, LEVEL);
      show();
      delay(50);
      setPixel(idx, 0, 0, 0);
      show();
    }
  }
};

struct WalkWhite : Pattern {
  WalkWhite(Adafruit_NeoPixel *pixels) : Pattern(pixels) { }

  virtual void apply() {
    for(int idx = 0; idx < NUMPIXELS; idx++) {
      setPixel(idx, 255, 255, 255);
      show();
      delay(10);
      setPixel(idx, 0, 0, 0);
      delay(10);
      show();
    }
  }
};

void blink(int sleep) {
  digitalWrite(13, HIGH);
  delay(sleep);
  digitalWrite(13, LOW);
  delay(sleep);
}

void setup_pin13() {
}

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Pattern *pattern;

void setup() {
  pinMode(13, OUTPUT);
  pattern = new Random(&pixels);
}

void loop() {
  pattern->apply();
}
