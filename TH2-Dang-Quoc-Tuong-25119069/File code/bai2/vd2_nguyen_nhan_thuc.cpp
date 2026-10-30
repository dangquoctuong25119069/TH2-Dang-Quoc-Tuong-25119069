#include <Arduino.h>
#include "driver/rtc_io.h"

#define LED_PIN 2
#define BUTTON_PIN 4
#define NGU_GIAY 10
#define THUC_MS 5000

RTC_DATA_ATTR int soLanKhoiDong = 0;

void inLyDoThuc() {
  esp_sleep_wakeup_cause_t lyDo = esp_sleep_get_wakeup_cause();
  Serial.printf("Ma ly do thuc = %d -> ", (int)lyDo);
  switch (lyDo) {
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("TIMER (het gio ngu)"); break;
    case ESP_SLEEP_WAKEUP_EXT0:  Serial.println("EXT0 (nhan nut GPIO4)"); break;
    default:                     Serial.println("khong phai thuc tu sleep (cap nguon / reset)"); break;
  }
}

void diNgu() {
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);

  digitalWrite(LED_PIN, LOW);
  Serial.printf("-> Deep sleep %d s (nhan nut de danh thuc som)\n", NGU_GIAY);
  Serial.flush();

  esp_sleep_enable_timer_wakeup(NGU_GIAY * 1000000ULL);
  rtc_gpio_pullup_en(GPIO_NUM_4);
  rtc_gpio_pulldown_dis(GPIO_NUM_4);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
  esp_deep_sleep_start();
}

void setup() {
  unsigned long msVaoSetup = millis();
  Serial.begin(115200);
  delay(200);
  pinMode(LED_PIN, OUTPUT);
  rtc_gpio_deinit(GPIO_NUM_4);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  soLanKhoiDong++;
  Serial.printf("\n\n=== TH2 BAI 2 - VD2: vao setup() lan %d ===\n", soLanKhoiDong);
  inLyDoThuc();
  Serial.printf("millis() luc vao setup = %lu ms\n", msVaoSetup);
  Serial.printf("Thuc %d s, LED nhap nhay = CPU dang chay\n", THUC_MS / 1000);
}

void loop() {
  static unsigned long batDau = millis();
  static unsigned long mocNhay = millis();
  static bool ledSang = false;

  if (millis() - mocNhay >= 200) {
    mocNhay += 200;
    ledSang = !ledSang;
    digitalWrite(LED_PIN, ledSang);
  }

  if (millis() - batDau >= THUC_MS) diNgu();
}
