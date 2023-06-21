bool isPrime(int num){
  int numSq = int(sqrt(num))+1;

  for(int i = 2; i < numSq; i++){
    if(num % i == 0) return false;
  }
  
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  int count = 0;
  int num = 2;

  Serial.println("Generating the first 30 primes!");
  while(count < 30){
    //Serial.println(num);
    if(isPrime(num)){
      Serial.print(count + 1);
      Serial.print(" : ");
      Serial.println(num);
      count += 1;
    }
    num += 1;
  }
  Serial.println("Done!");
}

void loop() {
  
}
