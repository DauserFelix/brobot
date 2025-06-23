#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1



AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
float newPosition;
String Stop_Go="";
bool enable=1;

void setup() {
  Serial.begin(9600);  // Starte die serielle Kommunikation
  stepper.setMaxSpeed(10000);   //Maximale Schritte pro Sekunde, 10000 entspricht 10 kpps
  stepper.setAcceleration(10000);
  stepper.moveTo(0);
  
  pinMode(4, OUTPUT);  
}

void loop() {
  // Prüfen, ob Daten über die serielle Schnittstelle empfangen wurden
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Lese die Eingabe bis zum Zeilenumbruch#
    Stop_Go=input;
    newPosition =input.toFloat();


    if(Stop_Go=="Stop")    //Motor anhalten
    {
      Serial.println("Code2: Vorgang wurde abgebrochen");
      stepper.stop();
      digitalWrite(4, HIGH);   //deaktiviert die Stromzufuhr zum Motor
      enable=1;
    }
    else if(Stop_Go=="Go")   //Motor starten
    {
      digitalWrite(4,LOW);
      Serial.println("Code3: Motor ist einsatzbereit!");
      enable=0;
    }
    else if (newPosition>=0 && newPosition<=10000000000000 && enable==0)   //Position anfahren
    {
      stepper.runToNewPosition(newPosition);
      Serial.println("Code1: Position wurde angefahren");
    }
    else
    {
      Serial.println("Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet");
      digitalWrite(4, HIGH);
    }
  }


  
}