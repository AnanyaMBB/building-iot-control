import threading
import subprocess
import netifaces
import socket
import re
import sys
import json
import requests
import time

import paho.mqtt.client as paho

broker = '146.190.138.255'
port = 1883
PORT = 8080
# PORT = 80
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
gw = netifaces.gateways()

# ip = gw['default'][netifaces.AF_INET][0]
ip = '192.168.137.1'
ip = ip.split(".")
ip.pop()
ip = ".".join(ip)

ESP_Devices = []

FAN_HOSTS = {

}

DUCT_HOSTS = {

}

fanModels = {}
ductModels = {}
# allModels = {}
toConnect = {}
connectionsDict = {}
ductIPDict = {}

# FAN_API_URL = 'http://localhost:8000/fans'
# DUCT_API_URL = 'http://localhost:8000/ducts'

# FAN_API_URL = 'https://remote-full-controller.du.r.appspot.com/fans'
# DUCT_API_URL = 'https://remote-full-controller.du.r.appspot.com/ducts'

FAN_API_URL = 'https://lionfish-app-pi973.ondigitalocean.app/fans'
DUCT_API_URL = 'https://lionfish-app-pi973.ondigitalocean.app/ducts'
DUCT_POS_API_URL = 'https://lionfish-app-pi973.ondigitalocean.app/ductsPosition'

SEMI_OPEN_LENGTH = 1000
OPEN_LENGTH = 7500


def getFanModels():
    # global allModels
    global fanModels
    response = requests.get(FAN_API_URL)
    items = json.loads(response.text)
    # allModels = {item['model']: item['btnID'] for item in items}
    fanModels = {item['model']: item['btnID'] for item in items}
    print(fanModels)


def getDuctModels():
    # global allModels
    global ductModels
    response = requests.get(DUCT_API_URL)
    items = json.loads(response.text)
    # allModels = {item['btnID']: item['model'] for item in items}
    # allModels = {item['model']: item['btnID'] for item in items}
    ductModels = {item['model']: item['btnID'] for item in items}

    print(ductModels)


getFanModels()
getDuctModels()


def scan(start, end):
    global ip
    global toConnect
    global ductIPDict

    pattern = r'ESP_\w+'
    pattern2 = r'esp-\S+'

    for i in range(start, end):
        test_ip = ip + f".{i}"
        print(f'Testing {test_ip}')

        try:
            response = subprocess.check_output(
                ['ping', '-a', test_ip, '-n', '1'], encoding='utf-8')
            if 'ESP' in response:
                s1.connect((test_ip, PORT))
                print('Connected')
                # a, b = re.search(pattern, response).span()
                # deviceModel = response[a:b]

                deviceModel = response.split(' ')[1].split('.')[0]
                print(f"Found device model {deviceModel}")

                # if deviceModel + '@1' in allModels.keys() or deviceModel + '@2' in allModels.keys():
                #     toConnect[allModels[deviceModel]] = (test_ip, PORT)
                if deviceModel + '@1' in fanModels.keys():
                    toConnect[fanModels[deviceModel+'@1']] = (test_ip, PORT)
                elif deviceModel + '@2' in fanModels.keys():
                    toConnect[fanModels[deviceModel+'@2']] = (test_ip, PORT)
        except Exception as ex:
            pass

        try:
            print(f"Duct reponse: {response}")
            if 'esp' in response:
                s2.connect((test_ip, 80))
                print(f"Connected: {s2}")
                # matched = re.search(pattern2, response)
                # if matched:
                #     a, b = matched.span()
                # else:
                #     print("No match")
                # a, b = re.search(pattern2, response).span()
                # deviceModel = response[a:b]
                deviceModel = response.split(' ')[1].split('.')[0]
                print(f"Found device model {deviceModel}")
                print(f"Duct model keys: {ductModels.keys()}")
                if deviceModel + '@1' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@1']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@2']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@3']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@4']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@5']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@6']] = test_ip
                elif deviceModel + '@2' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@2']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@1']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@3']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@4']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@5']] = test_ip
                    ductIPDict[ductModels[deviceModel + '@6']] = test_ip
                elif deviceModel + '@3' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@3']] = test_ip
                elif deviceModel + '@4' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@4']] = test_ip
                elif deviceModel + '@5' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@5']] = test_ip
                elif deviceModel + '@6' in ductModels.keys():
                    ductIPDict[ductModels[deviceModel + '@6']] = test_ip

        except Exception as ex:
            # print("Exception at duct", ex)
            pass


# for key, value in allModels.items():
#     connectionsDict[key] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     connectionsDict[key].connect(toConnect[key])

# Run the scanning function in a thread
threads = [threading.Thread(target=scan, args=(i-3, i))
           for i in range(3, 256, 3)]

for thread in threads:
    thread.start()

for thread in threads:
    thread.join()

print("================================")
for key, value in fanModels.items():
    try:
        print(f"Value: {value}")
        print(f"Key: {key}")
        connectionsDict[value] = socket.socket(
            socket.AF_INET, socket.SOCK_STREAM)
        print(f"To connect: {toConnect}")
        connectionsDict[value].connect(toConnect[value])
    except Exception as ex:
        print("Exception: ", ex)
        pass
