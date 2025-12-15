/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// g_weapon.c
//#include <stdlib.h> 
#include "g_local.h"
#include "m_player.h"

//mattMod methods I should have made ages ago

static void healMatt(edict_t* ent)
{
	Com_Printf("Be healed!\n");
}

static void  jammedMatt()
{
	Com_Printf("YOUR GUN IS JAMMED!\n");
}

static void  instaMatt()
{
	Com_Printf("You have insta kill!\n");
}

static void backwardsMatt()
{
	Com_Printf("Your gun shoots backwards!\n");
}

static void regenMatt()
{
	Com_Printf("Your gun regens ammo!\n");
}

static qboolean	is_quad;
static byte		is_silenced;


void weapon_grenade_fire (edict_t *ent, qboolean held);


static void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	vec3_t	_distance;

	VectorCopy (distance, _distance);
	if (client->pers.hand == LEFT_HANDED)
		_distance[1] *= -1;
	else if (client->pers.hand == CENTER_HANDED)
		_distance[1] = 0;
	G_ProjectSource (point, _distance, forward, right, result);
}


/*
===============
PlayerNoise

Each player can have two noise objects associated with it:
a personal noise (jumping, pain, weapon firing), and a weapon
target noise (bullet wall impacts)

Monsters that don't directly see the player can move
to a noise in hopes of seeing the player from there.
===============
*/
void PlayerNoise(edict_t *who, vec3_t where, int type)
{
	edict_t		*noise;

	if (type == PNOISE_WEAPON)
	{
		if (who->client->silencer_shots)
		{
			who->client->silencer_shots--;
			return;
		}
	}

	if (deathmatch->value)
		return;

	if (who->flags & FL_NOTARGET)
		return;


	if (!who->mynoise)
	{
		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet (noise->mins, -8, -8, -8);
		VectorSet (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		who->mynoise = noise;

		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet (noise->mins, -8, -8, -8);
		VectorSet (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		who->mynoise2 = noise;
	}

	if (type == PNOISE_SELF || type == PNOISE_WEAPON)
	{
		noise = who->mynoise;
		level.sound_entity = noise;
		level.sound_entity_framenum = level.framenum;
	}
	else // type == PNOISE_IMPACT
	{
		noise = who->mynoise2;
		level.sound2_entity = noise;
		level.sound2_entity_framenum = level.framenum;
	}

	VectorCopy (where, noise->s.origin);
	VectorSubtract (where, noise->maxs, noise->absmin);
	VectorAdd (where, noise->maxs, noise->absmax);
	noise->teleport_time = level.time;
	gi.linkentity (noise);
}


qboolean Pickup_Weapon (edict_t *ent, edict_t *other)
{
	//mattMod pickup
	// mattmodpickup
	//ent seems to be the weapon
	//other seems to be the player

	if (!ent || !other)
	{
		return ;
	}
	
	other->mattPlayer = ent; //IDK What this is for but I am leaving it
	//I think this is so weapons can remeber what mods they have?
	// Call ent-> and fill their values based on what they are?
	// Mess with this once I can drop weapons
	

	if (Q_stricmp(ent->item->pickup_name,"Rocket Launcher") == 0)
	{
		//mattmod pickup rocket
		
		Com_Printf("Player picked up a rocket launcher!\n");
		
		if (other->rocketModded == 0)
		{
			//int test = 1+ (int)(random() * 5);
			//Com_Printf("Test is: %d\n",test);

			other->rocketAOE = 1 + (int)(random() * 2);
			other->rocketDMG = 1 + (int)(random() * 2);
			other->rocketNuke = 1 + (int)(random() * 2);
			//generic
			other->rocketRegen = 1 + (int)(random() * 5);
			other->rocketJam = 1 + (int)(random() * 5);
			other->rocketHeal = 1 + (int)(random() * 5);
			other->rocketInstaKill = 1 + (int)(random() * 5);
			other->rocketGunBack = 1 + (int)(random() * 5);
			other->rocketModded = 1;

			Com_Printf("Rocket Mods:\n AOE: %d\nDMG: %d\nNuke: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta:%d\nBack: %d\n",
				other->rocketAOE, other->rocketDMG, other->rocketNuke, other->rocketRegen,
				other->rocketJam, other->rocketHeal, other->rocketInstaKill, other->rocketGunBack);
		}

		
	}
	else if (Q_stricmp(ent->item->pickup_name,"Super Shotgun") == 0)
	{
		//mattmod pickup super shotgun
		Com_Printf("Player picked up a super shotgun!\n");
		//srand(time(NULL));
		
		if (other->superModded == 0)
		{
			int num = (int)(random() * 2);

			other->superShot1 = random() * 2;
			//other->superShot1 = 1;
			other->superShot2 = random() * 2;
			other->superShot3 = random() * 2;

			other->superRegen = 1 + (int)(random() * 5);
			other->superJam = 1 + (int)(random() * 5);
			other->superHeal = 1 + (int)(random() * 5);
			other->superInsta = 1 + (int)(random() * 5);
			other->superBack = 1 + (int)(random() * 5);
			other->superModded = 1;

			Com_Printf("Super Shotgun Mods:\n1: %d\n2: %d\n3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->superShot1, other->superShot2, other->superShot3, other->superRegen,
				other->superJam, other->superHeal, other->superInsta, other->superBack);
		}



	}
	else if (Q_stricmp(ent->item->pickup_name,"machinegun") == 0)
	{
		//mattmod pickup machine
		Com_Printf("Player picked up a machine gun!\n");
		
		if (other->machineModded == 0)
		{
			//srand(time(NULL));

			int num = (int)(random() * 3);;
			//num = 1;


			if (num == 0)
			{
				other->machineGunBolt = 1;
				other->machineGunDump = 0;
			}
			else if (num == 1)
			{
				other->machineGunBolt = 0;
				other->machineGunDump = 1;
			}
			else
			{
				other->machineGunBolt = 0;
				other->machineGunDump = 0;
			}

			other->machineShootRockets = (int)(random() * 2);
			other->machineGunRegen = 1 + (int)(random() * 5);
			other->machineGunJam = 1 + (int)(random() * 5);
			other->machineGunHeal = 1 + (int)(random() * 5);
			other->machineGunInstaKill = 1 + (int)(random() * 5);
			other->machineGunBack = 1 + (int)(random() * 5);
			other->machineModded = 1;


			Com_Printf("Machine Gun Mods:\nBolt: %d\nDump: %d\nRockets: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->machineGunBolt, other->machineGunDump, other->machineShootRockets, other->machineGunRegen,
				other->machineGunJam, other->machineGunHeal, other->machineGunInstaKill, other->machineGunBack);
		}

		

	}
	else if (Q_stricmp(ent->item->pickup_name, "Shotgun") == 0)
	{
		//mattmod pickup shotgun
		Com_Printf("Player picked up shotgun!\n");

		if (other->shotgunModded == 0)
		{
			//srand(time(NULL));

			int num = (int)(random() * 2);
			//num = 1;

			if (num == 1)
			{
				other->shotgunMeme2 = 1;
			}
			num = (int)(random() * 2);
			//num = 2;

			if (num == 1)
			{
				other->shotgunSlug = 1;
			}
			else
			{
				other->shotgunMeme3 = 1;
			}




			other->shotgunGunRegen = (int)(random() * 5) + 1;
			other->shotgunGunJam = (int)(random() * 5) + 1;
			other->shotgunGunHeal = (int)(random() * 5) + 1;
			other->shotgunGunInstaKill = (int)(random() * 5) + 1;
			other->shotgunGunBack = (int)(random() * 5) + 1;
			other->shotgunModded = 1;

			/*other->shotgunGunRegen = 5;
			other->shotgunGunJam = rand() % 5 + 1;
			other->shotgunGunHeal = 5;
			other->shotgunGunInstaKill = 5;
			other->shotgunGunBack = 5;*/


			Com_Printf("Shotgun Gun Mods:\nSlug: %d\nmeme2: %d\nmeme3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->shotgunSlug, other->shotgunMeme2, other->shotgunMeme3, other->shotgunGunRegen,
				other->shotgunGunJam, other->shotgunGunHeal, other->shotgunGunInstaKill, other->shotgunGunBack);
		}
	}
	else if (Q_stricmp(ent->item->pickup_name, "Chaingun") == 0)
	{
		//mattmod pickup chain
		Com_Printf("Player picked up chaingun!\n");

		if (other->chaingunModded == 0)
		{
			int num = (int)(random() * 3);
			other->chaingun3 = num;

			num = (int)(random() * 3);
			other->chaingun2 = num;

			num = (int)(random() * 3);
			other->chaingun1 = num;

			other->chainRegen = (int)(random() * 5) + 1;
			other->chainJam = (int)(random() * 5) + 1;
			other->chainHeal = (int)(random() * 5) + 1;
			other->chainInsta = (int)(random() * 5) + 1;
			other->chainBack = (int)(random() * 5) + 1;
			other->chaingunModded = 1;

			Com_Printf("Chain Gun Mods:\nNo Spread: %d\nchain2: %d\nchain3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->chaingun1, other->chaingun2, other->chaingun3, other->chainRegen,
				other->chainJam, other->chainHeal, other->chainInsta, other->chainBack);
		}


	}
	else if (Q_stricmp(ent->item->pickup_name, "BFG10k") == 0)
	{
		Com_Printf("Player picked up bfg!\n");

		if (other->BFGModded == 0)
		{
			int num = (int)(random() * 3);
			//num = 1;
			other->BFG1 = num;

			num = (int)(random() * 3);
			other->BFG2 = num;

			num = (int)(random() * 3);
			other->BFG3 = num;

			other->BFGRegen = (int)(random() * 5) + 1;
			other->BFGJam = (int)(random() * 5) + 1;
			other->BFGHeal = (int)(random() * 5) + 1;
			other->BFGInsta = (int)(random() * 5) + 1;
			other->BFGBack = (int)(random() * 5) + 1;
			other->BFGModded = 1;

			Com_Printf("BFG Mods:\nBFG1: %d\nBFG2: %d\nBFG3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->BFG1, other->BFG2, other->BFG3, other->BFGRegen,
				other->BFGJam, other->BFGHeal, other->BFGInsta, other->BFGBack);
		}

	}
	//grenades dealt with in ammo_pickup
	else if (Q_stricmp(ent->item->pickup_name, "Grenade launcher") == 0)
	{
		//mattmod launcher
		Com_Printf("Player picked up grenadelauncher!\n");

		if (other->launcerModded == 0)
		{
			int num = (int)(random() * 2);
			num = 1;
			other->launcer1 = num;

			num = (int)(random() * 2);
			other->launcer2 = num;

			num = (int)(random() * 2);
			other->launcer3 = num;

			other->launcerBack = (int)(random() * 5) + 1;
			other->launcerJam = (int)(random() * 5) + 1;
			other->launcerHeal = (int)(random() * 5) + 1;
			other->launcerInsta = (int)(random() * 5) + 1;
			other->launcerBack = (int)(random() * 5) + 1;
			other->launcerModded = 1;

			Com_Printf("Grenade Launcher Mods:\nmeme1: %d\nmeme2: %d\nmeme3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->launcer1, other->launcer2, other->launcer3, other->launcerRegen,
				other->launcerJam, other->launcerHeal, other->launcerInsta, other->launcerBack);
		}

	}
	else if (Q_stricmp(ent->item->pickup_name, "hyperblaster") == 0)
	{
		Com_Printf("Player picked up hyperblaster!\n");
		//mattmod pickup hyperblsater

		if (other->hyperModded == 0)
		{
			int num = (int)(random() * 2);
			other->hyper1 = num;

			num = (int)(random() * 2);
			other->hyper2 = num;

			num = (int)(random() * 2);
			other->hyper2 = num;

			other->hyperRegen = (int)(random() * 5) + 1;
			other->hyperJam = (int)(random() * 5) + 1;
			other->hyperHeal = (int)(random() * 5) + 1;
			other->hyperInsta = (int)(random() * 5) + 1;
			other->hyperBack = (int)(random() * 5) + 1;
			other->hyperModded = 1;

			Com_Printf("Hyperblaster Mods:\nmeme1: %d\nmeme2: %d\nmeme3: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->hyper1, other->hyper2, other->hyper3, other->hyperRegen,
				other->hyperJam, other->hyperHeal, other->hyperInsta, other->hyperBack);
		}

	}
	else if (Q_stricmp(ent->item->pickup_name, "Railgun") == 0)
	{
		Com_Printf("Player picked up railgun!\n");

		if (other->railgunModded == 0)
		{
			int num = (int)(random() * 3);
			other->rail1 = num;

			num = (int)(random() * 3);
			other->rail2 = num;

			num = (int)(random() * 3);
			other->rail3 = num;

			other->railRegen = (int)(random() * 5) + 1;
			other->railJam = (int)(random() * 5) + 1;
			other->railHeal = (int)(random() * 5) + 1;
			other->railInsta = (int)(random() * 5) + 1;
			other->railBack = (int)(random() * 5) + 1;
			other->railgunModded = 1;

			Com_Printf("Rail Gun Mods:\nMany Shots: %d\nHalf Range: %d\nRandomDamage: %d\nRegen: %d\nJam: %d\nHeal: %d\nInsta: %d\nBack: %d\n",
				other->rail1, other->rail2, other->rail3, other->rail3,
				other->railJam, other->railHeal, other->railInsta, other->railBack);
		}
	}
	else
	{
		Com_Printf("Player picked up some item %s!\n",ent->item->pickup_name);
	}

	/*if (ent->item == FindItem("Machinegun"))
	{
		Com_Printf("Picked up machinegun using pointers!\n");
	}*/

	int			index;
	gitem_t		*ammo;

	index = ITEM_INDEX(ent->item);

	

	if ( ( ((int)(dmflags->value) & DF_WEAPONS_STAY) || coop->value) 
		&& other->client->pers.inventory[index])
	{
		if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->client->pers.inventory[index]++;

	if (!(ent->spawnflags & DROPPED_ITEM) )
	{
		// give them some ammo with it
		ammo = FindItem (ent->item->ammo);
		if ( (int)dmflags->value & DF_INFINITE_AMMO )
			Add_Ammo (other, ammo, 1000);
		else
			Add_Ammo (other, ammo, ammo->quantity);

		if (! (ent->spawnflags & DROPPED_PLAYER_ITEM) )
		{
			if (deathmatch->value)
			{
				if ((int)(dmflags->value) & DF_WEAPONS_STAY)
					ent->flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 30);
			}
			if (coop->value)
				ent->flags |= FL_RESPAWN;
		}
	}

	if (other->client->pers.weapon != ent->item && 
		(other->client->pers.inventory[index] == 1) &&
		( !deathmatch->value || other->client->pers.weapon == FindItem("blaster") ) )
		other->client->newweapon = ent->item;

	return true;
}


/*
===============
ChangeWeapon

The old weapon has been dropped all the way, so make the new one
current
===============
*/
void ChangeWeapon (edict_t *ent)
{
	int i;

	if (ent->client->grenade_time)
	{
		ent->client->grenade_time = level.time;
		ent->client->weapon_sound = 0;
		weapon_grenade_fire (ent, false);
		ent->client->grenade_time = 0;
	}

	ent->client->pers.lastweapon = ent->client->pers.weapon;
	ent->client->pers.weapon = ent->client->newweapon;
	ent->client->newweapon = NULL;
	ent->client->machinegun_shots = 0;

	// set visible model
	if (ent->s.modelindex == 255) {
		if (ent->client->pers.weapon)
			i = ((ent->client->pers.weapon->weapmodel & 0xff) << 8);
		else
			i = 0;
		ent->s.skinnum = (ent - g_edicts - 1) | i;
	}

	if (ent->client->pers.weapon && ent->client->pers.weapon->ammo)
		ent->client->ammo_index = ITEM_INDEX(FindItem(ent->client->pers.weapon->ammo));
	else
		ent->client->ammo_index = 0;

	if (!ent->client->pers.weapon)
	{	// dead
		ent->client->ps.gunindex = 0;
		return;
	}

	ent->client->weaponstate = WEAPON_ACTIVATING;
	ent->client->ps.gunframe = 0;
	ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);

	ent->client->anim_priority = ANIM_PAIN;
	if(ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
			ent->s.frame = FRAME_crpain1;
			ent->client->anim_end = FRAME_crpain4;
	}
	else
	{
			ent->s.frame = FRAME_pain301;
			ent->client->anim_end = FRAME_pain304;
			
	}
}

/*
=================
NoAmmoWeaponChange
=================
*/
void NoAmmoWeaponChange (edict_t *ent)
{
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("slugs"))]
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("railgun"))] )
	{
		ent->client->newweapon = FindItem ("railgun");
		return;
	}
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("cells"))]
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("hyperblaster"))] )
	{
		ent->client->newweapon = FindItem ("hyperblaster");
		return;
	}
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("bullets"))]
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("chaingun"))] )
	{
		ent->client->newweapon = FindItem ("chaingun");
		return;
	}
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("bullets"))]
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("machinegun"))] )
	{
		ent->client->newweapon = FindItem ("machinegun");
		return;
	}
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))] > 1
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("super shotgun"))] )
	{
		ent->client->newweapon = FindItem ("super shotgun");
		return;
	}
	if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))]
		&&  ent->client->pers.inventory[ITEM_INDEX(FindItem("shotgun"))] )
	{
		ent->client->newweapon = FindItem ("shotgun");
		return;
	}
	ent->client->newweapon = FindItem ("blaster");
}

