model Test2
constant Integer N = 100;
Real a[N], x[N], b[N];
equation
for i in 2:N-1 loop
der(x[i]) - a[i] + x[i] = 0; //F1
a[i + 1] - a[i] - b[i] = 0; //F2
b[i] - x[i - 1] = 0; //F3
end for;
b[1] = 0; //F4
b[N] - x[N - 1] = 0; //F5
der(x[1]) - a[1] + x[1] = 0; //F6
der(x[N]) - a[N] + x[N] = 0; //F7
a[2] - a[1] - b[1] = 0; //F8
a[N] = 1; //F9
end Test2;
