A = input('');
B = input('');
C = input('');

K = ctrb(A, B)
N = obsv(A, C)

rank(K)
rank(N)

noO = null(N)
Control = rref(K')'
Control = Control(1:10, 1:4)

coeffs = null([noO Control])
CnoO = noO*coeffs(1:4)
rank([Control(1:10, [1,3,4]) CnoO])
CO = Control(1:10, [1,3,4])
noC = [noO(1:10,1:3) [zeros(7, 3); eye(3)]]
rank([CnoO CO noC])
rank([noC noO])
%coeffs = null([noC noO])
%noCnoO = noC*coeffs(1:6, 1:3)
T = [CnoO CO noC]
A0 = inv(T)*A*T
B0 = inv(T)*B
C0 = C*T