/*
=================
Think_Weapon

Called by ClientBeginServerFrame and ClientThink
=================
*/
void Think_Weapon (edict_t *ent)
{
	// if just died, put the weapon away
	if (ent->health < 1)
	{
		ent->client->newweapon = NULL;
		ChangeWeapon (ent);
	}

	// call active weapon think routine
	if (ent->client->pers.weapon && ent->client->pers.weapon->weaponthink)
	{
		is_quad = (ent->client->quad_framenum > level.framenum);
		if (ent->client->silencer_shots)
			is_silenced = MZ_SILENCED;
		else
			is_silenced = 0;
		ent->client->pers.weapon->weaponthink (ent);
	}
}


/*
================
Use_Weapon

Make the weapon ready if there is ammo
================
*/
void Use_Weapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;

	// see if we're already using it
	if (item == ent->client->pers.weapon)
		return;

	if (item->ammo && !g_select_empty->value && !(item->flags & IT_AMMO))
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);

		if (!ent->client->pers.inventory[ammo_index])
		{
			gi.cprintf (ent, PRINT_HIGH, "No %s for %s.\n", ammo_item->pickup_name, item->pickup_name);
			return;
		}

		if (ent->client->pers.inventory[ammo_index] < item->quantity)
		{
			gi.cprintf (ent, PRINT_HIGH, "Not enough %s for %s.\n", ammo_item->pickup_name, item->pickup_name);
			return;
		}
	}

	// change to this weapon when down
	ent->client->newweapon = item;
}



