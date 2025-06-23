import serial
import time
import threading
import queue

# Fehlercodes - synchronisiert mit Arduino
Fehlercode1 = "Code1: Position wurde angefahren"
Fehlercode2 = "Code2: Vorgang wurde abgebrochen"
Fehlercode3 = "Code3: Motor ist einsatzbereit!"
Fehlercode4 = "Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet"

# Message Queue für Thread-sichere Kommunikation
message_queue = queue.Queue()
running = True

# Serielle Verbindungsparameter
arduino_port = 'COM3'  # Passe den Port an
baud_rate = 9600

# Verbindung zur seriellen Schnittstelle herstellen
ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)  # Arduino Zeit zum Initialisieren geben

def send_data(data):
    """Daten an Arduino senden."""
    ser.flushInput()   # Eingangspuffer leeren
    ser.flushOutput()  # Ausgangspuffer leeren
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
    #print("Puffer geleert")

def main():
    """Hauptprogramm - Einfache Positionssteuerung."""
    print("=" * 50)
    print("• Position         → Fahre zu Position")
    print("• Stop             → Bewegung stoppen") 
    print("• Go               → Motor aktivieren")
    print("=" * 50)
    
    # Puffer initial leeren
    clear_buffers()
    
    while True:
        try:
            user_input = input("\nPosition eingeben: ").strip()
            
            if user_input.lower() == 'exit':
                print("Programm wird beendet...")
                break
            elif user_input.lower() == 'clear':
                clear_buffers()
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
                        print("Motor mit 'Go' aktivieren!")
                        response_received = True
                    else:
                        # Normale Arduino-Nachricht
                        print(f" Arduino: {message}")
                        # Bei normalen Nachrichten nicht als "response_received" markieren
                        # außer es ist offensichtlich eine Bestätigung
                        if any(keyword in message.lower() for keyword in ['gesetzt', 'geändert', 'aktiviert', 'deaktiviert', 'durchgeführt']):
                            response_received = True
                
                except queue.Empty:
                    # Kein Timeout-Fehler, einfach weiter warten
                    continue
            
            if not response_received:
                print(" Timeout - Arduino antwortet nicht!")
                print(" Versuche 'clear' oder 'Stop'")
    
        except KeyboardInterrupt:
            print("\n Programm unterbrochen!")
            send_data("Stop")
            break
        except Exception as e:
            print(f" Fehler: {e}")

# Programm starten
try:
    # Starte kontinuierliches Lesen in separatem Thread
    read_thread = threading.Thread(target=continuous_read, daemon=True)
    read_thread.start()
    
    # Hauptprogramm starten
    main()
    
except KeyboardInterrupt:
    print("\n Programm wird beendet...")
    send_data("Stop")
finally:
    running = False
    ser.close()
    print("Verbindung geschlossen")