
#include <Wire.h>
#include <PCA9698.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

/* I2C Class Constructorfdf*/

#define AdrGpioIN 0x24;
//#define AdrGpioOUT 0x25;
#define AdrGpioOUT 0x20;

PCA9698 gpioIN = AdrGpioIN;
  // uses 5 vars to remember last port state and just write when necessary  
  int inPort[5] = {0,0,0,0,0};
  int inPortLast[5] = {0,0,0,0,0};
 
PCA9698 gpioOUT = AdrGpioOUT;
  // uses 5 vars to remember last port state and just write when necessary
  int outPort[5] = {0,0,0,0,0};
  int outPortLast[5] = {0,0,0,0,0};





#define firmVersion "V.0.2"

#define Pi 3.1415927

// I/O pinout

  // IN

  // OUT

  // System vars
    boolean debug_serial = true;

  // std vars
    int i = 0;
    String tmpStr = "";
    float tmpFloat = 0;
    unsigned long temp_ulong = 0;
    unsigned int temp_int = 0;

  // Serial communication
    boolean stringComplete = false;  // whether the string is complete
    #define long_buffer_RCV 500
    char char_RECV[long_buffer_RCV] = "";
    int char_RCV = 0;
    String Command = "";
    char Channel[1] = {0};        // char to retrieve Channel from serial data
    int CH = 0;                      // int to handle Channel

    #define bps_Serial 19200
    #define Welcome_message_serial_0 "--------------------------------"
    #define Welcome_message_serial_1 "  Maquinas Control Vision, SL"
    #define Welcome_message_serial_2 "--------------------------------"
    #define Welcome_message_serial_3 "        Siembra y Masa"
    #define Welcome_message_serial_4 "--------------------------------"

  // machine status vars
    unsigned int cycleStatus = 0;
    boolean FirstCycle = true;
    boolean run = false;
    boolean stop = true;
    boolean flushing = false;
    unsigned long timer = 0;

  // vars used to handle i2C IOs



void setup() {

  if (debug_serial)
  {
    Serial.begin(bps_Serial);
    Serial.println(F(Welcome_message_serial_0));
    Serial.println(F(Welcome_message_serial_1));
    Serial.println(F(Welcome_message_serial_2));
    Serial.println(F(Welcome_message_serial_3));
    Serial.println(F(Welcome_message_serial_4));
    Serial.println(F(firmVersion));
  }

  //Begin I2C

    // IN
    Serial.println("Starting i2C IN");

    gpioIN.begin();
    gpioIN.config(OE_POL_HIGH | OUTPUT_CHANGE_ACK | GPIO_ALL_CALL_ON | SMBA_ON);

    uint8_t modesIN[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    gpioIN.setModePorts(modesIN, sizeof(modesIN)); 

    Serial.println("i2C IN running");


    
    Serial.println("Starting i2C OUT");

    gpioOUT.begin();   
    //gpioOUT.config(OE_POL_HIGH | OUTPUT_CHANGE_ACK | GPIO_ALL_CALL_ON | SMBA_ON);

    uint8_t modesOUT[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    gpioOUT.setModePorts(modesOUT, sizeof(modesOUT)); 

    Serial.println("i2C OUT running");
    
 // setup I/O





}

  int portOUTval = 0;

void loop() 
{

    Serial.println("IN adr 0x24 ----------------------------------------------");

    gpioIN.updateInput(); //Reads input over I2C
    uint8_t input[5];   //Array for input data
    gpioIN.digitalReadPorts(input, sizeof(input));    //Reads all inputs 
    for (uint8_t index = 0; index < sizeof(input); index++) 
    {
        Serial.print(index, DEC);       //Print port number
        Serial.print(": ");       
        Serial.println(input[index]);   //Print port as HEX
    }
    

    Serial.println("OUT adr 0x25 ----------------------------------------------");

    
    gpioOUT.togglePin(0);
    gpioOUT.togglePin(1);

    /*
    if (portOUTval == 0)
    {
      portOUTval = 1;
      Serial.println("OUT Ports: 0b00000000");  
      gpioOUT.digitalWritePort(0, 0b00000000);
      
      gpioOUT.digitalWritePort(1, 0b00000000);
      gpioOUT.digitalWritePort(2, 0b00000000);
      gpioOUT.digitalWritePort(3, 0b00000000);
      gpioOUT.digitalWritePort(4, 0b00000000);
    }
    else
    {
      portOUTval = 0;
      Serial.println("OUT Ports: 0b11111111");  
      gpioOUT.digitalWritePort(0, 0b11111111);

      gpioOUT.digitalWritePort(1, 0b11111111);
      gpioOUT.digitalWritePort(2, 0b11111111);
      gpioOUT.digitalWritePort(3, 0b11111111);
      gpioOUT.digitalWritePort(4, 0b11111111);
    }
    */

    gpioOUT.updateOutput();  
  




    delay(1000);

 


}




// decodes received messages from serial port and execute the corresponding command
void PARAM_Decode()
{

 
  if (Command == "PAR")  
  {      
  
    Serial.println("--- P A R A M E T E R   L I S T ---"); 
  
    Serial.print("DEBUG = "); 
    Serial.println(debug_serial);
    Serial.print("cycleStatus = "); 
    Serial.println(cycleStatus);


    

    Serial.println("------------------------------------");
  } 
  
  if (Command == "DEB")  // Debug bit
  {
      temp_int = 0; 
      temp_int = atol(char_RECV);
      
      if (temp_int == 0 || temp_int == 1)
      {                    
        Serial.print("Set DEBUG = ");
        Serial.println(temp_int);
        
        if (temp_int == 0)
          debug_serial = false;
        else
          debug_serial = true;
        
        // save to EEPROM
        #ifdef EEPROM_enabled
          EEPROM.writeBool(EM_debug, debug_serial);
        #endif
      }
      else
      {
        Serial.print("Value must be 0 or 1");
      }
  }



  // clear the string ---------------------------------------------------------------------------
    Command = "";
    Channel[0] = 0;
    for (int i = 0; i < long_buffer_RCV ; i ++)
    {
      char_RECV[i] = '\0';
    }
    stringComplete = false; 
  
}

// Log for develop
void Log(String message, int priority, bool showPriority)
{
  if (debug_serial >= priority)
  {
    if (showPriority) Serial.print(priority + ": ");
    Serial.print((message));
    Serial.println(F(""));
  }   
}



