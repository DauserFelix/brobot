import serial
import time
import threading
import queue
import math

# Roboter Parameter (Gliedlängen in mm)
L1 = 150  # Höhe von Basis zu Joint 1
L2 = 200  # Länge von Joint 1 zu Joint 2
L3 = 180  # Länge von Joint 2 zu Joint 3
L4 = 120  # Länge von Joint 3 zum TCP

# Schritte pro Grad für jeden Joint
STEPS_PER_DEGREE = {
    1: 61.11,  # 5500 Steps für 90° = 61.11 Steps/Grad
    2: 88.89,  # 8000 Steps für 90° = 88.89 Steps/Grad
    3: 55.56,  # 5000 Steps für 90° = 55.56 Steps/Grad
    4: 55.56   # Annahme: gleich wie Joint 3
}

# Fehlercodes - synchronisiert mit Arduino
Fehlercode1 = "Code1: Position wurde angefahren"
Fehlercode2 = "Code2: Vorgang wurde abgebrochen"
Fehlercode3 = "Code3: Motor ist einsatzbereit!"
Fehlercode4 = "Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet"

# Message Queue
message_queue = queue.Queue()
running = True

# Serielle Verbindung
arduino_port = 'COM3'
baud_rate = 9600
ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)

# Globale Variablen für TCP und Winkel (werden in main() initialisiert)
current_tcp = None
current_angles = None

def send_joint_position(joint, angle_degrees):
    """Sendet Winkel für ein Joint an Arduino."""
    steps = int(angle_degrees * STEPS_PER_DEGREE[joint])
    command = f"J{joint}:{steps}"
    ser.write((command + '\n').encode())
    print(f"→ Joint {joint}: {angle_degrees:.1f}° ({steps} Steps)")

def read_data():
    """Daten vom Arduino empfangen."""
    if ser.in_waiting > 0:
        received_data = ser.readline().decode().strip()
        return received_data
    return None

def continuous_read():
    """Kontinuierliches Lesen von Arduino-Nachrichten."""
    global running
    while running:
        try:
            message = read_data()
            if message:
                message_queue.put(message)
            time.sleep(0.01)
        except Exception as e:
            print(f"Fehler beim Lesen: {e}")
            break

def inverse_kinematics(x, y, z):
    """
    Berechnet inverse Kinematik für 4-DOF Roboterarm.
    
    Args:
        x, y, z: Zielposition des TCP
    
    Returns:
        dict: Joint-Winkel in Grad oder None bei Fehler
    """
    try:
        # Joint 1 (Base): Rotation um Z-Achse
        theta1 = math.degrees(math.atan2(y, x))
        
        # Reduziere auf 2D Problem in der X-Z Ebene
        r = math.sqrt(x*x + y*y)  # Radiale Distanz vom Zentrum
        z_eff = z - L1  # Effektive Höhe über Joint 1
        
        # Distanz vom Joint 1 zum TCP
        d = math.sqrt(r*r + z_eff*z_eff)
        
        # Prüfe Erreichbarkeit
        max_reach = L2 + L3 + L4
        min_reach = abs(L2 - L3 - L4)
        
        if d > max_reach or d < min_reach:
            print(f"Position nicht erreichbar! Distanz: {d:.1f}mm, Bereich: {min_reach:.1f}-{max_reach:.1f}mm")
            return None
        
        # Winkel für Joint 2 und 3 berechnen (2-Link Problem)
        # Verwende L2 und (L3+L4) als effektive Längen
        L23 = L3 + L4  # Effektive Länge von Joint 2 zum TCP
        
        # Cosinus-Regel für Joint 3
        cos_theta3 = (L2*L2 + L23*L23 - d*d) / (2 * L2 * L23)
        cos_theta3 = max(-1, min(1, cos_theta3))  # Begrenze auf [-1, 1]
        theta3 = math.degrees(math.acos(cos_theta3))
        
        # Joint 2 Winkel
        alpha = math.atan2(z_eff, r)
        beta = math.acos((L2*L2 + d*d - L23*L23) / (2 * L2 * d))
        theta2 = math.degrees(alpha + beta)
        
        # Joint 4: Halte End-Effektor horizontal
        theta4 = -(theta2 + theta3 - 90)
        
        # Begrenze Winkel auf sinnvolle Bereiche
        theta1 = max(-180, min(180, theta1))
        theta2 = max(-90, min(90, theta2))
        theta3 = max(0, min(180, theta3))
        theta4 = max(-90, min(90, theta4))
        
        return {
            1: theta1,
            2: theta2,
            3: theta3,
            4: theta4
        }
        
    except Exception as e:
        print(f"Fehler bei inverser Kinematik: {e}")
        return None

