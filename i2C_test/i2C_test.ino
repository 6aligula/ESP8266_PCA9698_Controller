#include <Wire.h>

#define PCA9698_ADDRESS 0x20 // Dirección del PCA9698
#define OE_PIN 0             // Pin GPIO para controlar el Output Enable (OE)

// Definiciones de pines
#define LED1_PIN 0  // P0_0
#define LED2_PIN 8  // P1_0

void configurePinAsOutput(uint8_t pin);
void writeToOutputRegister(uint8_t pin, bool state);
void setOutputEnable(bool enable);

void setup() {
  // Inicializar I2C y serial
  Wire.begin(4, 5); // SDA = GPIO4 (D2), SCL = GPIO5 (D1) en ESP8266
  Serial.begin(115200);
  delay(1000); // Retraso para estabilizar la comunicación

  Serial.println("Iniciando configuración del PCA9698...");

  // Configurar el pin OE
  setOutputEnable(true);

  // Configurar P0_0 y P1_0 como salidas
  configurePinAsOutput(LED1_PIN); // LED 1 en P0_0
  configurePinAsOutput(LED2_PIN); // LED 2 en P1_0

  Serial.println("PCA9698 configurado. Iniciando loop...");
}

void loop() {
  // Encender el LED conectado a P0_0
  Serial.println("Encendiendo LED en P0_0...");
  writeToOutputRegister(0, true);
  delay(1000);

  // Apagar el LED conectado a P0_0
  Serial.println("Apagando LED en P0_0...");
  writeToOutputRegister(0, false);
  delay(1000);

  // Encender el LED conectado a P1_0
  Serial.println("Encendiendo LED en P1_0...");
  writeToOutputRegister(LED2_PIN, true);
  delay(1000);

  // Apagar el LED conectado a P1_0
  Serial.println("Apagando LED en P1_0...");
  writeToOutputRegister(LED2_PIN, false);
  delay(1000);
}

/**
 * Configura un pin específico como salida en el PCA9698 sin afectar otros pines.
 * 
 * @param pin Número del pin (0-7 para P0, 8-15 para P1, etc.)
 */
void configurePinAsOutput(uint8_t pin) {
  uint8_t port = pin / 8;                    // Determinar el puerto (P0, P1, etc.)
  uint8_t bit = pin % 8;                     // Bit correspondiente al pin
  uint8_t iocRegister = 0x18 + port;         // Dirección del registro IOC correspondiente
  uint8_t currentConfig = 0xFF;              // Valor por defecto (todos como entrada)

  // Leer el estado actual del registro IOC
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(iocRegister); // Seleccionar el registro IOCx
  if (Wire.endTransmission(false) == 0) { // Mantener la conexión activa
    Wire.requestFrom(PCA9698_ADDRESS, (uint8_t)1); // Solicitar un byte del registro
    if (Wire.available()) {
      currentConfig = Wire.read(); // Leer el valor actual
    }
  } else {
    Serial.print("Error al leer el registro IOC para el puerto P");
    Serial.println(port);
    return;
  }

  // Modificar el bit correspondiente para configurarlo como salida (0)
  currentConfig &= ~(1 << bit);

  // Escribir el nuevo estado en el registro IOC
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(iocRegister); // Seleccionar el registro IOCx
  Wire.write(currentConfig); // Escribir el nuevo estado
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("Pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(bit);
    Serial.println(" configurado como salida correctamente.");
  } else {
    Serial.print("Error al configurar el pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(bit);
    Serial.print(". Código de error: ");
    Serial.println(error);
  }
}


/**
 * Escribe un estado (HIGH/LOW) en el registro de salida del PCA9698 sin afectar otros pines.
 * 
 * @param pin Número del pin (0-7 para P0, 8-15 para P1, etc.)
 * @param state Estado del pin: true para HIGH, false para LOW.
 */
void writeToOutputRegister(uint8_t pin, bool state) {
  uint8_t port = pin / 8;       // Determinar el puerto (P0, P1, etc.)
  uint8_t bitMask = (1 << (pin % 8)); // Máscara para el bit correspondiente
  uint8_t opRegister = 0x08 + port;  // Dirección del registro OP correspondiente
  uint8_t currentState = 0;

  // Leer el estado actual del registro de salida
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(opRegister); // Seleccionar el registro OPx
  if (Wire.endTransmission(false) == 0) { // Mantener la conexión activa
    Wire.requestFrom(PCA9698_ADDRESS, (uint8_t)1); // Solicitar un byte del registro
    if (Wire.available()) {
      currentState = Wire.read(); // Leer el valor actual
    }
  }

  // Modificar solo el bit correspondiente
  if (state) {
    currentState |= bitMask; // Establecer el bit en HIGH
  } else {
    currentState &= ~bitMask; // Establecer el bit en LOW
  }

  // Escribir el nuevo estado en el registro
  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(opRegister); // Seleccionar el registro OPx
  Wire.write(currentState); // Escribir el nuevo estado
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("Estado del pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(pin % 8);
    Serial.print(" cambiado a ");
    Serial.println(state ? "HIGH" : "LOW");
  } else {
    Serial.print("Error al cambiar el estado del pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(pin % 8);
    Serial.print(". Código de error: ");
    Serial.println(error);
  }
}


/**
 * Activa o desactiva el pin Output Enable (OE) del PCA9698.
 * 
 * @param enable true para activar las salidas (OE a LOW), false para desactivarlas (OE a HIGH).
 */
void setOutputEnable(bool enable) {
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, enable ? LOW : HIGH);

  Serial.print("Output Enable (OE) ");
  Serial.println(enable ? "activado." : "desactivado.");
}
