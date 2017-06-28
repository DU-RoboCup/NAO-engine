import socket, sys

print("Args: {}".format(sys.argv))
cmd_args = sys.argv
if(len(cmd_args) != 3):
    print("usage: udpclient <message> <port>")
    sys.exit(0)
message = str(cmd_args[1])
port = int(cmd_args[2])
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_addr = ('localhost', port)
try:
    sock.sendto(message.encode('utf-8'), server_addr)
    print("Message sent")
    recv = sock.recvfrom(1024)
    print("Received Message: {}".format(recv))

except Exception as e:
    print("Error: {}".format(e))
finally:
    print("closing connection!")
    sock.close()
