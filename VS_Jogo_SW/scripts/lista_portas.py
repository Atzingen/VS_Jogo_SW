# -*- coding: latin-1 -*-
'''
    Programa que lista as portas "seriais" conectadas
    ao computador e salva elas em um arquivo txt com nome
    lista_seriais.txt, sendo que acada linha contém a string
    com o nome da porta serial

    02/11/2016
    Gustavo Voltani von Atzingen
'''

import serial.tools.list_ports as verificador

with open('lista_seriais.txt','w') as f:
    dispositivos = verificador.comports()
    for dispositivo in dispositivos:
        f.write(dispositivo.device + '\n')
f.close()
