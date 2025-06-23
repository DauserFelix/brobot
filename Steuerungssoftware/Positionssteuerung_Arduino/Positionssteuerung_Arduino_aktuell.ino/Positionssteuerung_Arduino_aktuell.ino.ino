// TB6600 Schrittmotor-Treiber - Interaktiver Code
const int STEP_PIN = 8;    // PUL+ an Arduino Pin 8
const int DIR_PIN = 9;     // DIR+ an Arduino Pin 9
const int ENA_PIN = 10;    // ENA+ an Arduino Pin 10

// Konfigurierbare Parameter
double pulseDelay = 1;        // Verzögerung zwischen Schritten in ms (bestimmt die Geschwindigkeit)
boolean motorDirection = LOW;  // Drehrichtung (LOW oder HIGH)
boolean motorEnabled = true;   // Motor aktiviert (true) oder deaktiviert (false)
char command;                  // Empfangener Befehl vom seriellen Monitor

void setup() {
  // Pin-Modi setzen
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  
  // Serielle Kommunikation starten
  Serial.begin(9600);
  
  // Anfangszustand setzen
  digitalWrite(DIR_PIN, motorDirection);
  digitalWrite(ENA_PIN, motorEnabled ? LOW : HIGH);  // LOW = aktiviert, HIGH = deaktiviert
  
  // Anweisungen ausgeben
  printInstructions();
}

void loop() {
  // Auf serielle Befehle prüfen
  if (Serial.available() > 0) {
    command = Serial.read();
    processCommand(command);
  }
  
  // Motor drehen, wenn aktiviert
  if (motorEnabled) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(pulseDelay);
  }
}

// Verarbeitet empfangene Befehle
void processCommand(char cmd) {
  switch (cmd) {
    case '+':  // Geschwindigkeit erhöhen
      if (pulseDelay > 1) pulseDelay--;
      Serial.print("Geschwindigkeit erhöht. Delay: ");
      Serial.println(pulseDelay);
      break;
      
    case '-':  // Geschwindigkeit verringern
      pulseDelay++;
      Serial.print("Geschwindigkeit verringert. Delay: ");
      Serial.println(pulseDelay);
      break;
      
    case 'd':  // Richtung wechseln
    case 'D':
      motorDirection = !motorDirection;
      digitalWrite(DIR_PIN, motorDirection);
      Serial.print("Richtung geändert: ");
      Serial.println(motorDirection ? "Im Uhrzeigersinn" : "Gegen Uhrzeigersinn");
      break;
      
    case 'e':  // Motor ein-/ausschalten
    case 'E':
      motorEnabled = !motorEnabled;
      digitalWrite(ENA_PIN, motorEnabled ? LOW : HIGH);
      Serial.print("Motor ");
      Serial.println(motorEnabled ? "aktiviert" : "deaktiviert");
      break;
      
    case '1':  // Sehr langsam
      pulseDelay = 2000;
      Serial.println("1 Sehr langsame Geschwindigkeit");
      break;
      
    case '2':  // Langsam
      pulseDelay = 1000;    //150rpm bei Einstellung: 1Mikrostep
      Serial.println("2 Langsame Geschwindigkeit");
      break;
      
    case '3':  // Mittel
      pulseDelay = 400;    //300rpm bei Einstellung: 1Mikrostep
      Serial.println("3 Mittlere Geschwindigkeit (300rpm bei 1Mikrostep)");
      break;
      
    case '4':  // Schnell
      pulseDelay = 333;    //400rpm bei Einstellung: 1Mikrostep ->2sek für eine Umdrehung
      Serial.println("4 Schnelle Geschwindigkeit");
      break;
      
    case '5':  // Sehr schnell //600rpm bei Einstellung: 1Mikrostep ->
      pulseDelay = 250;
      Serial.println("5 Sehr schnelle Geschwindigkeit");
      break;
      
    case 'h':  // Hilfe anzeigen
    case 'H':
    case '?':
      printInstructions();
      break;
  }
}

// Zeigt Anweisungen im seriellen Monitor an
void printInstructions() {
  Serial.println("\n===== TB6600 Schrittmotor-Steuerung =====");
  Serial.println("Verfügbare Befehle:");
  Serial.println("+ : Geschwindigkeit erhöhen");
  Serial.println("- : Geschwindigkeit verringern");
  Serial.println("d : Drehrichtung wechseln");
  Serial.println("e : Motor ein-/ausschalten");
  Serial.println("1-5 : Voreingestellte Geschwindigkeiten");
  Serial.println("h : Diese Hilfe anzeigen");
  Serial.println("=======================================");
  Serial.print("Aktueller Status: ");
  Serial.print("Delay=");
  Serial.print(pulseDelay);
  Serial.print("ms, Richtung=");
  Serial.print(motorDirection ? "CW" : "CCW");
  Serial.print(", Motor ");
  Serial.println(motorEnabled ? "aktiviert" : "deaktiviert");
  Serial.println("=======================================\n");
}