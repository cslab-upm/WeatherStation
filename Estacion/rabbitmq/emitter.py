#!/usr/bin/env python
#coding = utf-8
import sys
def setup_connection():
    import pika
    import config as c
    #credenciales 
    credentials = pika.PlainCredentials(c.user,c.password)
    #conexion
    connection = pika.BlockingConnection(pika.ConnectionParameters(c.urlServer,c.portServer,'/',credentials))
    channel = connection.channel()

    #enviamos por nuestra cola
    channel.queue_declare(queue=c.me,durable=True)
    channel.exchange_declare(exchange=c.me,type='direct')
    return connection

def send(msg,severity="info"):
    channel.basic_publish(exchange = c.me,\
            routing_key=severity,\
            body = msg)
    print("[x] Sent \"%s\"\n\tSeverity: %s"%(msg,severity))
def usg():
    print("""Uso: python emitter.py [-s <severidad>] <mensaje>
Envía <mensaje> por la cola definida como \"me\"en config.py
Si no se especifica severidad, se entiende que es \"info\"
""")

def parse_args(argv):
    import argparse
    parser = argparse.ArgumentParser(description="Envía un mensaje por la cola definida como \"me\" en config.py")

    #Severidad
    parser.add_argument('-s','--severity',type=str,choices=["info","critical"],default="info",help="Severidad del mensaje que se envía")
    #Mensaje
    parser.add_argument('message',nargs='+',help="Mensaje que se quieren enviar")
    args = parser.parse_args(argv)
    return {'severity':args.severity,'message' : ''.join(args.message)}
def cli(argv):
    args = parse_args(argv[1:])
    connection = setup_connection()
    send(args['message'],args['severity'])
    connection.close()
if __name__=='__main__':
    cli(sys.argv)