/*
================
Drop_Weapon
================
*/
//mattMod drop weapon
void Drop_Weapon (edict_t *ent, gitem_t *item)
{
	int		index;

	if ((int)(dmflags->value) & DF_WEAPONS_STAY)
		return;

	index = ITEM_INDEX(item);
	// see if we're already using it
	if ( ((item == ent->client->pers.weapon) || (item == ent->client->newweapon))&& (ent->client->pers.inventory[index] == 1) )
	{
		gi.cprintf (ent, PRINT_HIGH, "Can't drop current weapon\n");
		return;
	}

	Drop_Item (ent, item);
	ent->client->pers.inventory[index]--;
}


/*
================
Weapon_Generic

A generic function to handle the basics of weapon thinking
================
*/
#define FRAME_FIRE_FIRST		(FRAME_ACTIVATE_LAST + 1)
#define FRAME_IDLE_FIRST		(FRAME_FIRE_LAST + 1)
#define FRAME_DEACTIVATE_FIRST	(FRAME_IDLE_LAST + 1)

void Weapon_Generic (edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST, int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames, int *fire_frames, void (*fire)(edict_t *ent))
{
	int		n;

	if(ent->deadflag || ent->s.modelindex != 255) // VWep animations screw up corpses
	{
		return;
	}

	if (ent->client->weaponstate == WEAPON_DROPPING)
	{
		if (ent->client->ps.gunframe == FRAME_DEACTIVATE_LAST)
		{
			ChangeWeapon (ent);
			return;
		}
		else if ((FRAME_DEACTIVATE_LAST - ent->client->ps.gunframe) == 4)
		{
			ent->client->anim_priority = ANIM_REVERSE;
			if(ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crpain4+1;
				ent->client->anim_end = FRAME_crpain1;
			}
			else
			{
				ent->s.frame = FRAME_pain304+1;
				ent->client->anim_end = FRAME_pain301;
				
			}
		}

		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		if (ent->client->ps.gunframe == FRAME_ACTIVATE_LAST)
		{
			ent->client->weaponstate = WEAPON_READY;
			ent->client->ps.gunframe = FRAME_IDLE_FIRST;
			return;
		}

		ent->client->ps.gunframe++;
		return;
	}

	if ((ent->client->newweapon) && (ent->client->weaponstate != WEAPON_FIRING))
	{
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = FRAME_DEACTIVATE_FIRST;

		if ((FRAME_DEACTIVATE_LAST - FRAME_DEACTIVATE_FIRST) < 4)
		{
			ent->client->anim_priority = ANIM_REVERSE;
			if(ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crpain4+1;
				ent->client->anim_end = FRAME_crpain1;
			}
			else
			{
				ent->s.frame = FRAME_pain304+1;
				ent->client->anim_end = FRAME_pain301;
				
			}
		}
		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		if ( ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK) )
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;
			if ((!ent->client->ammo_index) || 
				( ent->client->pers.inventory[ent->client->ammo_index] >= ent->client->pers.weapon->quantity))
			{
				ent->client->ps.gunframe = FRAME_FIRE_FIRST;
				ent->client->weaponstate = WEAPON_FIRING;

				// start the animation
				ent->client->anim_priority = ANIM_ATTACK;
				if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
				{
					ent->s.frame = FRAME_crattak1-1;
					ent->client->anim_end = FRAME_crattak9;
				}
				else
				{
					ent->s.frame = FRAME_attack1-1;
					ent->client->anim_end = FRAME_attack8;
				}
			}
			else
			{
				if (level.time >= ent->pain_debounce_time)
				{
					gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
					ent->pain_debounce_time = level.time + 1;
				}
				NoAmmoWeaponChange (ent);
			}
		}
		else
		{
			if (ent->client->ps.gunframe == FRAME_IDLE_LAST)
			{
				ent->client->ps.gunframe = FRAME_IDLE_FIRST;
				return;
			}

			if (pause_frames)
			{
				for (n = 0; pause_frames[n]; n++)
				{
					if (ent->client->ps.gunframe == pause_frames[n])
					{
						if (rand()&15)
							return;
					}
				}
			}

			ent->client->ps.gunframe++;
			return;
		}
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		for (n = 0; fire_frames[n]; n++)
		{
			if (ent->client->ps.gunframe == fire_frames[n])
			{
				if (ent->client->quad_framenum > level.framenum)
					gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage3.wav"), 1, ATTN_NORM, 0);

				fire (ent);
				break;
			}
		}

		if (!fire_frames[n])
			ent->client->ps.gunframe++;

		if (ent->client->ps.gunframe == FRAME_IDLE_FIRST+1)
			ent->client->weaponstate = WEAPON_READY;
	}
}


