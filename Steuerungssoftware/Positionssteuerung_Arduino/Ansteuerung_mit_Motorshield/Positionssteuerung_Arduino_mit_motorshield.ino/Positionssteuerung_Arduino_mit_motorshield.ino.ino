//FUnktioniert mit dem Pythonscript "Positionssteuerung_Python.py" im BroBot Ordner

#include <AccelStepper.h>

#define dirPin 7
#define stepPin 4
#define motorInterfaceType 1



AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
float newPosition;
String Stop_Go="";
bool enable=1;

void setup() {
  Serial.begin(9600);  // Starte die serielle Kommunikation
  stepper.setMaxSpeed(1800);   //500 entspricht 150rpm (gemessen), 1500=450rpm, 1333,3=400rpm, 1800 ist guter kompromiss
  stepper.setAcceleration(1333);
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
    else if (newPosition>=-10000000000000 && newPosition<=10000000000000 && enable==0)   //Position anfahren
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


/*

// Pins für Z-Achse am CNC Shield V3 (AZ-Delivery)
#define Z_STEP_PIN  4     // STEP-Pin des Z-Achsen-Treibers
#define Z_DIR_PIN   7     // DIR-Pin des Z-Achsen-Treibers
#define Z_ENABLE_PIN 8    // ENABLE-Pin des Z-Achsen-Treibers

// Einstellungen
int stepDelay = 1000;      // Verzögerung zwischen Schritten in Mikrosekunden (Geschwindigkeit)
bool enabled = false;      // Zustand des Enable-Pins

void setup() {
  // Pins als Output definieren
  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);

  // Treiber initial deaktivieren (ENABLE ist oft LOW-aktiv)
  digitalWrite(Z_ENABLE_PIN, HIGH); // HIGH = Treiber deaktiviert
  enabled = false;

  // Serial Monitor starten
  Serial.begin(9600);
  Serial.println("Z-Achse bereit. Befehle:");
  Serial.println("F [Schritte]  - Vorwärts");
  Serial.println("B [Schritte]  - Rückwärts");
  Serial.println("E             - Treiber ein/aus");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Befehl parsen
    if (input.startsWith("F") || input.startsWith("f")) {
      int steps = input.substring(1).toInt();
      moveMotor(HIGH, steps); // Vorwärts
    } 
    else if (input.startsWith("B") || input.startsWith("b")) {
      int steps = input.substring(1).toInt();
      moveMotor(LOW, steps); // Rückwärts
    } 
    else if (input.equalsIgnoreCase("E")) {
      enabled = !enabled;
      digitalWrite(Z_ENABLE_PIN, !enabled); // Enable-Pin umschalten
      Serial.print("Treiber: ");
      Serial.println(enabled ? "EIN" : "AUS");
    }
  }
}

// Schrittmotor bewegen
void moveMotor(bool dir, int steps) {
  if (!enabled) {
    Serial.println("Fehler: Treiber nicht aktiviert! Sende 'E'.");
    return;
  }

  digitalWrite(Z_DIR_PIN, dir); // Richtung setzen

  for (int i = 0; i < steps; i++) {
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(stepDelay);
  }

  Serial.print("Bewegt: ");
  Serial.print(steps);
  Serial.println(" Schritte");
}

*/

/*
// Pins für Z-Achse am CNC Shield V3 (AZ-Delivery)
#define Z_STEP_PIN  4     // STEP-Pin des Z-Achsen-Treibers
#define Z_DIR_PIN   7     // DIR-Pin des Z-Achsen-Treibers
#define Z_ENABLE_PIN 8    // ENABLE-Pin des Z-Achsen-Treibers

// Einstellungen
int stepDelay = 1000;      // Verzögerung zwischen Schritten in Mikrosekunden (Geschwindigkeit)
bool enabled = false;      // Zustand des Enable-Pins

void setup() {
  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  digitalWrite(Z_ENABLE_PIN, HIGH); // Treiber aktivieren
  digitalWrite(Z_DIR_PIN, HIGH);   // Vorwärts
}

void loop() {
  digitalWrite(Z_STEP_PIN, HIGH);
  delayMicroseconds(2000);
  digitalWrite(Z_STEP_PIN, LOW);
  delayMicroseconds(2000);
}*/