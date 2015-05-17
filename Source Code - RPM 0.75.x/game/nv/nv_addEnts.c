//#ifdef	_DEBUG
#include "../g_local.h"

#ifdef _MAPEDITOR

void NV_itemList		 (gentity_t *ent);
//void NV_itemMountedGun (gentity_t *ent);
void gametype_trigger_use(gentity_t *self, gentity_t *other, gentity_t *activator );

//void NV_Spawn_base      (gentity_t *ent)
//{
//	char		*origin;	
//	char		*angles2;
//	origin = va("%.0f %.0f %.0f",	ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	ent->client->ps.origin[2] - (ent->client->ps.viewheight));
//	angles2 = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
//
//	AddSpawnField("classname", "nv_model");
//	///AddSpawnField("model", "models/chunks/tray/tray.md3");
//	AddSpawnField("model", "models/chunks/canlid/canlid.md3");
//	AddSpawnField("origin", origin);
//	AddSpawnField("angles", angles2);
//	G_SpawnGEntityFromSpawnVars(qfalse);
//	level.numSpawnVars = 0;
//	level.numSpawnVarChars = 0;
//}
//{
//"target" "t49"
//"classname" "fx_play_effect"
//"origin" "152 -1240 -152"
//"spawnflags" "0"
//"effect" "fire/ground_fire"
//"count" "-1"
//}
/*
void NV_itemMountedGun (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]+20);
	
	AddSpawnField("classname", "mounted_gun");
	AddSpawnField("model", "models/objects/armory/m60_armory.md3");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
*/
void NV_itemEffect      (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	char		cmd[MAX_QPATH] = "\0";

	trap_Argv( 2, cmd, sizeof( cmd ) );

	//if(!cmd[0])
	//	return;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25));
 
	AddSpawnField("classname", "fx_play_effect");
	if(!cmd[0])
		AddSpawnField("effect", "effects/fire/blue_target_flame.efx");
	else
		AddSpawnField("effect", cmd);
	AddSpawnField("origin", origin);
	AddSpawnField("spawnflags", "0");
	AddSpawnField("angles", angles);
	AddSpawnField("count", "-1");

	//AddSpawnField("target", "t49");
	//AddSpawnField("mins", "100 100 100");
	//AddSpawnField("mins", "-100 -100 -100");
	//AddSpawnField("wait", "0.3");
	//AddSpawnField("health", "2");
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
#ifdef _LADDER
void NV_itemLadder		 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] - 46);
	
	AddSpawnField("classname", "nv_ladder");
	AddSpawnField("origin", origin);
	AddSpawnField("maxs", "15 15 200");
	AddSpawnField("mins", "-15 -15 0");
	AddSpawnField("model", "BLOCKED_TRIGGER");

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
#endif


void NV_SpawnBombTarget     (gentity_t *ent, char* model, qboolean virusModel)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);

	if(virusModel)
	{
		origin = va("%.0f %.0f %.0f",
			ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	((ent->client->ps.origin[2] + 42) - (ent->client->ps.viewheight * 1.25)));
	}
	else
	{
		origin = va("%.0f %.0f %.0f",
			ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25));
		AddSpawnField("mins", "-16 -16 -47");
		AddSpawnField("maxs", "16 16 73");
		AddSpawnField("spawnflags", "1");
	}
	AddSpawnField("classname", "nv_model");
	AddSpawnField("model", model);
	AddSpawnField("origin", origin);
	AddSpawnField("mins", "-16 -16 -47");
	AddSpawnField("maxs", "16 16 73");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	G_SpawnGEntityFromSpawnVars(qfalse);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
//void NV_Spawn_model      (gentity_t *ent, char* model)
void NV_Spawn_model      (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	char		cmd[MAX_QPATH] = "\0";

	trap_Argv( 2, cmd, sizeof( cmd ) );

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25));

	AddSpawnField("classname", "nv_model");

	// beams for CTF/INF markers
	if(strlen(cmd) < 4 ) 
	{
		AddSpawnField("model", "models/objects/Common/beam.md3");
	}
	else
	{
		AddSpawnField("model", cmd);
	}

	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	G_SpawnGEntityFromSpawnVars(qfalse);


	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}



//void NV_drawEntBox(gentity_t *ent)
//{	
//	gentity_t *te;
//	
//	if (!ent || ent == NULL)
//		return;
//
//	te = G_TempEntity(ent->s.origin, EV_BOTWAYPOINT);
//	VectorCopy(ent->s.origin, te->s.origin);
//	VectorCopy(ent->s.origin, te->s.angles);
//	te->s.angles[0] += 15;
//	te->s.weapon = 0x0000ff;
//	te->r.svFlags |= SVF_BROADCAST;
//	te->s.time = 1000;
//
//	Com_Printf(S_COLOR_YELLOW"Entity Box Added\n");
//}
void NV_itemWeapon		 (gentity_t *ent)
{
	gitem_t*	item = BG_FindWeaponItem ( ent->client->ps.weapon );
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	AddSpawnField("classname", item->classname);
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}

