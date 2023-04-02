import numpy as np
from copy import deepcopy
import random
import pygame as pg
import sys
#изменения в стр 1454
def self_all(fld):  # кол-во фишек у программы
    return np.sum([(fld == -1) | (fld == -2)])


def self_queen(fld):  # кол-во ферзей у программы
    return np.sum([fld == -2])


def enemy_all(fld):  # кол-во фишек игрока
    return np.sum([(fld == 1) | (fld == 2)])


def enemy_queen(fld):  # кол-во ферзей игрока
    return np.sum([fld == 2])


# эвристика: a + 4*b - c - 4*d, где
# a - кол-во своих фишек (и ферзей, и обычных)
# b - кол-во ферзей
# c - кол-во врагов
# d - кол-во ферзей игрока
# принимаемые значения эвристики:


def find_enemy_btw(fld, i0, j0, i1, j1, e_cas, e_queen):
    di = (i1-i0)//abs(i1-i0)
    dj = (j1-j0)//abs(j1-j0)
    while (i0 != i1) and (j0 != j1):
        i0 += di
        j0 += dj
        if (fld[i0][j0] == e_cas) or (fld[i0][j0] == e_queen):
            return [i0, j0]

def check_queen(field):
    fld = field.copy()
    for j in range(8):
        if fld[0][j] == 1:
            fld[0][j] = 2
        if fld[7][j] == -1:
            fld[7][j] = -2
    return fld


# сделать все ходы из mov_list[k]
def make_mov(field, mov_list, k, l, cas, queen):
    fld = check_queen(field)
    mov = mov_list[k][l]
    i = mov[0][0]
    j = mov[0][1]
    for coord in mov:
        if coord[2] == 1:
            tmp = fld[i][j]
            fld[i][j] = 0
            fld[coord[0]][coord[1]] = tmp
            enemy = find_enemy_btw(fld, i, j, coord[0], coord[1], -cas, -queen)
            if enemy != None:
                fld[enemy[0]][enemy[1]] = 0
            i = coord[0]
            j = coord[1]
            fld = check_queen(fld)
        elif coord[2] == -1:
            continue
        else:
            tmp = fld[i][j]
            fld[i][j] = 0
            fld[coord[0]][coord[1]] = tmp
            fld = check_queen(fld)
    return fld

def make_script(field, mov_list, k):
    fld = check_queen(field)
    script = mov_list[k]
    side = -1
    for mov in range(len(script)):
        fld = make_mov(fld, mov_list, k, mov, side, 2*side)
        side *= -1

    return fld

# доделать съедания до конца в mov_list[-1]
# mov_list = [[[[],...,[]],...],...]


