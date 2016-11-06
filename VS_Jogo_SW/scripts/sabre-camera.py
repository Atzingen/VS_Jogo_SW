# -*- coding: latin-1 -*-
'''
    Programa auxiliar para o jogo do sabre de luz star wars (disciplina de
    jogos), feita em c++ com allegro

    Este  programa controla o mouse na tela baseado no reconhecimento de
    imagem adquidida pela camera (camera 0 conectada ao computador). Duas bolas
    de cores diferentes (azul e vermelha)  serão colocadas na ponta do
    "sabre de luz" e pelo reconhecimeto delas a posicao do sabre no jogo será
    alterada.
'''

from __future__ import division
import time, serial, os, sys
import numpy as np
import cv2

with open("config-camera.txt",'r') as f:
    _, h_min1 = f.readline().split(',')
    _, s_min1 = f.readline().split(',')
    _, v_min1 = f.readline().split(',')
    _, h_max1 = f.readline().split(',')
    _, s_max1 = f.readline().split(',')
    _, v_max1 = f.readline().split(',')
    _, k1 = f.readline().split(',')
    _, iter1 = f.readline().split(',')
    _, blur1 = f.readline().split(',')
    _, h_min2 = f.readline().split(',')
    _, s_min2 = f.readline().split(',')
    _, v_min2 = f.readline().split(',')
    _, h_max2 = f.readline().split(',')
    _, s_max2 = f.readline().split(',')
    _, v_max2 = f.readline().split(',')
    _, k2 = f.readline().split(',')
    _, iter2 = f.readline().split(',')
    _, blur2 = f.readline().split(',')

hsv_min1 = (int(h_min1),int(s_min1),int(v_min1))
hsv_max1 = (int(h_max1),int(s_max1),int(v_max1))
hsv_min2 = (int(h_min2),int(s_min2),int(v_min2))
hsv_max2 = (int(h_max2),int(s_max2),int(v_max2))
blur1 = int(blur1)
blur2 = int(blur2)
iter1 = int(iter1)
iter2 = int(iter2)

print int(h_min1),int(s_min1),int(v_min1),int(h_max1),int(s_max1),int(v_max1), int(k1), int(iter1), int(blur1)
print int(h_min2),int(s_min2),int(v_min2),int(h_max2),int(s_max2),int(v_max2), int(k2), int(iter2), int(blur2)

cap = cv2.VideoCapture(0)
detector = cv2.SimpleBlobDetector_create()
kernel1 = np.ones((int(k1),int(k1)), np.uint8)
kernel2 = np.ones((int(k2),int(k2)), np.uint8)

_, frame = cap.read()
while frame is not None:
    frame1 = cv2.blur(frame,(blur1,blur1))
    frame2 = cv2.blur(frame,(blur2,blur2))
    hsv1 = cv2.cvtColor(frame1,cv2.COLOR_BGR2HSV)
    hsv2 = cv2.cvtColor(frame2,cv2.COLOR_BGR2HSV)
    limiar1 = cv2.inRange(hsv1,np.array(hsv_min1), np.array(hsv_max1))
    limiar1 = cv2.bitwise_not(limiar1)
    limiar1 = cv2.erode(limiar1, kernel1, iterations=iter1)
    limiar1 = cv2.dilate(limiar1, kernel1, iterations=iter1)
    limiar2 = cv2.inRange(hsv2,np.array(hsv_min2), np.array(hsv_max2))
    limiar2 = cv2.bitwise_not(limiar2)
    limiar2 = cv2.erode(limiar2, kernel2, iterations=iter2)
    limiar2 = cv2.dilate(limiar2, kernel2, iterations=iter2)
    keypoints1 = detector.detect(limiar1)
    keypoints2 = detector.detect(limiar2)
    try:
        p1_x = int(keypoints1[0].pt[0])
        p1_y = int(keypoints1[0].pt[1])
        cv2.circle(frame,(int(keypoints1[0].pt[0]),
                          int(keypoints1[0].pt[1])),
                          10,(255,0,0),2)
        p2_x = int(keypoints2[0].pt[0])
        p2_y = int(keypoints2[0].pt[1])
        cv2.circle(frame,(int(keypoints2[0].pt[0]),
                          int(keypoints2[0].pt[1])),
                          10,(0,255,0),2)
        cv2.line(frame,(p1_x,p1_y),(p2_x,p2_y), (10,0,10),15)
        cv2.line(frame,(p2_x,p2_y),(int(p2_x + 3.5*(p2_x-p1_x)),int(p2_y + 3.5*(p2_y-p1_y))),(0,0,255),15)
    except:
        pass
    cv2.imshow('camera',frame)
    cv2.imshow('l1',limiar1)
    cv2.imshow('l2',limiar2)
    _, frame = cap.read()
    k = cv2.waitKey(1) & 0xFF
    if k == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
