#include <Arduino.h>

#define LED_PIN 2
#define ADC_PIN 34

#define T_LED   500
#define T_ADC   2000
#define T_HEART 5000

#define PHA_MS 30000

struct Nhip {
  const char* ten;
  unsigned long chuKy;
  unsigned long lanTruoc;
  unsigned long dem;
  unsigned long tongLech;
  unsigned long lechMax;
};

Nhip nLed   = {"LED",   T_LED * 1000UL,   0, 0, 0, 0};
Nhip nAdc   = {"ADC",   T_ADC * 1000UL,   0, 0, 0, 0};
Nhip nHeart = {"HEART", T_HEART * 1000UL, 0, 0, 0, 0};

void ghiNhan(Nhip &n, unsigned long now) {
  n.dem++;
  if (n.dem > 2) {
    long lech = (long)(now - n.lanTruoc) - (long)n.chuKy;
    if (lech < 0) lech = -lech;
    n.tongLech += lech;
    if ((unsigned long)lech > n.lechMax) n.lechMax = lech;
  }
  n.lanTruoc = now;
}

void xoaSoLieu(Nhip &n) {
  n.dem = 0;
  n.tongLech = 0;
  n.lechMax = 0;
}

void inNhip(Nhip &n) {
  unsigned long soMau = (n.dem > 2) ? n.dem - 2 : 0;
  float lechTB = soMau ? n.tongLech / 1000.0 / soMau : 0;
  Serial.printf("%-5s | %3lu mau | lech TB = %9.3f ms | lech max = %9.3f ms | tong lech = %10.3f ms\n",
                n.ten, soMau, lechTB, n.lechMax / 1000.0, n.tongLech / 1000.0);
}

void inKetQua(int pha, const char* cach, Nhip &a, Nhip &b, Nhip &c) {
  Serial.printf("\n===== KET QUA %s | pha %d: %s =====\n", cach, pha + 1, (pha % 2) ? "CPU BAN" : "CPU RANH");
  inNhip(a);
  inNhip(b);
  inNhip(c);
  Serial.println();
}

int tai[] = {30, 180, 70, 150, 10, 120};
int iTai = 0;

void viecNang() {
  unsigned long t = micros();
  while (micros() - t < tai[iTai] * 1000UL) { }
  iTai = (iTai + 1) % 6;
}

const char* tenPha() {
  return ((millis() / PHA_MS) % 2) ? "CPU BAN" : "CPU RANH";
}

int phaCu = 0;

void kiemTraPha() {
  int pha = millis() / PHA_MS;
  if (pha % 2 == 1) viecNang();
  if (pha != phaCu) {
    inKetQua(phaCu, "delay()", nLed, nAdc, nHeart);
    xoaSoLieu(nLed);
    xoaSoLieu(nAdc);
    xoaSoLieu(nHeart);
    phaCu = pha;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n=== TH2 BAI 1 - VD1: delay() noi tiep ===");
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  ghiNhan(nLed, micros());
  delay(T_LED);
  digitalWrite(LED_PIN, LOW);
  ghiNhan(nLed, micros());
  delay(T_LED);

  ghiNhan(nAdc, micros());
  Serial.printf("ADC = %d\n", analogRead(ADC_PIN));
  delay(T_ADC);

  ghiNhan(nHeart, micros());
  Serial.printf("[heartbeat] t = %.1f s | %s\n", millis() / 1000.0, tenPha());
  delay(T_HEART);

  kiemTraPha();
}
