/* 
 *  Ex_14 - Physical Computing
 *  Revisit Input & Output
 *  
 *  LED 
 *  -- GP12
 *  Switch
 *  -- GP7
 *  
 */

#define ONBOARD_LED 12    // LED GP12
#define ONBOARD_SW  27    // Switch GP7

int current_LED = LOW;

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   pinMode(ONBOARD_LED, OUTPUT);
   pinMode(ONBOARD_SW, INPUT_PULLUP);

   digitalWrite(ONBOARD_LED, current_LED);    // turn the LED off by making the voltage LOW   
}

int mode = 0;
int pressedTime = 0;
void loop(){
  if(digitalRead(ONBOARD_SW) == LOW){
    pressedTime += 1;
  }else{
    pressedTime = 0;
  }

  if(pressedTime > 2000){
    Serial.println(pressedTime);
    digitalWrite(ONBOARD_LED, LOW);
  }else{
    digitalWrite(ONBOARD_LED, (millis() / 1000) % 2 == 0 ? LOW : HIGH);
  }


  delay(1);
}
                    
                