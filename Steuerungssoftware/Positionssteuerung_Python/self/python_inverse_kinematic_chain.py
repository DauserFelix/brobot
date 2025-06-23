import serial
import time
import math


def send_data(data):
    ser.write((str(data) + '\n').encode())

def read_data():
    received_data = ser.readline().decode().strip()
    return received_data

def print_from_py(x):
    print("Python-> " +x)

def print_from_ardu(x):
    print("Arduino-> "+x)

def inverse_kinematik(old_x, old_y, old_thetta, new_x, new_y, new_thetta):
    print_from_py("Starte berechnung der Inversen Kinematik")

    l1=154      #mm
    l2=368
    l3=289
    l4=260

    x_1=0
    y_1=0
    x_2=0
    y_2=l1

    #Berechnung TCP zu Joint 4 (OLD)
    x_tcp_old=old_x
    y_tcp_old=old_y

    x_4_old=x_tcp_old-math.cos(old_thetta)*l4
    y_4_old=y_tcp_old+math.sin(old_thetta)*l4

    #x_4_old=old_x
    #y_4_old=old_y

    #Berechnung Joint4 zu Joint 2 (OLD)
    l24_old=math.sqrt(math.pow((x_4_old-x_2),2)+math.pow((y_4_old-y_2),2))
    alpha_groß_old=math.acos((y_4_old-y_2)/l24_old)
    alpha_old=math.acos(((-1)*math.pow(l3,2)+math.pow(l2,2)+math.pow(l24_old,2))/(2*l2*l24_old))
    alpha_2_old=alpha_groß_old-alpha_old

    y_3_old=y_2+math.cos(alpha_2_old)*l2
    x_3_old=x_2+math.sin(180-90-alpha_2_old)*l2

    beta_old=math.acos(((-1)*math.pow(l24_old,2)+math.pow(l3,2)+math.pow(l2,2))/(2*l3*l2))
    gamma_old=math.acos((-math.pow(l2,2)+math.pow(l3,2)+math.pow(l24_old,2))/(2*l3*l24_old))


    #Berechnung TCP zu Joint 4 (OLD)
    x_tcp_new=new_x
    y_tcp_new=new_y

    x_4_new=x_tcp_new-math.cos(new_thetta)*l4
    y_4_new=y_tcp_new+math.sin(new_thetta)*l4

    #x_4_new=new_x
    #y_4_new=new_y

    #Berechnung Joint4 zu Joint 2
    l24_new=math.sqrt(math.pow((x_4_new-x_2),2)+math.pow((y_4_new-y_2),2))
    alpha_groß_new=math.acos((y_4_new-y_2)/l24_new)
    alpha_new=math.acos(((-1)*math.pow(l3,2)+math.pow(l2,2)+math.pow(l24_new,2))/(2*l2*l24_new))
    alpha_2_new=alpha_groß_new-alpha_new

    y_3_new=y_2+math.cos(alpha_2_new)*l2
    x_3_new=x_2+math.sin(180-90-alpha_2_new)*l2

    beta_new=math.acos(((-1)*math.pow(l24_new,2)+math.pow(l3,2)+math.pow(l2,2))/(2*l3*l2))
    gamma_new=math.acos((-math.pow(l2,2)+math.pow(l3,2)+math.pow(l24_new,2))/(2*l3*l24_new))


    #Berechnung der Differenz
    delta_alpha=alpha_2_old-alpha_2_new
    delta_beta=beta_new-beta_old
    

    #joint 2: 32800 Stepps für 360°
    alpha_stepps=(32800/(2*math.pi))*(delta_alpha)        #stepps die der motor jetzt machen muss, um auf die Position zu fahren

    #joint 3:25200 Stepps für 360°
    beta_stepps=(25200/(2*math.pi))*(delta_beta)


    print_from_py("Berechnung beendet, alpha um "+str(delta_alpha) +" rad verfahren und beta um "+str(delta_beta) +" rad verfahren!")
    return alpha_stepps, beta_stepps

    #Hier jetzt noch relative Bewegung zur alten tcp_x und tcp_y berechnen
    #Programmieren, dass:
    # 1. Bei der Initialisierung die X und Y Koordinaten der Aktuellen Position eingegeben werden müssen
    # 2. alte Position gespeichert wird
    # 3. Die Neue Position anhand der Alten angefahren wird
    # 4. Messen, wie Viele schritte pro Gelenk eine Umdrehung sind 
    #       [Joint 1 (rotation): 5500Stepps := 90°]" [Joint 2: 8200Stepps := 90°]" Joint 3:[6300Stepps := 90°]"
    #  5. Arduino gleichzeiteige Bewegung: alle Stepps gleich schnell, nur Schritte der Motoren übergeben (Taktung gleich bzw in
    #  einer Funktion!)


    #   6. Berechnen TCP zu Joint4
    #   7. Quadrantenprüfung (wann + und wann -) zu joint 4 und TCP
    #   8. programmieren dass alle gleichzeitig fahren
    #   9. eventuell threatten (skripte gleichzeitig ablaufen lassen)

    #Differenz der aktuellen


   

    

    print_from_py("Inverse Kinematik berechnet")


