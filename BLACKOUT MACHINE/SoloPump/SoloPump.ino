
#define pump1Pin D2
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  analogWrite(pump1Pin, 400);
  delay(4 * 1000);
  analogWrite(pump1Pin, 0);
  delay(4 * 1000);
}

