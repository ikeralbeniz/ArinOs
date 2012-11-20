import socket
import simplejson
import os

class ComandParser:
	
	def __init__(self):
		args = None
		self.avaliable_params = {
			'list_dir': self.list_dir
		}

	def run(self,string_value):
		self.params = eval(string_value)

		if self.params['comand'] in self.avaliable_params.keys():
			return self.avaliable_params[self.params['comand']](self.params['args'])
		else:
			return {
			  	'status':'fail',
			  	'error':{
			  		'code':500,
			  		'message':'Unavaliable comand'
			  	}
			  }


	def list_dir(self,args):
		try:
			dirname = args[0]
			return {
			  	'status':'success',
			  	'data':os.listdir(dirname)
			  	}
		except Exception,e:
			return {
			  	'status':'fail',
			  	'error':{
			  		'code':505,
			  		'message':str(e)
			  	}
			  }



# crea un socket INET de tipo STREAM
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# asocia el socket a un host
# y aun puesto bien conocido
serversocket.bind(('localhost', 4102))

# lo convierte en socket servidor
serversocket.listen(5)

while True:
  channel, details = serversocket.accept()
  channel.setblocking( 1 )
  recvData = channel.recv(2000)
  print "-------------- New Command -----------"
  aux = eval(recvData)
  print "\tCommand: "+str(aux['comand'])
  print "\tArguments: "+str(aux['args'])
  parse = ComandParser()
  response = parse.run(recvData)
  channel.send(simplejson.dumps(response))
  print "\tResponse: "+str(response)
  print ""
  channel.close()
