#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC  7
#define TFT_CS 8

// TFT kijelző inicializálása
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// Minta adatok a kördiagramhoz
//int values[] = {30, 70, 45, 55}; // Az egyes szeletek értékei
int values[] = {33, 67}; // Az egyes szeletek értékei
int numValues = sizeof(values) / sizeof(values[0]);

void setup() {
  // Soros port inicializálása
  Serial.begin(9600);
  
  // TFT kijelző inicializálása
  tft.begin();
  
  // Háttér kitöltése fehér színnel
  tft.fillScreen(GC9A01A_BLACK);
  
  // Kördiagram rajzolása
  drawPieChart();
}

void loop() {
  // Nincs szükség a loop függvényre ebben a példában
}

void drawPieChart() {
  // Kördiagram pozíciója és mérete
  int cx = tft.width() / 2;    // Középpont x koordináta
  int cy = tft.height() / 2;   // Középpont y koordináta
  int radius = (min(tft.width(), tft.height()) / 2)-10;
  int radiusSmaller = radius/3;

  // Színek a kördiagram szeleteihez
  uint16_t colors[] = {GC9A01A_RED, GC9A01A_DARKGREY, GC9A01A_BLUE, GC9A01A_YELLOW};

  // Összérték kiszámítása
  int total = 0;
  for (int i = 0; i < numValues; i++) {
    total += values[i];
  }

  // Kezdő szög
  float startAngle = 0;

  // Kördiagram szeleteinek rajzolása és számok kiírása
  for (int i = 0; i < numValues; i++) {
    float endAngle = startAngle + 360.0 * values[i] / total;
    
    if(i>0) radius = radiusSmaller;

    // Szelet kirajzolása
    fillArc(cx, cy, radius, startAngle, endAngle, colors[i]);
    //drawOutlineArc(cx, cy, radius, startAngle, endAngle, colors[i]);
    drawOutlineArc(cx, cy, radius, startAngle, endAngle, GC9A01A_WHITE);
    drawScale();
    drawCenteredText("Temp");

    // Szelet közepére szám kiírása
    if(i==0){
      float midAngle = (startAngle + endAngle) / 2.0;
      int labelX = cx + cos(radians(midAngle)) * (radius / 2); // Középpont x koordináta
      int labelY = cy + sin(radians(midAngle)) * (radius / 2); // Középpont y koordináta
      drawCenteredText(values[i], labelX, labelY);
    }

    startAngle = endAngle;
  }
}

// Arc kitöltése Adafruit GFX könyvtárral
void fillArc(int16_t x, int16_t y, int16_t radius, float startAngle, float endAngle, uint16_t color) {
  startAngle = radians(startAngle);
  endAngle = radians(endAngle);

  for (int16_t r = 0; r <= radius; r++) {
    for (float angle = startAngle; angle <= endAngle; angle += 0.005) {
      int16_t x0 = x - cos(angle) * r; // Vízszintes tükrözés
      int16_t y0 = y - sin(angle) * r; // Függőleges tükrözés
      tft.drawPixel(x0, y0, color);
    }
  }
}

void drawOutlineArc(int16_t x, int16_t y, int16_t radius, float startAngle, float endAngle, uint16_t color) {
  startAngle = radians(startAngle);
  endAngle = radians(endAngle);

  // Körvonalak rajzolása
  for (float angle = startAngle; angle <= endAngle; angle += 0.005) {
    int16_t x0 = x - cos(angle) * radius;
    int16_t y0 = y - sin(angle) * radius;
    tft.drawPixel(x0, y0, color);
  }

  // Körcikk két szélső vonalainak rajzolása
  int16_t x1 = x + cos(startAngle) * radius; // Kezdő pont
  int16_t y1 = y + sin(startAngle) * radius;
  int16_t x2 = x + cos(endAngle) * radius;   // Végső pont
  int16_t y2 = y + sin(endAngle) * radius;
  tft.drawLine(tft.width() - x, tft.height() - y, tft.width() - x1, tft.height() - y1, color);
  tft.drawLine(tft.width() - x, tft.height() - y, tft.width() - x2, tft.height() - y2, color);
}

void drawScale() {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int radius = min(centerX, centerY) - 10; // Kisebb legyen, mint a kijelző szélessége

  tft.drawCircle(centerX, centerY, radius, GC9A01A_WHITE);

  for (int i = 0; i <= 10; i++) {
    float angle = map(i, 0, 10, 180, -180);
    float radian = angle * PI / 180;
    
    int x1 = centerX + radius * cos(radian);
    int y1 = centerY + radius * sin(radian);
    int x2 = centerX + (radius - 10) * cos(radian);
    int y2 = centerY + (radius - 10) * sin(radian);

    tft.drawLine(x1, y1, x2, y2, GC9A01A_WHITE);

    // Számok rajzolása
    int number = (10 - i) * 10;
    if (number == 100) continue; // A 100-at nem írjuk ki

    int numX = centerX + (radius - 20) * cos(radian);
    int numY = centerY + (radius - 20) * sin(radian);

    tft.setCursor(numX - 5, numY - 5);
    tft.setTextColor(GC9A01A_WHITE);
    tft.setTextSize(1);
    tft.print(number);
  }
}

// Szöveg kiírása középre
void drawCenteredText(int value, int x, int y) {
  tft.setTextColor(GC9A01A_WHITE); // Szöveg színe fekete
  tft.setTextSize(3); // Szöveg mérete 2x
  
  // Vízszintesen tükrözés
  x = tft.width() - x; // A TFT kijelző szélességéből kivonjuk az x koordinátát

  // Függőleges tükrözés
  y = tft.height() - y;

  tft.setCursor(x - 10, y - 8); // Szöveg kiírásának helye
  tft.print(value); // Szöveg kiírása
}

void drawCenteredText(const char* text) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(1);
  tft.setTextColor(GC9A01A_WHITE);

  // Szöveg méretének kiszámítása
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  // Szöveg középre helyezése
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int x = centerX - w / 2;
  int y = centerY - h / 2;

  tft.setCursor(x, y);
  tft.print(text);
}