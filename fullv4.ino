//StateMachine
#include <GOTStateMachine.h>
GOTStateMachine stateMachine(50);
void bacaGSR();
void bacaBPM();
void interval();
void uji();
//GSR
const int GSRnumReadings = 30;
int GSRreadings[GSRnumReadings];
int GSRreadIndex = 0;
int GSRtotal = 0;
int GSRaverage = 0;
int GSRpin = A0;
int sensorValue = 0;
int GSR = 0;
//BPM
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
const int PulseWire = A1;
int Threshold = 550;
PulseSensorPlayground pulseSensor;
//Variable Global
int GSRrataRata = 0;
int GSRlive = 0;
int BPMrataRata = 0;

void setup() {
  stateMachine.setStartState(bacaGSR);
  Serial.begin(9600);
  //GSR
  for (int GSRthisReading = 0; GSRthisReading < GSRnumReadings; GSRthisReading++) {
    GSRreadings[GSRnumReadings] = 0;
  }
  //BPM
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.
  }
}

void loop() {
  stateMachine.execute();

}
void bacaGSR() {
  //StateLogic
  //GSR
  long sum = 0;
  for (int i = 0; i < 10; i++)    //Average the 10 measurements to remove the glitch
  {
    sensorValue = analogRead(GSRpin);
    sum += sensorValue;
    delay(5);
  }
  GSR = sum / 10;
  GSRlive = GSR;
  // put your main code here, to run repeatedly:
  GSRtotal = GSRtotal - GSRreadings[GSRreadIndex];
  GSRreadings[GSRreadIndex] = GSR;
  GSRtotal = GSRtotal + GSRreadings[GSRreadIndex];
  GSRreadIndex = GSRreadIndex + 1;
  if (GSRreadIndex >= GSRnumReadings) {
    GSRreadIndex = 0;
  }
  GSRaverage = GSRtotal / GSRnumReadings;
  GSRrataRata = GSRaverage;
  Serial.print("Rata2");
  Serial.print("\t");
  Serial.println(GSRaverage);
  Serial.print("live");
  Serial.print("\t");
  Serial.println(GSR);
  delay(300);
  //Transisi
  if (stateMachine.isDelayComplete(10000)) {
    stateMachine.changeState(bacaBPM);
    return;
  }
}
void bacaBPM() {
  //StateLogic
  int myBPM = pulseSensor.getBeatsPerMinute();
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
    Serial.println("A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                        // Print phrase "BPM: "
    Serial.println(myBPM);                        // Print the value inside of myBPM.
  }
  BPMrataRata = myBPM;
  delay(20);
  //Transisi
  if (stateMachine.isDelayComplete(10000)) {
    stateMachine.changeState(interval);
    return;
  }
}
void interval() {
  Serial.println("Memasuki State Uji");
  delay(2000);
  if (stateMachine.isDelayComplete(2000)) {
    stateMachine.changeState(uji);
    return;
  }
}
void uji() {
  int myBPM = pulseSensor.getBeatsPerMinute();
  int liveGSR = analogRead(GSRpin);
  Serial.println(myBPM);
  Serial.println(liveGSR);
  Serial.println(GSRrataRata);
  if (liveGSR > GSRrataRata + (GSRrataRata*3/100) && myBPM >103) {
    Serial.print("bohong");
  }
  else if (liveGSR > GSRrataRata + (GSRrataRata*3/100) && myBPM <103) {
    Serial.print("Jujur");
  }
  else {
    Serial.print("jujur");
  }
  delay(500);
  if (stateMachine.isDelayComplete(20000)) {
    stateMachine.changeState(bacaGSR);
    return;
  }
}
