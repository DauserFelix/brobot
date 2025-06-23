import serial
import time
import threading
import queue

## Joint-Bestätigungscodes
#Code1 = "→ Joint 1 wird angesteuert [5500Stepps := 90°]"
#Code2 = "→ Joint 2 wird angesteuert [8000Stepps := 90°]"
#Code3 = "→ Joint 3 wird angesteuert [5000Stepps := 90°]"

# Fehlercodes - synchronisiert mit Arduino
Fehlercode1 = "Code1: Position wurde angefahren"
Fehlercode2 = "Code2: Vorgang wurde abgebrochen"
Fehlercode3 = "Code3: Motor ist einsatzbereit!"
Fehlercode4 = "Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet"

# Joint-Bestätigungscodes
Code1 = "Arduino: Joint 1 wird angesteuert"
Code2 = "Arduino: Joint 2 wird angesteuert"
Code3 = "Arduino: Joint 3 wird angesteuert"

# Message Queue für Thread-sichere Kommunikation
message_queue = queue.Queue()
running = True

# Serielle Verbindungsparameter
arduino_port = 'COM3'  # Passe den Port an
baud_rate = 9600

# Verbindung zur seriellen Schnittstelle herstellen
ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)  # Arduino Zeit zum Initialisieren geben

# Globale Variablen
current_joint = 1  # Aktuell ausgewähltes Joint

def send_data(data):
    """Daten an Arduino senden."""
    ser.flushInput()   # Eingangspuffer leeren
    ser.flushOutput()  # Ausgangspuffer leeren
    if isinstance(data, int):
        # Für Joint-Auswahl: Sende als Byte
        ser.write(bytes([data]))
    else:
        # Für normale Befehle: Sende als String
        ser.write((str(data) + '\n').encode())
    print(f"→ Befehl gesendet: {data}")

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

def clear_buffers():
    """Alle seriellen Puffer leeren."""
    ser.flushInput()
    ser.flushOutput()
    time.sleep(0.1)
    while ser.in_waiting > 0:
        ser.read()
    # Queue auch leeren
    while not message_queue.empty():
        try:
            message_queue.get_nowait()
        except queue.Empty:
            break

def select_joint():
    """Joint-Auswahl am Programmstart."""
    global current_joint
    
    print("=" * 50)
    print("ROBOT JOINT AUSWAHL")
    print("=" * 50)
    print("Verfügbare Joints:")
    print("1 - Joint 1 (Pins 8,9,10)")
    print("2 - Joint 2 (Pins 11,12,13)")
    print("3 - Joint 3 (Pins 4,5,6)")
    print("=" * 50)
    
    while True:
        try:
            user_input = input("\nWelches Joint möchten Sie ansteuern? (1, 2 oder 3): ").strip()
            
            if user_input == '1':
                current_joint = 1
                send_data(1)  # Sende Byte-Wert 1 für Joint 1
                break
            elif user_input == '2':
                current_joint = 2
                send_data(2)  # Sende Byte-Wert 2 für Joint 2
                break
            elif user_input == '3':
                current_joint = 3
                send_data(3)  # Sende Byte-Wert 3 für Joint 3
                break
            else:
                print("Ungültige Eingabe! Bitte '1', '2' oder '3' eingeben.")
                
        except KeyboardInterrupt:
            print("\nProgramm unterbrochen!")
            return False
    
    # Warte auf Bestätigung vom Arduino
    print(f"Warte auf Bestätigung für Joint {current_joint}...")
    timeout_start = time.time()
    
    while (time.time() - timeout_start) < 5:
        try:
            message = message_queue.get(timeout=0.5)
            
            if ((current_joint == 1 and message == Code1) or 
                (current_joint == 2 and message == Code2) or 
                (current_joint == 3 and message == Code3)):
                print(f" {message}")
                print(f" Joint {current_joint} erfolgreich ausgewählt!")
                return True
            else:
                print(f"Arduino: {message}")
                
        except queue.Empty:
            continue
    
    print(" Timeout - Keine Bestätigung vom Arduino erhalten")
    return True  # Trotzdem fortfahren

