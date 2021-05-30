""" Server Malha de Controle UDP"""

import socket
import struct

UDP_IP = "192.168.0.103"
UDP_PORT = 9876

UDP_IP_ARD = "192.168.0.102"
UDP_PORT_ARD = 9876
server_address_ARD = (UDP_IP_ARD, UDP_PORT_ARD)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = (UDP_IP, UDP_PORT)
sock.bind(server_address)


print "Server is open on port", UDP_PORT, "\n"

while True:
	data, addr = sock.recvfrom(4)
	if len(data) < 4:
		pwm = 230
	else:
		value = struct.unpack('f', data)
		pwm = value[0]
		pwm = pwm*255/35
	if pwm > 255:
		pwm = 255
	elif pwm < 0:
		pwm = 0
	
	potencia = (pwm/255)*100
	
	print "Distancia do Sensor:", value[0]
	print "Potencia do Cooler:", potencia, "%  PWM:", pwm, "\n"
	
	pwmpack = struct.pack('f', pwm)

	
	sock1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock1.sendto(pwmpack, server_address_ARD)