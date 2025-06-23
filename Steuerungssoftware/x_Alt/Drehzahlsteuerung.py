import serial
import time

# Setze die seriellen Verbindungsparameter
arduino_port = 'COM4'  # Ersetze dies durch den richtigen COM-Port (bei Linux /dev/ttyUSB0 oder /dev/ttyACM0)
baud_rate = 9600  # Die Baudrate muss mit der des Arduino-Codes übereinstimmen
newPosition=""

# Verbindung zur seriellen Schnittstelle herstellen
ser = serial.Serial(arduino_port, baud_rate, timeout=1)

# Kurze Pause, damit der Arduino Zeit hat, die Verbindung zu initialisieren
time.sleep(2)

def send_data(data):
    """Daten an Arduino senden."""
    ser.write(data.encode())  # Sende Daten als Byte-String
    print(f"Daten gesendet: {data}")

def read_data():
    """Daten vom Arduino empfangen."""
    if ser.in_waiting > 0:  # Überprüfe, ob Daten zur Verfügung stehen
        received_data = ser.readline().decode().strip()  # Empfange und dekodiere die Daten
        print(f"Daten empfangen: {received_data}")
        return received_data
    return None

# Beispiel: Daten senden und empfangen
try:
    while True:
        newPosition=input("Neue Position angeben (in Stepps)")
        send_data(newPosition)
        time.sleep(0.5)

        while True:
             read_data()
             time.sleep(0.5)
             if(read_data()=="Position wurde angefahren"):
                  break
            
        
        time.sleep(0.5)


except KeyboardInterrupt:
        newPosition=0
        send_data(newPosition)
        print("Motor wird angehalten, Verbindung wird beendet.")

finally:
    ser.close()  # Serielle Verbindung schließen
