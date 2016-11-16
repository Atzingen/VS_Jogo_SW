# -*- coding: latin-1 -*-
'''
    Programa auxiliar para o jogo do sabre de luz star wars (disciplina de
    jogos), feita em c++ com allegro

    Este  programa abre um controle de filtros para que o usuário selecione
    os valores adequados para sua camera. Este programa deve ser rodado sempre
    que a câmera, o local ou as bolas coloridas forem alteradas. A configuração
    escolhida é salva em um arquivo txt chamado "config-camera.txt".
'''

from __future__ import division
import time, serial, os, sys
import numpy as np
import cv2

def impar(x):
    x = int(x)
    if x % 2 == 0:
        return x
    else:
        return x + 1

def nothing(x):
    pass

def sava_config(h_min1,s_min1,v_min1,h_max1,s_max1,v_max1,k1,iter1,blur1,
                h_min2,s_min2,v_min2,h_max2,s_max2,v_max2,k2,iter2,blur2):
    with open("scripts/config-camera.txt",'w') as f:
        f.write("h_min1,"+str(h_min1)+'\n')
        f.write("s_min1,"+str(s_min1)+'\n')
        f.write("v_min1,"+str(v_min1)+'\n')
        f.write("h_max1,"+str(h_max1)+'\n')
        f.write("s_max1,"+str(s_max1)+'\n')
        f.write("v_max1,"+str(v_max1)+'\n')
        f.write("k1,"+str(k1)+'\n')
        f.write("iter1,"+str(iter1)+'\n')
        f.write("blur1,"+str(blur1)+'\n')
        f.write("h_min2,"+str(h_min2)+'\n')
        f.write("s_min2,"+str(s_min2)+'\n')
        f.write("v_min2,"+str(v_min2)+'\n')
        f.write("h_max2,"+str(h_max2)+'\n')
        f.write("s_max2,"+str(s_max2)+'\n')
        f.write("v_max2,"+str(v_max2)+'\n')
        f.write("k2,"+str(k2)+'\n')
        f.write("iter2,"+str(iter2)+'\n')
        f.write("blur2,"+str(blur2))

cv2.namedWindow('image')
cv2.resizeWindow('image',800,800)
cv2.createTrackbar('blur','image',0,25,nothing)
cv2.createTrackbar('k_kernel','image',1,25,nothing)
cv2.createTrackbar('iterI_erode','image',1,25,nothing)
cv2.createTrackbar('h_min','image',0,179,nothing)
cv2.createTrackbar('s_min','image',0,255,nothing)
cv2.createTrackbar('v_min','image',0,255,nothing)
cv2.createTrackbar('h_max','image',0,179,nothing)
cv2.createTrackbar('s_max','image',0,255,nothing)
cv2.createTrackbar('v_max','image',0,255,nothing)

font = cv2.FONT_HERSHEY_SIMPLEX
cap = cv2.VideoCapture(0)
estado = 1
k = 5
kernel1 = np.ones((k,k), np.uint8)

_, frame = cap.read()
while frame is not None:
    blur = cv2.getTrackbarPos('blur','image')
    h_min = cv2.getTrackbarPos('h_min','image')
    s_min = cv2.getTrackbarPos('s_min','image')
    v_min = cv2.getTrackbarPos('v_min','image')
    h_max = cv2.getTrackbarPos('h_max','image')
    s_max = cv2.getTrackbarPos('s_max','image')
    v_max = cv2.getTrackbarPos('v_max','image')
    k_novo = impar(cv2.getTrackbarPos('k_kernel','image'))
    iterI = cv2.getTrackbarPos('iterI_erode','image')
    if (int(blur) > 0):
        frame2 = cv2.blur(frame,(impar(blur),impar(blur)))
    else:
        frame2 = frame
    hsv = cv2.cvtColor(frame2,cv2.COLOR_BGR2HSV)
    limiar = cv2.inRange(hsv,np.array((h_min,s_min,v_min)), np.array((h_max,s_max,v_max)))
    if k_novo != k:
        k = k_novo
        kernel = np.ones((k,k), np.uint8)
    if k_novo > 0:
        limiar = cv2.erode(limiar, kernel, iterations=iterI)
        limiar = cv2.dilate(limiar, kernel, iterations=iterI)
    if estado == 1:
        cv2.putText(frame,'bola 1',(10,80), font, 1,(255,0,0),2,cv2.LINE_AA)
        cv2.putText(frame,'S  ->  Salvar ',(10,180), font, 1,(255,0,0),2,cv2.LINE_AA)
        cv2.putText(frame,'Q  ->  Sair ',(10,280), font, 1,(255,0,0),2,cv2.LINE_AA)
    else:
        cv2.putText(frame,'bola 2',(10,80), font, 1,(255,0,0),2,cv2.LINE_AA)
        cv2.putText(frame,'S  ->  Salvar ',(10,180), font, 1,(255,0,0),2,cv2.LINE_AA)
        cv2.putText(frame,'V  ->  Voltar ',(10,280), font, 1,(255,0,0),2,cv2.LINE_AA)
        cv2.putText(frame,'Q  ->  Sair ',(10,380), font, 1,(255,0,0),2,cv2.LINE_AA)
    cv2.imshow('camera',frame)
    cv2.imshow('resultado',limiar)
    _, frame = cap.read()
    k = cv2.waitKey(1) & 0xFF
    if k == ord('q'):
        break
    elif k == ord('s') or k == ord('S'):
        if estado == 1:
            blur1 = blur
            h_min1 = h_min
            s_min1 = s_min
            v_min1 = v_min
            h_max1 = h_max
            s_max1 = s_max
            v_max1 = v_max
            k1 = k_novo
            iter1 = iterI
            estado = 2
        else:
            blur2 = blur
            h_min2 = h_min
            s_min2 = s_min
            v_min2 = v_min
            h_max2 = h_max
            s_max2 = s_max
            v_max2 = v_max
            k2 = k_novo
            iter2 = iterI
            sava_config(h_min1,s_min1,v_min1,h_max1,s_max1,v_max1,k1,iter1,blur1,
                        h_min2,s_min2,v_min2,h_max2,s_max2,v_max2,k2,iter2,blur2)
            break
    elif k == ord('v') or k == ord('V'):
        if estado == 2:
            estado = 1


cap.release()
cv2.destroyAllWindows()