print("================================")
print(f'Connections: {connectionsDict}')


def sendFanCommand(payload, openCommand1, openCommand2, closeCommand1, closeCommand2):
    try:
        for item in list(payload.values())[0].keys():
            print(f"===>{item}")
            if int(item.split('_')[-1]) % 2 != 0:
                closeData = bytes.fromhex(closeCommand1)
                print(connectionsDict[item])
                connectionsDict[item].sendall(closeData)
                print("command 1 sent")
                time.sleep(1)
                openData = bytes.fromhex(openCommand1)
                connectionsDict[item].sendall(openData)
                print("command 2 sent!")
            else:
                closeData = bytes.fromhex(closeCommand2)
                connectionsDict[item].sendall(closeData)
                print("command 1 sent")
                time.sleep(1)
                openData = bytes.fromhex(openCommand2)
                connectionsDict[item].sendall(openData)
                print("command 2 sent!")
    except Exception as ex:
        print(f"Exception: {ex}")
        pass


print(ductIPDict)


def sendDuctCommand(payload, command):
    command = ""
    try:
        axis = ('X', 'Y', 'Z', 'A', 'B', 'C')

        # Construct grbl command
        commandType = list(payload.keys())[0]
        previousFloor = int(
            list(list(payload.values())[0].keys())[0].split('_')[1])
        previousItem = None
        # command = "G1"
        for item in list(payload.values())[0].keys():
            currentFloor = int(item.split('_')[1])
            axis = int(item.split('_')[-1])

            if previousFloor != currentFloor:
                # command += ' F100'
                print(f"Final Command 1 {command}")
                # r = requests.get(
                #     f'http://{ductIPDict[previousItem]}/command?commandText={command}&PAGEID=0')
                r = requests.get(
                    f'http://{ductIPDict[previousItem]}/command?cmd={command}')
                print(f"sent Command: {command}")
                # command = "G1"
                command = ""

            if axis == 1:
                if commandType == 'semiOpen':
                    command += f' X{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' X{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' X-{OPEN_LENGTH}'

            elif axis == 2:
                if commandType == 'semiOpen':
                    command += f' Y{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' Y{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' Y-{OPEN_LENGTH}'

            elif axis == 3:
                if commandType == 'semiOpen':
                    command += f' Z{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' Z{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' Z-{OPEN_LENGTH}'

            elif axis == 4:
                if commandType == 'semiOpen':
                    command += f' A{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' A{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' A-{OPEN_LENGTH}'

            elif axis == 5:
                if commandType == 'semiOpen':
                    command += f' B{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' B{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' B-{OPEN_LENGTH}'

            elif axis == 6:
                if commandType == 'semiOpen':
                    command += f' C{SEMI_OPEN_LENGTH}'
                elif commandType == 'open':
                    command += f' C{OPEN_LENGTH}'
                elif commandType == 'close':
                    command += f' C-{OPEN_LENGTH}'

            previousItem = item
            previousFloor = currentFloor
        # command += ' F100'
        print(f"Final Command 2 {command}")
        # r = requests.get(
        #     f'http://{ductIPDict[previousItem]}/command?commandText={command}&PAGEID=0')
        print(f"==>{previousItem}")
        print(f"==>{ductIPDict}")
        print(f"==>{ductIPDict[previousItem]}")
        r = requests.get(
            f'http://{ductIPDict[previousItem]}/command?cmd={command}')
        print(f" Request response: {r}")
        print(f"sent Command 2: {command}")
        # command = "G1"
        command = ""
    except Exception as ex:
        print(ex)


def message(client, user_data, payload):
    payload = payload.payload.decode()
    payload = json.loads(payload)
    print(f"Command Received: ", payload)
    print(f"Current ducts: {ductIPDict}")

    command = list(payload.keys())[0]

    if command == 'supply':
        print("Called command")
        sendFanCommand(payload, 'A00101A2', 'A00301A4', 'A00200A2', 'A00400A4')
    elif command == 'exhaust':
        sendFanCommand(payload, 'A00201A3', 'A00401A5', 'A00100A1', 'A00300A3')
    elif command == 'off':
        sendFanCommand(payload, 'A00100A1', 'A00200A2', 'A00300A3', 'A00400A4')
    # elif command == 'fanStatus':
    #     sendCommand(payload, 'A00101A2', 'A00301A4', 'A00200A2', 'A00400A4')
    elif command == 'semiOpen':
        # http://192.168.137.228/command?commandText=dsds&PAGEID=0
        sendDuctCommand(payload, '$J=G21G91X0.8F102\r\n')
    elif command == 'open':
        sendDuctCommand(payload, '$J=G21G91X17.0F102\r\n')
    elif command == 'close':
        sendDuctCommand(payload, '$J=G21G91X-17.0F102\r\n')
    elif command == 'ductStatus':
        pass


client = paho.Client()
client.on_message = message

if client.connect(broker, port, 60) != 0:
    print('Connection Error')

client.subscribe('command')
client.loop_forever()
client.disconnect()