/*
======================================================================

GRENADE

======================================================================
*/

#define GRENADE_TIMER		3.0
#define GRENADE_MINSPEED	400
#define GRENADE_MAXSPEED	800

void weapon_grenade_fire (edict_t *ent, qboolean held)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 125;
	float	timer;
	int		speed;
	float	radius;

	//mattMod Grenade



	radius = damage+40;
	if (is_quad)
		damage *= 4;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	timer = ent->client->grenade_time - level.time;
	speed = GRENADE_MINSPEED + (GRENADE_TIMER - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER);

	if (ent->grenadeJam == 5)
	{
		jammedMatt();
		Com_Printf("Ummm... You jammed your grenade?");
		fire_grenade2(ent, start, 0, damage, speed, 1, radius, held);
	}

	if (ent->grenadeHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->grenadeBack == 5)
	{
		backwardsMatt();
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	if (ent->grenadeInsta == 5)
	{
		instaMatt();
		damage = 999;
	}

	if (ent->grenadeRegen == 5)
	{
		ent->client->pers.inventory[ent->client->ammo_index]++;
	}

	//mattmod grenade
	//mattmodgrenade
	if (ent->grenade1 == 1)
	{
		//many grenades
		fire_grenade2(ent, start, forward, damage, speed, timer, radius, held);

		start[0] += right[0] * 10;
		start[1] += right[1] * 10;
		start[2] += right[2] * 10;

		fire_grenade2(ent, start, forward, damage, speed, timer, radius, held);


		start[0] -= right[0] * 20;
		start[1] -= right[1] * 20;
		start[2] -= right[2] * 20;
		fire_grenade2(ent, start, forward, damage, speed, timer, radius, held);
	}
	else if (ent->grenade2 == 1)
	{
		//grenade random timer
		int mTimer = (ent->client->grenade_time - level.time);
		int num = (int)random() % (mTimer+50);
		fire_grenade2(ent, start, forward, damage, speed, num, radius, held);
	}
	else if (ent->grenade3 == 1)
	{
		//grenade throws a rocket
		//fire_grenade2(ent, start, forward, damage, speed, timer, radius, held);
		int radius_damage = 120;
		int damage_radius = 120;
		fire_rocket(ent, start, forward, damage, 650, damage_radius, radius_damage);
	}
	else
	{
		fire_grenade2(ent, start, forward, damage, speed, timer, radius, held);
	}
	

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;

	ent->client->grenade_time = level.time + 1.0;

	if(ent->deadflag || ent->s.modelindex != 255) // VWep animations screw up corpses
	{
		return;
	}

	if (ent->health <= 0)
		return;

	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		ent->client->anim_priority = ANIM_ATTACK;
		ent->s.frame = FRAME_crattak1-1;
		ent->client->anim_end = FRAME_crattak3;
	}
	else
	{
		ent->client->anim_priority = ANIM_REVERSE;
		ent->s.frame = FRAME_wave08;
		ent->client->anim_end = FRAME_wave01;
	}
}

void Weapon_Grenade (edict_t *ent)
{
	if ((ent->client->newweapon) && (ent->client->weaponstate == WEAPON_READY))
	{
		ChangeWeapon (ent);
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		ent->client->weaponstate = WEAPON_READY;
		ent->client->ps.gunframe = 16;
		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		if ( ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK) )
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;
			if (ent->client->pers.inventory[ent->client->ammo_index])
			{
				ent->client->ps.gunframe = 1;
				ent->client->weaponstate = WEAPON_FIRING;
				ent->client->grenade_time = 0;
			}
			else
			{
				if (level.time >= ent->pain_debounce_time)
				{
					gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
					ent->pain_debounce_time = level.time + 1;
				}
				NoAmmoWeaponChange (ent);
			}
			return;
		}

		if ((ent->client->ps.gunframe == 29) || (ent->client->ps.gunframe == 34) || (ent->client->ps.gunframe == 39) || (ent->client->ps.gunframe == 48))
		{
			if (rand()&15)
				return;
		}

		if (++ent->client->ps.gunframe > 48)
			ent->client->ps.gunframe = 16;
		return;
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		if (ent->client->ps.gunframe == 5)
			gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/hgrena1b.wav"), 1, ATTN_NORM, 0);

		if (ent->client->ps.gunframe == 11)
		{
			if (!ent->client->grenade_time)
			{
				ent->client->grenade_time = level.time + GRENADE_TIMER + 0.2;
				ent->client->weapon_sound = gi.soundindex("weapons/hgrenc1b.wav");
			}

			// they waited too long, detonate it in their hand
			if (!ent->client->grenade_blew_up && level.time >= ent->client->grenade_time)
			{
				ent->client->weapon_sound = 0;
				weapon_grenade_fire (ent, true);
				ent->client->grenade_blew_up = true;
			}

			if (ent->client->buttons & BUTTON_ATTACK)
				return;

			if (ent->client->grenade_blew_up)
			{
				if (level.time >= ent->client->grenade_time)
				{
					ent->client->ps.gunframe = 15;
					ent->client->grenade_blew_up = false;
				}
				else
				{
					return;
				}
			}
		}

		if (ent->client->ps.gunframe == 12)
		{
			ent->client->weapon_sound = 0;
			weapon_grenade_fire (ent, false);
		}

		if ((ent->client->ps.gunframe == 15) && (level.time < ent->client->grenade_time))
			return;

		ent->client->ps.gunframe++;

		if (ent->client->ps.gunframe == 16)
		{
			ent->client->grenade_time = 0;
			ent->client->weaponstate = WEAPON_READY;
		}
	}
}

/*
======================================================================

GRENADE LAUNCHER

======================================================================
*/

