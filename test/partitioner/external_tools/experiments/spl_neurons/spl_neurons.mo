model spl_neurons
  import math;
  constant Integer N = 10000;
  constant Integer M = 100;
  constant Integer OFFSET = 100;
  constant Integer CONN = 10;
  Real v[N](each start=-60),gex[N],ginh[N];
  parameter Real tau=20, vrest=-60, vthres=-50,Trefrac=5,Eex=0,Einh=-80,tauex=5,tauinh=10,dgex=0.4,dgin=1.6;
  discrete Real  active[N](each start=1),tfire[N](each start=10000);
  discrete Real  nextev[M](each start=random(1));
  parameter Real Period=10,lastTime=100;
  parameter Real ex[N];
  discrete Real exReinit[N];
  discrete Real inhReinit[N];
  discrete Real nextGlobalTime[M];
  discrete Real inputs[M];
  initial algorithm

    /*for i in 1:600 loop
      ex[i]:=1;
    end for;
    for i in 601:800 loop
      ex[i]:=0;
    end for;

    for i in 801:1500 loop
      ex[i]:=1;
    end for;*/

  equation
    for i  in 1:N loop
      der(v[i])=active[i]*((vrest-v[i])+gex[i]*(Eex-v[i])+ginh[i]*(Einh-v[i]))/tau;
      der(gex[i])=-gex[i]/tauex;
      der(ginh[i])=-ginh[i]/tauinh;
      end for;

  algorithm
    for i in 1:N loop
      when exReinit[i] > 0.5 then
         reinit(gex[i],gex[i]+dgex);
         exReinit[i]:=0;
      end when;
    end for;

    for i in 1:N loop
      when inhReinit[i] > 0.5 then
         reinit(ginh[i],ginh[i]+dgin);
         inhReinit[i]:=0;
      end when;
    end for;

    for i in 1:N loop
      when time>tfire[i]+Trefrac then
        active[i]:=1;
      end when;
    end for;

    for i in 1:M loop
      when time>nextev[i] then
        exReinit[i+100] := 1;
         inputs[i] := inputs[i] + 1;
        nextev[i]:=time+10*sin(i*inputs[i])^2;
      end when;
    end for;

    for i in 1:N-OFFSET-CONN loop
      when v[i]*ex[i]>vthres then
        reinit(v[i],vrest);
        reinit(ginh[i],0);
        reinit(gex[i],0);
         exReinit[i+100] := 1;
         exReinit[i+101] := 1;
         exReinit[i+102] := 1;
         exReinit[i+103] := 1;
         exReinit[i+104] := 1;
         exReinit[i+105] := 1;
         exReinit[i+106] := 1;
         exReinit[i+107] := 1;
         exReinit[i+108] := 1;
         exReinit[i+109] := 1;
        active[i]:=0;
        tfire[i]:=time;
      end when;
    end for;

    for i in 1:N-OFFSET-CONN loop
      when v[i]*(1-ex[i])>vthres then
        reinit(v[i],vrest);
        reinit(ginh[i],0);
        reinit(gex[i],0);
         inhReinit[i+100] := 1;
         inhReinit[i+101] := 1;
         inhReinit[i+102] := 1;
         inhReinit[i+103] := 1;
         inhReinit[i+104] := 1;
         inhReinit[i+105] := 1;
         inhReinit[i+106] := 1;
         inhReinit[i+107] := 1;
         inhReinit[i+108] := 1;
         inhReinit[i+109] := 1;  
        active[i]:=0;
        tfire[i]:=time;
      end when;
    end for;
	annotation(
    experiment(
        MMO_Description = "",
        MMO_Solver = QSS2,
        MMO_SymDiff = false,
        MMO_Period = {300/5000},
        MMO_Parallel = true,
        MMO_PartitionMethod = Scotch,
        MMO_LPS = 2,
        MMO_DT_Min = 0.5,
        MMO_Output = {v[1:N/20:N]},
        MMO_OutputType = CI_Sampled,
        Jacobian = Dense,
        MMO_BDF_PDepth = 1,
        MMO_BDF_Max_Step = 0,
        StartTime = 0.0,
        StopTime = 300,
        Tolerance = {1e-3},
        AbsTolerance = {1e-3}
    )
);









end spl_neurons;
