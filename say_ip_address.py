# For 2.1.4.13
import sys, socket, time
from naoqi import ALProxy


IP = 'localhost'
PORT =  9559
my_ip, my_port = 'NULL', 'NULL'
try:
    tts = ALProxy("ALTextToSpeech", IP, PORT)
except Exception as e:
    print "Error TTS: "
    sys.exit(1)
try:
    connection = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    connection.connect(('10.255.255.255',1))
    my_ip, my_port = connection.getsockname()
except Exception as e:
    print "Error Connection: "

tts.say("My IP Address is: " + my_ip)
time.sleep(3)
tts.say("My port is: " + str(my_port))

