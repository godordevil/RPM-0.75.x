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
#include "ui_local.h"
#include "../game/bg_public.h"

/*
===============
UI_LoadSongs
===============
*/
void UI_LoadSongs(void) 
{
	int				i, filelen;
	char			MP3Files[4096];
	char			*fileptr;

	uiInfo.songCount = trap_FS_GetFileList(/*ui_musicFolder.string*/ "RPM_Music", ".mp3", MP3Files, 4096 );
	
	//save the actual song count for the ui scripts
	trap_Cvar_Set("ui_songCount", va("%d", uiInfo.songCount));

	//Count the defaut game music as 1 song as well
	uiInfo.songCount++;

	//256 songs MAX
	if (uiInfo.songCount) 
	{
		if (uiInfo.songCount > MAX_SONGS) 
		{
			uiInfo.songCount = MAX_SONGS;
		}
	}

	fileptr = "SOF2 Game Music";
	uiInfo.songList[0] = fileptr;

	fileptr = MP3Files;

	for( i = 1; i < uiInfo.songCount; i++ ) 
	{
		filelen = strlen( fileptr );
		//cut the ".mp3" off the end for display in the list
		fileptr[filelen - 4] = '\0'; 

		uiInfo.songList[i] = String_Alloc(fileptr);

		fileptr += filelen + 1;
	}

}
