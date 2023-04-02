import numpy as np
import pygame as pg
import sys
from math import pi

def res_to_pos(ind, pos):
    if ind == 0:
        pos[1] -= 1
        return pos
    if ind == 1:
        pos[1] += 1
        return pos
    if ind == 2:
        pos[0] -= 1
        return pos
    if ind == 3:
        pos[0] += 1
        return pos

const = 3.
def f(pos, prev_pos, matr, final_pos):
    pos_copy = pos.copy()
    res = np.zeros(4)

    if pos[1] > 0:
        pos_copy[1] -= 1
        print("Left:", matr[*pos_copy], pos_copy)
        if matr[*pos_copy] < 0.98:
            res[0] = ((1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos))/2. - 400
        elif matr[*pos_copy] >= const:
            res[0] = (1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos) + 400
        else:
            res[0] = 400 * max(matr.shape)
    else:
        res[0] = 400 * max(matr.shape)
    pos_copy = pos.copy()

    if pos[1] < matr.shape[1] - 1:
        pos_copy[1] += 1
        if matr[*pos_copy] < 0.98:
            res[1] = ((1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos))/2. - 400
        elif matr[*pos_copy] >= const:
            res[1] = (1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos) + 400
        else:
            res[1] = 400 * max(matr.shape)
    else:
        res[1] = 400 * max(matr.shape)
    pos_copy = pos.copy()

    if pos[0] > 0:
        pos_copy[0] -= 1
        print("Up:", matr[*pos_copy], pos_copy)
        if matr[*pos_copy] < 0.98:
            res[2] = ((1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos))/2. - 400
        elif matr[*pos_copy] >= const:
            res[2] = (1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos) + 400
        else:
            res[2] = 400 * max(matr.shape)
    else:
        res[2] = 400 * max(matr.shape)
    pos_copy = pos.copy()

    if pos[0] < matr.shape[0] - 1:
        pos_copy[0] += 1
        if matr[*pos_copy] < 0.98:
            res[3] = ((1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos))/2. - 400
        elif matr[*pos_copy] >= const:
            res[3] = (1.5 + matr[*pos_copy]) + np.linalg.norm(pos_copy - final_pos) + 400
        else:
            res[3] = 400 * max(matr.shape)
    else:
        res[3] = 400 * max(matr.shape)
    pos_copy = pos.copy()


    next_pos = res_to_pos(np.argmin(res), pos)
    if np.all(res == 400 * max(matr.shape)):
        print(res, np.argmin(res))
        next_pos = prev_pos

    return next_pos, np.all(next_pos == final_pos), np.all(res == 20 * max(matr.shape))

width = 1000
height = 700
frames = 1

pg.init()
screen = pg.display.set_mode((width, height))
clock = pg.time.Clock()

matr_width = 11
matr_height = 10


matr = np.abs(np.random.normal(0, 0.7, (matr_height, matr_width)))
matr[matr > 0.7] = 1.
print(matr)

final_pos = np.array([0, 0])
start_pos = np.array([matr_height - 1, matr_width - 1])
print(start_pos)
matr[*start_pos] = const

if matr[*final_pos] >= 0.9:
    matr[*final_pos] = 0.5

path_difficulty = 0.

pg.display.update()

pg.event.clear()

stop_flag = False

pos_1 = start_pos.copy()
pos_2 = start_pos.copy()

for event in pg.event.get():
    if event.type == pg.QUIT:
        pg.quit()
        sys.exit()

start_pos, _, stop_flag = f(start_pos, start_pos, matr, final_pos)

if stop_flag:
    print("Cannot solve the puzzle.")
    sys.exit()

screen.fill((0, 0, 0))

for i in range(matr_height):
    for j in range(matr_width):
        pg.draw.rect(screen, (0, 0, (255 - matr[i, j] * 255) % 256), [j * width/matr_width, i * height/matr_height, width/matr_width, height/matr_height])

pg.draw.rect(screen, (0, 200, 0), [start_pos[1] * width/matr_width, start_pos[0] * height/matr_height, width/matr_width, height/matr_height])

pg.display.flip()
clock.tick(frames)

path_difficulty += matr[*start_pos]

print(start_pos, matr[*start_pos], stop_flag)



for event in pg.event.get():
    if event.type == pg.QUIT:
        pg.quit()
        sys.exit()

pos_2 = pos_1
pos_1 = start_pos.copy()
matr[*start_pos] = const
start_pos, stop_flag, _ = f(start_pos, pos_2, matr, final_pos)
screen.fill((0, 0, 0))

for i in range(matr_height):
    for j in range(matr_width):
        pg.draw.rect(screen, (0, 0, (255 - matr[i, j] * 255) % 256), [j * width/matr_width, i * height/matr_height, width/matr_width, height/matr_height])

