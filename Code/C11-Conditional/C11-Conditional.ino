
String command = "";
void setup() {
  Serial.begin(115200); //set up serial library baud rate to 115200
  delay(2000);
  Serial.println("Am I overweight?");
  Serial.println("Enter your weight (kg)");
}

bool enteredWeight = false;
float weight, height;
void loop() {
  if(Serial.available()){
    command = Serial.readString();

    if(!enteredWeight){
      //first input (weight)
      weight = command.toFloat();
      enteredWeight = true;
      Serial.println(weight);
      Serial.println("Enter your height (m)");
    }else{
      //second input (height)
      height = command.toFloat();
      float bmi = weight / sq(height);
      String category = "";

      Serial.println(height);
      //for displaying the table
      Serial.println("BMI         | Category");
      Serial.println("< 16.0      |	Severely Undeweight");
      Serial.println("16.0 - 18.4 | Undeweight");
      Serial.println("18.5 - 24.9 |	Normal");
      Serial.println("25.0 - 29.9 |	Overweight");
      Serial.println("30.0 - 34.9 |	Moderately Obese");
      Serial.println("35.0 - 39.9 |	Severely Obese");
      Serial.println("> 40        | Morbidly Obese");
      Serial.println("");

      if(bmi < 16) category = "Severely Undeweight";
      else if(bmi >= 16 && bmi < 18.5) category = "Undeweight";
      else if(bmi >= 18.5 && bmi < 25) category = "Normal";
      else if(bmi >= 25 && bmi < 30) category = "Overweight";
      else if(bmi >= 30 && bmi < 35) category = "Moderately Obese";
      else if(bmi >= 35 && bmi < 40) category = "Severely Obese";
      else category = "Morbidly Obese";

      Serial.print("Your BMI: ");
      Serial.println(bmi);
      Serial.print("Category: ");
      Serial.println(category);
      enteredWeight = false;
    }
  }
}
