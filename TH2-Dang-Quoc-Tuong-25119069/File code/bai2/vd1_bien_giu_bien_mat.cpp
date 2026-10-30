#include <Arduino.h>

#define LED_PIN 2
#define NGU_GIAY 10

RTC_DATA_ATTR int soLanThuc = 0;
int demThuong = 0;

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n\n=== TH2 BAI 2 - VD1: bien giu / bien mat qua Deep Sleep ===");
  Serial.printf("Vua thuc day : soLanThuc (RTC) = %d | demThuong (RAM) = %d\n", soLanThuc, demThuong);

  delay(3000);

  soLanThuc++;
  demThuong++;
  Serial.printf("Truoc khi ngu: soLanThuc (RTC) = %d | demThuong (RAM) = %d\n", soLanThuc, demThuong);

  digitalWrite(LED_PIN, LOW);
  Serial.printf("-> Deep sleep %d s...\n", NGU_GIAY);
  Serial.flush();
  esp_sleep_enable_timer_wakeup(NGU_GIAY * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {
}