def change_joint():
    """Joint während der Laufzeit wechseln."""
    global current_joint
    
    print(f"\nAktuelles Joint: {current_joint}")
    print("Verfügbare Joints: 1, 2, 3")
    
    try:
        new_joint = input("Neues Joint auswählen (1, 2 oder 3, Enter zum Abbrechen): ").strip()
        
        if new_joint == '':
            return
        elif new_joint == '1':
            current_joint = 1
            send_data(1)
        elif new_joint == '2':
            current_joint = 2
            send_data(2)
        elif new_joint == '3':
            current_joint = 3
            send_data(3)
        else:
            print("Ungültige Eingabe!")
            return
            
        # Warte auf Bestätigung
        print(f"Wechsle zu Joint {current_joint}...")
        timeout_start = time.time()
        
        while (time.time() - timeout_start) < 3:
            try:
                message = message_queue.get(timeout=0.5)
                if ((current_joint == 1 and message == Code1) or 
                    (current_joint == 2 and message == Code2) or 
                    (current_joint == 3 and message == Code3)):
                    print(f" {message}")
                    return
                else:
                    print(f"Arduino: {message}")
            except queue.Empty:
                continue
                
        print(" Timeout beim Joint-Wechsel")
        
    except Exception as e:
        print(f"Fehler beim Joint-Wechsel: {e}")

def show_joint_info():
    """Zeigt Informationen über alle Joints an."""
    print("\n" + "=" * 50)
    print("JOINT ÜBERSICHT")
    print("=" * 50)
    print("Joint 1: Pins 8 (STEP), 9 (DIR), 10 (ENA)")
    print("Joint 2: Pins 11 (STEP), 12 (DIR), 13 (ENA)")
    print("Joint 3: Pins 4 (STEP), 5 (DIR), 6 (ENA)")
    print("=" * 50)
    print(f"Aktuell aktiv: Joint {current_joint}")
    print("=" * 50)

def main():
    """Hauptprogramm für Positionssteuerung."""
    
    # Joint-Auswahl am Start
    if not select_joint():
        return
    
    time.sleep(1)
    clear_buffers()
    
    print("\n" + "=" * 50)
    print("POSITIONSSTEUERUNG - 3 JOINT SYSTEM")
    print("=" * 50)
    print("• Position         → Fahre zu Position")
    print("• Stop             → Bewegung stoppen") 
    print("• Go               → Motor aktivieren")
    print("• joint            → Joint wechseln")
    print("• info             → Joint-Übersicht anzeigen")
    print("• exit             → Programm beenden")
    print("=" * 50)
    print(f"Aktuelles Joint: {current_joint}")
    
    while True:
        try:
            user_input = input(f"\n[Joint {current_joint}] Position eingeben: ").strip()
            
            if user_input.lower() == 'exit':
                print("Programm wird beendet...")
                break
            elif user_input.lower() == 'joint':
                change_joint()
                continue
            elif user_input.lower() == 'info':
                show_joint_info()
                continue
            
            # Befehl senden
            send_data(user_input)
            
            # Auf Antwort warten
            print("Warte auf Arduino-Antwort...")
            timeout_start = time.time()
            response_received = False
            
            while not response_received and (time.time() - timeout_start) < 15:
                try:
                    message = message_queue.get(timeout=0.5)
                    
                    # Prüfe auf spezielle Fehlercodes
                    if message == Fehlercode1:
                        print(f" {message}")
                        response_received = True
                    elif message == Fehlercode2:
                        print(f" {message}")
                        response_received = True
                    elif message == Fehlercode3:
                        print(f" {message}")
                        response_received = True
                    elif message == Fehlercode4:
                        print(f" {message}")
                        print(" Motor mit 'Go' aktivieren!")
                        response_received = True
                    else:
                        # Normale Arduino-Nachricht
                        print(f"Arduino: {message}")
                
                except queue.Empty:
                    continue
            
            if not response_received:
                print(" Timeout - Arduino antwortet nicht!")
    
        except KeyboardInterrupt:
            print("\n Programm unterbrochen!")
            send_data("Stop")
            break
        except Exception as e:
            print(f" Fehler: {e}")

# Programm starten
if __name__ == "__main__":
    try:
        print("Starte Robot Control System...")
        print("Verbinde mit Arduino...")
        
        # Starte kontinuierliches Lesen in separatem Thread
        read_thread = threading.Thread(target=continuous_read, daemon=True)
        read_thread.start()
        
        # Kurz warten für erste Arduino-Nachrichten
        time.sleep(1)
        
        # Hauptprogramm starten
        main()
        
    except KeyboardInterrupt:
        print("\n Programm wird beendet...")
        send_data("Stop")
    except Exception as e:
        print(f" Kritischer Fehler: {e}")
    finally:
        running = False
        if ser.is_open:
            ser.close()
        print(" Verbindung geschlossen")
