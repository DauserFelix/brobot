#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

float newPosition;

void setup() {
  Serial.begin(9600);  // Starte die serielle Kommunikation
  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(5000);
  stepper.moveTo(0);

  Serial.println("Position wird angefahren");
}

void loop() {
  // Prüfen, ob Daten über die serielle Schnittstelle empfangen wurden
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Lese die Eingabe bis zum Zeilenumbruch

    newPosition =input.toFloat();
    if(newPosition>0)
    {
      stepper.runToNewPosition(newPosition);
      Serial.println("Position wurde angefahren");
    }
  }


  
}