void NV_itemAmmo		 (gentity_t *ent)
{
	gitem_t*	item;
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	for ( item = bg_itemlist + 1 ; item->classname; item++) {
		if(item->giType == IT_AMMO && item->giTag == weaponData[ent->client->ps.weapon].attack[ATTACK_NORMAL].ammoIndex)
			break; 
	}
	
	if(!item->classname || item->classname == NULL)
		return;

	AddSpawnField("classname", item->classname);
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_itemaltAmmo		 (gentity_t *ent)
{
	gitem_t*	item;
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	for ( item = bg_itemlist + 1 ; item->classname; item++) {
		if(item->giType == IT_AMMO && item->giTag == weaponData[ent->client->ps.weapon].attack[ATTACK_ALTERNATE].ammoIndex)
			break; 
	}

	if(!item->classname || item->classname == NULL)
		return;

	AddSpawnField("classname", item->classname);
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}	

void NV_itemBackpack	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);
	
	AddSpawnField("classname", "pickup_backpack");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_itemHealthSmall  (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);
		
	AddSpawnField("classname", "pickup_health_small");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	
	G_SpawnGEntityFromSpawnVars(qfalse);
		
	level.numSpawnVars = 0;		
	level.numSpawnVarChars = 0;
}	
void NV_itemHealthBig	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);
		
	AddSpawnField("classname", "pickup_health_big");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);
	
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

}	
void NV_itemArmorBig	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	AddSpawnField("classname", "pickup_armor_big");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}		
void NV_itemArmorSmall	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	AddSpawnField("classname", "pickup_armor_small");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}	
void NV_itemArmorMedium	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 8.0f);

	AddSpawnField("classname", "pickup_armor_medium");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_itemBlueFlag	 (gentity_t *ent)
{
	gentity_t	*ent2 = NULL;
	gitem_t*	item;

	item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM ];
	ent2 = G_Spawn();
	ent2->item			= item;
	ent2->nextthink		= level.time + 200;
	ent2->think			= G_GametypeItemThink;
	ent2->classname		= "gametype_item";
	ent2->targetname	= "blue_flag";
	ent2->s.eType		= ET_ITEM;
	ent2->s.modelindex2 = 0;
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->touch			= Touch_Item;
	
	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	VectorSet(ent2->r.maxs, 15, 15, 30);
	VectorSet(ent2->r.mins, -15, -15, -15);
	
	trap_LinkEntity (ent2);
	
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

}

void NV_itemRedFlag		 (gentity_t *ent)
{
	gentity_t	*ent2 = NULL;
	gitem_t*	item;
	
	item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM + 1 ];
	ent2 = G_Spawn();
	ent2->item			= item;
	ent2->nextthink		= level.time + 200;
	ent2->think			= G_GametypeItemThink;
	ent2->classname		= "gametype_item";
	ent2->targetname	= "red_flag";
	ent2->s.eType		= ET_ITEM;
	//ent2->s.modelindex	= ent->item - bg_itemlist; //crashes but workins without
	ent2->s.modelindex2 = 0;
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->touch			= Touch_Item;
	
	VectorSet(ent2->r.maxs, 15, 15, 30);
	VectorSet(ent2->r.mins, -15, -15, -15);
	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	
	trap_LinkEntity (ent2);
	
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);
}		

