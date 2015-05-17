@set include=
cd gt_ctf
del /q vm
mkdir vm
cd vm
set cc=sof2lcc -A -DQ3_VM -DMISSIONPACK -S -Wf-target=bytecode -Wf-g -I..\..\..\gametype -I..\..\gt_ctf -I..\..\..\game %1


%cc%  ../gt_main.c
@if errorlevel 1 goto quit
%cc%  ../gt_ctf.c
@if errorlevel 1 goto quit
%cc%  ../gt_rush.c
@if errorlevel 1 goto quit
%cc%  ../gt_con.c
@if errorlevel 1 goto quit
%cc%  ../gt_dom.c
@if errorlevel 1 goto quit
%cc%  ../gt_actf.c
@if errorlevel 1 goto quit
%cc%  ../gt_cctf.c
@if errorlevel 1 goto quit
%cc%  ../gt_tctf.c
@if errorlevel 1 goto quit
%cc%  ../gt_octf.c
@if errorlevel 1 goto quit
%cc%  ../gt_koth.c
@if errorlevel 1 goto quit
%cc%  ../gt_op.c
@if errorlevel 1 goto quit
%cc%  ../gt_vip.c
@if errorlevel 1 goto quit

%cc%  ../../gt_syscalls.c
@if errorlevel 1 goto quit

%cc%  ../../../game/bg_lib.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_shared.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_math.c
@if errorlevel 1 goto quit

sof2asm -f ../gt_ctf
@if errorlevel 1 goto quit


copy *.map "..\..\..\..\SOF2\RPM\vm"
copy *.qvm "..\..\..\..\SOF2\RPM\vm"

:quit
cd ..
PAUSE