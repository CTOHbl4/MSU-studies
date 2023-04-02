n = 5;
% 1.
a = rand(1, 2*n)
b = rand(2*n, 1)
c = rand(1, 2*n)

a_plus_c = a + c

c_mul_b = c * b

mean_a = mean(a)

sum_b = sum(b)

min(c)
max(c)
sorted_c = sort(c);
cmin = sorted_c(1) == min(c)
cmax = sorted_c(2*n) == max(c)

% 2.
A = rand(2, 3)
B = rand(3, 3)
C = rand(2, 3)

A_plus_C = A + C
A_minus_C = A - C

C_mul_B = C * B

C_mul_B_mul_5 = C_mul_B * 5

det_B = det(B)

B_neg = B^(-1)

B_neg*B