# работаем в рамках одного хода
def can_eat_full(field, mov_list, cas, queen, script_num=0, mov_num=0, copy_flag=False):
    if (len(mov_list[script_num][mov_num]) >= 1):
        if(mov_list[script_num][mov_num][-1][2] == 0):
            return

    fld = make_script(field,  mov_list, script_num)
    copy_flag = False
    i = mov_list[script_num][mov_num][-1][0]
    j = mov_list[script_num][mov_num][-1][1]

    if fld[i][j] == cas:
        if (i > 1) and (i < 6) and (j > 1) and (j < 6):  # Во все 4 стороны
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j > 1) and (j < 6):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i > 1) and (i < 6) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)

        elif (i > 1) and (i < 6) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j > 1) and (j < 6):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j < 2):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j > 5):
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

    elif fld[i][j] == queen:
        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj < 7):
            qi += 1
            qj += 1
            if ((qi == 7) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj+1] == -cas) or (fld[qi+1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj > 0):
            qi += 1
            qj -= 1
            if ((qi == 7) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj-1] == -cas) or (fld[qi+1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj < 7):
            qi -= 1
            qj += 1
            if ((qi == 0) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj+1] == -cas) or (fld[qi-1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj > 0):
            qi -= 1
            qj -= 1
            if ((qi == 0) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj-1] == -cas) or (fld[qi-1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break


# возможность съесть из данной позиции, кладёт в mov_list все возможные съедающие ходы из данной позиции
# создаёт сценарии
def can_eat(i, j, fld, mov_list, cas, queen):
    if fld[i][j] == cas:
        if (i > 1) and (i < 6) and (j > 1) and (j < 6):  # Во все 4 стороны
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i < 2) and (j > 1) and (j < 6):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i < 2) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i < 2) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i > 1) and (i < 6) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i > 1) and (i < 6) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i+2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i > 5) and (j > 1) and (j < 6):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i > 5) and (j < 2):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j+2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

        elif (i > 5) and (j > 5):
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([i-2, j-2, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)

    elif fld[i][j] == queen:
        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj < 7):
            qi += 1
            qj += 1
            if ((qi == 7) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj+1] == -cas) or (fld[qi+1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag:
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([qi, qj, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj > 0):
            qi += 1
            qj -= 1
            if ((qi == 7) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj-1] == -cas) or (fld[qi+1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag:
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([qi, qj, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj < 7):
            qi -= 1
            qj += 1
            if ((qi == 0) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj+1] == -cas) or (fld[qi-1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag:
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([qi, qj, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj > 0):
            qi -= 1
            qj -= 1
            if ((qi == 0) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj-1] == -cas) or (fld[qi-1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag:
                mov_list.append([[[i, j, -1]]])
                mov_list[-1][-1].append([qi, qj, 1])
                script_num = len(mov_list)-1
                mov_num = len(mov_list[script_num])-1
                can_eat_full(fld, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

    for scr in range(len(mov_list)):
        mov_num = len(mov_list[scr])-1
        can_eat_full(fld, mov_list, cas, queen, scr, mov_num)










def can_eat_add_mov(field, mov_list, cas, queen, i, j, script_num, copy_flag):
    if not copy_flag:
        fld = make_script(field,  mov_list, script_num)
        mov_list[script_num].append([[i, j, -1]])
    else:
        tmp = deepcopy(mov_list[script_num][-1])
        mov_list[script_num].pop(-1)
        fld = make_script(field,  mov_list, script_num)
        script = deepcopy(mov_list[script_num])
        script.append([[i, j, -1]])
        mov_list[script_num].append(tmp)

    mov_num = -1

    if fld[i][j] == cas:
        if (i > 1) and (i < 6) and (j > 1) and (j < 6):  # Во все 4 стороны
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j > 1) and (j < 6):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)

        elif (i < 2) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)

        elif (i > 1) and (i < 6) and (j < 2):
            if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)

        elif (i > 1) and (i < 6) and (j > 5):
            if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i+2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i+2, j-2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j > 1) and (j < 6):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j < 2):
            if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j+2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j+2, 1])
                mov_list.append(script1)

        elif (i > 5) and (j > 5):
            if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0) and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([i-2, j-2, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                script1 = deepcopy(script)
                script1[-1].append([i-2, j-2, 1])
                mov_list.append(script1)

    elif fld[i][j] == queen:
        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj < 7):
            qi += 1
            qj += 1
            if ((qi == 7) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj+1] == -cas) or (fld[qi+1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi < 7) and (qj > 0):
            qi += 1
            qj -= 1
            if ((qi == 7) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi+1][qj-1] == -cas) or (fld[qi+1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj < 7):
            qi -= 1
            qj += 1
            if ((qi == 0) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj+1] == -cas) or (fld[qi-1][qj+1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break

        qi = i
        qj = j
        eat_flag = False
        while (qi > 0) and (qj > 0):
            qi -= 1
            qj -= 1
            if ((qi == 0) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                break
            if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                    and ((fld[qi-1][qj-1] == -cas) or (fld[qi-1][qj-1] == -queen)):
                break
            if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                eat_flag = True
            elif (fld[qi][qj] == 0) and eat_flag and not copy_flag:
                script = deepcopy(mov_list[script_num])
                mov_list[script_num][mov_num].append([qi, qj, 1])
                copy_flag = True
                can_eat_full(field, mov_list, cas, queen, script_num, mov_num)
            elif (fld[qi][qj] == 0) and eat_flag:
                script1 = deepcopy(script)
                script1[-1].append([qi, qj, 1])
                mov_list.append(script1)
            elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                break
    
    if not copy_flag:
        mov_list[script_num].pop(-1)
        return copy_flag
    for scr in range(len(mov_list)):
        mov_num = len(mov_list[scr])-1
        can_eat_full(field, mov_list, cas, queen, scr, mov_num)
    return copy_flag


def mov_init(i, j, fld, mov_list, cas, queen): # no eat
    if fld[i][j] == cas: # move front
        if (j > 0) and (j < 7):
            if (fld[i-cas][j-1] == 0):
                mov_list.append([[[i, j, -1], [i-cas, j-1, 0]]])
            if (fld[i-cas][j+1] == 0):
                mov_list.append([[[i, j, -1], [i-cas, j+1, 0]]])
        elif j == 0:
            if (fld[i-cas][j+1] == 0):
                mov_list.append([[[i, j, -1], [i-cas, j+1, 0]]])
        elif j == 7:
            if (fld[i-cas][j-1] == 0):
                mov_list.append([[[i, j, -1], [i-cas, j-1, 0]]])
    
    elif fld[i][j] == queen:
        qi = i
        qj = j
        while (qi < 7) and (qj < 7):
            qi += 1
            qj += 1
            if (fld[qi][qj] == 0):
                mov_list.append([[[i, j, -1], [qi, qj, 0]]])
            else:
                break

        qi = i
        qj = j
        while (qi < 7) and (qj > 0):
            qi += 1
            qj -= 1
            if (fld[qi][qj] == 0):
                mov_list.append([[[i, j, -1], [qi, qj, 0]]])
            else:
                break

        qi = i
        qj = j
        while (qi > 0) and (qj < 7):
            qi -= 1
            qj += 1
            if (fld[qi][qj] == 0):
                mov_list.append([[[i, j, -1], [qi, qj, 0]]])
            else:
                break

        qi = i
        qj = j
        while (qi > 0) and (qj > 0):
            qi -= 1
            qj -= 1
            if (fld[qi][qj] == 0):
                mov_list.append([[[i, j, -1], [qi, qj, 0]]])
            else:
                break


def mov_fill(i, j, field, mov_list, cas, queen, script_num, copy_flag):
    if not copy_flag:
        fld = make_script(field,  mov_list, script_num)
    else:
        tmp = deepcopy(mov_list[script_num][-1])
        mov_list[script_num].pop(-1)
        fld = make_script(field,  mov_list, script_num)
        mov_list[script_num].append(tmp)

    if fld[i][j] == cas: # move front
        if (j > 0) and (j < 7):
            if (fld[i-cas][j-1] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [i-cas, j-1, 0]])
                copy_flag = True
            elif fld[i-cas][j-1] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [i-cas, j-1, 0]])
                mov_list.append(script)
            if (fld[i-cas][j+1] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [i-cas, j+1, 0]])
                copy_flag = True
            elif fld[i-cas][j+1] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [i-cas, j+1, 0]])
                mov_list.append(script)
        elif (j == 0):
            if (fld[i-cas][j+1] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [i-cas, j+1, 0]])
                copy_flag = True
            elif fld[i-cas][j+1] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [i-cas, j+1, 0]])
                mov_list.append(script)
        elif (j == 7):
            if (fld[i-cas][j-1] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [i-cas, j-1, 0]])
                copy_flag = True
            elif fld[i-cas][j-1] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [i-cas, j-1, 0]])
                mov_list.append(script)   
    
    elif fld[i][j] == queen:
        qi = i
        qj = j
        while (qi < 7) and (qj < 7):
            qi += 1
            qj += 1
            if (fld[qi][qj] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [qi, qj, 0]])
                copy_flag = True
            elif fld[qi][qj] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [qi, qj, 0]])
                mov_list.append(script)
            else:
                break

        qi = i
        qj = j
        while (qi < 7) and (qj > 0):
            qi += 1
            qj -= 1
            if (fld[qi][qj] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [qi, qj, 0]])
                copy_flag = True
            elif fld[qi][qj] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [qi, qj, 0]])
                mov_list.append(script)
            else:
                break

        qi = i
        qj = j
        while (qi > 0) and (qj < 7):
            qi -= 1
            qj += 1
            if (fld[qi][qj] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [qi, qj, 0]])
                copy_flag = True
            elif fld[qi][qj] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [qi, qj, 0]])
                mov_list.append(script)
            else:
                break

        qi = i
        qj = j
        while (qi > 0) and (qj > 0):
            qi -= 1
            qj -= 1
            if (fld[qi][qj] == 0) and not copy_flag:
                mov_list[script_num].append([[i, j, -1], [qi, qj, 0]])
                copy_flag = True
            elif fld[qi][qj] == 0:
                script = deepcopy(mov_list[script_num])
                script.pop(-1)
                script.append([[i, j, -1], [qi, qj, 0]])
                mov_list.append(script)
            else:
                break
    return copy_flag


