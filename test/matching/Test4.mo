model Test4
constant Integer N = 10000;
Real a1[N], x1[N], b1[N];
Real a2[N], x2[N], b2[N];
equation
for i in 2:N-1 loop
der(x1[i]) - a1[i] + x1[i] = 0;
a1[i + 1] - a1[i] - b1[i] = 0;
b1[i] - x1[i - 1] = 0;
end for;
b1[1] = 0;
b1[N] - x1[N - 1] = 0;
der(x1[1]) - a1[1] + x1[1] = 0;
der(x1[N]) - a1[N] + x1[N] = 0;
a1[2] - a1[1] - b1[1] = 0;
a1[1] - 1 = 0;
for i in 2:N-1 loop
der(x2[i]) - a2[i] + x2[i] = 0;
a2[i + 1] - a2[i] - b2[i] = 0;
b2[i] - x2[i - 1] = 0;
end for;
b2[1] = 0;
b2[N] - x2[N - 1] = 0;
der(x2[1]) - a2[1] + x2[1] = 0;
der(x2[N]) - a2[N] + x2[N] = 0;
a2[2] - a2[1] - b2[1] = 0;
a2[1] - 1 = 0;
end Test4;
