#include "../g_local.h"

// This will be an alternate if no database is used.

#define MAX_COUNTRIES 30

const char *countryAbr[MAX_COUNTRIES] = {
"US", "CA", "CN", "GB", "NZ",
"GR", "FR", "TZ", "RU", "MX", 
"BR", "AU", "SE", "ES", "NO",
"IT", "IR", "IE", "KZ", "DE",
"CO", "CU", "DK", "IS", "PT",
"BF", "BE", "GR", "CH", "--"
};



qboolean NV_ip2Country (unsigned long ip, char *filename, int clientNum)	
{
	char			buffer[15000] = "\0";	
	char			*dec; 
	unsigned long	u1, u2;
	const char		*token;
	char			abr[4] = "";
	
	NV_LoadFile( va("ip2country\\%s.cfg", filename), buffer, qtrue );
	token = buffer;
	
	while(token) 
	{	
		dec = COM_ParseExt(&token, qtrue);
		u1 = atof(dec);
		dec = COM_ParseExt(&token, qtrue);
		u2 = atof(dec);
		dec = COM_ParseExt(&token, qtrue);
		
		if(ip >= u1 && ip <= u2)
		{
			Com_sprintf(abr, sizeof(abr) ,"%s", dec);
			strcpy( g_entities[clientNum].client->sess.country, Q_strupr(abr));
			//Com_Printf("\n\nrange:  %u > %u < %u\n", u1, ip, u2  );
			//Com_Printf("COUNTRY SET: %s\n", Q_strupr(abr) );
			//Com_Printf("SUCCESS!  USING FILE: %s \n\n", filename );
			return qtrue;
		}
	}		
		
	//Com_Printf("\nIP not found in any files.\n");
	return qfalse;
}

void NV_countryCodeLists (unsigned long ip, int clientNum )
{
	char		filelist[MAX_QPATH * 63];
	char		*fileptr, *s;
	int			filelen;
	int			filecount;
	int			i;
	unsigned long  fileIP = 0;
	unsigned long  nextFileIP = 0;

	filecount = trap_FS_GetFileList("ip2country\\", ".cfg", filelist, sizeof(filelist) );
	fileptr   = filelist;
	
	filelen = strlen(fileptr);

	for ( i = 0; i < filecount; i++, fileptr += filelen+1) 
	{	
		filelen = strlen(fileptr);
		s  = strchr ( fileptr, '.' );
		*s = '\0';		
		fileIP = atof(trap_VM_LocalStringAlloc ( fileptr ));
		sscanf(fileptr + filelen+1, "%u.cfg", &nextFileIP);

		if(ip > fileIP && ip < nextFileIP)
		{	
		//	Com_Printf("%u >= %u  --  file: %s - next file\n", ip, fileIP, fileptr, nextFileIP  );
			NV_ip2Country(ip, fileptr, clientNum);
			return;
		}				
	}
}

void NV_ip2CountryFileSelect ( int clientNum)
{
	unsigned long  z = 0;	
	int					oct[4], i; 
	char				ip[16];
	char				*s;
	char				userinfo[MAX_INFO_STRING];

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS)
		return;

	if(g_entities[clientNum].client->pers.connected != CON_CONNECTING &&
		g_entities[clientNum].client->pers.connected != CON_CONNECTED)
		return;

	strcpy( g_entities[clientNum].client->sess.country, "??" );

#ifdef _SOF2_BOTS
	if ( (g_entities[clientNum].r.svFlags & SVF_BOT))
	{
		//strcpy(g_entities[clientNum].client->nvSess.ip, "76.6.89.25"); 
		//strcpy(g_entities[clientNum].client->nvSess.ip, "228.255.255.254"); 
		strcpy( g_entities[clientNum].client->sess.country, countryAbr[irand(0, MAX_COUNTRIES-1)]); 
		return;
	}
#endif

	//  See if they have their own set	
	trap_GetUserinfo( clientNum , userinfo, sizeof( userinfo ) );
	s = Info_ValueForKey (userinfo, "nvc_country");
	if(s[0] && s[1] && (s[0] != ' ' && s[1] != ' ') && Q_isalpha(s[0]) && Q_isalpha(s[1]) )	
	{
		s[2] = '\0';
		Q_strncpyz(g_entities[clientNum].client->sess.country, s, 3);
		return;
	}

	strcpy(ip, g_entities[clientNum].client->nvSess.ip); 
	sscanf((const char*)ip, "%d.%d.%d.%d", &oct[0], &oct[1], &oct[2], &oct[3]);
	
	if((oct[0] == 192 && oct[1] == 168) || (oct[0] == 207 && oct[1] == 0) || !g_entities[clientNum].client->nvSess.ip[0])
	{
		strcpy( g_entities[clientNum].client->sess.country, "XX"); 
		return;
	}

	/// make sure it is a valid number ... being safe!
	for (i = 0; i < 4; i++)
	{
		if(oct[i] < 0 || oct[i] > 255) 
			return;
	}
	/// Address to Decimal
	z = oct[0] * 16777216; z += oct[1] * 65536; z += oct[2] * 256; z += oct[3];
	
	/// Is it a valid range? [3.0.0.0 to 222.255.255.255] ... should be!
	if(!(z >= 50331648 && z <= 3741319167)) 
		return;
	
	//Com_Printf("IP: %lu\n", z  );

	NV_countryCodeLists(z, clientNum );
}