# эвристика: a + 4*b - c - 4*d, где
# a - кол-во своих фишек (и ферзей, и обычных)
# b - кол-во ферзей
# c - кол-во врагов
# d - кол-во ферзей игрока
def est(script_arr, field):
    arr = []
    for script in range(len(script_arr)):
        fld = make_script(field, script_arr, script)
        fld = check_queen(fld)
        pc_all1 = self_all(fld)
        pl_all1 = enemy_all(fld)
        pc_qn1 = self_queen(fld)
        pl_qn1 = enemy_queen(fld)
        if (pc_all1 == 0):
            arr.append(-100)
        elif (pl_all1 == 0):
            arr.append(100)
        else:
            arr.append(pc_all1+10*pc_qn1-pl_all1-10*pl_qn1)
    return arr

def cut(mov_list, arr_est, start, side, num):
    alpha = -1000
    beta = 1000

    if side < 0: # alpha
        for i in range(len(arr_est)-1, start-1, -1):
            if arr_est[i] > alpha:
                alpha = arr_est[i]
        if num < start:
            if arr_est[num] > alpha:
                alpha = arr_est[num]
        for i in range(len(arr_est)-1, start-1, -1):
            if arr_est[i] < alpha:
                mov_list.pop(i)
        if num < start:
            if arr_est[num] < alpha:
                mov_list.pop(num)
                return 1
    else:
        for i in range(len(arr_est)-1, start-1, -1):
            if arr_est[i] < beta:
                beta = arr_est[i]
        if num < start:
            if arr_est[num] < beta:
                beta = arr_est[num]
        for i in range(len(arr_est)-1, start-1, -1):
            if arr_est[i] > beta:
                mov_list.pop(i)
        if num < start:
            if arr_est[num] > beta:
                mov_list.pop(num)
                return 1
    return 0




