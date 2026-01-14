model adv_dif_reac
  constant Integer N=1000;
  parameter Real a=1;
  parameter Real d=1e-4;
  parameter Real r=10;
  parameter Real L=10;
  parameter Real dx=L/N;
  Real u[N];

initial algorithm
  for i in 1:N/3 loop
    u[i]:=1;
  end for;

equation
  der(u[1])=-a*(u[1]-1)/dx+d*(-2*u[1]+1)/(dx^2)+r*(u[1]^2)*(1-u[1]);
  for i in 2:N loop
    der(u[i])=-a*(u[i]-u[i-1])/dx+ d*(-2*u[i]+u[i-1])/(dx^2)+ r*(u[i]^2)*(1-u[i]);
  end for;
	annotation(

	experiment(
		MMO_Description=" Advection",
		MMO_Solver=LIQSS2,
		MMO_Parallel=true,
		MMO_PartitionMethod=Metis,
		MMO_LPS=2,
		MMO_DT_Min= 2,
		MMO_Output={u[N]},
		Jacobian=Dense,
		MMO_BDF_PDepth=1,
		MMO_BDF_Max_Step= 0,
		StartTime= 0.0,
		StopTime=10,
		Tolerance={1e-3},
		AbsTolerance={1e-3}
	));
end adv_dif_reac;
