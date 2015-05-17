// Copyright (C) 2004-2005 Ryan Powell

//  This file is part of RPM.

//  RPM is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation;either version 2 of the License, or
//  (at your option) any later version.

//  RPM is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with RPM; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "g_local.h"

void RPM_ReadyCheck (gentity_t *ent)
{
#ifdef _SOF2_BOTS
	if (ent->r.svFlags & SVF_BOT)
	{
		if(!ent->client->pers.ready)
		{
			ent->client->pers.ready = 1;
		}
		return;
	}
#endif
	if(level.time < ent->client->pers.readyMessageTime + 3000)
	{
		return;
	}

	if(g_doWarmup.integer == 2 && !ent->client->pers.ready)
	{
		//trap_SendServerCommand( ent-g_entities, va("cp \"@You are not ready for the match\nPlease type ^1/ready ^7in console\n^7or\n^1!r ^7in chat!\n\""));
		trap_SendServerCommand( ent-g_entities, va("cp \"@You are not ready for the match\nPlease type ^1!r ^7in chat!\n\""));
		ent->client->pers.readyMessageTime = level.time;
	}
}