def fill_mov_list(field, mov_list, cas, queen, depth=1):
    copy_flag = False
    fld = check_queen(field)
    side = -1
    for i in range(7, -1, -1):
        for j in range(8):
            can_eat(i, j, fld, mov_list, cas, queen)
    
    if len(mov_list) != 0:
        arr_est = est(mov_list, field)
        cut(mov_list, arr_est, 0, side, 0)
    if len(mov_list) == 0: #  cannot eat
        for i in range(7, -1, -1):
            for j in range(8):
                mov_init(i, j, fld, mov_list, cas, queen)

    for dp in range(2 * depth - 1):
        script_num = 0
        count_delete_scripts = 0
        mov_len = len(mov_list)
        
        while script_num < mov_len - count_delete_scripts:
            add_moves = len(mov_list[script_num])
            start_cut = len(mov_list)
            for i in range(7, -1, -1):
                for j in range(8):
                    copy_flag = can_eat_add_mov(fld, mov_list, side * cas, side * queen, i, j, script_num, copy_flag) or copy_flag
            
            leave_len = len(mov_list[script_num])
            if start_cut < len(mov_list):
                arr_est = est(mov_list, field)
                count_delete_scripts += cut(mov_list, arr_est, start_cut, -side, script_num)
            copy_flag = False
            if add_moves == leave_len: # cannot eat, take first 100 options 
                for i in range(7, -1, -1):
                    for j in range(8):
                        copy_flag = mov_fill(i, j, fld, mov_list, side * cas, side * queen, script_num, copy_flag) or copy_flag
                        if len(mov_list) > 3000:
                            break
                copy_flag = False
            script_num += 1
        arr_est = est(mov_list, field)
        for script_num in range(len(mov_list)-1, -1, -1):
            if (len(mov_list[script_num]) != dp + 2) and (abs(arr_est[script_num]) != 100):
                mov_list.pop(script_num)
            elif (len(mov_list[script_num]) > dp + 2):
                mov_list.pop(script_num)
        side *= -1

