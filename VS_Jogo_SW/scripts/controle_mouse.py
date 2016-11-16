# -*- coding: latin-1 -*-
'''
    Programa auxiliar para o jogo do sabre de luz star wars (disciplina de
    jogos), feita em c++ com allegro

    Este  programa controla o mouse na tela, com dados recebidos pela porta
    serial. Os funciona independente de quem envia os dados (arduino+acelerometro
    via cabo usb-serial / bluetooh, acelerometro-android via bluetooh ),
    assumindo que estes cheguem via serial com as seguintes informações:
        * baudrate: 9600
        * paridade: None
        * bit de parada: 1
        * Assincrono

    A porta serial escolhida é obtida pelo arquivo "serial_escolhida.txt"

    Formato dos dados:
        "valor1,valor2,valor3'\r\n'"
            valor1: pith
            valor2: roll
            valor3: yaw
        "L"
            Toogle sabre de luz
'''
from __future__ import division
import time, serial, os, sys, win32api
# Tamanho da tela
L_tela, H_tela = win32api.GetSystemMetrics(0), win32api.GetSystemMetrics(1)
L, H = 1200, 800
print L, H
# abre o arquivo para ver a porta escolhida
with open("scripts/serial_escolhida.txt") as f:
    porta = f.readline()
    porta = porta.strip('\n')
    print porta
try:
    if 'COM' in porta and porta != 'COM1':
        s = serial.Serial(port=porta,baudrate=115200)
        s.close()
        if s.is_open:
           s.close()
           time.sleep(0.05)
        s.open()
        while 1:
            if win32api.GetAsyncKeyState(ord('Q')):
        		sys.exit()
            linha = s.readline()
            pitch, roll, yaw = linha.split(',')
            win32api.SetCursorPos((int(L_tela/2.0+(int(L/180.0))*int(float(roll))),int(H_tela/2.0)))
except:
    pass
