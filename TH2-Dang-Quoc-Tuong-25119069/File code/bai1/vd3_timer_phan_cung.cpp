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

void IRAM_ATTR ghiNhan(Nhip &n, unsigned long now) {
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

hw_timer_t *timer = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

volatile int demNgat = 0;
volatile bool ledSang = false;
volatile bool coAdc = false, coHeart = false;
volatile unsigned long tNgatAdc = 0;
unsigned long treMax = 0;

void IRAM_ATTR onTimer() {
  unsigned long now = micros();
  ledSang = !ledSang;
  digitalWrite(LED_PIN, ledSang);

  portENTER_CRITICAL_ISR(&mux);
  demNgat++;
  ghiNhan(nLed, now);
  if (demNgat % 4 == 0) {
    ghiNhan(nAdc, now);
    tNgatAdc = now;
    coAdc = true;
  }
  if (demNgat % 10 == 0) {
    ghiNhan(nHeart, now);
    coHeart = true;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void batTimer() {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, T_LED * 1000, true, 0);
#else
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, T_LED * 1000, true);
  timerAlarmEnable(timer);
#endif
}

int phaCu = 0;

void kiemTraPha() {
  int pha = millis() / PHA_MS;
  if (pha % 2 == 1) viecNang();
  if (pha != phaCu) {
    Nhip a, b, c;
    portENTER_CRITICAL(&mux);
    a = nLed;
    b = nAdc;
    c = nHeart;
    xoaSoLieu(nLed);
    xoaSoLieu(nAdc);
    xoaSoLieu(nHeart);
    portEXIT_CRITICAL(&mux);

    inKetQua(phaCu, "hw timer", a, b, c);
    Serial.printf("Tre xu ly ADC lon nhat trong loop = %.3f ms\n\n", treMax / 1000.0);
    treMax = 0;
    phaCu = pha;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n=== TH2 BAI 1 - VD3: timer phan cung ===");
  pinMode(LED_PIN, OUTPUT);
  batTimer();
}

void loop() {
  if (coAdc) {
    coAdc = false;
    int adc = analogRead(ADC_PIN);
    unsigned long tre = micros() - tNgatAdc;
    if (tre > treMax) treMax = tre;
    Serial.printf("ADC = %d | tre xu ly = %.3f ms\n", adc, tre / 1000.0);
  }

  if (coHeart) {
    coHeart = false;
    Serial.printf("[heartbeat] t = %.1f s | %s\n", millis() / 1000.0, tenPha());
  }

  kiemTraPha();
}
