                    
/* 
 *  Ex_18 - NEC Decoder
 *  Decode incoming infrared Signal on the Robot
 *   
 *  I2C 
 *  --  GP18 (SDA) 
 *  --  GP19 (SCL) 
 *  
 *  LED
 *  -- GP12
 *  
 *  IR_RECV
 *  -- GP6
 *  
 *  NeoPixel
 *  -- GP20
 */
                    
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C1_SDA 18
#define I2C1_SCL 19
// I2C address
#define SR04_I2CADDR 0x57
#define OLED_I2CAADR 0x3C
#define TONE_PIN 22

#define PEXP_I2CADDR 0x23
#define OLED_I2CAADR 0x3C

#define NEO_PIXEL 20
#define LED_COUNT 3

#define LED 12
#define IR_RECV 6

#define MotorA_1 8
#define MotorA_2 9
#define MotorB_1 10
#define MotorB_2 11

SSD1306AsciiWire oled(Wire1);

uint8_t LEDState = LOW;

//Just some variables to hold temporary data
char text[10];
uint8_t  i, nec_state = 0; 
unsigned int command, address;

static unsigned long nec_code;
static boolean nec_ok = false;
static unsigned long timer_value_old;
static unsigned long timer_value;

void irISR() {
    
    timer_value = micros() - timer_value_old;       // Store elapse timer value in microS
    
    switch(nec_state){
        case 0:   //standby:                                      
            if (timer_value > 67500) {          // Greater than one frame...
                timer_value_old = micros();     // Reset microS Timer
                nec_state = 1;                  // Next state: end of 9ms pulse + LeadingSpace 4.5ms
                i = 0;
            }
        break;

        // Leading Mark = Leading pulse + leading space = 9000 + 4500 = 13500
        // max tolerance = 1000
        case 1:   //startPulse:
            if (timer_value >= (13500 - 1000) && timer_value <= (13500 + 1000)) { //its a Leading Mark
                i = 0;
                timer_value_old = micros();
                nec_state = 2;
            }
            else 
                nec_state = 0;
        break;

        //Bit 0 Mark length = 562.5µs pulse + 562.5µs space = 1125
        //Bit 1 Mark length = 562.5µs pulse + 3 * 562.5µs space = 2250
        //max tolerance = (2250 - 1125)/2 = 562.5 
        case 2:   //receiving:
            if (timer_value < (1125 - 562) || timer_value > (2250 + 562)) nec_state = 0; //error, not a bit mark
            else { // it's M0 or M1
                  nec_code = nec_code << 1; //push a 0 from Right to Left (will be left at 0 if it's M0)
                  if(timer_value >= (2250 - 562)) nec_code |= 0x01; //it's M1, change LSB to 1
                  i++;
                  
                  if (i==32) { //all bits received
                      nec_ok = true;
//                    detachInterrupt(IR_RECV);   // Optional: Disable external interrupt to prevent next incoming signal
                      nec_state = 0;
                      timer_value_old = micros();
                  }
                  else {
                      nec_state = 2; //continue receiving
                      timer_value_old = micros();
                  }
              }
        break;
      
        default:
            nec_state = 0;
        break;
    }
}

char decodeIr(unsigned int command){
  switch(command){
    case 0x0: return '1';
    case 0x80: return '2';
    case 0x40: return '3';
    case 0x20: return '4';
    case 0xA0: return '5';
    case 0x60: return '6';
    case 0x10: return '7';
    case 0x90: return '8';
    case 0x50: return '9';
    case 0xB0: return '0';
    case 0x30: return 'M'; // mute
    case 0x70: return 'D'; // del
    case 0x88: return 'u'; // up
    case 0x98: return 'd'; // down
    case 0x28: return 'l'; // left
    case 0x68: return 'r'; // right
    case 0xA8: return 'o'; // ok


    default: return 0;
  }
}

