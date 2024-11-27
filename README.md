### **Informe Técnico: Uso del PCA9698 con ESP8266**

Este informe describe las conexiones necesarias y el código base para utilizar el expansor de E/S PCA9698 con un ESP8266. Aquí se detalla cómo conectar un LED, configurar los pines de dirección, habilitar las salidas y definir pines como salidas en el código. Este diseño se basa en el **datasheet del PCA9698** proporcionado.

---

### **1. Conexiones necesarias**

#### **1.1 Alimentación**
- **VDD**: Conéctalo a **3.3V** del ESP8266.
- **GND**: Conéctalo al **GND** del ESP8266.

#### **1.2 Conexión I²C**
El PCA9698 se comunica con el ESP8266 mediante el bus I²C:
- **SDA (Datos)**: Conecta el pin SDA del PCA9698 al pin **D2 (GPIO4)** del ESP8266.
- **SCL (Reloj)**: Conecta el pin SCL del PCA9698 al pin **D1 (GPIO5)** del ESP8266.
- Añade **resistencias pull-up de 4.7kΩ** entre SDA/SCL y VDD (si no están incluidas en el circuito).

#### **1.3 Configuración de la dirección I²C**
Los pines de dirección (`A0`, `A1`, `A2`) determinan la dirección I²C del PCA9698. Para usar la dirección **0x20**:
- Conecta **A0, A1 y A2** a **GND**.

#### **1.4 Habilitación de las salidas (OE)**
El pin **OE (Output Enable)** debe estar en **bajo (LOW)** para habilitar las salidas:
- Conecta el pin **OE** del PCA9698 al pin **GPIO0** del ESP8266 y configúralo como salida.

#### **1.5 Conexión del LED**
Para conectar un LED al pin `P0_0` del PCA9698:
1. Conecta el **cátodo** del LED a **GND**.
2. Conecta el **ánodo** del LED al pin **P0_0** del PCA9698 mediante una resistencia en serie (220Ω - 330Ω).

---

### **2. Configuración del software**

El código está diseñado para:
1. Configurar el pin `P0_0` como salida.
2. Controlar el estado del LED conectado a `P0_0` (encender/apagar).

#### **2.1 Definición del pin como salida**
Para que el pin funcione como salida, el registro de configuración correspondiente (`IOC0`) debe configurarse. En el código, esto se logra mediante la función `configurePinAsOutput(0)`.

#### **2.2 Control del LED**
El estado del LED se controla escribiendo en el registro de salida correspondiente (`OP0`) con la función `writeToOutputRegister(0, true)` (para encender) o `writeToOutputRegister(0, false)` (para apagar).

---

### **3. Código completo**

A continuación, se muestra el código que debe cargarse en el ESP8266 para controlar el PCA9698:

```cpp
#include <Wire.h>

#define PCA9698_ADDRESS 0x20 // Dirección del PCA9698
#define OE_PIN 0             // Pin GPIO para controlar el Output Enable (OE)

void configurePinAsOutput(uint8_t pin);
void writeToOutputRegister(uint8_t pin, bool state);
void setOutputEnable(bool enable);

void setup() {
  Wire.begin(4, 5); // SDA = GPIO4 (D2), SCL = GPIO5 (D1) en ESP8266
  Serial.begin(115200);
  delay(1000);

  Serial.println("Iniciando configuración del PCA9698...");

  // Habilitar salidas con OE en bajo
  setOutputEnable(true);

  // Configurar P0_0 como salida
  configurePinAsOutput(0);

  Serial.println("PCA9698 configurado. Iniciando loop...");
}

void loop() {
  // Encender el LED en P0_0
  Serial.println("Encendiendo LED en P0_0...");
  writeToOutputRegister(0, true);
  delay(1000);

  // Apagar el LED en P0_0
  Serial.println("Apagando LED en P0_0...");
  writeToOutputRegister(0, false);
  delay(1000);
}

void configurePinAsOutput(uint8_t pin) {
  uint8_t port = pin / 8;
  uint8_t bitMask = ~(1 << (pin % 8));
  uint8_t iocRegister = 0x18 + port;

  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(iocRegister);
  Wire.write(bitMask);
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("Pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(pin % 8);
    Serial.println(" configurado como salida correctamente.");
  } else {
    Serial.print("Error al configurar el pin P");
    Serial.print(port);
    Serial.print("_");
    Serial.print(pin % 8);
    Serial.print(". Código de error: ");
    Serial.println(error);
  }
}

void writeToOutputRegister(uint8_t pin, bool state) {
  uint8_t port = pin / 8;
  uint8_t bitMask = (1 << (pin % 8));
  uint8_t opRegister = 0x08 + port;

  Wire.beginTransmission(PCA9698_ADDRESS);
  Wire.write(opRegister);
  Wire.write(state ? bitMask : 0x00);
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

void setOutputEnable(bool enable) {
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, enable ? LOW : HIGH);

  Serial.print("Output Enable (OE) ");
  Serial.println(enable ? "activado." : "desactivado.");
}
```

---

### **4. Resumen del funcionamiento**
1. **Conexión física:**
   - Alimentación, I²C, dirección, OE, y el LED conectado al pin `P0_0`.

2. **Configuración del código:**
   - Configuración de `P0_0` como salida.
   - Control del estado de `P0_0` mediante los registros `IOC0` y `OP0`.

3. **Referencias al datasheet:**
   - Todas las configuraciones están basadas en las especificaciones y ejemplos del datasheet del PCA9698.

---

### **5. Notas importantes**
- Asegúrate de que las conexiones físicas estén correctas, especialmente las resistencias pull-up en las líneas SDA y SCL.
- Si necesitas configurar más pines o manejar múltiples LEDs, puedes reutilizar y expandir las funciones `configurePinAsOutput` y `writeToOutputRegister`.

---

