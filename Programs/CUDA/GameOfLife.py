from numba import jit
import pygame as pg
import numpy as np
from numba import cuda
from numba import *
import sys

FPS = 40

# Ядро для обработки окружения каждой клетки.


@cuda.jit
def kernel(arr, new_arr, n, m):
    j = cuda.blockIdx.x
    i = cuda.threadIdx.x
    if arr[i][j] == 2:
        new_arr[i][j] = 1
    elif arr[i][j] == 1:
        new_arr[i][j] = 0
    else:
        if (int(arr[i-1][j-1] == 2) + int(arr[i-1][j] == 2) + int(arr[i-1][(j+1) % m] == 2) + int(arr[i][j-1] == 2) + int(arr[i][(j+1) % m] == 2)
                + int(arr[(i+1) % n][j-1] == 2) + int(arr[(i+1) % n][j] == 2) + int(arr[(i+1) % n][(j+1) % m] == 2) == 2):
            new_arr[i][j] = 2


# Размерность поля. Размер окна фиксирован, но можно менять в программе в параметрах pic_w, pic_h. Окно делится линиями на введённое кол-во клеток.
# Сначала число строк, потом число столбцов.
# Ввод в одну строку через пробел.
flag = True
print("Size:")
while flag:
    try:
        n, m = map(int, input().split())
        if (n == 0 or m == 0):
            print("Exit on 0")
            sys.exit()
        flag = False
    except ValueError:
        print("ValueError, try again. Example: 100 100")

# Создание окна с линиями раздела
pg.init()

pic_w = 1000 + m - 1000 % m
pic_h = 800 + n - 800 % n

screen = pg.display.set_mode((pic_w, pic_h))
for i in range(pic_w // m, pic_w, pic_w // m):
    pg.draw.line(screen, (255, 255, 255), (i, 0), (i, pic_h - 1))
for i in range(pic_h // n, pic_h, pic_h // n):
    pg.draw.line(screen, (255, 255, 255), (0, i), (pic_w - 1, i))

pg.display.flip()


fld = np.zeros((n, m), dtype=np.uint8)  # field
# Ввод начальных положений. Также, как размерность покоординатно. Конец ввода - английская Т.
print("Initialization:")
line = ""
line = input()

while line != "T":
    try:
        line = line.split()
        a0 = int(line[0])
        a1 = int(line[1])
        state = int(line[2])
        fld[a0][a1] = state
    except IndexError:
        print("IndexError, try again")
        line = input()
        continue
    except ValueError:
        print("ValueError, try again")
        line = input()
        continue
    rect = pg.Rect(a1 * pic_w // m + 1, a0 * pic_h //
                   n + 1, pic_w // m - 2, pic_h // n - 2)
    pg.draw.rect(screen, (255, 255 - 255*int(state == 2),
                 255 - 255*int(state == 1)), rect)
    line = input()

# Старт. Состояние меняется при нажатии любой клавиши. Конец - закрытие окна (крестик справа сверху).
pg.display.update()

pg.event.clear()

fld_d = cuda.to_device(fld)
fld_d_new = cuda.to_device(fld)

clock = pg.time.Clock()
while True:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            pg.quit()
            sys.exit()

        # Из интереса решил воспользоваться cuda. (Могут возникнуть проблемы из-за нестыковок версий cuda и numba, устанавливал самую позднюю версию с сайта)
        # Разбил матрицу по столбцам и создаю копию.
    kernel[m, n](fld_d, fld_d_new, n, m)
    fld = fld_d_new.copy_to_host()
    fld_d.copy_to_device(fld_d_new)
    for i in range(n):
        for j in range(m):
            rect = pg.Rect(j * pic_w // m + 1, i * pic_h //
                            n + 1, pic_w // m - 2, pic_h // n - 2)
            if fld[i][j]:
                pg.draw.rect(screen, 
                            (255, 255 - 255*int(fld[i][j] == 2), 255 - 255*int(fld[i][j] == 1)), rect)
            else:
                pg.draw.rect(screen, (0, 0, 0), rect)
    pg.display.update()
    clock.tick(FPS)
