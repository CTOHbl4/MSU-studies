% Ёмкость
target_h = 0.2;
target_V = 0.005;
target_area = target_V/target_h;

% Насос
input_5litre = 11/12;
pump_switch = 0.5;
pump_max_speed = 1/10000;
pump_speed = 5/6*pump_max_speed;

% Общее
g = 9.8;

% Слив
drain_d = 0.01;
drain_area = pi*(drain_d^2)/4;
v_init = 0;
angle_init = abs(pi/2*(v_init-128)/128);
theta0 = asin(1 - pump_speed/drain_area/sqrt(2*g*target_h));

% МК
mc_freq = 16*10^6;
mc_timer_coeff = 0.2;

% ШИМ
pwm_bits = 8;
pwm_T = 2^pwm_bits/mc_freq;

% Сонар
sonar_T = 0.1;
sonar_height = 0.1;
sonar_timer_freq = 16*10^6;
sonar_signal_time = 20*10^(-6);

% Коэффициенты линеаризации в x0, theta0, f0
A = -(1-sin(theta0))*sqrt(g/2/target_h) * drain_area / target_area;
B = sqrt(2*g*target_h) * cos(theta0) * drain_area / target_area;
F = 1/target_area;

% Регулятор
R = -26/16

% Диаграмма Боде для замкнутой системы => ЛАЧХ поднимается над 0 =>
% появляется грубость.