void NV_itemVIPBeacon  (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1] - 90, 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) );

	AddSpawnField("classname",  "trigger_vip_beacon");
	AddSpawnField("model",		"models/objects/Hospital/carts/monitor_cart.md3");
	AddSpawnField("model2",		"models/objects/Hospital/carts/monitor_cart.md3");
	AddSpawnField("origin",		origin);
	AddSpawnField("spawnflags", "1");
	AddSpawnField("maxs", "35 35 70");
	AddSpawnField("mins", "-35 -35 0");
	AddSpawnField("angles",		angles);

	G_SpawnGEntityFromSpawnVars(qfalse);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_itemJumpPad(gentity_t *ent)
{
	char		origin[128];
	char		target[128];
	
	Com_sprintf(origin, 128, "%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) - 8);
		
	level.spawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	AddSpawnField("model", "models/objects/common/search_light.md3");
	AddSpawnField("classname", "nv_model");
	AddSpawnField("origin", origin);
	AddSpawnField("angles",  "-0 -90 -90");
	G_SpawnGEntityFromSpawnVars(qfalse);
		
	Com_sprintf(origin, 128, "%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25));
	Com_sprintf(target, 128, "jump_push%.0f", ent->client->ps.origin[0] + ent->client->ps.origin[1]);

	level.spawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	AddSpawnField("model", "BLOCKED_TRIGGER");
	AddSpawnField("classname", "trigger_multiple");
	AddSpawnField("origin", origin);
	AddSpawnField("target",  target);
	AddSpawnField("mins",  "-50 -50 0");
	AddSpawnField("maxs",  "50 50 10");
	G_SpawnGEntityFromSpawnVars(qfalse);
	
	level.spawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	
	AddSpawnField("classname", "target_push");
	AddSpawnField("origin", origin);
	AddSpawnField("targetname",  target);
	Com_sprintf(target, 128, "jump_pos%.0f", ent->client->ps.origin[0] + ent->client->ps.origin[1]);
	AddSpawnField("target",  target);
	AddSpawnField("mins",  "-50 -50 0");
	AddSpawnField("maxs",  "50 50 10");
	G_SpawnGEntityFromSpawnVars(qfalse);
	
	level.spawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	Com_sprintf(origin, 128, "%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] + 400);
	AddSpawnField("classname", "target_position");
	AddSpawnField("origin", origin);
	AddSpawnField("targetname",  target);
	AddSpawnField("mins",  "-50 -50 10");
	AddSpawnField("maxs",  "50 50 10");
	G_SpawnGEntityFromSpawnVars(qfalse);

	
	level.spawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}	


void NV_itemRedCapture	 (gentity_t *ent)
{
	char		*origin;
	gentity_t	*ent2 = NULL;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) );
		
	ent2 = G_Spawn();
	ent2->health		= 200;
	ent2->touch			= gametype_trigger_touch;
	ent2->classname		= "gametype_trigger";	
	ent2->model			= "NV_MODEL";	
	ent2->targetname	= "red_capture_point";
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->r.svFlags		= SVF_NOCLIENT;
	ent2->s.eType		= ET_GRAPPLE;	
	ent2->use			= gametype_trigger_use;

	//VectorSet(ent2->r.maxs, 32, 32, 32);
	//VectorSet(ent2->r.mins, -32, -32, -15);
	VectorSet(ent2->r.maxs, 15, 15, 30);
	VectorSet(ent2->r.mins, -15, -15, -15);
	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);	
	trap_LinkEntity (ent2);	
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

	NV_Spawn_model(ent);

}
void NV_itemBlueCapture  (gentity_t *ent)
{
	char		*origin;
	gentity_t	*ent2 = NULL;
	char		*angles;
	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) );

	ent2 = G_Spawn();
	ent2->health		= 201;
	ent2->touch			= gametype_trigger_touch;
	ent2->classname		= "gametype_trigger";	
	ent2->model			= "NV_MODEL";	
	ent2->targetname	= "blue_capture_point";
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->r.svFlags		= SVF_NOCLIENT;
	ent2->s.eType		= ET_GRAPPLE;	
	ent2->use			= gametype_trigger_use;

	VectorSet (ent2->r.maxs, 15, 15, 30);
	VectorSet (ent2->r.mins, -15, -15, -15);

	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	trap_LinkEntity (ent2);	

	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

	NV_Spawn_model(ent);
}	
void NV_itemCase		 (gentity_t *ent)
{
	if(NV_isVIP())
	{
		NV_itemVIPBeacon(ent);
		return;
	}
	if(NV_isOctf()) // Use this for the One CTF Item
	{
		NV_itemRedFlag(ent);
		return;
	}
	else
	{
		gentity_t	*ent2 = NULL;
		gitem_t*	item;
		
		item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM ];
		ent2 = G_Spawn();
		ent2->item			= item;
		ent2->nextthink		= level.time + 200;
		ent2->think			= G_GametypeItemThink;
		ent2->classname		= "gametype_item";
		ent2->targetname	= "briefcase";
		ent2->s.eType		= ET_ITEM;
		//ent2->s.modelindex	= ent->item - bg_itemlist;
		ent2->s.modelindex2 = 0;
		ent2->r.contents	= CONTENTS_TRIGGER;
		ent2->touch			= Touch_Item;
		
		VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
		VectorCopy(ent->client->ps.origin, ent2->s.origin);
		
		trap_LinkEntity (ent2);
		
		VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);
	}
}	

void NV_itemCaseCapture  (gentity_t *ent)
{
	char		*origin;
	gentity_t	*ent2 = NULL;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) );
		
	ent2 = G_Spawn();
	ent2->health		= 200;
	ent2->touch			= gametype_trigger_touch;
	ent2->classname		= "gametype_trigger";	
	ent2->model			= "NV_MODEL";	
	ent2->targetname	= "briefcase_destination";
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->r.svFlags		= SVF_NOCLIENT;
	ent2->s.eType		= ET_GRAPPLE;
	ent2->use			= gametype_trigger_use;

	VectorSet(ent2->r.maxs, 40, 40, 15);
	VectorSet(ent2->r.mins, -40, -40, -15);

	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	
	trap_LinkEntity (ent2);
	
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

	NV_Spawn_model(ent);

}

void NV_BSPTarget  (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] + 110 );

	AddSpawnField("classname",  "blocker");
	AddSpawnField("bspmodel",	"instances/GENERIC/WATERTOWER");
	AddSpawnField("model",		"BLOCKED_TRIGGER");
	AddSpawnField("health",		"10000");
	AddSpawnField("origin",		origin);
	//AddSpawnField("breakable",	"1");
	AddSpawnField("spawnflags", "16");

	//AddSpawnField("angles",		angles);
	//AddSpawnField("targetname",	"briefcase_destination_red");
	//G_SpawnGEntityFromSpawnVars(qtrue);
	//G_SpawnGEntityFromSpawnVars(qfalse);
	G_SpawnGEntityFromSpawnVars(qtrue);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}