void weapon_grenadelauncher_fire (edict_t *ent)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 120;
	float	radius;
	radius = damage + 40;
	//mattmod grenadelauncher
	//mattmodgrenadelauncher
	//mattmod launcher
	//mattmodlauncher

	if (ent->grenadeJam == 5)
	{
		jammedMatt();
		Com_Printf("Ummm... You jammed your grenadelauncher?");
		fire_grenade2(ent, start, 0, damage, 0, 1, radius, false);
	}

	if (ent->grenadeRegen == 5)
	{
		regenMatt();
		ent->client->pers.inventory[ent->client->ammo_index]++;
	}

	if (ent->grenadeHeal == 5)
	{
		healMatt(ent);
		ent->health+=10;
	}

	if (ent->grenadeBack == 5)
	{
		backwardsMatt();
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	if (ent->grenadeInsta == 5)
	{
		instaMatt();
		damage = 999;
	}

	
	if (is_quad)
		damage *= 4;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;


	if (ent->launcer1 == 1)
	{
		fire_grenade(ent, start, forward, damage, 600, 2.5, radius);
		forward[0] += right[0] * 10;
		forward[1] += right[1] * 10;
		forward[2] += right[2] * 10;
		fire_grenade(ent, start, forward, damage, 600, 2.5, radius);
		forward[0] -= right[0] * 20;
		forward[1] -= right[1] * 20;
		forward[2] -= right[2] * 20;
		fire_grenade(ent, start, forward, damage, 600, 2.5, radius);
	}
	else if (ent->launcer2 == 1)
	{
		int num = ent->client->pers.inventory[ent->client->ammo_index];


		for (int c = 1;c < num / 2;c++)
		{
			fire_bullet(ent, start, forward, damage, 0, 0, 0, MOD_MACHINEGUN);
			//Com_Printf("Fired: %d \n",c);
		}

		ent->client->pers.inventory[ent->client->ammo_index = 0];
	}
	else if (ent->launcer3 == 1)
	{
		fire_shotgun(ent, start, forward, damage, 0, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);
		fire_shotgun(ent, start, forward, damage, 0, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);
		fire_shotgun(ent, start, forward, damage, 0, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);
	}
	else
		fire_grenade (ent, start, forward, damage, 600, 2.5, radius);

	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_GRENADE | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_GrenadeLauncher (edict_t *ent)
{
	static int	pause_frames[]	= {34, 51, 59, 0};
	static int	fire_frames[]	= {6, 0};

	Weapon_Generic (ent, 5, 16, 59, 64, pause_frames, fire_frames, weapon_grenadelauncher_fire);
}

/*
======================================================================

ROCKET

======================================================================
*/

void Weapon_RocketLauncher_Fire (edict_t *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage;
	float	damage_radius;
	int		radius_damage;

	damage = 100 + (int)(random() * 20.0);
	radius_damage = 120;
	damage_radius = 120;
	
	if (is_quad)
	{
		damage *= 4;
		radius_damage *= 4;
	}

	if (ent->rocketJam == 5)
	{
		jammedMatt(ent);
		return;
	}

	if (ent->rocketRegen == 5)
	{
		ent->client->pers.inventory[ent->client->ammo_index] += 1;
	}

	if (ent->rocketHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->rocketInstaKill == 5)
	{
		instaMatt(ent);
		damage = 999;
		damage_radius = 999;
	}
	else
	{
		ent->rocketDMG = rand() % 20;
	}

	if (ent->rocketGunBack == 5)
	{
		backwardsMatt(ent);
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	//mattMod rocket
	if (ent->rocketNuke == 1)
	{
		damage_radius = 9999;
	}
	

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	fire_rocket (ent, start, forward, damage, 650, damage_radius, radius_damage);


	//mattMod
	if (ent->rocketAOE == 1)
	{
		//directly behind
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		fire_rocket(ent, start, forward, damage, 650, damage_radius, radius_damage);

		//to the right
		fire_rocket(ent, start, right, damage, 650, damage_radius, radius_damage);

		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;

		//to the left
		fire_rocket(ent, start, right, damage, 650, damage_radius, radius_damage);



		//back left
		vec3_t diag;
		VectorAdd(forward, right, diag);
		VectorNormalize(diag);

		fire_rocket(ent, start, diag, damage, 650, damage_radius, radius_damage);

		//back right
		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;

		vec3_t diag2;
		VectorAdd(forward, right, diag2);
		VectorNormalize(diag2);

		fire_rocket(ent, start, diag2, damage, 650, damage_radius, radius_damage);

		//not working
		//front right

		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		vec3_t diag3;

		VectorAdd(forward, right, diag3);
		VectorNormalize(diag3);

		fire_rocket(ent, start, diag3, damage, 650, damage_radius, radius_damage);

		//front left
		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;
		vec3_t diag4;
		VectorAdd(forward, right, diag4);
		VectorNormalize(diag4);

		fire_rocket(ent, start, diag4, damage, 650, damage_radius, radius_damage);
	}
	else
	{
		//Com_Printf("ent->rocketAOE is 0!\n");
	}



	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_ROCKET | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_RocketLauncher (edict_t *ent)
{
	static int	pause_frames[]	= {25, 33, 42, 50, 0};
	static int	fire_frames[]	= {5, 0};

	Weapon_Generic (ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_RocketLauncher_Fire);
}


/*
======================================================================

BLASTER / HYPERBLASTER

======================================================================
*/

void Blaster_Fire (edict_t *ent, vec3_t g_offset, int damage, qboolean hyper, int effect)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;
	
	if (hyper && ent->hyperJam == 5)
	{
		jammedMatt();
		return;
	}

	//regen in hyperblasterfire
	if (hyper && ent->hyperHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}



	if (is_quad)
		damage *= 4;

	AngleVectors (ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, 8, ent->viewheight-8);
	VectorAdd (offset, g_offset, offset);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	//fire_blaster (ent, start, forward, damage, 1000, effect, hyper);

	

	//mattMod
	float* s = start;
	float* r = right;
	float* f = forward;

	//remeber default values!
	float s0 = s[0];
	float s1 = s[1];
	float s2 = s[2]; //This affects the Y coordiante


	int go = 1;

	if (hyper && ent->hyperBack)
	{
		f[0] *= -1;
		f[1] *= -1;
		f[2] *= -1;
	}

	if (!hyper && ent->blasterBack == 5)
	{
		f[0] *= -1;
		f[1] *= -1;
		f[2] *= -1;
		//Com_Printf("Blaster shooting backwards!\n");
		backwardsMatt(ent);
	}

	//hyperblaster mods
	if(hyper)
	{
		if (ent->hyper1 == 1)
		{
			int s1, s2, s3;
			s1 = start[0];
			s2 = start[1];
			s3 = start[2];

			//do something
			// fire in a 3x3
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

			start[0] += right[0] * 10;
			start[1] += right[1] * 10;
			start[2] += right[2] * 10;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

			start[0] -= right[0] * 20;
			start[1] -= right[1] * 20;
			start[2] -= right[2] * 20;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

			start[0] = s1;
			start[1] = s2;
			start[2] = s3;

			vec3_t up;
			CrossProduct(forward, right, up);
			//1 line up
			start[0] += up[0] * 10;
			start[1] += up[1] * 10;
			start[2] += up[2] * 10;


			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

			start[0] += up[0] * 10;
			start[1] += up[1] * 10;
			start[2] += up[2] * 10;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

			start[0] -= up[0] * 20;
			start[1] -= up[1] * 20;
			start[2] -= up[2] * 20;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

		}
		else if (ent->hyper2 == 1)
		{
			//do something
			//random damage
			damage = ((int)random() % 20) + 1;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
			Com_Printf("Hyperblaster Damage was randomized:%d\n",damage);
		}
		else
		{
			//fire normally
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
		}
	}

		
	
	fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

	//blaster mods
	//mattMod blaster
	//ent->blasterBall = 1;
	//ent->blasterLine = 0;
	if (!hyper && ent->blasterHeal == 1)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (!hyper && ent->blasterLine == 1)
	{

		//Com_Printf("In blasterLine, shooting line:\n");
		while (go <= 50) //horizontal right
		{
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

				s[0] += r[0] * 10;
				s[1] += r[1] * 10;
				s[2] += r[2] * 10; //PITCH

				//s[2] += 20;

			go++;

		}

		s[0] = s0; //angle?
		s[1] = s1; //horizon
		s[2] = s2; //height
		go = 1;

		while (go <= 50) //horizontal left
		{
			
				fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
				s[0] -= r[0] * 10;
				s[1] -= r[1] * 10;
				s[2] -= r[2] * 10;

			go++;
		}
	}

	if (!hyper && ent->blasterBall == 1)
	{
		//Com_Printf("In blasterBall, shooting ball:\n");

		fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
		for (int c = 0;c < 3;c++)
		{
			s[1] += r[1] * 10;
			s[2] += r[2] * 10;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
		}

		s[2] = s2;

		for (int c = 0;c < 3;c++)
		{
			s[1] -= r[1] * 10;
			s[2] -= r[2] * 10;
			fire_blaster(ent, start, forward, damage, 1000, effect, hyper);
		}
	}

	/*
	start[0] += right[0]* 10;
	start[1] += right[1] *10;
	start[2] += right[2]*10;
	fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

	start[0] -= right[0] * 20;
	start[1] -= right[1] * 20;
	start[2] -= right[2] * 20;
	fire_blaster(ent, start, forward, damage, 1000, effect, hyper);

	//fire_rocket(ent, start, forward, 100, 8000, 100, 50);*/

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	if (hyper)
		gi.WriteByte (MZ_HYPERBLASTER | is_silenced);
	else
		gi.WriteByte (MZ_BLASTER | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);
}


void Weapon_Blaster_Fire (edict_t *ent)
{
	int		damage;

	if (deathmatch->value)
		damage = 15;
	//mattMod blaster damage
	else if (ent->blasterBall == 1)
	{
		damage = 8;
	}
	else if (ent->blasterLine == 1)
	{
		damage = 1;
	}
	else if (ent->blasterInstaKill == 5 || ent->hyperInsta == 5)
	{
		damage = 999;
		instaMatt();
	}
	else
	{
		damage = ent->blasterDMG;
		//Com_Printf("Blaster fired, no other mods Damage is: %d\n", damage);
	}
	

	Blaster_Fire (ent, vec3_origin, damage, false, EF_BLASTER);
	ent->client->ps.gunframe++;
}

void Weapon_Blaster (edict_t *ent)
{
	static int	pause_frames[]	= {19, 32, 0};
	static int	fire_frames[]	= {5, 0};

	Weapon_Generic (ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_Blaster_Fire);
}


void Weapon_HyperBlaster_Fire (edict_t *ent)
{
	float	rotation;
	vec3_t	offset;
	int		effect;
	int		damage;

	if (ent->hyperRegen == 5)
	{
		regenMatt();
		ent->client->pers.inventory[ent->client->ammo_index]++;
	}

	ent->client->weapon_sound = gi.soundindex("weapons/hyprbl1a.wav");

	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunframe++;
	}
	else
	{
		if (! ent->client->pers.inventory[ent->client->ammo_index] )
		{
			if (level.time >= ent->pain_debounce_time)
			{
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
				ent->pain_debounce_time = level.time + 1;
			}
			NoAmmoWeaponChange (ent);
		}
		else
		{
			rotation = (ent->client->ps.gunframe - 5) * 2*M_PI/6;
			offset[0] = -4 * sin(rotation);
			offset[1] = 0;
			offset[2] = 4 * cos(rotation);

			if ((ent->client->ps.gunframe == 6) || (ent->client->ps.gunframe == 9))
				effect = EF_HYPERBLASTER;
			else
				effect = 0;
			if (deathmatch->value)
				damage = 15;
			else
				damage = 20;
			Blaster_Fire (ent, offset, damage, true, effect);
			if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
				ent->client->pers.inventory[ent->client->ammo_index]--;

			ent->client->anim_priority = ANIM_ATTACK;
			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crattak1 - 1;
				ent->client->anim_end = FRAME_crattak9;
			}
			else
			{
				ent->s.frame = FRAME_attack1 - 1;
				ent->client->anim_end = FRAME_attack8;
			}
		}

		ent->client->ps.gunframe++;
		if (ent->client->ps.gunframe == 12 && ent->client->pers.inventory[ent->client->ammo_index])
			ent->client->ps.gunframe = 6;
	}

	if (ent->client->ps.gunframe == 12)
	{
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/hyprbd1a.wav"), 1, ATTN_NORM, 0);
		ent->client->weapon_sound = 0;
	}

}

