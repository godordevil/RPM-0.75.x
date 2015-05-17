@set include=
cd gt_inf
del /q vm
mkdir vm
cd vm
set cc=sof2lcc -A -DQ3_VM -DMISSIONPACK -S -Wf-target=bytecode -Wf-g -I..\..\..\gametype -I..\..\gt_inf -I..\..\..\game %1

%cc%  ../gt_main.c
@if errorlevel 1 goto quit

%cc%  ../gt_inf.c
@if errorlevel 1 goto quit

%cc%  ../gt_sab.c
@if errorlevel 1 goto quit

%cc%  ../gt_dem.c
@if errorlevel 1 goto quit

%cc%  ../gt_raze.c
@if errorlevel 1 goto quit

%cc%  ../gt_cr.c
@if errorlevel 1 goto quit

%cc%  ../gt_ctc.c
@if errorlevel 1 goto quit

%cc%  ../../gt_syscalls.c
@if errorlevel 1 goto quit

%cc%  ../../../game/bg_lib.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_shared.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_math.c
@if errorlevel 1 goto quit

sof2asm -f ../gt_inf
@if errorlevel 1 goto quit

copy *.map "..\..\..\..\SOF2\RPM\vm"
copy *.qvm "..\..\..\..\SOF2\RPM\vm"

:quit
cd ..
PAUSE
