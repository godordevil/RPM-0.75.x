The mapcycle for NV MOD is different then the normal SOF2 mapcycle. 

// 1. 'set vote' will make a static vote map for the map on the currrent line,
// ... assuring 2 maps always go head to head.
// 2. A list of random maps can also be used by executing a cfg containing rmap cvars.  
// ... This will effect the NEXT line(s), NOT the line!  


SAMPLE MAPCYCLE (OLDER WAY, see new way below):
----------------
set map1  "set nextmap vstr map2;  set prevmap vstr map3; g_gametype sab;  exec mapcycle/rmg/snowy.cfg; map "arioche/mp_small"; scorelimit 3; timelimit 20; g_roundtimelimit 8; nv_reinforcementsBlueTime 0;" 
set map2  "set nextmap vstr map3;  set prevmap vstr map1; g_gametype tft;  map mp_finca; scorelimit 150; timelimit 20; g_roundtimelimit 5; nv_reinforcementsBlueTime 0;" 
set map3  "set nextmap vstr map1;  set prevmap vstr map2; g_gametype tctf; map mp_pra1;  scorelimit 3; timelimit 20; exec tctfSettings.cfg" 



ALTERNATIVE MAP USING A MAPLIST.CFG (CURRENT SETTINGS):
--------------------------------------------
set map1  "set next vstr map2;	vstr mp_shop_ctf; 	set vote mp_pra1_tft; exec mapcycle/votes/ran_1.cfg;"  // The vote is set to static (mp_pra1_tft). A vote list (ran_1.cfg) is also set for the next map
set map2  "set next vstr map3;	vstr mp_hk1_inf;"                                                              // vote map is selected from the cfg executed above and the next map is a static vote, so no need for a vote map or random list here.
set map3  "set next vstr map4;	vstr mp_raven_ctf;	set vote pra6_tft;"                                    // This and the next use static votes
set map4  "set next vstr map5;	vstr mp_hos1_DM;	set vote liner1_ctf;  exec mapcycle/votes/ran_2.cfg"   // Static vote map with vote list for the next map.
set map5  "set next vstr map6;	vstr mp_kam1_ctf;	exec mapcycle/votes/ran_1.cfg;"                        // Using vote list cfg above, but need to set vote list for the next map
set map6  "set next vstr map1;	vstr mp_finca_ctf;"                                                            // vote list from above, also set next to the starting map for the loop



1.) Always set the map# in sequential order starting with map1.  (i.e.: map1, map2, map3, map4, etc.. )
2.) nextmap should always point to the map that will come next in the cycle.   This is the variable called on each mapcycle so its very important!
3.) (OPTIONAL) Set prevmap to the previous map#.  This is useful to reload the last map that was played and should point to the previous map in the cycle.
4.) set any settings or cvars needed (scorelimit, g_roundlimit, exec myINFSettings.cfg, etc..)
5.) gametype and map you wish to load.   See RMG below for information regarding RMG maps.



SETTING RMG MAPS
-------------------
RMG's are special cases and are unique to the SOF2 Engine.  There are 2 ways to load RMG's

1. You can load an RMG with the 'RMGMAP' command as you would do with a normal mapcycle. 

2. If you wish to load custom settings (fog, entities, texture, time, etc..) then use the 'map' command followed by one of the two rmg maps (including the path to the RMG folder):  'arioche/mp_small' or 'arioche/mp_medium'.
example:  map arioche/mp_small;

If you chose to use the 2nd method, you will NEED to execute the settings for the RMG map.  I have added generic cfgs for both small and medium RMGs. 
These are just basic settings used to control the sky, fog, music, texture type, entities, and other settings.  You can edit the ones in the 'nv/mapcycle' folder or create your own for different occcations.
For more settings you can look in the therest.pk3 file in the "\ext_data\arioche" directory and see some of the setups you can have.
Once you have your settings saved to the config you want, you can then execute them in the mapcycle.  example:  exec mapcycle/rmg/myCustomRMG.cfg;


 - *IMPORTANT* -   Always execute the RMG config files *BEFORE* you call the map.  If you do not do this, the map will load without the settings and it will not be playable!
example: exec mapcycle/rmg/desert.cfg; map arioche/mp_small



