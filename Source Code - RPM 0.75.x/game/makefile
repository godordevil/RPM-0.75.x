
# gcc compiler
CC = gcc

# Compiler Flags
CFLAGS += -I../../cgame -I../../game -I../../ui -I../ghoul2
CFLAGS += -w -Wall -c -fPIC -D__linux__ -D__GCC__
CFLAGS += -DQAGAME -DMISSIONPACK -D_WeaponTime

# static libs
BSTATIC = -Wl,-Bstatic
LIBS = -ldl -lm

# output file
# TARGET = /home/xl/Documents/Sof2/RPM/sof2mp_gamei386.so
TARGET = ./sof2mp_gamei386.so

# Object Files
OFILES = g_main.o \
g_syscalls.o \
bg_lib.o \
bg_misc.o \
bg_player.o \
bg_pmove.o \
bg_slidemove.o \
bg_weapons.o \
bg_gametype.o \
q_math.o \
q_shared.o \
bots/ai_main.o \
bots/ai_wpnav.o \
ai_util.o \
g_active.o \
g_antilag.o \
g_bot.o \
g_client.o \
g_cmds.o \
g_combat.o \
g_gametype.o \
g_items.o \
g_misc.o \
g_missile.o \
g_mover.o \
g_session.o \
g_spawn.o \
g_svcmds.o \
g_target.o \
g_team.o \
g_trigger.o \
g_utils.o \
g_weapon.o \
rpm_combat.o \
rpm_utils.o \
rpm_cmds.o \
rpm_svcmds.o \
rpm_admincmds.o \
rpm_actions.o \
rpm_tcmds.o \
rpm_refcmds.o \
rpm_main.o \
zg_bboxadj.o \
crs/zai_main.o \
crs/zg_waypoint.o \
crs/zai_team.o \
crs/zg_cmds.o \
zg_mover.o \
g_bug.o \
nv/nv_loadEnts.o \
nv/nv_customGametypes.o \
nv/nv_ip2Country.o \
nv/nv_utils.o \
nv/nv_adm.o \
nv/nv_saveEnts.o \
nv/nv_editEnts.o \
nv/nv_addEnts.o \



$(TARGET): 	$(OFILES)
		$(CC) -fPIC -shared -o $(TARGET) $(OFILES) $(BSTATIC) $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

save:
	cp /media/storage/sof2/RPM/sof2mp_gamei386.so ./out/
	
clean:
	rm -f $(OFILES)
	rm -f $(TARGET)

