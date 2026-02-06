# 🤖 BroBot - 4-DOF Roboterarm mit Cycloidgetriebe

Ein selbstgebauter 4-Achsen-Roboterarm mit Cycloid-Getrieben und inverser Kinematik-Steuerung.

![BroBot Prototyp](docs/images/robot_homepos.jpg)

## 📋 Inhaltsverzeichnis

- [Übersicht](#übersicht)
- [Features](#features)
- [Hardware](#hardware)
- [Software](#software)
- [Installation](#installation)
- [Verwendung](#verwendung)
- [Kinematik](#kinematik)
- [Lizenz](#lizenz)

## 🎯 Übersicht

BroBot ist ein DIY-Roboterarm mit 4 Freiheitsgraden (DOF), der mit Schrittmotoren und Cycloidgetrieben angetrieben wird. Das Projekt umfasst sowohl die mechanische Konstruktion als auch die Software-Steuerung mit inverser Kinematik für präzise TCP-Positionierung (Tool Center Point).

### Technische Daten

- **Freiheitsgrade:** 4 DOF
- **Arbeitshöhe Basis zu Joint 1:** 150 mm
- **Gliedlängen:**
  - L1: 150 mm (Basis zu Joint 1)
  - L2: 200 mm (Joint 1 zu Joint 2)
  - L3: 180 mm (Joint 2 zu Joint 3)
  - L4: 120 mm (Joint 3 zu TCP)
- **Maximale Reichweite:** ~500 mm
- **Antrieb:** NEMA 17/23 Schrittmotoren
- **Getriebe:** Cycloidgetriebe (siehe Explosionszeichnung)

## ✨ Features

### Hardware
- ✅ Cycloid-Getriebe für hohe Untersetzung und Präzision
- ✅ 3D-gedruckte Komponenten
- ✅ TB6600 Schrittmotor-Treiber (bis 4A)
- ✅ Arduino-basierte Steuerung
- ✅ Kompakte Bauweise

### Software
- ✅ **Inverse Kinematik** - Steuerung über kartesische Koordinaten (X, Y, Z)
- ✅ **Vorwärts-Kinematik** - Berechnung der TCP-Position aus Gelenkwinkeln
- ✅ **Python-Steuerung** - Komfortable Kommandozeilen-Schnittstelle
- ✅ **Arduino-Firmware** - Echtzeitsteuerung der Schrittmotoren
- ✅ **Relative & Absolute Bewegung** - Flexible Positionierung
- ✅ **Home-Position** - Sichere Referenzposition

## 🔧 Hardware

### Komponenten

#### Antrieb
- 4x NEMA 17/23 Schrittmotoren
- 4x TB6600 Schrittmotor-Treiber
- 1x Netzteil 24-36V DC (empfohlen)
- Arduino Uno/Mega

#### Mechanik
- Cycloid-Getriebe (3D-gedruckt)
- Strukturteile (3D-gedruckt oder Aluminium)
- Diverse Schrauben, Lager und Verbindungselemente

### TB6600 Schrittmotor-Treiber

**Spezifikationen:**
- Versorgungsspannung: 9-40 VDC (optimal: 24-36V)
- Ausgangsstrom: 0.7-4.0A (einstellbar via DIP-Schalter)
- Mikroschritt-Modi: bis zu 6400 Steps/Umdrehung
- Eingangssignal: 5V (optisch isoliert)
- Max. Pulsfrequenz: 20 kHz

**Anschluss:**
- **PUL+/PUL-:** Puls-Eingang (Step)
- **DIR+/DIR-:** Richtungs-Eingang
- **ENA+/ENA-:** Enable-Eingang
- **A+/A-, B+/B-:** Motoranschlüsse

⚠️ **Wichtig:** Bei 12V Steuersignalen: 1kΩ Widerstand in Reihe schalten
⚠️ **Wichtig:** Bei 24V Steuersignalen: 2kΩ Widerstand in Reihe schalten

### Cycloid-Getriebe

![Cycloid Getriebe Explosionszeichnung](docs/images/cycloid_exploded.png)

Das Cycloid-Getriebe bietet:
- Hohe Untersetzungsverhältnisse (typisch 1:30 bis 1:100)
- Sehr geringes Spiel
- Hohe Steifigkeit
- Kompakte Bauform
- Selbsthemmung bei hohen Untersetzungen

## 💻 Software

### Architektur

```
┌─────────────────────────────────────┐
│   Python TCP Control Interface      │
│  (Inverse & Forward Kinematics)     │
└────────────────┬────────────────────┘
                 │ Serial (9600 baud)
                 │ Commands: J1:steps, J2:steps...
┌────────────────▼────────────────────┐
│     Arduino Firmware                │
│  (Stepper Control & Positioning)    │
└────────────────┬────────────────────┘
                 │ Step/Dir Signals
┌────────────────▼────────────────────┐
│      TB6600 Stepper Drivers         │
└────────────────┬────────────────────┘
                 │
┌────────────────▼────────────────────┐
│        Stepper Motors               │
└─────────────────────────────────────┘
```

### Schritte pro Grad

Die Kalibrierung erfolgt über gemessene Werte:

| Joint | Steps/90° | Steps/Grad |
|-------|-----------|------------|
| 1     | 5500      | 61.11      |
| 2     | 8000      | 88.89      |
| 3     | 5000      | 55.56      |
| 4     | 5000      | 55.56      |

## 📦 Installation

### Voraussetzungen

- Python 3.7+
- Arduino IDE
- pySerial Library

### Python Setup

```bash
# Repository klonen
git clone https://github.com/yourusername/brobot.git
cd brobot

# Virtuelle Umgebung erstellen (optional)
python -m venv venv
source venv/bin/activate  # Linux/Mac
# oder
venv\Scripts\activate  # Windows

# Dependencies installieren
pip install pyserial
```

### Arduino Setup

1. Arduino IDE öffnen
2. `arudino_inverse_kinematics_automatic_ansteuerung_brobot.ino` laden
3. Board auswählen (Arduino Uno/Mega)
4. COM-Port auswählen
5. Upload

### Konfiguration

In `python_inverse_kinematics_brobot.py`:

```python
# Serieller Port anpassen
arduino_port = 'COM3'  # Windows
# arduino_port = '/dev/ttyUSB0'  # Linux
# arduino_port = '/dev/tty.usbserial-XXX'  # Mac

# Gliedlängen (falls abweichend)
L1 = 150  # mm
L2 = 200  # mm
L3 = 180  # mm
L4 = 120  # mm
```

## 🚀 Verwendung

### Starten der Steuerung

```bash
python python_inverse_kinematics_brobot.py
```

### Initialisierung

Beim Start werden die aktuellen Gelenkwinkel abgefragt:

```
Bitte aktuelle Gelenkwinkel eingeben (in Grad):
  Joint 1: 0
  Joint 2: 45
  Joint 3: 90
  Joint 4: -45
```

### Befehle

#### Absolute Positionierung
```bash
TCP> goto 300 100 200
# Bewegt TCP zu Position X=300mm, Y=100mm, Z=200mm
```

#### Relative Bewegung
```bash
TCP> up 10        # 10mm nach oben
TCP> down 5       # 5mm nach unten
TCP> forward 20   # 20mm nach vorne
TCP> back 15      # 15mm nach hinten
TCP> left 10      # 10mm nach links
TCP> right 10     # 10mm nach rechts
```

#### Weitere Befehle
```bash
TCP> status       # Zeigt aktuelle Position und Winkel
TCP> home         # Fährt in Home-Position
TCP> stop         # Stoppt alle Bewegungen
TCP> help         # Zeigt alle Befehle
TCP> exit         # Beendet das Programm
```

### Beispiel-Session

```bash
TCP> goto 400 0 150
Bewege TCP zu Position: X=400.0, Y=0.0, Z=150.0
Berechnete Joint-Winkel:
  Joint 1: 0.0°
  Joint 2: 45.2°
  Joint 3: 67.8°
  Joint 4: -23.0°
→ Joint 1: 0.0° (0 Steps)
→ Joint 2: 45.2° (4018 Steps)
→ Joint 3: 67.8° (3767 Steps)
→ Joint 4: -23.0° (-1278 Steps)

TCP> up 50
[...]

TCP> status
============================================================
ROBOTER STATUS
============================================================
TCP Position: X=400.0, Y=0.0, Z=200.0 mm

Joint-Winkel:
  Joint 1: 0.0°
  Joint 2: 52.3°
  Joint 3: 71.2°
  Joint 4: -33.5°
============================================================
```

## 🧮 Kinematik

### Inverse Kinematik

Die inverse Kinematik berechnet die erforderlichen Gelenkwinkel für eine gewünschte TCP-Position:

**Koordinatensystem:**
- X: Vorwärts/Rückwärts
- Y: Links/Rechts
- Z: Oben/Unten

**Lösungsansatz:**
1. Joint 1 (Basis): `θ₁ = atan2(y, x)`
2. Reduktion auf 2D-Problem in der XZ-Ebene
3. Berechnung von Joint 2 & 3 mittels Cosinus-Satz
4. Joint 4: Kompensation für horizontalen End-Effektor

**Reichweitengrenzen:**
- Maximum: L2 + L3 + L4 = 500 mm
- Minimum: |L2 - L3 - L4| = abhängig von Konfiguration

### Vorwärts-Kinematik

Berechnet die TCP-Position aus gegebenen Gelenkwinkeln:

```
x_tcp = (L2·cos(θ₂) + L3·cos(θ₂+θ₃) + L4·cos(θ₂+θ₃+θ₄)) · cos(θ₁)
y_tcp = (L2·cos(θ₂) + L3·cos(θ₂+θ₃) + L4·cos(θ₂+θ₃+θ₄)) · sin(θ₁)
z_tcp = L1 + L2·sin(θ₂) + L3·sin(θ₂+θ₃) + L4·sin(θ₂+θ₃+θ₄)
```

## 🛠️ Fehlerbehebung

### Häufige Probleme

**Problem: "Position nicht erreichbar!"**
- Lösung: Prüfe, ob die Zielposition innerhalb der Reichweite liegt
- Maximum: ~500mm vom Ursprung

**Problem: Motoren bewegen sich nicht**
- Prüfe Verkabelung zu TB6600
- Prüfe Stromversorgung (min. 9V)
- Prüfe Enable-Signal
- Prüfe DIP-Schalter Einstellungen am TB6600

**Problem: Schritte werden verloren**
- Reduziere Geschwindigkeit (Delay im Arduino-Code erhöhen)
- Prüfe Stromeinstellung am TB6600
- Prüfe mechanische Reibung

**Problem: Serielle Verbindung schlägt fehl**
- Prüfe COM-Port in `python_inverse_kinematics_brobot.py`
- Prüfe, ob Arduino angeschlossen ist
- Schließe Arduino IDE Serial Monitor

## 📸 Galerie

### Home-Position
![Home Position](docs/images/home_position.jpg)

### Arbeitsposition
![Work Position](docs/images/work_position.jpg)

### Cycloid-Getriebe Detail
![Cycloid Gear](docs/images/cycloid_detail.png)

## 🔮 Geplante Features

- [ ] GUI für einfachere Steuerung
- [ ] Trajektorienplanung mit Geschwindigkeitsprofilen
- [ ] Pick-and-Place Sequenzen
- [ ] Teach-In Modus
- [ ] Kraftregelung mit Stromsensoren
- [ ] ROS Integration
- [ ] WebSocket Remote-Control

## 🤝 Beitragen

Contributions sind willkommen! Bitte erstelle einen Pull Request oder öffne ein Issue.

## 📄 Lizenz

Dieses Projekt steht unter der MIT Lizenz - siehe [LICENSE](LICENSE) Datei für Details.

## 🙏 Danksagungen

- TB6600 Datenblatt von SOROTEC
- Inspiration von diversen Open-Source Roboterarm-Projekten
- Community Support

## 📧 Kontakt

Bei Fragen oder Anregungen, öffne ein Issue auf GitHub.

---

**⚠️ Sicherheitshinweis:** Dieser Roboterarm kann erhebliche Kräfte entwickeln. Immer ausreichend Abstand halten und nie in den Arbeitsbereich greifen während der Roboter in Betrieb ist. Für industrielle Anwendungen nicht geeignet.

---

Made with ❤️ and lots of ☕