void Weapon_HyperBlaster (edict_t *ent)
{
	static int	pause_frames[]	= {0};
	static int	fire_frames[]	= {6, 7, 8, 9, 10, 11, 0};

	Weapon_Generic (ent, 5, 20, 49, 53, pause_frames, fire_frames, Weapon_HyperBlaster_Fire);
}

/*
======================================================================

MACHINEGUN / CHAINGUN

======================================================================
*/

void Machinegun_Fire (edict_t *ent)
{
	int	i;
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		angles;
	int			damage = 8;
	int			kick = 2;
	vec3_t		offset;

	if (ent->machineGunJam == 5)
	{
		jammedMatt(ent);
		return;
	}


	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->machinegun_shots = 0;
		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->ps.gunframe == 5)
		ent->client->ps.gunframe = 4;
	else
		ent->client->ps.gunframe = 5;

	if (ent->client->pers.inventory[ent->client->ammo_index] < 1)
	{
		ent->client->ps.gunframe = 6;
		if (level.time >= ent->pain_debounce_time)
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
			ent->pain_debounce_time = level.time + 1;
		}
		NoAmmoWeaponChange (ent);
		return;
	}

	//Com_Printf("Inside shooting of machine gun\n");
	//Com_Printf("Jammed is: %d\n", ent->machineGunJam);
	//Com_Printf("Result of 5 and int: %d\n", ent->machineGunJam == 5);
	//Com_Printf("\n");



	
	if (ent->machineGunHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}
		

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	for (i=1 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}
	ent->client->kick_origin[0] = crandom() * 0.35;
	ent->client->kick_angles[0] = ent->client->machinegun_shots * -1.5;

	// raise the gun as it is firing
	if (!deathmatch->value)
	{
		ent->client->machinegun_shots++;
		if (ent->client->machinegun_shots > 9)
			ent->client->machinegun_shots = 9;
	}

	// get start / end positions
	VectorAdd (ent->client->v_angle, ent->client->kick_angles, angles);
	AngleVectors (angles, forward, right, NULL);
	VectorSet(offset, 0, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (ent->machineGunBack == 5)
	{
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		backwardsMatt(ent);
	}

	
	//mattMod Machinegun
	if (ent->machineGunBolt == 1)
	{
		//Com_Printf("Inside machine gunBolt\n");
		float* s = start;
		float* r = right;
		float* f = forward;

		damage = 3;
		kick = 0;

		if (ent->machineGunInstaKill == 5)
		{
			instaMatt();
			damage = 999;
		}



		if (ent->machineShootRockets == 1)
		{
			fire_rocket(ent, start, forward, damage, 650, 120, 120);
		}
		else
			fire_bullet(ent, start, forward, damage, kick, 0, 0, MOD_MACHINEGUN);

		s[0] += r[0] * 10;
		s[1] += r[1] * 10;
		s[2] += r[2] * 10;


		if (ent->machineShootRockets == 1)
		{
			fire_rocket(ent, start, forward, damage, 650, 120, 120);
		}
		else
			fire_bullet(ent, start, forward, damage, kick, 0, 0, MOD_MACHINEGUN);
		//fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);
		

		s[0] -= r[0] * 20;
		s[1] -= r[1] * 20;
		s[2] -= r[2] * 20;

		if (ent->machineShootRockets == 1)
		{
			fire_rocket(ent, start, forward, damage, 650, 120, 120);
		}
		else
			fire_bullet(ent, start, forward, damage, kick, 0, 0, MOD_MACHINEGUN);
		//fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);
	}
	else if (ent->machineGunDump == 1)
	{
		//mag dump the gun and massive kick
		//how to get access to the gun's current ammo count?
		//Com_Printf("Inside machine machineGunDump\n");
		kick = 10000000;

		if (ent->machineGunInstaKill == 5)
		{
			instaMatt();
			damage = 999;
		}

		int num = ent->client->pers.inventory[ent->client->ammo_index];


		for (int c = 1;c < num/2;c++)
		{
			if (ent->machineShootRockets == 1)
			{
				fire_rocket(ent, start, forward, damage, 650, 120, 120);
			}
			else
				fire_bullet(ent, start, forward, damage, kick, 0, 0, MOD_MACHINEGUN);
			//Com_Printf("Fired: %d \n",c);
		}

		ent->client->pers.inventory[ent->client->ammo_index = 0];



	}
	else if (ent->machineShootRockets == 1)
	{
		fire_rocket(ent, start, forward, damage, 650, 120, 120);
	}
	else
	{
		fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);
	}

	if (ent->machineGunRegen == 5)
	{
		//use this for generic ammo regen
		regenMatt(ent);
		ent->client->pers.inventory[ent->client->ammo_index]+=1;
	}

	

	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_MACHINEGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;

	ent->client->anim_priority = ANIM_ATTACK;
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		ent->s.frame = FRAME_crattak1 - (int) (random()+0.25);
		ent->client->anim_end = FRAME_crattak9;
	}
	else
	{
		ent->s.frame = FRAME_attack1 - (int) (random()+0.25);
		ent->client->anim_end = FRAME_attack8;
	}
}

void Weapon_Machinegun (edict_t *ent)
{
	static int	pause_frames[]	= {23, 45, 0};
	static int	fire_frames[]	= {4, 5, 0};

	Weapon_Generic (ent, 3, 5, 45, 49, pause_frames, fire_frames, Machinegun_Fire);
}