void NV_RushTarget  (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", ent->client->ps.viewangles[0], ent->client->ps.viewangles[1], ent->client->ps.viewangles[2]);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1] - 20, 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.15) );

	AddSpawnField("classname",  "blocker");
	AddSpawnField("bspmodel",	"INSTANCES/Kamchatka/npc_jump1");
	AddSpawnField("model",		"BLOCKED_TRIGGER");
	AddSpawnField("health",		"10000");
	AddSpawnField("origin",		origin);
	//AddSpawnField("breakable",	"1");
	AddSpawnField("spawnflags", "32");

	//AddSpawnField("angles",		angles);
	//AddSpawnField("targetname",	"briefcase_destination_red");
	//G_SpawnGEntityFromSpawnVars(qtrue);
	//G_SpawnGEntityFromSpawnVars(qfalse);
	G_SpawnGEntityFromSpawnVars(qtrue);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_itemCaseCaptureBlue  (gentity_t *ent)
{
	char		*origin;
	gentity_t	*ent2 = NULL;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight * 1.25) );
		
	ent2 = G_Spawn();
	ent2->health		= 200;
	ent2->touch			= gametype_trigger_touch;
	ent2->classname		= "gametype_trigger";	
	ent2->model			= "NV_MODEL";	
	ent2->targetname	= "briefcase_destination_blue";
	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->r.svFlags		= SVF_NOCLIENT;
	ent2->s.eType		= ET_GRAPPLE;
	ent2->use			= gametype_trigger_use;

	if(NV_isRaze())
	{
		ent2->target	= "demolish1";
		VectorSet(ent2->r.maxs, 115, 115, 75);
		VectorSet(ent2->r.mins, -115, -115, -75);
		
		VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
		VectorCopy(ent->client->ps.origin, ent2->s.origin);
		ent2->s.origin[2] += 300;
		VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);
		ent2->r.currentOrigin[2] += 300;
		trap_LinkEntity (ent2);	

		NV_BSPTarget(ent);
		return;
	}
	
	VectorSet(ent2->r.maxs, 40, 40, 15);
	VectorSet(ent2->r.mins, -40, -40, -15);

	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

	if(NV_isRush())
	{		
		ent2->target	= "demolish1";
		ent2->spawnflags = nv_editMap.integer; // will determine the 'phase'
		NV_Spawn_Target_Models(ent2);
		trap_LinkEntity (ent2);	
		return;
		//NV_RushTarget(ent);
		//NV_SpawnBombTarget(ent, "models/objects/Armory/virus.md3", qtrue);
		//Todo: spawn Rush style targets
		//"INSTANCES/Kamchatka/npc_jump1"
	}


	if(NV_isDem() || NV_isSab())
	{
		ent2->target	= "demolish1";
		//NV_SpawnBombTarget(ent, "models/objects/Hospital/carts/monitor_cart.md3", qfalse);
		NV_SpawnBombTarget(ent, "models/objects/Armory/virus.md3", qtrue);
		NV_SpawnBombTarget(ent, "models/objects/Common/barrels/barrel_hk.md3", qfalse);
	}
	trap_LinkEntity (ent2);	

	NV_Spawn_model(ent);
}


