import serial
import time

#globale Variablen
newPosition=""
Fehlercode1="Code1: Position wurde angefahren"                          #nicht abändern, sind synchronisiert mit Arduino Code
Fehlercode2="Code2: Vorgang wurde abgebrochen"                          #nicht abändern, sind synchronisiert mit Arduino Code
Fehlercode3="Code3: Motor ist einsatzbereit!"                           #nicht abändern, sind synchronisiert mit Arduino Code
Fehlercode4="Code4: Fehlerhafte Eingabe. Motor wurde abgeschaltet"      #nicht abändern, sind synchronisiert mit Arduino Code


# Setze die seriellen Verbindungsparameter
arduino_port = 'COM3'  # Ersetze dies durch den richtigen COM-Port (bei Linux /dev/ttyUSB0 oder /dev/ttyACM0)
baud_rate = 9600  # Die Baudrate muss mit der des Arduino-Codes übereinstimmen


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
        #print(f"Daten empfangen von Arduino: {received_data}")
        return received_data
    return None


try:    # Beispiel: Daten senden und empfangen
    while True:
        newPosition=input("     Absolute Position in Stepps angeben [Stop/Go]: ")
        send_data(newPosition)       

        while True:
            currentMessage =read_data()

            if(currentMessage==Fehlercode1):      #Function readData ausführen und Rückgabewert überprüfen
                print(currentMessage+": "+newPosition)
                break
            elif(currentMessage==Fehlercode2):
                print(currentMessage)
                break
            elif(currentMessage==Fehlercode3):
                print(currentMessage)
                break
            elif(currentMessage==Fehlercode4):   
                print(currentMessage+": Bitte Stromzufuhr mit 'Go' freigeben!")
                break
            #time.sleep(0.5)


            


except KeyboardInterrupt:
        newPosition=0
        send_data(newPosition)
        print("Motor wird angehalten, Verbindung wird beendet.")

finally:
    ser.close()  # Serielle Verbindung schließen
