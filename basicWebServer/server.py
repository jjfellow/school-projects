#Justin Fellows
#ID: 1001865403

import socket
from threading import Thread, Lock

# Host IP and Port number used by the server. Change as needed
HOST = "127.0.0.1"
PORT = 8080


# Main function definition for the Server Thread. It takes a connection object from the main thread
# and will handle the request coming through said connection
# NOTE: the connection is not explicitly closed because the with: statement inherintly handles that
# Server thread retains ownership of its client_connection
def serverThread(client_connection, client_address):
	with client_connection:
		request = client_connection.recv(4096).decode()
		# With this tokenization scheme, token[0] is the request type, GET, PUT, etc
		# And token[1] is the requested resource
		tokens = request.split(' ' or '\n' or '/')
		reqType = tokens[0]
		# This replace method is used to remove the leading / from the requested resource
		# Sometimes, empty requests come in, so tokens[1] does not exist
		# Python catches these so they don't do any harm, but are annoying to see in the terminal
		reqResource = ""
		try:
			reqResource = tokens[1].replace('/','', 1)
		except IndexError:
			reqResource = ""
		#Compose and send a reply
		parseRequest(reqType, reqResource, client_connection)

# This does the bulk of the processing. It takes in the type of request, the requested resource,
# as well as the connection object. It uses these to determine what type of request is being made,
# and to send the appropriate response to the client
def parseRequest(reqType, resource, connection):
	if reqType == "GET":
		# If the requested resource is empty, like if you typed http://localhost:8080 into a browser search bar,
		# display the index.html page
		if resource == "":
			file = open("index.html", 'r')
			payload = file.read()
			file.close()
			length = len(payload)
			# Composing the HTTP response, it incorporates the payload text as well as the length of the payload
			message = f"HTTP/1.1 200 OK\nContent-Length: {length}\nContent-Type: text/html\nConnection: Closed\n\n{payload}"
			connection.sendall(message.encode())
		# This is the hardcoded 301 Moved Permanently section
		elif resource == "page1.html":
			message = f"HTTP/1.1 301 Moved Permanently\nLocation: http://{HOST}:{PORT}/page2.html"
			connection.sendall(message.encode())
		else:
			# This large try block is used to handle requested resources that don't exist
			# If any exception is raised while trying to handle/open a file, treat it as
			# if the file does not exist and send the client a 404 Not Found message
			try:
				# Need to check what type of data is being requested, as different types need to be sent differently
				# This implementation can only handle images of .webp format or .html files

				# To begin, split the requested resource into filename and file extension
				# As an aside, this implementation does not support files without extensions
				newTokens = resource.split('.')
				extension = newTokens[1]
				# If the requested file is the supported image type, then the file needs to be open as a binary file
				if extension == "webp":
					file = open(resource, 'rb')
					payload = file.read()
					file.close()
					length = len(payload)
					# conType is short for Content Type, it goes into one of the header fields for the HTTP message
					# And is used by the client browser to interpret the byte stream
					conType = "image/webp"
					message = f"HTTP/1.1 200 OK\nContent-Length: {length}\nContent-Type: {conType}\nConnection: Closed\n\n"
					connection.sendall(message.encode())
					# Images don't take kindly to being appended to the end of strings, so just immediately send the image afterwards
					# Also note, no encoding needed, as payload is already a bytes object
					connection.sendall(payload)
				else:
					# Else, if the requested resource is not an image, treat it like a text file
					file = open(resource, 'r')
					payload = file.read()
					file.close()
					length = len(payload)
					conType = "text/html"
					message = f"HTTP/1.1 200 OK\nContent-Length: {length}\nContent-Type: {conType}\nConnection: Closed\n\n{payload}"
					connection.sendall(message.encode())
			except Exception:
				message = "HTTP/1.1 404 Not Found\nConnection: Closed\n\n"
				connection.sendall(message.encode())

# This is the "main method", in Java/C speak
# Its the main thread that listens on PORT and accepts incoming connections, then spins up
# a new serverThread to handle the incoming request
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
	sock.bind((HOST, PORT))
	while True:
		sock.listen()
		conn, addr = sock.accept()
		newThread = Thread(target = serverThread, args = (conn, addr))
		newThread.start()