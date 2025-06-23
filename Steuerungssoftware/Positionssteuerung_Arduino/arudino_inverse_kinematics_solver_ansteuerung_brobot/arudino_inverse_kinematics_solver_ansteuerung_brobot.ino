// TB6600 Multi-Joint Stepper Control für TCP Steuerung
const int JOINT1_PINS[3] = {8, 9, 10};   // STEP, DIR, ENA für Joint 1 (Base)
const int JOINT2_PINS[3] = {11, 12, 13}; // STEP, DIR, ENA für Joint 2 (Shoulder)
const int JOINT3_PINS[3] = {5, 6, 7};    // STEP, DIR, ENA für Joint 3 (Elbow)
const int JOINT4_PINS[3] = {2, 3, 4};    // STEP, DIR, ENA für Joint 4 (Wrist)

// Motor-Parameter für jeden Joint
struct JointMotor {
  int stepPin;
  int dirPin;
  int enaPin;
  long targetPosition;
  long currentPosition;
  bool moving;
  bool direction;
  unsigned long lastStepTime;
  double pulseDelay;
  bool stepState;
};

// Joint-Motoren initialisieren
JointMotor joints[4] = {
  {8, 9, 10, 0, 0, false, false, 0, 400, false},   // Joint 1
  {11, 12, 13, 0, 0, false, false, 0, 400, false}, // Joint 2
  {5, 6, 7, 0, 0, false, false, 0, 400, false},    // Joint 3
  {2, 3, 4, 0, 0, false, false, 0, 400, false}     // Joint 4
};

// Globale Parameter
double globalSpeed = 400;  // Standard Geschwindigkeit in µs
String receivedCommand = "";
boolean commandComplete = false;
boolean allMotorsEnabled = true;
int activeMotors = 0;

// Status-Codes
const String CODE1 = "Code1: Position wurde angefahren";
const String CODE2 = "Code2: Vorgang wurde abgebrochen";
const String CODE3 = "Code3: Motor ist einsatzbereit!";
const String CODE4 = "Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet";

void setup() {
  Serial.begin(9600);
  
  // Initialisiere alle Joint-Pins
  for(int i = 0; i < 4; i++) {
    pinMode(joints[i].stepPin, OUTPUT);
    pinMode(joints[i].dirPin, OUTPUT);
    pinMode(joints[i].enaPin, OUTPUT);
    
    // Motoren aktivieren (LOW = aktiv)
    digitalWrite(joints[i].enaPin, LOW);
    digitalWrite(joints[i].dirPin, LOW);
    digitalWrite(joints[i].stepPin, LOW);
    
    joints[i].pulseDelay = globalSpeed;
  }
  
  Serial.println("DEBUG: 4-Joint TCP Control bereit");
  Serial.println(CODE3);
  printStatus();
}

void loop() {
  // Serielle Kommunikation verarbeiten
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
  
  // Alle Motoren bewegen
  moveAllMotors();
  
  // Prüfe ob alle Motoren ihre Ziele erreicht haben
  checkMovementComplete();
}

void processCommand(String command) {
  command.trim();
  
  Serial.print("DEBUG: Befehl empfangen: '");
  Serial.print(command);
  Serial.println("'");
  
  if (command.equalsIgnoreCase("STOP")) {
    Serial.println("DEBUG: STOP - Alle Motoren stoppen");
    stopAllMotors();
    Serial.println(CODE2);
  }
  else if (command.equalsIgnoreCase("GO")) {
    Serial.println("DEBUG: GO - Alle Motoren aktivieren");
    enableAllMotors();
    Serial.println(CODE3);
  }
  else if (command.equalsIgnoreCase("status")) {
    printStatus();
  }
  else if (command.startsWith("SPEED:")) {
    // Globale Geschwindigkeit setzen: SPEED:300
    double newSpeed = command.substring(6).toDouble();
    if (newSpeed >= 100 && newSpeed <= 2000) {
      globalSpeed = newSpeed;
      for(int i = 0; i < 4; i++) {
        joints[i].pulseDelay = globalSpeed;
      }
      Serial.print("Geschwindigkeit gesetzt auf ");
      Serial.print(globalSpeed);
      Serial.println("µs");
    } else {
      Serial.println("Ungültige Geschwindigkeit (100-2000µs)");
    }
  }
  else if (command.startsWith("J") && command.indexOf(":") > 0) {
    // Joint-Befehl: J1:1000, J2:-500, etc.
    int colonPos = command.indexOf(":");
    int jointNum = command.substring(1, colonPos).toInt();
    long targetSteps = command.substring(colonPos + 1).toInt();
    
    if (jointNum >= 1 && jointNum <= 4) {
      setJointTarget(jointNum - 1, targetSteps);  // Array ist 0-basiert
    } else {
      Serial.println("Ungültige Joint-Nummer (1-4)");
      Serial.println(CODE4);
    }
  }
  else if (command.startsWith("MULTI:")) {
    // Multi-Joint Befehl: MULTI:J1:1000,J2:-500,J3:200,J4:0
    parseMultiJointCommand(command.substring(6));
  }
  else {
    Serial.println("DEBUG: Unbekannter Befehl");
    Serial.println(CODE4);
  }
}