void NV_itemCaseCaptureRed  (gentity_t *ent)
{
	gentity_t	*ent2 = NULL;
		
	// Use only BLUE for Raze.  Bomb 
	if(NV_isRaze() || NV_isRush() || NV_isDem())
	{
		NV_itemCaseCaptureBlue (ent);
		return;
	}
	ent2 = G_Spawn();
	ent2->health		= 201;
	ent2->touch			= gametype_trigger_touch;
	ent2->classname		= "gametype_trigger";	
	ent2->model			= "NV_MODEL";	
	ent2->targetname	= "briefcase_destination_red";

	ent2->r.contents	= CONTENTS_TRIGGER;
	ent2->r.svFlags		= SVF_NOCLIENT;
	ent2->s.eType		= ET_GRAPPLE;
	ent2->use			= gametype_trigger_use;
		
	//if(NV_isRaze())
	//{
	//	//ent2->targetname	= "briefcase_destination_blue";
	//	//ent2->health		= 200;
	//	ent2->target		= "demolish2";
	//	VectorSet(ent2->r.maxs, 115, 115, 75);
	//	VectorSet(ent2->r.mins, -115, -115, -75);

	//	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	//	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	//	ent2->s.origin[2] += 300;
	//	trap_LinkEntity (ent2);	
	//	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);
	//	ent2->r.currentOrigin[2] += 300;
	//	NV_BSPTarget(ent);
	//	return;
	//}

	VectorSet(ent2->r.maxs, 40, 40, 15);
	VectorSet(ent2->r.mins, -40, -40, -15);

	VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1], 0);
	VectorCopy(ent->client->ps.origin, ent2->s.origin);
	VectorCopy(ent->r.currentOrigin, ent2->r.currentOrigin);

	if(/*NV_isDem() ||*/ NV_isSab())
	{
		VectorSet (ent2->s.angles, 0, ent->client->ps.viewangles[1] - 90, 0);
		ent2->target	= "demolish2";
		NV_SpawnBombTarget(ent, "models/objects/Hospital/carts/monitor_cart.md3", qfalse);
		//NV_SpawnBombTarget(ent, "models/objects/Armory/virus.md3", qtrue);
		//NV_SpawnBombTarget(ent, "models/objects/Common/barrels/barrel_hk.md3", qfalse);
	}
	trap_LinkEntity (ent2);	

	NV_Spawn_model(ent);

}
void NV_itemLocName		 (gentity_t *ent)
{
	char		*origin;
	char		*arg = "\0";

	arg = ConcatArgs( 2 );

	if(trap_Argc() <= 2){	
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"^3Usage: addItem locname ''locationname''\n\"") );
		return;
	}

	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2]);

	AddSpawnField("classname", "target_location");
	AddSpawnField("origin", origin);
	AddSpawnField("message", arg);
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;


	/// Set the locationLinked to false so it will add it to CS_LOCATIONS 
	level.locationLinked = qfalse;	
	///
}	
void NV_itemSpawn		 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		
	AddSpawnField("classname", "gametype_player");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	if(NV_isDom() || NV_isKoth())
	{
		//AddSpawnField("spawnflags", va("%i",ent->client->sess.team));
		AddSpawnField("spawnflags", "0");
	}
	else if(NV_isRush())
	{
		AddSpawnField("spawnflags", "0");
		AddSpawnField("count", nv_editMap.string);
	}
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	
	/// This will put a model showing as an origin marker.. just for visual effects
	/// ... while editing
	/*
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		///ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		ent->client->ps.origin[2] - (ent->client->ps.viewheight));

	AddSpawnField("classname", "model_static");
	///AddSpawnField("model", "models/players/sarge/head.md3");
	///AddSpawnField("model", "models/objects/Colombia/misc/clanmore.md3");
	AddSpawnField("model", "nv_spawnMarker");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	G_SpawnGEntityFromSpawnVars(qfalse);


	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	///Com_Printf("added spawn for team: %i\n" ent->client->sess.team);
	*/
	
}
void NV_itemBlueSpawn	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	
	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		
	AddSpawnField("classname", "gametype_player");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	AddSpawnField("spawnflags", "2");

	if(nv_editMap.integer > 1 && NV_isRush())
	{
		AddSpawnField("spawnflags", "0");
		AddSpawnField("count", nv_editMap.string);
	}
	else
	{
		AddSpawnField("spawnflags", "2");
	}

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;	

}
void NV_itemRedSpawn	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		
	AddSpawnField("classname", "gametype_player");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);

	if(nv_editMap.integer > 1 && NV_isRush())
	{
		AddSpawnField("spawnflags", "0");
		AddSpawnField("count", nv_editMap.string);
	}
	else
	{
		AddSpawnField("spawnflags", "1");
	}
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	
	/// This will put a model showing as an origin marker.. just for visual effects
	/// ... while editing
	/*
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		///ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		ent->client->ps.origin[2] - (ent->client->ps.viewheight));

	AddSpawnField("classname", "model_static");
	///AddSpawnField("model", "models/players/sarge/head.md3");
	///AddSpawnField("model", "models/objects/Colombia/misc/clanmore.md3");
	AddSpawnField("model", "nv_spawnMarker");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	///Com_Printf("added spawn for team: RED %i\n", adm->client->sess.team);
	*/
}
void NV_itemBsp			 (gentity_t *ent)
{
	char			*origin;
	//char			*angles;
	char			cmd[MAX_QPATH] = "\0";
	char			cmd2[2] = "\0";
	int				count = 0;
	int				i=0;
	fileHandle_t	f;
	extern int		uniqueBSPS;

	if(uniqueBSPS == MAX_SUB_BSP) {
		CPe(va("print \"^1WARNING: ^3Too many BSP's in the current map\n", cmd));
		return;
	}

	trap_Argv( 2, cmd, sizeof( cmd ) );
	trap_Argv( 3, cmd2, sizeof( cmd2 ) );
	
	if(!cmd[0])
		return;
	
	if(cmd2[0])
		count = atoi(cmd2);

	///Make sure it exists
	if(trap_FS_FOpenFile( va("maps\\%s.bsp", cmd), &f, FS_READ ) <= 0)
	{
		CPe(va("print \"^3%s ^7not found\n", cmd));
		return;
	}	
	trap_FS_FCloseFile(f);
	///

	while(cmd[i])
	{		
		if(cmd[i] == '\\')
			cmd[i] = '/';
		i++;
	}
	
	//angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 
		ent->client->ps.origin[1], 
		///ent->client->ps.origin[2] - (ent->client->ps.viewheight) + 45.0f);
		ent->client->ps.origin[2] - (ent->client->ps.viewheight));
		
	AddSpawnField("classname", "blocker");
	AddSpawnField("bspmodel",	cmd);
	AddSpawnField("origin",		origin);
	//AddSpawnField("angles",		angles);
	AddSpawnField("model",		"BLOCKED_TRIGGER");

	if(NV_isRush())
	{
		AddSpawnField("count", nv_editMap.string);
		//ent2->spawnflags = nv_editMap.integer;
	}
	else
	{
		AddSpawnField("count",		 cmd2);
	}

	///G_SpawnGEntityFromSpawnVars(qfalse);
	G_SpawnGEntityFromSpawnVars(qtrue);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	
	CP(va("print \"^3Created bsp: ^5%i ^7of ^5%i\n", uniqueBSPS, MAX_SUB_BSP));
	///Com_Printf("added BSP:\n", cmd);
}

