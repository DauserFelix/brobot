// TB6600 Schrittmotor-Treiber - Debug Version
const int STEP_PIN = 8;    // PUL+ an Arduino Pin 8
const int DIR_PIN = 9;     // DIR+ an Arduino Pin 9
const int ENA_PIN = 10;    // ENA+ an Arduino Pin 10

// Konfigurierbare Parameter
double pulseDelay = 400;     // Verzögerung zwischen Schritten in Mikrosekunden
boolean motorDirection = LOW;  // Drehrichtung (LOW oder HIGH)
boolean motorEnabled = true;   // Motor aktiviert (true) oder deaktiviert (false)
String receivedCommand = "";   // Empfangener Befehl
boolean commandComplete = false;

unsigned long lastStepTime = 0;
bool stepState = LOW;

// Fehlercodes - synchronisiert mit Python
const String CODE1 = "Code1: Position wurde angefahren";
const String CODE2 = "Code2: Vorgang wurde abgebrochen";
const String CODE3 = "Code3: Motor ist einsatzbereit!";
const String CODE4 = "Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet";

// Positionsvariablen
long targetPosition = 0;
long currentPosition = 0;
boolean movingToPosition = false;

// Debug-Variablen
unsigned long lastDebug = 0;
unsigned long stepCount = 0;

// Geschwindigkeitsstufen (1-5)
const double speedLevels[6] = {0, 1000, 600, 400, 200, 100}; // Index 0 unbenutzt, 1-5 für Stufen

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
  
  // Bereitschaftsmeldung
  Serial.println(CODE3);
  Serial.println("DEBUG: Setup abgeschlossen");
  printStatus();
}

void loop() {
  
  // Auf serielle Befehle prüfen
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (receivedCommand.length() > 0) {
        commandComplete = true;
      }
    } else {
      receivedCommand += inChar;
    }
  }
  
  // Befehl verarbeiten
  if (commandComplete) {
    processCommand(receivedCommand);
    receivedCommand = "";
    commandComplete = false;
  }
  
  // Motor bewegen, wenn Zielposition gesetzt
  if (movingToPosition && motorEnabled) {
    moveToPosition();
  }
}

void printStatus() {
  Serial.println("=== STATUS ===");
  Serial.print("Position: ");
  Serial.print(currentPosition);
  Serial.print(" | Ziel: ");
  Serial.print(targetPosition);
  Serial.print(" | Bewegung: ");
  Serial.print(movingToPosition ? "JA" : "NEIN");
  Serial.print(" | Motor: ");
  Serial.print(motorEnabled ? "EIN" : "AUS");
  Serial.print(" | Delay: ");
  Serial.print(pulseDelay);
  Serial.print("µs | Steps: ");
  Serial.println(stepCount);
  Serial.println("==============");
}

void processCommand(String command) {
  command.trim();
  
  Serial.print("DEBUG: Befehl empfangen: '");
  Serial.print(command);
  Serial.println("'");
  
  if (command.equalsIgnoreCase("Stop")) {
    Serial.println("DEBUG: Stop-Befehl");
    movingToPosition = false;
    targetPosition = currentPosition;
    Serial.println(CODE2);
  }
  else if (command.equalsIgnoreCase("Go")) {
    Serial.println("DEBUG: Go-Befehl");
    motorEnabled = true;
    digitalWrite(ENA_PIN, LOW);
    Serial.println(CODE3);
  }
  else if (command == "test") {
    Serial.println("DEBUG: Test-Bewegung gestartet");
    targetPosition = currentPosition + 200;  // 200 Steps vorwärts
    movingToPosition = true;
    motorDirection = HIGH;
    digitalWrite(DIR_PIN, motorDirection);
    Serial.println("Test durchgeführt!"); // Bestätigung
  }
  // NEUE Geschwindigkeitsstufen s1-s5 (s für speed)
  else if (command == "s1" || command == "s2" || command == "s3" || command == "s4" || command == "s5") {
    int speedLevel = command.substring(1).toInt(); // "s1" -> 1
    pulseDelay = speedLevels[speedLevel];
    Serial.print("Geschwindigkeitsstufe ");
    Serial.print(speedLevel);
    Serial.print(" gesetzt (");
    Serial.print(pulseDelay);
    Serial.println("µs)");
  }
  else if (command.equalsIgnoreCase("d")) {
    Serial.println("DEBUG: Richtungswechsel");
    motorDirection = !motorDirection;  // Richtung umkehren
    digitalWrite(DIR_PIN, motorDirection);
    Serial.println("Richtung geändert"); // Bestätigung
  }
  else if (command.equalsIgnoreCase("e")) {
    Serial.println("DEBUG: Motor ein/aus");
    motorEnabled = !motorEnabled;
    digitalWrite(ENA_PIN, motorEnabled ? LOW : HIGH);  // LOW = aktiviert
    Serial.println("Motor " + String(motorEnabled ? "EIN" : "AUS")); // Bestätigung
  }
  else if (command.equals("+")) {
    Serial.println("DEBUG: Geschwindigkeit erhöhen");
    pulseDelay = max(pulseDelay - 200, 100);  // Mindestverzögerung 100µs
    Serial.print("Geschwindigkeit erhöht auf ");
    Serial.print(pulseDelay);
    Serial.println("µs");
  }
  else if (command.equals("-")) {
    Serial.println("DEBUG: Geschwindigkeit verringern");
    pulseDelay += 200;  // Verzögerung erhöhen
    Serial.print("Geschwindigkeit verringert auf ");
    Serial.print(pulseDelay);
    Serial.println("µs");
  }
  else if (command.equalsIgnoreCase("h")) {
    Serial.println("=== HILFE ===");
    Serial.println("s1-s5: Geschwindigkeitsstufen");
    Serial.println("d: Richtung wechseln");
    Serial.println("e: Motor ein/aus");
    Serial.println("+: Geschwindigkeit erhöhen");
    Serial.println("-: Geschwindigkeit verringern");
    Serial.println("Stop: Bewegung stoppen");
    Serial.println("Go: Motor aktivieren");
    Serial.println("Zahl: Position anfahren");
    Serial.println("==============");
  }
  else {
    // Versuche als Zahl zu interpretieren (Positionsbefehl)
    long position = command.toInt();
    
    if (position != 0 || command == "0") {
      // Korrektur: Berechne die Richtung anhand der Differenz
      long delta = position - currentPosition;
      motorDirection = (delta > 0) ? HIGH : LOW;
      digitalWrite(DIR_PIN, motorDirection);
      
      targetPosition = position;
      movingToPosition = true;
      
      Serial.print("DEBUG: Zielposition: ");
      Serial.print(targetPosition);
      Serial.print(" | Delta: ");
      Serial.println(delta);
    } else {
      Serial.println("DEBUG: Ungültige Eingabe");
      motorEnabled = false;
      digitalWrite(ENA_PIN, HIGH);
      movingToPosition = false;
      Serial.println(CODE4);
    }
  }
}

void moveToPosition() {
  if (currentPosition != targetPosition) {
    if (micros() - lastStepTime >= pulseDelay) {
      stepState = !stepState;
      digitalWrite(STEP_PIN, stepState);
      
      if (stepState == LOW) {
        // Korrektur: Positionsupdate mit Überprüfung
        if (motorDirection == HIGH) {
          currentPosition++;
        } else {
          currentPosition--;
        }
      }
      lastStepTime = micros();
    }
  } else {
    movingToPosition = false;
    Serial.println(CODE1);  // WICHTIG: Bestätigung senden
  }
}