void parseMultiJointCommand(String jointCommands) {
  Serial.println("DEBUG: Multi-Joint Befehl wird verarbeitet");
  
  // Teile die Befehle bei Kommas
  int startPos = 0;
  int commaPos = 0;
  
  while (commaPos >= 0) {
    commaPos = jointCommands.indexOf(',', startPos);
    String singleCommand;
    
    if (commaPos >= 0) {
      singleCommand = jointCommands.substring(startPos, commaPos);
      startPos = commaPos + 1;
    } else {
      singleCommand = jointCommands.substring(startPos);
    }
    
    singleCommand.trim();
    
    // Verarbeite einzelnen Joint-Befehl
    if (singleCommand.startsWith("J") && singleCommand.indexOf(":") > 0) {
      int colonPos = singleCommand.indexOf(":");
      int jointNum = singleCommand.substring(1, colonPos).toInt();
      long targetSteps = singleCommand.substring(colonPos + 1).toInt();
      
      if (jointNum >= 1 && jointNum <= 4) {
        setJointTarget(jointNum - 1, targetSteps);
      }
    }
  }
}

void setJointTarget(int jointIndex, long targetSteps) {
  if (jointIndex < 0 || jointIndex >= 4) return;
  
  joints[jointIndex].targetPosition = targetSteps;
  
  // Berechne Richtung
  long delta = targetSteps - joints[jointIndex].currentPosition;
  joints[jointIndex].direction = (delta > 0);
  digitalWrite(joints[jointIndex].dirPin, joints[jointIndex].direction ? HIGH : LOW);
  
  // Starte Bewegung wenn Unterschied vorhanden
  if (delta != 0) {
    joints[jointIndex].moving = true;
    activeMotors++;
  }
  
  Serial.print("Joint ");
  Serial.print(jointIndex + 1);
  Serial.print(" Ziel: ");
  Serial.print(targetSteps);
  Serial.print(" (Delta: ");
  Serial.print(delta);
  Serial.println(")");
}

void moveAllMotors() {
  for (int i = 0; i < 4; i++) {
    if (joints[i].moving && allMotorsEnabled) {
      moveJoint(i);
    }
  }
}

void moveJoint(int jointIndex) {
  if (jointIndex < 0 || jointIndex >= 4) return;
  
  JointMotor* joint = &joints[jointIndex];
  
  if (joint->currentPosition != joint->targetPosition) {
    if (micros() - joint->lastStepTime >= joint->pulseDelay) {
      joint->stepState = !joint->stepState;
      digitalWrite(joint->stepPin, joint->stepState);
      
      if (joint->stepState == LOW) {
        // Position aktualisieren
        if (joint->direction) {
          joint->currentPosition++;
        } else {
          joint->currentPosition--;
        }
      }
      joint->lastStepTime = micros();
    }
  } else {
    // Ziel erreicht
    if (joint->moving) {
      joint->moving = false;
      activeMotors--;
      Serial.print("Joint ");
      Serial.print(jointIndex + 1);
      Serial.println(" Ziel erreicht");
    }
  }
}

void checkMovementComplete() {
  static unsigned long lastCheck = 0;
  
  if (millis() - lastCheck > 100) {  // Prüfe alle 100ms
    bool anyMoving = false;
    for (int i = 0; i < 4; i++) {
      if (joints[i].moving) {
        anyMoving = true;
        break;
      }
    }
    
    if (!anyMoving && activeMotors == 0) {
      static bool wasMoving = false;
      if (wasMoving) {
        Serial.println(CODE1);  // Alle Positionen erreicht
        wasMoving = false;
      }
    } else {
      static bool wasMoving = false;
      wasMoving = true;
    }
    
    lastCheck = millis();
  }
}

void stopAllMotors() {
  for (int i = 0; i < 4; i++) {
    joints[i].moving = false;
    joints[i].targetPosition = joints[i].currentPosition;
  }
  activeMotors = 0;
}

void enableAllMotors() {
  allMotorsEnabled = true;
  for (int i = 0; i < 4; i++) {
    digitalWrite(joints[i].enaPin, LOW);  // LOW = aktiviert
  }
}

void disableAllMotors() {
  allMotorsEnabled = false;
  for (int i = 0; i < 4; i++) {
    digitalWrite(joints[i].enaPin, HIGH);  // HIGH = deaktiviert
    joints[i].moving = false;
  }
  activeMotors = 0;
}

void printStatus() {
  Serial.println("=== MULTI-JOINT STATUS ===");
  Serial.print("Motoren aktiv: ");
  Serial.print(allMotorsEnabled ? "JA" : "NEIN");
  Serial.print(" | Bewegende Joints: ");
  Serial.println(activeMotors);
  
  for (int i = 0; i < 4; i++) {
    Serial.print("Joint ");
    Serial.print(i + 1);
    Serial.print(": Pos=");
    Serial.print(joints[i].currentPosition);
    Serial.print(" | Ziel=");
    Serial.print(joints[i].targetPosition);
    Serial.print(" | Bewegung=");
    Serial.print(joints[i].moving ? "JA" : "NEIN");
    Serial.print(" | Speed=");
    Serial.print(joints[i].pulseDelay);
    Serial.println("µs");
  }
  Serial.println("========================");
}

// Hilfsfunktionen für individuelle Joint-Steuerung
void setJointSpeed(int jointIndex, double speed) {
  if (jointIndex >= 0 && jointIndex < 4) {
    joints[jointIndex].pulseDelay = speed;
  }
}

long getJointPosition(int jointIndex) {
  if (jointIndex >= 0 && jointIndex < 4) {
    return joints[jointIndex].currentPosition;
  }
  return 0;
}

bool isJointMoving(int jointIndex) {
  if (jointIndex >= 0 && jointIndex < 4) {
    return joints[jointIndex].moving;
  }
  return false;
}