def player_can_eat(fld, i, j):
    cas = 1
    queen = 2
    if i == -1:
        for i in range(8):
            for j in range(8):
                if fld[i][j] == cas:
                    if (i > 1) and (i < 6) and (j > 1) and (j < 6):  # Во все 4 стороны
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i < 2) and (j > 1) and (j < 6):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i < 2) and (j < 2):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True

                    elif (i < 2) and (j > 5):
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i > 1) and (i < 6) and (j < 2):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True

                    elif (i > 1) and (i < 6) and (j > 5):
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                    elif (i > 5) and (j > 1) and (j < 6):
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                    elif (i > 5) and (j < 2):
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True

                    elif (i > 5) and (j > 5):
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                elif fld[i][j] == queen:
                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi < 7) and (qj < 7):
                        qi += 1
                        qj += 1
                        if ((qi == 7) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi+1][qj+1] == -cas) or (fld[qi+1][qj+1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi < 7) and (qj > 0):
                        qi += 1
                        qj -= 1
                        if ((qi == 7) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi+1][qj-1] == -cas) or (fld[qi+1][qj-1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi > 0) and (qj < 7):
                        qi -= 1
                        qj += 1
                        if ((qi == 0) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi-1][qj+1] == -cas) or (fld[qi-1][qj+1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi > 0) and (qj > 0):
                        qi -= 1
                        qj -= 1
                        if ((qi == 0) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi-1][qj-1] == -cas) or (fld[qi-1][qj-1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break
    else:
                if fld[i][j] == cas:
                    if (i > 1) and (i < 6) and (j > 1) and (j < 6):  # Во все 4 стороны
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i < 2) and (j > 1) and (j < 6):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i < 2) and (j < 2):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True

                    elif (i < 2) and (j > 5):
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True

                    elif (i > 1) and (i < 6) and (j < 2):
                        if ((fld[i+1][j+1] == -cas) or (fld[i+1][j+1] == -queen)) and (fld[i+2][j+2] == 0):
                            return True
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True

                    elif (i > 1) and (i < 6) and (j > 5):
                        if ((fld[i+1][j-1] == -cas) or (fld[i+1][j-1] == -queen)) and (fld[i+2][j-2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                    elif (i > 5) and (j > 1) and (j < 6):
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                    elif (i > 5) and (j < 2):
                        if ((fld[i-1][j+1] == -cas) or (fld[i-1][j+1] == -queen)) and (fld[i-2][j+2] == 0):
                            return True

                    elif (i > 5) and (j > 5):
                        if ((fld[i-1][j-1] == -cas) or (fld[i-1][j-1] == -queen)) and (fld[i-2][j-2] == 0):
                            return True

                elif fld[i][j] == queen:
                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi < 7) and (qj < 7):
                        qi += 1
                        qj += 1
                        if ((qi == 7) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi+1][qj+1] == -cas) or (fld[qi+1][qj+1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi < 7) and (qj > 0):
                        qi += 1
                        qj -= 1
                        if ((qi == 7) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi+1][qj-1] == -cas) or (fld[qi+1][qj-1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi > 0) and (qj < 7):
                        qi -= 1
                        qj += 1
                        if ((qi == 0) or (qj == 7)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi-1][qj+1] == -cas) or (fld[qi-1][qj+1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

                    qi = i
                    qj = j
                    eat_flag = False
                    while (qi > 0) and (qj > 0):
                        qi -= 1
                        qj -= 1
                        if ((qi == 0) or (qj == 0)) and ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)):
                            break
                        if (fld[qi][qj] == cas) or (fld[qi][qj] == queen) or ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen))\
                                and ((fld[qi-1][qj-1] == -cas) or (fld[qi-1][qj-1] == -queen)):
                            break
                        if ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and not eat_flag:
                            eat_flag = True
                        elif (fld[qi][qj] == 0) and eat_flag:
                            return True
                        elif ((fld[qi][qj] == -cas) or (fld[qi][qj] == -queen)) and eat_flag:
                            break

