n = 10

% 1.
x = [0:0.5:5];

y1 = n * x.^2 - n;
y2 = -x.^3 / n - n/2;
y3 = sin(x) / n + (n + 1) * log(x);


subplot(1,2,1), plot(x, y1)
hold on
subplot(1,2,1), plot(x, y2)
subplot(1,2,1), plot(x, y3)
legend('y1', 'y2', 'y3')
hold off

% 2.
t = [0:pi/60:2*pi];
r1 = sqrt(n * t.^4 + t);
r2 = cos(n * t / 3);
% Если рисовать их на одном графике, то r2 не будет виден
% в масштабах r1, поэтому вывел в разные подокна

subplot(2,2,2), polar(t, r1, '')
legend('r1')
hold on
subplot(2,2,4), polar(t, r2, '')
legend('r2')
hold off

% 3.
A = [(n-15)-0.5 -(1+n)-0.2 0.5; n-10+0.3 -5.2 n+2+0.5; 10.2 -(n-17)+0.4 0.3]
B = [-2.6; (n+3)+0.7; (n-20)-0.5]
X = linsolve(A, B)

