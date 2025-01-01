#include <LiquidCrystal_PCF8574.h>  // 使用 C++ 的 LiquidCrystal_PCF8574 庫
#include "DHT.h"                 // 使用 C++ 的 DHT 庫

// 設定 DHT
#define DHTPIN 12 // DHT 資料針腳
#define DHTTYPE DHT11 // DHT 型號

// 初始化 LCD 和 DHT
LiquidCrystal_PCF8574 lcd(0x27);  // 設定 i2c 位址
DHT dht(DHTPIN, DHTTYPE);

// 定義 Degree 符號
byte degree[8] = {
  0b00011,
  0b00011,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// 定義 PWM
#define pwm 9

void setup() {
  // 初始化序列埠、DHT 和 LCD
  Serial.begin(9600);   // 使用 C++ 初始化序列埠
  Serial.println("DHTxx test!");
  dht.begin();          // 使用 C++ 初始化 DHT

  lcd.begin(16, 2); // 使用 C++ 初始化 LCD
  lcd.setBacklight(255);
  lcd.createChar(1, degree);  // 加入 Degree 符號

  lcd.clear();
  lcd.print("System Ready");
  delay(2000);
}

// 攝氏轉華氏的函數
float celsiusToFahrenheit(float celsius) {
  return celsius * 9.0 / 5.0 + 32.0;
}

void loop() {
  unsigned long currentMillis = millis();  // 使用 C++ 獲取目前毫秒數
  static unsigned long previousMillis = 0;
  const unsigned long interval = 10000; // 每 10 秒更新一次

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 讀取溫度
    float t = dht.readTemperature();

    // 檢查是否讀取失敗
    if (isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // 計算華氏溫度
    float f = celsiusToFahrenheit(t);

    // 印出溫度到序列埠
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("*C ");
    Serial.print(f);
    Serial.println("*F");

    // 更新 LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.write(1);  // 使用 Degree 符號
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print(f);
    lcd.write(1);  // 使用 Degree 符號
    lcd.print("F");

    // 設定風扇速度與顯示
    int fanSpeed = 0;
    String fanStatus = "OFF";

    if (t < 20) {
      fanSpeed = 0;  // 風扇關閉
      fanStatus = "OFF";
    } else if (t >= 20 && t < 26) {
      fanSpeed = 150;  // 最小速度
      fanStatus = "Min";
    } else if (t >= 26 && t < 30) {
      fanSpeed = 200;  // 中等速度
      fanStatus = "Med";
    } else {
      fanSpeed = 255;  // 最大速度
      fanStatus = "Max";
    }

    // 寫入 PWM 並更新 LCD 狀態
    analogWrite(pwm, fanSpeed);  // 使用 C++ 控制 PWM

    // 使用 for 迴圈填充 LCD 其餘部分
    lcd.setCursor(8, 1);
    lcd.print("fan:");
    for (int i = 0; i < fanStatus.length(); i++) {
      lcd.print(fanStatus[i]);
    }

    // 確保多餘的字符被清除
    for (int i = fanStatus.length(); i < 5; i++) {
      lcd.print(" ");
    }
  }
}
