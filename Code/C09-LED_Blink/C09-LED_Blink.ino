//Define our Constants
#define ONBOARD_LED 12
#define Math_E 2.71828


//Sets up all our pins when it loads
void setup(){
  pinMode(12, OUTPUT);
}


const int N = 500;
const float Y = 0.14;
float x = 0;
//runs every cpu cycle
void loop(){
  //reset x once its big
  if(x >= N) x = 0;

  float value = pow(Math_E, -(sq(x / (N - 0.5)))/(2 * Y * Y));
  analogWrite(ONBOARD_LED, int(255.0*value));
  delay(5);
  x += 1;
}