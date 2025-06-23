// TB6600 Schrittmotor-Treiber - Multi Joint Version (4 Joints)
const int JOINT1_PINS[3] = {8, 9, 10};   // STEP, DIR, ENA für Joint 1
const int JOINT2_PINS[3] = {11, 12, 13}; // STEP, DIR, ENA für Joint 2
const int JOINT3_PINS[3] = {5, 6, 7};    // STEP, DIR, ENA für Joint 3
const int JOINT4_PINS[3] = {2, 3, 4};    // STEP, DIR, ENA für Joint 4

// Aktuelle Pin-Variablen (werden je nach ausgewähltem Joint gesetzt)
int STEP_PIN = 8;
int DIR_PIN = 9;
int ENA_PIN = 10;

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

// Joint-Bestätigungscodes
const String JOINT1_CODE = "Arduino: Joint 1 wird angesteuert";
const String JOINT2_CODE = "Arduino: Joint 2 wird angesteuert";
const String JOINT3_CODE = "Arduino: Joint 3 wird angesteuert";
const String JOINT4_CODE = "Arduino: Joint 4 wird angesteuert";

// Positionsvariablen
long targetPosition = 0;
long currentPosition = 0;
boolean movingToPosition = false;

// Aktuell aktiver Joint
int currentJoint = 1;

// Debug-Variablen
unsigned long lastDebug = 0;
unsigned long stepCount = 0;

// Geschwindigkeitsstufen (1-5)
const double speedLevels[6] = {0, 1000, 600, 400, 200, 100}; // Index 0 unbenutzt, 1-5 für Stufen

void setup() {
  // Pin-Modi für alle vier Joints setzen
  for(int i = 0; i < 3; i++) {
    pinMode(JOINT1_PINS[i], OUTPUT);
    pinMode(JOINT2_PINS[i], OUTPUT);
    pinMode(JOINT3_PINS[i], OUTPUT);
    pinMode(JOINT4_PINS[i], OUTPUT);
  }
  
  // Serielle Kommunikation starten
  Serial.begin(9600);
  
  // Standardmäßig Joint 1 aktivieren
  setActiveJoint(1);
  
  // Anfangszustand setzen
  digitalWrite(DIR_PIN, motorDirection);
  digitalWrite(ENA_PIN, motorEnabled ? LOW : HIGH);  // LOW = aktiviert, HIGH = deaktiviert
  
  // Bereitschaftsmeldung
  Serial.println(CODE3);
  Serial.println("DEBUG: Setup abgeschlossen - Joint 1 aktiv");
  printStatus();
}

void loop() {
  // Auf serielle Befehle prüfen
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    
    // Spezielle Joint-Auswahl Befehle (als Byte-Werte)
    if (inChar == 1) {
      setActiveJoint(1);
      Serial.println(JOINT1_CODE);
      return;
    }
    else if (inChar == 2) {
      setActiveJoint(2);
      Serial.println(JOINT2_CODE);
      return;
    }
    else if (inChar == 3) {
      setActiveJoint(3);
      Serial.println(JOINT3_CODE);
      return;
    }
    else if (inChar == 4) {
      setActiveJoint(4);
      Serial.println(JOINT4_CODE);
      return;
    }
    
    // Normale String-Befehle verarbeiten
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

void setActiveJoint(int jointNumber) {
  currentJoint = jointNumber;
  
  if (jointNumber == 1) {
    STEP_PIN = JOINT1_PINS[0];
    DIR_PIN = JOINT1_PINS[1];
    ENA_PIN = JOINT1_PINS[2];
  } else if (jointNumber == 2) {
    STEP_PIN = JOINT2_PINS[0];
    DIR_PIN = JOINT2_PINS[1];
    ENA_PIN = JOINT2_PINS[2];
  } else if (jointNumber == 3) {
    STEP_PIN = JOINT3_PINS[0];
    DIR_PIN = JOINT3_PINS[1];
    ENA_PIN = JOINT3_PINS[2];
  } else if (jointNumber == 4) {
    STEP_PIN = JOINT4_PINS[0];
    DIR_PIN = JOINT4_PINS[1];
    ENA_PIN = JOINT4_PINS[2];
  }
  
  // Bewegung stoppen beim Joint-Wechsel
  movingToPosition = false;
  currentPosition = 0;
  targetPosition = 0;
  
  // Motor-Zustand für neuen Joint setzen
  digitalWrite(DIR_PIN, motorDirection);
  digitalWrite(ENA_PIN, motorEnabled ? LOW : HIGH);
  
  Serial.print("DEBUG: Joint ");
  Serial.print(jointNumber);
  Serial.println(" ist jetzt aktiv");
}

void printStatus() {
  Serial.println("=== STATUS ===");
  Serial.print("Aktiver Joint: ");
  Serial.print(currentJoint);
  Serial.print(" | Position: ");
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
  // Geschwindigkeitsstufen s1-s5
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
  else if (command.equalsIgnoreCase("status")) {
    printStatus();
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
    Serial.println("status: Status anzeigen");
    Serial.println("Zahl: Position anfahren");
    Serial.println("==============");
  }
  else {
    // Versuche als Zahl zu interpretieren (Positionsbefehl)
    long position = command.toInt();
    
    if (position != 0 || command == "0") {
      // Berechne die Richtung anhand der Differenz
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
        // Positionsupdate mit Richtungsüberprüfung
        if (motorDirection == HIGH) {
          currentPosition++;
        } else {
          currentPosition--;
        }
        stepCount++;
      }
      lastStepTime = micros();
    }
  } else {
    movingToPosition = false;
    Serial.println(CODE1);  // Bestätigung senden
  }
}