pg.draw.rect(screen, (0, 200, 0), [start_pos[1] * width/matr_width, start_pos[0] * height/matr_height, width/matr_width, height/matr_height])

pg.display.flip()
clock.tick(frames)

path_difficulty += matr[*start_pos]

cycling = False

while not stop_flag:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            pg.quit()
            sys.exit()

    pos_2 = pos_1.copy()
    pos_1 = start_pos.copy()
    matr[*start_pos] = const
    start_pos, stop_flag, _ = f(start_pos, pos_2, matr, final_pos)

    if (np.all(start_pos == pos_2)) and not cycling:
        cycling = True
        matr[*start_pos] = 1.
    elif (np.all(start_pos != pos_2)) and cycling:
        cycling  = False
    elif (np.all(start_pos == pos_2) or np.all(start_pos == pos_1)) and cycling:
        stop_flag = True

    screen.fill((0, 0, 0))

    for i in range(matr_height):
        for j in range(matr_width):
            pg.draw.rect(screen, (0, 0, (255 - matr[i, j] * 255) % 256), [j * width/matr_width, i * height/matr_height, width/matr_width, height/matr_height])

    pg.draw.rect(screen, (0, 200, 0), [start_pos[1] * width/matr_width, start_pos[0] * height/matr_height, width/matr_width, height/matr_height])

    pg.display.flip()
    clock.tick(frames)

    path_difficulty += matr[*start_pos]

    print(start_pos, matr[*start_pos], stop_flag)

if np.all(start_pos == final_pos):
    print("Puzzle is solved.")
    print(path_difficulty)
else:
    print("Cannot solve the puzzle.")


'''
[[0.16045037 0.23500149 1.         0.13203888 0.12663954 0.1108979
  0.00133375 1.         1.         0.32737444]
 [1.         0.57022766 1.         1.         0.05928783 1.
  0.4012237  0.30545663 1.         1.        ]
 [1.         0.39508638 0.0576456  0.04712079 0.66014243 1.
  0.20182171 0.61685577 0.46434928 1.        ]
 [1.         0.56217842 1.         0.52978937 1.         0.11767211
  1.         0.0294647  0.38205263 1.        ]
 [1.         0.44283593 1.         1.         1.         0.12855467
  1.         0.10515446 1.         1.        ]
 [0.04396278 0.13923753 0.37368869 0.50059643 0.04111968 0.64148568
  0.51479076 0.35779311 0.64196348 0.49887187]
 [0.33377942 0.452885   0.58334518 0.60979466 0.19743263 0.35018272
  1.         0.15492525 0.13360522 1.        ]
 [1.         0.35957493 0.59215552 0.59787514 1.         0.46899025
  1.         0.43216209 0.27702881 0.21277836]
 [0.16744476 1.         0.30497293 0.48979428 0.04052003 0.09560477
  0.56133376 0.55723597 0.4190949  1.        ]
 [0.48756095 0.26460898 1.         1.         0.11646144 0.15100281
  1.         0.23902044 1.         0.2976821 ]]
  

  [[0.25396567, 0.50952392, 0.67837265, 0.42837535, 1.,         0.0084518,
  0.31290891, 0.62191679, 0.17530893, 0.19717825]
 [0.20791892, 1.,         0.36521329, 0.2933854,  0.57635989, 0.3049674,
  0.18953016, 0.42873991, 1.,         0.12370714]
 [0.00921633, 1.,         0.10232229, 1.,         0.66471958, 0.11139743,
  0.30082074, 0.29139805, 0.07271427, 1.        ]
 [1.,         1.,         1.,         0.51444818, 0.36357587, 1.,
  0.16068962, 0.19476992, 0.23600741, 1.        ]
 [0.35233715, 0.69026053, 0.15873143, 1.,         0.28343778, 0.09092564,
  1.,         0.22627427, 0.42873174, 0.00931518]
 [0.53187122, 1.,         1.,         0.53436793, 0.20701871, 1.,
  0.50073751, 1.,         1.,         0.3727347 ]
 [0.18538173, 0.02861155, 0.0128185,  0.37901697, 0.42301593, 0.10053386,
  0.04940071, 0.51166124, 0.17635585, 1.        ]
 [0.43049712, 1.,         0.38337243, 1.,         1.,         0.21817838,
  1.,         1.,         0.00701237, 0.60821742]
 [0.39940591, 0.30624968, 0.68872238, 1.,         0.20036355, 0.4021617,
  0.11391203, 0.1394623,  0.10272475, 0.3939216 ]
 [0.02176826, 0.09897151, 0.00349395, 0.03020455, 1.,         0.33555362,
  1.,         0.24111256, 0.29027402, 0.51686098]]
  
  '''