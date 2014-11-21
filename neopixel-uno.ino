/**
 * This is where the fucking magic happens, yo
 */

#define PIN 2
#define NUMPIXELS 50
#define PIXELS_PER_STRIP 50

struct Backend {

protected:
  virtual void setPixel(int idx, int r, int g, int b) = 0;
  virtual void show() = 0;
};


#include <Adafruit_NeoPixel.h>
struct Adafruit : Backend {

  Adafruit() {
    m_pixels = new Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
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
};

#include <OctoWS2811.h>
struct Octo : Backend {
  Octo() {
    m_frameBuf = new int[NUMPIXELS * 6];
    m_drawBuf  = new int[NUMPIXELS * 6];

    //m_pixels = new OctoWS2811(PIXELS_PER_STRIP, m_frameBuf, m_drawBuf, WS2811_GRB + WS2811_800kHz);
    m_pixels = new OctoWS2811(PIXELS_PER_STRIP, m_frameBuf, m_drawBuf, WS2811_RGB + WS2811_800kHz);
    m_pixels->begin();
  }

  ~Octo() {
    delete[] m_frameBuf;
    delete[] m_drawBuf;
  }

protected:
  void *m_frameBuf;
  void *m_drawBuf;
  OctoWS2811 *m_pixels;

  void setPixel(int idx, int r, int g, int b) {
    m_pixels->setPixel(idx, r, g, b);
  }

  void show() {
    m_pixels->show();
    while(m_pixels->busy());
  }
};

struct Pattern : Adafruit {
  Pattern() : Adafruit() { }

  virtual void apply() = 0;
};

struct Random : Pattern {
  Random() = default;

  virtual void apply() {
    int maxval = 16;
    for(int idx = 0; idx <= NUMPIXELS; idx++) {
      setPixel(idx, random(maxval), random(maxval), random(maxval));
    }
    m_pixels->show();
    delay(50);
  }
};

struct SerialStrobe : Pattern {
  SerialStrobe() : Pattern() { }

  virtual void apply() {
    int sleep_for = 15;
    for(int idx = 0; idx < NUMPIXELS; idx++) {
      setPixel(idx, m_level, 0, 0);
      show();
      delay(sleep_for);
      setPixel(idx, 0, m_level, 0);
      show();
      delay(sleep_for);
      setPixel(idx, 0, 0, m_level);
      show();
      delay(sleep_for);
      setPixel(idx, 0, 0, 0);
      show();
    }
  }
private:

  int m_level = 64;
};

struct WalkWhite : Pattern {
  WalkWhite() : Pattern() { }

  virtual void apply() {
    for(int idx = 0; idx < NUMPIXELS; idx++) {
      setPixel(idx, 255, 255, 255);
      show();
      delay(10);
      setPixel(idx, 0, 0, 0);
      show();
      delay(10);
    }
  }
};

struct GlowWorm : Pattern {
  GlowWorm() : Pattern() {
    m_r = 0;
    m_g = 0;
    m_b = 0;
  }

  virtual void apply() {
    int idx = random(NUMPIXELS);
    int maxR = random(255);
    int maxG = random(255);
    int maxB = random(255);

    while(up(idx, maxR, maxG, maxB));
    while(down(idx));
  }

private:

  int m_r;
  int m_g;
  int m_b;

  void namesAreHard(int idx, float sleep) {
    setPixel(idx, m_r, m_g, m_b);
    show();
    delay(sleep);
  }

  bool up(int idx, int maxR, int maxG, int maxB) {
    bool more = false;

    if(m_r < maxR) {
      m_r++;
      more = true;
    }
    if(m_g < maxG) {
      m_g++;
      more = true;
    }
    if(m_b < maxB) {
      m_b++;
      more = true;
    }

    namesAreHard(idx, 0.25);
    return more;
  }

  bool down(int idx) {
    bool more = false;

    if(m_r > 0) {
      m_r--;
      more = true;
    }
    if(m_g > 0) {
      m_g--;
      more = true;
    }
    if(m_b > 0) {
      m_b--;
      more = true;
    }

    namesAreHard(idx, 3);
    return more;
  }

};

struct Heating : Pattern {
  Heating() : Pattern() {
    m_focus = random(NUMPIXELS);
    memset(m_elements, 0, NUMPIXELS * sizeof(int));

    m_count = 100;
  }

  virtual void apply() {
    cool();
    heat();

    for(int idx = 0; idx <= NUMPIXELS; idx++) {
      setPixel(idx, m_elements[idx], 0, 0);
    }
    show();
    delay(25);

    m_count--;
    if(m_count <= 50 && m_count > 0) {
      m_focus = -1;
    } else if(m_count <= 0) {
      m_focus = random(NUMPIXELS);
      m_count = 100;
    }
  }

private:
  int m_focus;
  int m_elements[NUMPIXELS + 1];
  int m_count;

  void cool() {
    int factor = 5;
    for(int idx = 0; idx <= NUMPIXELS; idx++) {
      if(m_elements[idx] > factor) {
        m_elements[idx] -= factor;
      } else {
        m_elements[idx] = 0;
      }
    }
  }

  void heat() {
    float scale = 0.1;

    for(int idx = 0; idx <= NUMPIXELS; idx++) {
      int heating = 0;
      if(idx == m_focus) {
        heating = 20;
      } else if(idx == 0) {
        heating = (m_elements[1] - m_elements[idx]) * 0.1;
      } else if(idx == NUMPIXELS) {
        heating = (m_elements[NUMPIXELS - 1] - m_elements[idx]) * 0.5;
      } else {
        heating += (m_elements[idx - 1] - m_elements[idx]) * 0.5;
        heating += (m_elements[idx + 1] - m_elements[idx]) * 0.5;
      }

      m_elements[idx] += heating;

      if(m_elements[idx] > 255) {
        m_elements[idx] = 255;
      }
    }
  }
};


void blink(int sleep) {
  digitalWrite(13, HIGH);
  delay(sleep);
  digitalWrite(13, LOW);
  delay(sleep);
}

Pattern *pattern;

void setup() {
  pinMode(13, OUTPUT);

  randomSeed(analogRead(0));

  switch(random(4)) {
    case 4:
      pattern = new Random();
      break;
    case 3:
      pattern = new SerialStrobe();
      break;
    case 2:
      pattern = new GlowWorm();
      break;
    case 1:
      pattern = new WalkWhite();
      break;
    case 0:
      pattern = new Heating();
      break;
  }
}

void loop() {
  pattern->apply();
}
