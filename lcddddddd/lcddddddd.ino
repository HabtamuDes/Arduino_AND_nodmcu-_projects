#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  lcd.autoscroll();
  lcd.setCursor(16, 0);
  for (int x = 0; x < 10; x++) {
    lcd.print(x);
    delay(500);
  }
  lcd.clear();
}