def forward_kinematics(angles):
    """
    Berechnet Vorwärtskinematik.
    
    Args:
        angles: Dict mit Joint-Winkeln in Grad
    
    Returns:
        dict: TCP Position (x, y, z)
    """
    try:
        # Konvertiere zu Radiant
        t1 = math.radians(angles[1])
        t2 = math.radians(angles[2])
        t3 = math.radians(angles[3])
        t4 = math.radians(angles[4])
        
        # Berechne TCP Position
        # Joint 2 Position
        x2 = L2 * math.cos(t2)
        z2 = L1 + L2 * math.sin(t2)
        
        # Joint 3 Position
        x3 = x2 + L3 * math.cos(t2 + t3)
        z3 = z2 + L3 * math.sin(t2 + t3)
        
        # TCP Position
        x_tcp = x3 + L4 * math.cos(t2 + t3 + t4)
        z_tcp = z3 + L4 * math.sin(t2 + t3 + t4)
        
        # Rotiere um Z-Achse für Joint 1
        x = x_tcp * math.cos(t1)
        y = x_tcp * math.sin(t1)
        z = z_tcp
        
        return {'x': x, 'y': y, 'z': z}
        
    except Exception as e:
        print(f"Fehler bei Vorwärtskinematik: {e}")
        return {'x': 0, 'y': 0, 'z': 0}

def move_tcp_to(x, y, z):
    """Bewegt TCP zu gewünschter Position."""
    global current_angles, current_tcp
    
    print(f"\nBewege TCP zu Position: X={x:.1f}, Y={y:.1f}, Z={z:.1f}")
    
    # Berechne inverse Kinematik
    target_angles = inverse_kinematics(x, y, z)
    
    if target_angles is None:
        print("Position nicht erreichbar!")
        return False
    
    print("Berechnete Joint-Winkel:")
    for joint in range(1, 5):
        print(f"  Joint {joint}: {target_angles[joint]:.1f}°")
    
    # Sende Befehle an alle Joints
    for joint in range(1, 5):
        angle_diff = abs(target_angles[joint] - current_angles[joint])
        if angle_diff > 0.5:  # Nur bewegen wenn Unterschied > 0.5°
            send_joint_position(joint, target_angles[joint])
            time.sleep(0.1)  # Kurze Pause zwischen Befehlen
    
    # Aktualisiere aktuelle Winkel und Position
    current_angles = target_angles.copy()
    current_tcp = {'x': x, 'y': y, 'z': z}
    
    return True

def move_tcp_relative(dx=0, dy=0, dz=0):
    """Bewegt TCP relativ zur aktuellen Position."""
    new_x = current_tcp['x'] + dx
    new_y = current_tcp['y'] + dy
    new_z = current_tcp['z'] + dz
    
    return move_tcp_to(new_x, new_y, new_z)

def show_status():
    """Zeigt aktuellen Status an."""
    print("\n" + "=" * 60)
    print("ROBOTER STATUS")
    print("=" * 60)
    print(f"TCP Position: X={current_tcp['x']:.1f}, Y={current_tcp['y']:.1f}, Z={current_tcp['z']:.1f} mm")
    print("\nJoint-Winkel:")
    for joint in range(1, 5):
        print(f"  Joint {joint}: {current_angles[joint]:.1f}°")
    print("=" * 60)

def show_help():
    """Zeigt Hilfe-Menü."""
    print("\n" + "=" * 60)
    print("TCP STEUERUNG - BEFEHLE")
    print("=" * 60)
    print("Absolute Bewegung:")
    print("  goto X Y Z     → Fahre zu absoluter Position")
    print("  Beispiel: goto 300 100 200")
    print("")
    print("Relative Bewegung (in mm):")
    print("  up [dist]      → Nach oben (Standard: 10mm)")
    print("  down [dist]    → Nach unten")
    print("  forward [dist] → Nach vorne")
    print("  back [dist]    → Nach hinten")
    print("  left [dist]    → Nach links")
    print("  right [dist]   → Nach rechts")
    print("")
    print("Sonstige Befehle:")
    print("  status         → Zeige aktuellen Status")
    print("  home           → Fahre in Home-Position")
    print("  stop           → Stoppe alle Bewegungen")
    print("  help           → Zeige diese Hilfe")
    print("  exit           → Programm beenden")
    print("=" * 60)