void NV_itemBlocker		 (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	//char		mins[32]="\0", maxs[32]="\0";
	char		cmd[2] = "\0";

	trap_Argv( 2, cmd, sizeof( cmd ) );

	//Com_sprintf ( maxs, sizeof(maxs), "%.0f %.0f %.0f",  20, 20, 20);
	//Com_sprintf ( mins, sizeof(mins), "-%.0f -%.0f -%.0f",  20, 20, 0);
	
	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] - 46);
	
	AddSpawnField("classname", "blocked_trigger");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	//AddSpawnField("maxs", maxs);
	//AddSpawnField("mins", mins);
	AddSpawnField("maxs", "30 30 60");
	AddSpawnField("mins", "-30 -30 0");
		
	if(NV_isRush())
	{
		AddSpawnField("count", nv_editMap.string);
		//ent2->spawnflags = nv_editMap.integer;
	}
	else
	{
		AddSpawnField("count", cmd);
	}

	AddSpawnField("model", "BLOCKED_TRIGGER");

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
#if _DEBUG
void NV_itemTerrain		 (gentity_t *ent)
{
	char		*origin;
	//char		*angles;
	char		cmd[2] = "\0";
	
	//angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] - 46);
	
	AddSpawnField("classname", "nv_terrain");
	AddSpawnField("origin", origin);
	//AddSpawnField("angles", angles);
	AddSpawnField("maxs", "5000 5000 50");
	AddSpawnField("mins", "-5000 -5000 0");

	AddSpawnField("minx" ,"-1000");
	AddSpawnField("miny", "-1000");
	AddSpawnField("minz", "0");
	AddSpawnField("maxx", "1000");
	AddSpawnField("maxy", "1000");
	AddSpawnField("maxz", "1000");	
	AddSpawnField("seed", "2");	
	//AddSpawnField("model",	"NV_MODEL");
	//AddSpawnField("model", "BLOCKED_TRIGGER");
	AddSpawnField("model", "BLOCKED_TRIGGER");
	AddSpawnField("heightmap",	"heightmaps/random_small");
	AddSpawnField("densitymap",	"heightmaps/density_small");

///	AddSpawnField("terrainId",	"2");
///	AddSpawnField("shader",	"2");
///	AddSpawnField("instancedef","maps/arioche/small");
	
	///AddSpawnField("modelIndex", "5");	
	
	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
#endif

void NV_itemBlueGroup	 (gentity_t *ent)
{
	if(NV_isVIP())
	{
		NV_itemVIPBeacon(ent);
		return;
	}
	if(NV_isOctf()) // Flags are capture zones in One CTF
	{
		//NV_itemRedFlag(ent);
		NV_itemBlueCapture(ent);
		return;
	}
	if(NV_isSab() || NV_isDem() || NV_isRaze() || NV_isRush())
	{
		NV_itemCaseCaptureBlue(ent);
		return;
	}
	if (NV_isInf())
	{
		NV_itemCaseCapture(ent);
		return;
	}
	NV_itemBlueFlag(ent);
	if(!NV_isDom() && !NV_isKoth())
		NV_itemBlueCapture(ent);
	else
		NV_Spawn_model(ent);
}
void NV_itemRedGroup	 (gentity_t *ent)
{
	if(NV_isVIP())
	{
		NV_itemVIPBeacon(ent);
		return;
	}
	if(NV_isOctf()) // Flags are capture zones in One CTF
	{
		NV_itemRedCapture(ent);
		return;
	}
	if(NV_isSab() || NV_isDem() || NV_isRaze() || NV_isRush())
	{
		NV_itemCaseCaptureRed(ent);
		return;
	}	
	if (NV_isInf())
	{
		NV_itemCaseCapture(ent);
		return;
	}
	NV_itemRedFlag(ent);

	if(!NV_isDom() && !NV_isKoth())
		NV_itemRedCapture(ent);
	else
		NV_Spawn_model(ent);
}

void NV_itemStartTeleport(gentity_t *ent)
{
	char		*origin;
	char		*arg = "\0";

	arg = ConcatArgs( 2 );
	if(trap_Argc() <= 2){	
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"^3Usage: ^7addItem startTeleport ''destination name''\nThe ^3destination name ^7will be used to match the ^3endteleport ^7location") );
		return;
	}

	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]);

	AddSpawnField("classname", "blocked_teleporter");
	AddSpawnField("origin", origin);
	AddSpawnField("mins",  "-100 -100 -100");
	AddSpawnField("maxs",  "100 100 100");
	AddSpawnField("model", "BLOCKED_TRIGGER");
	AddSpawnField("count", "5");
	AddSpawnField("target", arg);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}	
