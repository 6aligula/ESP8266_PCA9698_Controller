#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Iniciando escaneo I2C...");
}

void loop() {
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Dispositivo encontrado en dirección 0x");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Escaneo completado.");
  delay(5000); // Escanear cada 5 segundos
}