def home_position():
    """Fährt in Home-Position."""
    print("Fahre in Home-Position...")
    home_x = L2 + L3 + L4
    home_y = 0
    home_z = L1
    return move_tcp_to(home_x, home_y, home_z)

def main():
    """Hauptprogramm."""
    global current_angles, current_tcp
    
    print("=" * 60)
    print("TCP STEUERUNG - 4 DOF ROBOTERARM")
    print("=" * 60)
    print(f"Gliedlängen: L1={L1}mm, L2={L2}mm, L3={L3}mm, L4={L4}mm")
    
    # Initialisierung durch Benutzereingabe
    print("\nBitte aktuelle Gelenkwinkel eingeben (in Grad):")
    angles = []
    for joint in range(1, 5):
        while True:
            try:
                angle = float(input(f"  Joint {joint}: "))
                angles.append(angle)
                break
            except ValueError:
                print("Ungültige Eingabe! Bitte numerischen Wert eingeben.")
    
    # Setze aktuelle Winkel und berechne TCP-Position
    current_angles = {
        1: angles[0],
        2: angles[1],
        3: angles[2],
        4: angles[3]
    }
    current_tcp = forward_kinematics(current_angles)
    
    show_status()
    print("\nGeben Sie 'help' für Befehle ein.")
    
    while True:
        try:
            user_input = input("\nTCP> ").strip().lower()
            
            if user_input == 'exit':
                print("Programm wird beendet...")
                break
                
            elif user_input == 'help':
                show_help()
                
            elif user_input == 'status':
                show_status()
                
            elif user_input == 'home':
                home_position()
                
            elif user_input == 'stop':
                ser.write(b'STOP\n')
                print("Stop-Befehl gesendet")
                
            elif user_input.startswith('goto '):
                parts = user_input.split()
                if len(parts) >= 4:
                    try:
                        x = float(parts[1])
                        y = float(parts[2])
                        z = float(parts[3])
                        move_tcp_to(x, y, z)
                    except ValueError:
                        print("Ungültige Koordinaten!")
                else:
                    print("Format: goto X Y Z")
                    
            elif user_input.startswith(('up', 'down', 'forward', 'back', 'left', 'right')):
                parts = user_input.split()
                distance = 10  # Standard-Distanz
                
                if len(parts) > 1:
                    try:
                        distance = float(parts[1])
                    except ValueError:
                        print("Ungültige Distanz, verwende 10mm")
                
                if user_input.startswith('up'):
                    move_tcp_relative(dz=distance)
                elif user_input.startswith('down'):
                    move_tcp_relative(dz=-distance)
                elif user_input.startswith('forward'):
                    move_tcp_relative(dx=distance)
                elif user_input.startswith('back'):
                    move_tcp_relative(dx=-distance)
                elif user_input.startswith('left'):
                    move_tcp_relative(dy=distance)
                elif user_input.startswith('right'):
                    move_tcp_relative(dy=-distance)
                    
            else:
                print("Unbekannter Befehl! Geben Sie 'help' für Hilfe ein.")
                
        except KeyboardInterrupt:
            print("\n\nProgramm unterbrochen!")
            ser.write(b'STOP\n')
            break
        except Exception as e:
            print(f"Fehler: {e}")

# Programm starten
if __name__ == "__main__":
    try:
        print("Starte TCP Control System...")
        print("Verbinde mit Arduino...")
        
        # Starte kontinuierliches Lesen
        read_thread = threading.Thread(target=continuous_read, daemon=True)
        read_thread.start()
        
        time.sleep(1)
        main()
        
    except KeyboardInterrupt:
        print("\nProgramm wird beendet...")
        ser.write(b'STOP\n')
    except Exception as e:
        print(f"Kritischer Fehler: {e}")
    finally:
        running = False
        if ser.is_open:
            ser.close()
        print("Verbindung geschlossen")