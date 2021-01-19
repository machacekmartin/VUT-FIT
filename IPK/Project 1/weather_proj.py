#!/usr/bin/env python3
#-*- coding: utf-8 -*-

#   ---------------------------------
#     IPK projekt c.1
#     Varianta 2.  (OpenWeatherMap)
#     Autor 	Martin Machacek
#           	xmacha73
#   ---------------------------------

import sys
import json
import socket

def accessServerGetData(s, p, req):
    """ 
    Access the server and fetches data from the server
    Socket libray needed
    :param s: server to connect to
    :param p: port of the server in param s
    :param req: request for the server (GET / ...)
    :return: received data from the request
    """

    loadedSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try: loadedSocket.connect((s, p))  
    except:
        print("Could not connect to the server")
        sys.exit(1)

    loadedSocket.sendall(req)
    try: data = (loadedSocket.recv(1024)).decode('utf-8') 
    except: 
        print("Could not decode the received data")
        sys.exit(1) 

    return data.split("\r\n\r\n")


def checkJsonAndPrint(data):
    """
    Checks server response, splits data into headers and data + prints final information.
    Json library needed.
    :param data: data originaly received from server
    """

    headers = data[0]
    jsonData = json.loads(data[1])
    status = headers.split('\n', 1)[0].split(' ')[1]

    if status != "200":
        if (jsonData.get('message')):
            print(jsonData['message'].capitalize())
            sys.exit(1)
        else:
            print("Error occured: " + status)
            sys.exit(1)
    else:
        try: print(jsonData['name'] + ', ' + jsonData['sys']['country']) 
        except: print('Name: N/A')
        
        try: print(jsonData['weather'][0]['description'].capitalize())
        except: print('N/A')

        try: print('Temp:\t\t' + str(jsonData['main']['temp']) + '°C')
        except: print('Temp:\tN/A')

        try: print('Humidity:\t' + str(jsonData['main']['humidity']) + '%')
        except: print('Humidity:\tN/A')
        
        try: print('Pressure:\t' + str(jsonData['main']['pressure']) + ' hPa')
        except: print('Pressure:\nN/A')
        
        try: print('Wind speed:\t' + str(round(jsonData['wind']['speed']) * 3.6) + ' km/h')
        except: print('Wind speed:\tN/A')
        
        try: print('Wind degree:\t' + str(jsonData['wind']['deg']) + '°')
        except: print('Wind degree:\tN/A')

def main():

    if len(sys.argv) != 3:
        print("Invalid Input Arguments")
        sys.exit(1)

    apiKey = sys.argv[1]
    city = sys.argv[2]

    url = '/data/2.5/weather?appid={}&q={}&units=metric'.format(apiKey, city)
    server = 'api.openweathermap.org'
    port = 80

    request = "GET " + url + " HTTP/1.1\nHost:" + server + "\r\n\r\n"
    try: request = request.encode()
    except:
        print("Could not encode the information")
        sys.exit(1)

    weatherData = accessServerGetData(server, port, request)
    checkJsonAndPrint(weatherData)   

main()