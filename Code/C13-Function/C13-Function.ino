char caesarChr(char c, int off, bool encode){
  if(!encode) off = -off;
  
  if(c >= 'a' && c <= 'z') return 'a' + (26 + (c - 'a' + off) % 26) % 26;
  else if(c >= 'A' && c <= 'Z') return 'A' + (c - 'A' + off) % 26;
  else return c;
}

String command = "";
void setup() {
  Serial.begin(115200); //set up serial library baud rate to 115200
  delay(2000);
  Serial.println("Enter your string");
}

int mode = 0;
int offset = 0;
String string;
void loop() {
  if(Serial.available()){
    if(mode == 0){
      mode = 1;
      string = Serial.readString();
      Serial.println(string); 
      Serial.println("Enter the offset");
    }else if(mode == 1){
      mode = 2;
      offset = Serial.readString().toInt();
      Serial.println(offset); 
      Serial.println("Enter 0 for Encode and 1 for Decode");
    }else{
      char command = Serial.readString()[0];
      
      Serial.println(command);
      if(command != '0' && command != '1'){
        Serial.println("Invalid Input!");
        return;
      }

      String result = "";
      for(int i = 0; i < string.length(); i++){
        result += caesarChr(string[i], offset, command == '0');
      }
      Serial.print("The result is: ");
      Serial.println(result);
      Serial.println("Enter your string");

      mode = 0;
    }
  }
}
