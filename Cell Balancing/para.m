C1 = 1E-6;  %filter cap [F]
C2 = 330E-6;    %Balancing cap [F]
R1 = 100e-3; %additional loop resistance [Ohm]
R2 = 3.4;   %Inductor resistance [Ohm]
L = 1E-3;   %Inductance [H]

%% MOSFET Parameters
Rds = 0.2;  %[Ohm]
Ciss = 400e-12; %[F]
Coss = 170e-12; %[F]
Crss = 42e-12;  %[F]

Cgd = Crss; %[F]
Cgs = Ciss - Cgd;   %[F]
Cds = Coss - Cgd;   %[F]