void NV_itemEndTeleport	 (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	char		*arg = "\0";
	char		location[64] = "";

	arg = ConcatArgs( 2 );

	if(trap_Argc() <= 2){	
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"^3Usage: ^7addItem endTeleport ''Location name''\nThe ^3Location name ^1MUST ^7be the same as the ^3destination name ^7in the ^3startTeleport ^7command\n\"") );
		return;
	}
	
	Team_GetLocationMsg(ent, location, sizeof(location));
	origin = va("%.0f %.0f %.0f", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]);
	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);

	AddSpawnField("classname", "target_location");
	AddSpawnField("origin", origin);
	AddSpawnField("targetname", arg);
	AddSpawnField("message", location);
	AddSpawnField("angles", angles);

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}	



void NV_newShader		 (gentity_t *ent)
{
	//char		cmd[MAX_QPATH] = "\0";
	char		cmd[MAX_QPATH] = "textures/skies/inf_shop";
	char		cmd2[MAX_QPATH] = "textures/skies/jor1";
	extern		int remapCount;

	trap_Argv( 3, cmd, sizeof( cmd ) );
	trap_Argv( 2, cmd2, sizeof( cmd2 ) );
//&g_entities[128].targetShaderName
	//if (ent->targetShaderName && ent->targetShaderNewName) 
	{
		float f = level.time * 0.001;
		//AddRemap(ent->targetShaderName, ent->targetShaderNewName, f);
	//	AddRemap("textures/hospital/metbright_env", "textures/shop/trashcan_gray", f);
		AddRemap(cmd2, cmd, f);
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}
	CPe(va("print \"^3Shader Remapped: ^5%i ^7of ^5%i\n", remapCount, 128));
}

void NV_itemNoDrop		 (gentity_t *ent)
{
	trap_Cvar_Set("g_noDropOrigin",	va("%.0f",  ent->client->ps.origin[2]) );
	CPe(va("print \"^3NO DROP Zone added at: ^5%0.f\n", ent->client->ps.origin[2]));

}


//
//#if _DEBUG
//static fileHandle_t shaderSaveFile;
//extern char *buffer;
//
//void QDECL NV_writeLine( const char *text, ... )
//{
//	char		string[1024] = "\0";
//	va_list		argptr;
//
//	va_start( argptr, text );
//	vsprintf( string, text, argptr );
//	va_end( argptr );
//		
//	if ( !shaderSaveFile )
//		return;
//
//	trap_FS_Write( string, strlen( string ), shaderSaveFile );
//	trap_FS_Write( "\n", 1, shaderSaveFile);
//}
//void NV_ParseShadersList( char* filename )
//{
//	int			len;
//	fileHandle_t f;
//	const char	*ptoken = "";
//	char		*token = "";
//	char		shader[MAX_QPATH]="\0";
//	int			i = 0;
//	int			open = 0;
//
//	len = trap_FS_FOpenFile(filename, &f, FS_READ );
//	if(!f){
//		Com_Printf("ERROR: File not found!\n");
//		return;
//	}
//
//	trap_FS_Read( buffer, len, f );
//
//	if(len > 131072)	
//		len = 131072;
//	buffer[len] = 0;
//	ptoken = buffer;
//	
//	NV_writeLine (va("========= %s =========", filename));
//
//	while(ptoken) {	
//		token = COM_ParseExt(&ptoken, qtrue);
//		if (!token || token[0] == 0) 
//			continue;	
//		if(i == 0){
//			i++;
//			NV_writeLine (token);
//		}
//
//		if(!strcmp(token, "{")) {
//			open++;
//			continue;
//		}
//		if(!strcmp(token, "}")){
//			open--;
//			continue;
//		}
//		if (!token || token[0] == 0) 
//			continue;
//		if(open)
//			continue;
//		NV_writeLine (token);
//	}
//	trap_FS_FCloseFile(f);
//}
//
//void NV_shadersLists ( gentity_t *ent )
//{
//	char		filename[MAX_QPATH];
//	char		filelist[MAX_QPATH * 63];
//	char*		fileptr;
//	int			filelen;
//	int			filecount;
//	int			i;
//
//	filecount = trap_FS_GetFileList("shaders\\", ".shader", filelist, 65000 );
//	fileptr   = filelist;
//		
//	if(!trap_FS_FOpenFile( "maps/shaders/allshaders.txt", &shaderSaveFile, FS_WRITE))
//	{
//		Com_Printf("Error:  Can not open file -> ^3maps\\shaders\\allshaders.txt");
//		return;
//	}
//
//
//	for ( i = 0; i < filecount; i++, fileptr += filelen+1) 
//	{
//		filelen = strlen(fileptr);
//		strcpy(filename, "shaders\\");
//		strcat(filename, fileptr );		
//		NV_ParseShadersList ( filename );
//	}
//	trap_FS_FCloseFile(shaderSaveFile);
//}
//#endif