int ping_mm()
{
    unsigned long distance = 0;
    byte i;   
    byte ds[3];
    long timeChecked;
    
    Wire1.beginTransmission(SR04_I2CADDR);
    Wire1.write(1);          //1 = cmd to start measurement.
    Wire1.endTransmission();

    delay(200); // Delay 200ms

    i = 0;
    Wire1.requestFrom(0x57,3);  //read distance       
    while (Wire1.available())
    {
     ds[i++] = Wire1.read();
    }        
    
    distance = (unsigned long)(ds[0] << 16);
    distance = distance + (unsigned long)(ds[1] << 8);
    distance = (distance + (unsigned long)(ds[2])) / 1000;
    //measured value between 10 mm (1 cm) to 6 meters (600 cm)
    return (int)distance; return -1;
}

void setup() {
    Wire1.setSDA(I2C1_SDA);
    Wire1.setSCL(I2C1_SCL);
    Wire1.begin();
    
    oled.begin(&Adafruit128x32, OLED_I2CAADR); 
    Serial.begin(115200); //set up serial library baud rate to 115200
    delay(2000);
                    
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LEDState);
    pinMode(IR_RECV, INPUT);
    delay(2000);
    
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("NEC Decoder!");

    pinMode(MotorA_1, OUTPUT);
    pinMode(MotorA_2, OUTPUT);
    pinMode(MotorB_1, OUTPUT);
    pinMode(MotorB_2, OUTPUT);
    pinMode(TONE_PIN, OUTPUT);
    
    attachInterrupt(IR_RECV, irISR, FALLING);  //Setting IR detection on FALLING edge.
}

// Direction 0: forward | 1: backward
// Speed = from 0 to 100
// Motor  0: Motor A | 1: MOtor B
void motorMoveControl(unsigned short motor, unsigned short direction, unsigned short speed) {    
  analogWrite((motor == 0) ? MotorA_1 : MotorB_1, (direction == 0) ? 0 : ((speed > 0) && (speed < 100)? speed +150 : 0));
  analogWrite((motor == 0) ? MotorA_2 : MotorB_2, (direction == 1) ? 0 : ((speed > 0) && (speed < 100)? speed +150 : 0));
}

char direction = 's';
char pDirection = 's';
int speed = 1;
void loop() {
    if(nec_ok) {                                     // If a good NEC message is received
        nec_ok = false;                             // Reset decoding process
        
        LEDState = (LEDState == LOW) ? HIGH: LOW;   // Toggle LED to show new IR result
        digitalWrite(LED, LEDState);

        oled.clear();
        oled.println("NEC IR Received:");
        
        oled.print("Addr: ");
        address = nec_code >> 16;
        command = (nec_code & 0xFFFF) >> 8;         // Remove inverted Bits
        sprintf(text, "%04X", address);
        oled.println(text);                             // Display address in hex format

        if(address != 0xFD){
          return;
        }

        oled.print("Cmd: ");
        sprintf(text, "%02X", command);
        oled.println(text);                             // Display command in hex format
        oled.println(decodeIr(command));
        direction = decodeIr(command);
        pDirection = decodeIr(command);
//      attachInterrupt(IR_RECV, irISR, FALLING);  //Setting IR detection on FALLING edge.
    } 
    
    int distance = ping_mm();
    if(distance < 300){
      tone(TONE_PIN, 3000);
      if(distance < 200) direction = 'd';
      else direction = 'r';
    }else{
      noTone(TONE_PIN);
      direction = pDirection;
    }

    switch(direction){
      case 'o':
        motorMoveControl(0, 1, 0);
        motorMoveControl(1, 1, 0);
        break;
      case 'u':
        motorMoveControl(0, 1, 20 + 10*speed*2);
        motorMoveControl(1, 1, 20 + 10*speed*2);
        break;
      case 'r':
        motorMoveControl(0, 0, 20 + 10*speed*2);
        motorMoveControl(1, 1, 20 + 10*speed*2);
        break;
      case 'l':
        motorMoveControl(0, 1, 20 + 10*speed*2);
        motorMoveControl(1, 0, 20 + 10*speed*2);
        break;
      case 'd':
        motorMoveControl(0, 0, 20 + 10*speed*2);
        motorMoveControl(1, 0, 20 + 10*speed*2);
        break;
      case '1':
        speed = 1;
        break;
      case '2':
        speed = 2;
        break;
      case '3':
        speed = 3;
        break;
      case '4':
        speed = 4;
        break;
      default:
        break;
    }

}