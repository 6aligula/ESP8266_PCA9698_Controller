#include <Wire.h>

#define PCA9698_ADDRESS 0x20 // Dirección del PCA9698

void setup() {
  Wire.begin(4, 5); // SDA = GPIO4 (D2), SCL = GPIO5 (D1) en ESP8266
  Serial.begin(115200);
  delay(1000); // Retraso para estabilizar la comunicación

  Serial.println("Iniciando configuración del PCA9698...");

  // Configurar el pin P0_0 como salida (Registro IOC0 en 0x18)
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(0x18); // Dirección del registro IOC0
  Wire.write(0xFE); // 1111 1110: P0_0 salida, el resto entradas
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Configuración de IOC0 completada correctamente.");
  } else {
    Serial.print("Error en la configuración del registro IOC0. Código: ");
    Serial.println(error);
    return;
  }

  // Verificar que el pin OE esté en bajo (activado)
  pinMode(0, OUTPUT); // Asegúrate de que el pin conectado a OE esté configurado como salida
  digitalWrite(0, LOW); // Mantener OE en nivel bajo para habilitar las salidas

  Serial.println("PCA9698 configurado. Iniciando loop...");
}

void loop() {
  // Encender el LED (escribir en el registro de salida OP0 en 0x08)
  Serial.println("Encendiendo LED en P0_0...");
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(0x08); // Registro de salida OP0
  Wire.write(0x01); // P0_0 en alto
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LED encendido correctamente.");
  } else {
    Serial.print("Error al encender el LED. Código: ");
    Serial.println(error);
  }
  delay(1000);

  // Apagar el LED (escribir en el registro de salida OP0 en 0x08)
  Serial.println("Apagando LED en P0_0...");
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(0x08); // Registro de salida OP0
  Wire.write(0x00); // P0_0 en bajo
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LED apagado correctamente.");
  } else {
    Serial.print("Error al apagar el LED. Código: ");
    Serial.println(error);
  }
  delay(1000);
}
