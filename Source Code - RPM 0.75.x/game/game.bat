@set include=

del /q vm
mkdir vm
cd vm
set cc=sof2lcc -A -DQ3_VM -DQAGAME -DMISSIONPACK -D_WeaponTime -S -Wf-target=bytecode -Wf-g -I..\..\cgame -I..\..\game -I..\..\ui %1

%cc%  ../g_main.c
@if errorlevel 1 goto quit

%cc%  ../g_syscalls.c
@if errorlevel 1 goto quit

%cc%  ../bg_misc.c
@if errorlevel 1 goto quit
%cc%  ../bg_lib.c
@if errorlevel 1 goto quit
%cc%  ../bg_pmove.c
@if errorlevel 1 goto quit
%cc%  ../bg_slidemove.c
@if errorlevel 1 goto quit
%cc%  ../bg_player.c
@if errorlevel 1 goto quit
%cc%  ../bg_weapons.c
@if errorlevel 1 goto quit
%cc%  ../bg_gametype.c
@if errorlevel 1 goto quit
%cc%  ../q_math.c
@if errorlevel 1 goto quit
%cc%  ../q_shared.c
@if errorlevel 1 goto quit

%cc%  ../bots/ai_main.c
@if errorlevel 1 goto quit
%cc%  ../bots/ai_wpnav.c
@if errorlevel 1 goto quit
%cc%  ../ai_util.c
@if errorlevel 1 goto quit


%cc%  ../g_active.c
@if errorlevel 1 goto quit
%cc%  ../g_antilag.c
@if errorlevel 1 goto quit
%cc%  ../g_bot.c
@if errorlevel 1 goto quit
%cc%  ../g_client.c
@if errorlevel 1 goto quit
%cc%  ../g_cmds.c
@if errorlevel 1 goto quit
%cc%  ../g_combat.c
@if errorlevel 1 goto quit
%cc%  ../g_gametype.c
@if errorlevel 1 goto quit
%cc%  ../g_items.c
@if errorlevel 1 goto quit
%cc%  ../g_misc.c
@if errorlevel 1 goto quit
%cc%  ../g_missile.c
@if errorlevel 1 goto quit
%cc%  ../g_mover.c
@if errorlevel 1 goto quit
%cc%  ../g_session.c
@if errorlevel 1 goto quit
%cc%  ../g_spawn.c
@if errorlevel 1 goto quit
%cc%  ../g_svcmds.c
@if errorlevel 1 goto quit
%cc%  ../g_target.c
@if errorlevel 1 goto quit
%cc%  ../g_team.c
@if errorlevel 1 goto quit
%cc%  ../g_trigger.c
@if errorlevel 1 goto quit
%cc%  ../g_utils.c
@if errorlevel 1 goto quit
%cc%  ../g_weapon.c
@if errorlevel 1 goto quit

%cc%  ../rpm_main.c
@if errorlevel 1 goto quit
%cc%  ../rpm_combat.c
@if errorlevel 1 goto quit
%cc%  ../rpm_cmds.c
@if errorlevel 1 goto quit
%cc%  ../rpm_utils.c
@if errorlevel 1 goto quit
%cc%  ../rpm_svcmds.c
@if errorlevel 1 goto quit
%cc%  ../rpm_admincmds.c
@if errorlevel 1 goto quit
%cc%  ../rpm_actions.c
@if errorlevel 1 goto quit
%cc%  ../rpm_tcmds.c
@if errorlevel 1 goto quit
%cc%  ../rpm_refcmds.c
@if errorlevel 1 goto quit
%cc%  ../zg_bboxadj.c
@if errorlevel 1 goto quit

%cc%  ../g_bug.c
@if errorlevel 1 goto quit

%cc%  ../nv/nv_loadEnts.c
@if errorlevel 1 goto quit
%cc%  ../nv/nv_customGametypes.c
@if errorlevel 1 goto quit
%cc%  ../nv/nv_ip2Country.c
@if errorlevel 1 goto quit
%cc%  ../nv/nv_utils.c
@if errorlevel 1 goto quit

%cc%  ../nv/nv_adm.c
@if errorlevel 1 goto quitf

%cc%  ../nv/nv_editEnts.c
@if errorlevel 1 goto quit
%cc%  ../nv/nv_saveEnts.c
@if errorlevel 1 goto quit
%cc%  ../nv/nv_addEnts.c
@if errorlevel 1 goto quit



%cc%  ../zg_mover.c
@if errorlevel 1 goto quit

%cc%  ../crs/zai_main.c
@if errorlevel 1 goto quit
%cc%  ../crs/zg_waypoint.c
@if errorlevel 1 goto quit
%cc%  ../crs/zai_team.c
@if errorlevel 1 goto quit
%cc%  ../crs/zg_cmds.c
@if errorlevel 1 goto quit

sof2asm -f ../game
@if errorlevel 1 goto quit


copy *.map "..\..\..\SOF2\RPM\vm"
copy *.qvm "..\..\..\SOF2\RPM\vm"
:quit
cd ..
PAUSE
