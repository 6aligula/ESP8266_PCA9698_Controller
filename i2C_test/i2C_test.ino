#include <Wire.h>

#define PCA9698_ADDRESS 0x20 // Dirección I²C configurada

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000); // Pequeño retraso para estabilizar la comunicación
  Serial.println("Iniciando configuración del PCA9698...");

  // Configurar P0_0 como salida
  Wire.beginTransmission(PCA9698_ADDRESS);
  Serial.println("Enviando configuración al registro (0x06)...");
  Wire.write(0x06); // Registro de configuración de P0
  Wire.write(0xFE); // 11111110: P0_0 salida, el resto entradas
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Configuración completada correctamente.");
  } else {
    Serial.print("Error en la configuración I²C. Código: ");
    Serial.println(error);
  }
}

void loop() {
  // Encender el LED
  Serial.println("Encendiendo LED en P0_0...");
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(0x02); // Registro de salida de P0
  Wire.write(0x01); // 00000001: P0_0 en alto
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LED encendido correctamente.");
  } else {
    Serial.print("Error al encender el LED. Código: ");
    Serial.println(error);
  }
  delay(1000);

  // Apagar el LED
  Serial.println("Apagando LED en P0_0...");
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(0x02); // Registro de salida de P0
  Wire.write(0x00); // 00000000: P0_0 en bajo
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LED apagado correctamente.");
  } else {
    Serial.print("Error al apagar el LED. Código: ");
    Serial.println(error);
  }
  delay(1000);
}