# Serielle Verbindungsparameter
arduino_port = 'COM3'  # Passe den Port an
baud_rate = 9600

# Verbindung zur seriellen Schnittstelle herstellen
ser = serial.Serial(arduino_port, baud_rate, timeout=5)
time.sleep(2)  # Arduino Zeit zum Initialisieren geben

#main
print('----Brobot-Controll----\n Modus Wählen (automatic "a" or manuell "m") ')
inputdata=input()
send_data(inputdata)

data=""
stepindex=0

while(True):
    data=read_data()
    print_from_ardu(data)
    
    if(data=="end./"):
        stepindex=0
        break

    elif(data=="manuell_mode"):
        print_from_py("Manueller Modus aktiviert")
        stepindex=1

    elif(data=="end_of_manuell_motion"):
        print_from_py("Bewegung abegschlossen!\n")
        stepindex=1

    elif(data=="automatic_mode"):
        print_from_py("Automatikmodus aktiviert")
        stepindex=2

    elif(data=="end_of_automatic_motion"):
        print_from_py("Bewegung abegschlossen!\n")
        stepindex=2

    else:
        print_from_py("Error in the communication!")
        print_from_ardu(data)
        break


    print_from_py("current step in chain:" +str(stepindex))

    if(stepindex==1):       #manuell mode
        print_from_py(" Joint: ")
        joint_from_terminal=input()
        send_data(joint_from_terminal)
        print_from_py(" Stepps:")
        stepps_from_terminal=input()
        send_data(stepps_from_terminal)


    elif(stepindex==2):
        print_from_py("Alte absolute X-koordinate eingeben (ausmessen)")
        old_x_from_terminal=input()
        
        print_from_py("Alte absolute Y-koordinate eingeben (ausmessen)")
        old_y_from_terminal=input()

        print_from_py("Thetta eingeben (pos_winkelzaehlung_ab_Befestignug)")
        old_thetta_from_terminal=input()


        print_from_py("Neue absolute X-koordinate eingeben")
        x_from_terminal=input()
        
        print_from_py("Neue absolute Y-koordinate eingeben")
        y_from_terminal=input()

        print_from_py("Thetta eingeben (pos_winkelzaehlung_ab_Befestignug)")
        new_thetta_from_terminal=input()


        alpha_stepps, beta_stepps=inverse_kinematik(int(old_x_from_terminal),int(old_y_from_terminal),int(old_thetta_from_terminal),int(x_from_terminal),int(y_from_terminal), int(new_thetta_from_terminal))      #UNBEDINGT noch 0,0 zu x_old und y_old anpassen!

        print_from_py("AlphaStepps:" +str(alpha_stepps)+", BetaStepps: "+str(beta_stepps))
        #print_from_ardu(read_data(), read_data())


        send_data(str(alpha_stepps))
        time.sleep(2)
        send_data(str(beta_stepps))


print_from_py("End of communication")