void Chaingun_Fire (edict_t *ent)
{
	int			i;
	int			shots;
	vec3_t		start;
	vec3_t		forward, right, up;
	float		r, u;
	vec3_t		offset;
	int			damage;
	int			kick = 2;

	//generics
	if (ent->chainJam == 5)
	{
		jammedMatt();
		return;
	}

	if (deathmatch->value)
		damage = 6;
	else
		damage = 8;

	if (ent->client->ps.gunframe == 5)
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnu1a.wav"), 1, ATTN_IDLE, 0);

	if ((ent->client->ps.gunframe == 14) && !(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunframe = 32;
		ent->client->weapon_sound = 0;
		return;
	}
	else if ((ent->client->ps.gunframe == 21) && (ent->client->buttons & BUTTON_ATTACK)
		&& ent->client->pers.inventory[ent->client->ammo_index])
	{
		ent->client->ps.gunframe = 15;
	}
	else
	{
		ent->client->ps.gunframe++;
	}

	if (ent->client->ps.gunframe == 22)
	{
		ent->client->weapon_sound = 0;
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnd1a.wav"), 1, ATTN_IDLE, 0);
	}
	else
	{
		ent->client->weapon_sound = gi.soundindex("weapons/chngnl1a.wav");
	}

	ent->client->anim_priority = ANIM_ATTACK;
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		ent->s.frame = FRAME_crattak1 - (ent->client->ps.gunframe & 1);
		ent->client->anim_end = FRAME_crattak9;
	}
	else
	{
		ent->s.frame = FRAME_attack1 - (ent->client->ps.gunframe & 1);
		ent->client->anim_end = FRAME_attack8;
	}

	if (ent->client->ps.gunframe <= 9)
		shots = 1;
	else if (ent->client->ps.gunframe <= 14)
	{
		if (ent->client->buttons & BUTTON_ATTACK)
			shots = 2;
		else
			shots = 1;
	}
	else
		shots = 3;

	if (ent->client->pers.inventory[ent->client->ammo_index] < shots)
		shots = ent->client->pers.inventory[ent->client->ammo_index];

	if (!shots)
	{
		if (level.time >= ent->pain_debounce_time)
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
			ent->pain_debounce_time = level.time + 1;
		}
		NoAmmoWeaponChange (ent);
		return;
	}


	//mattmod chaingun

	


	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	for (i=0 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}

	if (ent->chainBack == 5)
	{
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		backwardsMatt();
	}

	if (ent->chainHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->chainInsta)
	{
		damage = 999;
		instaMatt();
	}

	if (ent->chaingun1 == 2)
	{
		//No spread
		for (i = 0; i < shots; i++)
		{
			// get start / end positions
			AngleVectors(ent->client->v_angle, forward, right, up);
			//r = 7 + crandom() * 4;
			//u = crandom() * 4;
			//VectorSet(offset, 0, r, u + ent->viewheight - 8);
			P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

			fire_bullet(ent, start, forward, damage, kick, 0, 0, MOD_CHAINGUN);
		}
	}
	else if (ent->chaingun2 == 2)
	{
		//Double the bullets, massive spread
		for (i = 0; i < shots; i++)
		{
			AngleVectors(ent->client->v_angle, forward, right, up);
			r = 7 + crandom() * 240;
			u = crandom() * 240;
			VectorSet(offset, 0, r, u + ent->viewheight - 8);
			P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

			fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
			fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
		}
	}
	else if (ent->chaingun3 == 2)
	{
		for (i = 0; i < shots; i++)
		{
			// get start / end positions
			AngleVectors(ent->client->v_angle, forward, right, up);
			r = 7 + crandom() * 4;
			u = crandom() * 4;
			VectorSet(offset, 0, r, u + ent->viewheight - 8);
			P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

			fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
			fire_bullet(ent, start, right, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
			right[0] *= -1;
			right[1] *= -1;
			right[2] *= -1;
			fire_bullet(ent, start, right, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
		}
	}
	else
	{
		//default behavior
		for (i = 0; i < shots; i++)
		{
			// get start / end positions
			AngleVectors(ent->client->v_angle, forward, right, up);
			r = 7 + crandom() * 4;
			u = crandom() * 4;
			VectorSet(offset, 0, r, u + ent->viewheight - 8);
			P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

			fire_bullet(ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
		}
	}
	

	if (ent->chainRegen == 5)
	{
		ent->client->pers.inventory[ent->client->ammo_index] += shots;
	}

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte ((MZ_CHAINGUN1 + shots - 1) | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= shots;
}


void Weapon_Chaingun (edict_t *ent)
{
	static int	pause_frames[]	= {38, 43, 51, 61, 0};
	static int	fire_frames[]	= {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 0};

	Weapon_Generic (ent, 4, 31, 61, 64, pause_frames, fire_frames, Chaingun_Fire);
}


/*
======================================================================

SHOTGUN / SUPERSHOTGUN

======================================================================
*/

void weapon_shotgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage = 4;
	int			kick = 8;
	

	if (ent->shotgunGunJam == 5)
	{
		jammedMatt();
		return;
	}

	if (ent->client->ps.gunframe == 9)
	{
		ent->client->ps.gunframe++;
		return;
	}




	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	VectorSet(offset, 0, 8,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	//mattMod shotgun


	if (ent->shotgunGunBack == 5)
	{
		backwardsMatt(ent);
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	if (ent->shotgunGunHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->shotgunGunRegen == 5)
	{
		regenMatt();
		ent->client->pers.inventory[ent->client->ammo_index] += 1;
	}

	if (ent->shotgunGunInstaKill == 5)
	{
		instaMatt();
		damage = 999;
	}

	int s1 = start[0];
	int s2 = start[1];
	int s3 = start[2];

	if (ent->shotgunMeme2 == 1)
	{
		damage = rand() % 20 + 1;
	}

	Com_Printf("Shotgun damage is: %d\n", damage);

	if (ent->shotgunSlug == 1)
	{
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
		
		start[0] += right[0] * 10;
		start[1] += right[1] * 10;
		start[2] += right[2] * 10;
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

		start[0] = s1 -= right[0] * 10;
		start[1] = s2 -= right[1] * 10;
		start[2] = s3 -= right[2] * 10;
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
		
	}
	else if (ent->shotgunMeme3 == 1)
	{
		// Up/Down start[0]
		//start[0] += start[0] *10;
		
		//Com_Printf("Inside Shotgun Meme3\n");

		int s1 = start[0];
		int s2 = start[1];
		int s3 = start[2];

		for (int c = 0;c < 10;c++)
		{
			if (c % 2 == 0)
			{
				fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
				start[0] = s1;
				start[0] += right[0] * (c * 10);
			}
			else
			{
				fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
				start[0] = s1;
				start[0] -= right[0] * (c * 10);
			}


		}


	}
	else
	{
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
	}

	/*if (deathmatch->value)
	{
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_DEATHMATCH_SHOTGUN_COUNT, MOD_SHOTGUN);
	}
	else
	{
		fire_shotgun(ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
	}*/
		

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_SHOTGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_Shotgun (edict_t *ent)
{
	static int	pause_frames[]	= {22, 28, 34, 0};
	static int	fire_frames[]	= {8, 9, 0};

	Weapon_Generic (ent, 7, 18, 36, 39, pause_frames, fire_frames, weapon_shotgun_fire);
}


void weapon_supershotgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	vec3_t		v;
	int			damage = 6;
	int			kick = 12;

	//mattMod superShotgun
	if (ent->superJam == 5)
	{
		jammedMatt();
		ent->client->ps.gunframe++;
		return;
	}

	if (ent->superHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->superInsta == 5)
	{
		instaMatt();
		damage = 999;
	}

	if (ent->superRegen == 5)
	{
		regenMatt();
		ent->client->pers.inventory[ent->client->ammo_index] += 1;
	}

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	VectorSet(offset, 0, 8,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	v[PITCH] = ent->client->v_angle[PITCH];
	v[YAW]   = ent->client->v_angle[YAW] - 5;
	v[ROLL]  = ent->client->v_angle[ROLL];
	AngleVectors (v, forward, NULL, NULL);
	//fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);
	v[YAW]   = ent->client->v_angle[YAW] + 5;
	AngleVectors (v, forward, NULL, NULL);
	//fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	//mattmod
	if (ent->superShot1 == 1)
	{
		float	timer;
		int		speed;
		float	radius;

		radius = damage + 40;
		timer = ent->client->grenade_time - level.time;
		speed = GRENADE_MINSPEED + (GRENADE_TIMER - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER);
		fire_grenade2(ent, start, forward, damage, speed, timer, radius, false);

		start[0] *= -1;
		start[1] *= -1;
		start[2] *= -1;

		fire_grenade2(ent, start, forward, damage, speed, timer, radius, false);
	}
	else if (ent->superShot2 == 1)
	{
		fire_shotgun(ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);

		start[0] += right[0] * 10;
		start[1] += right[1] * 10;
		start[2] += right[2] * 10;

		start[0] -= right[0] * 20;
		start[1] -= right[1] * 20;
		start[2] -= right[2] * 20;
	}
	else if (ent->superShot3)
	{
		fire_rocket(ent, start, forward, 0, 650, 0, 0);
		fire_shotgun(ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);
	}
	else
	{
		v[PITCH] = ent->client->v_angle[PITCH];
		v[YAW] = ent->client->v_angle[YAW] - 5;
		v[ROLL] = ent->client->v_angle[ROLL];
		AngleVectors(v, forward, NULL, NULL);
		fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);
		v[YAW] = ent->client->v_angle[YAW] + 5;
		AngleVectors(v, forward, NULL, NULL);
		fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);
	}
	



	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_SSHOTGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= 2;
}

void Weapon_SuperShotgun (edict_t *ent)
{
	static int	pause_frames[]	= {29, 42, 57, 0};
	static int	fire_frames[]	= {7, 0};

	Weapon_Generic (ent, 6, 17, 57, 61, pause_frames, fire_frames, weapon_supershotgun_fire);
}



/*
======================================================================

RAILGUN

======================================================================
*/

void weapon_railgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage;
	int			kick;

	//mattmod railgun
	//mattmodrailgun

	if (ent->railJam == 5)
	{
		jammedMatt();
		return;
	}

	if (deathmatch->value)
	{	// normal damage is too extreme in dm
		damage = 100;
		kick = 200;
	}
	else if (ent->railInsta == 5)
	{
		damage = 999;
		kick = 200;
	}
	else
	{
		damage = 150;
		kick = 250;
	}

	if (ent->railRegen == 5)
	{
		ent->client->pers.inventory[ent->client->ammo_index]++;
		regenMatt();
	}

	if (ent->railHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->railBack == 5)
	{
		backwardsMatt();
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -3, ent->client->kick_origin);
	ent->client->kick_angles[0] = -3;

	VectorSet(offset, 0, 7,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (ent->rail1 == 1)
	{
		fire_rail(ent, start, forward, damage, kick);
		fire_rail(ent, start, right, damage, kick);
		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;
		fire_rail(ent, start, right, damage, kick);

	}
	else if (ent->rail2 == 1)
	{
		//halfRange
		fire_railMatt(ent, start, forward, damage, kick);
	}
	else if (ent->rail3 == 1)
	{
		damage = random() * 100 + 10;
		fire_rail(ent, start, forward, damage, kick);
	}
	else
		fire_rail (ent, start, forward, damage, kick);

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_RAILGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}


void Weapon_Railgun (edict_t *ent)
{
	static int	pause_frames[]	= {56, 0};
	static int	fire_frames[]	= {4, 0};

	Weapon_Generic (ent, 3, 18, 56, 61, pause_frames, fire_frames, weapon_railgun_fire);
}


/*
======================================================================

BFG10K

======================================================================
*/

void weapon_bfg_fire (edict_t *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage;
	float	damage_radius = 1000;

	if (deathmatch->value)
		damage = 200;
	else
		damage = 500;

	if (ent->BFGJam == 5)
	{
		jammedMatt();
		return;
	}

	if (ent->BFGRegen == 5)
	{
		regenMatt();
		ent->client->pers.inventory[ent->client->ammo_index] += 50;
	}

	if (ent->BFGHeal == 5)
	{
		healMatt(ent);
		ent->health += 10;
	}

	if (ent->BFGInsta == 5)
	{
		instaMatt();
		damage = 999;
	}

	if (ent->BFGBack == 5)
	{
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
	}

	if (ent->client->ps.gunframe == 9)
	{
		// send muzzle flash
		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent-g_edicts);
		gi.WriteByte (MZ_BFG | is_silenced);
		gi.multicast (ent->s.origin, MULTICAST_PVS);

		ent->client->ps.gunframe++;

		PlayerNoise(ent, ent->s.origin, PNOISE_WEAPON);
		return;
	}

	// cells can go down during windup (from power armor hits), so
	// check again and abort firing if we don't have enough now
	if (ent->client->pers.inventory[ent->client->ammo_index] < 50)
	{
		ent->client->ps.gunframe++;
		return;
	}

	if (is_quad)
		damage *= 4;

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);

	// make a big pitch kick with an inverse fall
	ent->client->v_dmg_pitch = -40;
	ent->client->v_dmg_roll = crandom()*8;
	ent->client->v_dmg_time = level.time + DAMAGE_TIME;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	//mattmod bfg
	if (ent->BFG1 == 1)
	{
		
		//directly behind
		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		//to the right
		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;

		//to the left
		fire_bfg(ent, start, forward, damage, 400, damage_radius);



		//back left
		vec3_t diag;
		VectorAdd(forward, right, diag);
		VectorNormalize(diag);

		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		//back right
		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;

		vec3_t diag2;
		VectorAdd(forward, right, diag2);
		VectorNormalize(diag2);

		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		//not working
		//front right

		forward[0] *= -1;
		forward[1] *= -1;
		forward[2] *= -1;
		vec3_t diag3;

		VectorAdd(forward, right, diag3);
		VectorNormalize(diag3);

		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		//front left
		right[0] *= -1;
		right[1] *= -1;
		right[2] *= -1;
		vec3_t diag4;
		VectorAdd(forward, right, diag4);
		VectorNormalize(diag4);

		fire_bfg(ent, start, forward, damage, 400, damage_radius);
	}

	if (ent->BFG2 == 1)
	{
		int num = ent->client->pers.inventory[ent->client->ammo_index];

		for (int c = 1;c < num / 2;c++)
		{
			fire_bfg(ent, start, forward, damage, 400, damage_radius);
			//Com_Printf("Fired: %d \n",c);
		}

		ent->client->pers.inventory[ent->client->ammo_index = 0];
	}

	if (ent->BFG3 == 1)
	{
		//fire everything
		fire_bfg(ent, start, forward, damage, 400, damage_radius);

		fire_bullet(ent, start, forward, damage, 0, 0, 0, MOD_MACHINEGUN);
		fire_shotgun(ent, start, forward, damage, 0, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);
		fire_blaster(ent, start, forward, damage, 1000, 0, true);

		fire_grenade(ent, start, forward, damage, 600, 2.5, 150);
		fire_grenade2(ent, start, forward, damage, 90, 10, 90, 0);
		fire_rocket(ent, start, forward, damage, 650, damage_radius, 150);
		fire_rail(ent, start, forward, damage, 0);
	}
	
	fire_bfg (ent, start, forward, damage, 400, damage_radius);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= 50;
}

void Weapon_BFG (edict_t *ent)
{
	static int	pause_frames[]	= {39, 45, 50, 55, 0};
	static int	fire_frames[]	= {9, 17, 0};

	Weapon_Generic (ent, 8, 32, 55, 58, pause_frames, fire_frames, weapon_bfg_fire);
}


//======================================================================