typedef struct 
{
	char	*cmd;
	void	(*function)(gentity_t *ent);
	const char	*usage;

} mapEntCommand_t;

static mapEntCommand_t commands[] = 
{
	{ "weapon",			NV_itemWeapon,			"Current Weapon"					},
	{ "ammo",			NV_itemAmmo,			"Current Weapon's ammo"				},
	{ "altAmmo",		NV_itemaltAmmo,			"Current Weapon's Alt Ammo"			},	
	
	{ "backpack",		NV_itemBackpack,		"Back Pack"							},
	{ "healthsmall",	NV_itemHealthSmall,		"Small health Pack"					},
	{ "healthbig",		NV_itemHealthBig,		"Big health Pack"					},
	{ "armorbig",		NV_itemArmorBig,		"Big Armor"							},
	{ "armorsmall",		NV_itemArmorSmall,		"Small Armor"						},
	{ "armormedium",	NV_itemArmorMedium,		"Medium Armor"						},

	{ "blueflagGroup",	NV_itemBlueGroup,		"Blue trigger group"				},
	{ "redflagGroup",	NV_itemRedGroup,		"Blue trigger group"				},
	{ "blueflag",		NV_itemBlueFlag,		"Blue Flag"							},
	{ "redflag",		NV_itemRedFlag,			"Red Flag"							},
	{ "bluecapture",	NV_itemBlueCapture,		"Blue Capture Point"				},
	{ "redcapture",		NV_itemRedCapture,		"Red Capture Point"					},
	{ "case",			NV_itemCase,			"Briefcase Or OCTF Flag"							},
	
	{ "spawn",			NV_itemSpawn,			"DM Spawn point"					},
	{ "bluespawn",		NV_itemBlueSpawn,		"Blue team Spawn point"				},
	{ "redspawn",		NV_itemRedSpawn,		"Red team Spawn point"				},
	
	{ "locName",		NV_itemLocName,			"Map location Name"					},
	{ "startTeleport",	NV_itemStartTeleport,	"Add a teleport trigger"			},
	{ "endTeleport",	NV_itemEndTeleport,		"Add the location of the teleport trigger"},
	{ "bsp",			NV_itemBsp,				".bsp instance"						},
	{ "blocker",		NV_itemBlocker,			"Area Blocker"						},
	{ "model",			NV_Spawn_model,			"Add model"							},
#if _DEBUG
	{ "terrain",		NV_itemTerrain,			"Add Terrain"						},
	//{ "makeshaderlist",	NV_shadersLists,		"makeshaderlist"			},
	//{ "mounted",		NV_itemMountedGun,		"Mounted Gun"						},	
#endif

#ifdef _LADDER
	{ "ladder",			NV_itemLadder,			"ladder"							},	
#endif

	{ "nodrop",			NV_itemNoDrop,			"Set origin as NODROP area (resets gametype items)"},	
	{ "jump",			NV_itemJumpPad,			"Jump pad group"					},	
	{ "effect",			NV_itemEffect,			"Add Effect"						},	
	{ "shader",			NV_newShader,			"Remap Shader"						},
	{ "save",			NV_createEntFile,		"Save to *.ent file"	},
	{ "?",				NV_itemList,			"Help"		},
	

};

qboolean NV_mapEntCommands( gentity_t *ent ) 
{
	char arg[16] = "\0";
	int		i;

	/// Hault and give a warning because the game will crash. Allow 50 ents
	/// ... for safety incase some are created w/out freeing (missles, players, etc) 
	if(level.num_entities == MAX_GENTITIES - 50){
		CPe(va("cp \"^1WARNING! - Adding more ^3ENTITIES ^1could crash the server!!\n\""));
		return qfalse;
	}
	
	trap_Argv( 1, arg, sizeof(arg));

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		if ( !Q_stricmp( arg, commands[i].cmd ) ) {
			commands[i].function(ent);
			return qtrue;
		}
	}
	return qfalse;
}

void NV_itemList		(gentity_t *ent)
{
	int		i;
	int		len = 0;
	char	string[1024] = "";
	char	*line;

	CPe(va("print \"@---------------------------------------\n\""));

	for( i = 0; i < sizeof(commands)/sizeof(commands[0]) - 1; i++) {
		line = va("  ^4%-12s ^3- ^7%s\n", commands[i].cmd, commands[i].usage);
		strncpy( &string[len], line, 128 );
		len += strlen( line );

		if ( len > 750 ){
			string[len++] = 0;
			CPe(va("print \"@%s\"", string));
			len = 0;
		}
	}
	if ( len ){
		string[len++] = 0;
		CPe(va("print \"@%s\"", string));
	}
	CPe(va("print \"@---------------------------------------\n\""));

	///CPe(va("print \"NOTE: Anything between ^3< >^7 is ^1OPTIONAL^7!\nTo see more info on command type a '?' after the cmd (ex. /adm pop ?)\n\""));
}


#endif