def check_win(fld):
    if(self_all(fld) == 0):
        return 1
    if(enemy_all(fld) == 0):
        return -1
    return 0


def game(fld, mov_list, depth):
    pic_w = 550
    m = 8
    pg.init()

    print("Welcome to a game of checkers!")
    random.seed()
    side = random.randint(0, 1)
    pl_color = 85
    pc_color = 250
    if side == 0:
            print("You start!")
            pl_color = 250
            pc_color = 85
            
    screen = pg.display.set_mode((pic_w, pic_w))

    for i in range(m):
        for j in range(i%2, m, 2):
            rect = pg.Rect(j * pic_w // m, i * pic_w //
                m, pic_w // m, pic_w // m)
            pg.draw.rect(screen, (255, 255, 255), rect)
            
    for i in range(m):
        for j in range(1-(i%2), m, 2):
            if(fld[i][j] == -1):
                pg.draw.circle(screen, (pc_color, pc_color, pc_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
            elif(fld[i][j] == 1):
                pg.draw.circle(screen, (pl_color, pl_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
            elif(fld[i][j] == -2):
                pg.draw.circle(screen, (pc_color, pl_color, pc_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
            elif(fld[i][j] == 2):
                pg.draw.circle(screen, (pl_color, pc_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
    pg.display.flip()
    
    if side == 0: #  first move
        movi0 = 8-int(input())
        if movi0 == 10:
            sys.exit()
        movj0 = int(input())-1
        movi1 = 8-int(input())
        movj1 = int(input())-1
        fld[movi0][movj0] = 0
        fld[movi1][movj1] = 1

        pg.draw.circle(screen, (0, 0, 0), (pic_w // m * movj0 + pic_w // m // 2, pic_w // m * movi0 + pic_w // m // 2), pic_w // m // 2 - 5)
        pg.draw.circle(screen, (pl_color, pl_color, pl_color), (pic_w // m * movj1 + pic_w // m // 2, pic_w // m * movi1 + pic_w // m // 2), pic_w // m // 2 - 10)
        pg.display.flip()
        pg.time.delay(2000)
    
    while True:
        for event in pg.event.get():
            if event.type == pg.QUIT:
                pg.quit()
                sys.exit()

        
        while check_win(fld) == 0:
            fill_mov_list(fld, mov_list, -1, -2, depth)
            est_arr = est(mov_list, fld)
            fld = make_mov(fld, mov_list, est_arr.index(max(est_arr)), 0, -1, -2)
            for i in range(m):
                for j in range(1-(i%2), m, 2):
                    if(fld[i][j] == -1):
                        pg.draw.circle(screen, (pc_color, pc_color, pc_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
                    elif(fld[i][j] == 1):
                        pg.draw.circle(screen, (pl_color, pl_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
                    elif(fld[i][j] == -2):
                        pg.draw.circle(screen, (pc_color, pl_color, pc_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
                    elif(fld[i][j] == 2):
                        pg.draw.circle(screen, (pl_color, pc_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
                    else:
                        pg.draw.circle(screen, (0, 0, 0), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 5)
            pg.display.flip()
            if check_win(fld) != 0:
                break
            mov_list = []
            i = -1
            j = -1
            if player_can_eat(fld, i, j):
                while player_can_eat(fld, i, j):
                    print("Eat!")
                    if i == -1:
                        print("Enter start pos:")
                        movi0 = 8-int(input())
                        if movi0 == 10:
                            sys.exit()
                        movj0 = int(input())-1
                    else:
                        movi0 = i
                        movj0 = j
                    print("Enter end pos:")
                    i = 8-int(input())
                    if i == 10:
                        sys.exit()
                    j = int(input())-1
                    tmp = fld[movi0][movj0]
                    fld[movi0][movj0] = 0
                    fld[i][j] = tmp
                    pc = find_enemy_btw(fld, movi0, movj0, i, j, -1, -2)
                    fld[pc[0]][pc[1]] = 0
                    fld = check_queen(fld)
                    pg.draw.circle(screen, (0, 0, 0), (pic_w // m * movj0 + pic_w // m // 2, pic_w // m * movi0 + pic_w // m // 2), pic_w // m // 2 - 5)
                    pg.draw.circle(screen, (0, 0, 0), (pic_w // m * pc[1] + pic_w // m // 2, pic_w // m * pc[0] + pic_w // m // 2), pic_w // m // 2 - 5)

                    if(tmp == 1):
                        pg.draw.circle(screen, (pl_color, pl_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)
                    elif(tmp == 2):
                        pg.draw.circle(screen, (pl_color, pc_color, pl_color), (pic_w // m * j + pic_w // m // 2, pic_w // m * i + pic_w // m // 2), pic_w // m // 2 - 10)

                    pg.display.flip()
            else:
                print("Enter start pos:")
                movi0 = 8-int(input())
                if movi0 == 10:
                    sys.exit()
                movj0 = int(input())-1
                print("Enter end pos:")
                movi1, movj1 = 8-int(input()), int(input())-1
                tmp = fld[movi0][movj0]
                fld[movi0][movj0] = 0
                fld[movi1][movj1] = tmp
                fld = check_queen(fld)
                pg.draw.circle(screen, (0, 0, 0), (pic_w // m * movj0 + pic_w // m // 2, pic_w // m * movi0 + pic_w // m // 2), pic_w // m // 2 - 5)
                if(tmp == 1):
                    pg.draw.circle(screen, (pl_color, pl_color, pl_color), (pic_w // m * movj1 + pic_w // m // 2, pic_w // m * movi1 + pic_w // m // 2), pic_w // m // 2 - 10)
                elif(tmp == 2):
                    pg.draw.circle(screen, (pl_color, pc_color, pl_color), (pic_w // m * movj1 + pic_w // m // 2, pic_w // m * movi1 + pic_w // m // 2), pic_w // m // 2 - 10)

                pg.display.flip()
            pg.time.delay(2000)
        if check_win(fld) == -1:
            print("PC won!")
        else:
            print("You won!")
        break





















"""fld = np.array([[0, -1, 0, -1, 0, -1, 0, -1],
                [-1, 0, -1, 0, -1, 0, -1, 0],
                [0, -1, 0, -1, 0, -1, 0, -1],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [1, 0, 1, 0, 1, 0, 1, 0],
                [0, 1, 0, 1, 0, 1, 0, 1],
                [1, 0, 1, 0, 1, 0, 1, 0]])"""


fld = np.array([[0, 0, 0, 0, 0, 0, 0, -1],
                [0, 0, 1, 0, 0, 0, -1, 0],
                [0, -1, 0, 0, 0, -1, 0, -1],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [0, 0, 0, 1, 0, 0, 0, 1],
                [-1, 0, 0, 0, 1, 0, 0, 0],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [0, 0, 1, 0, 0, 0, 1, 0]])
"""fld = np.zeros((8, 8))"""


"""
fld[1][4] = -1
fld[5][2] = 1"""
# симметрия относительно центра доски
# компьютер ходит вниз => i растёт

mov_list = []

game(fld, mov_list, 2)
