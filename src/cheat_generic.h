/*

	PROJECT:		m0d_s0beit_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright 2007, 2008, 2009, 2010 we_sux

	m0d_s0beit_sa is available from http://code.google.com/p/m0d-s0beit-sa/

	m0d_s0beit_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	m0d_s0beit_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with m0d_s0beit_sa.  If not, see <http://www.gnu.org/licenses/>.

	$LastChangedDate: 2009-12-14 01:12:56 -0600 (Mon, 14 Dec 2009) $
	$LastChangedBy: futnucks $
	$Revision: 31 $
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/cheat_generic.h $
	$Id: cheat_generic.h 31 2009-12-14 07:12:56Z futnucks $

*/

struct gta_cheat
{
   uint8_t     *addr;
   const char  *name;
};

int cheat_panic(void);
void cheat_prevent_freeze(void);
void cheat_teleport(const float pos[3], int interior_id);
void cheat_teleport_nearest_car(void);
void cheat_handle_misc(void);
void cheat_handle_debug(HWND wnd);
void cheat_handle_spoof_weapon(void);
void cheat_handle_weapon_disable(void);
void cheat_handle_freeze_vehicles(struct vehicle_info *vehicle_info, struct actor_info *actor_info);
void cheat_handle_unlock(void);
void cheat_handle_hp(struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff);
void cheat_handle_stick(struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff);
void cheat_handle_money(void);
void cheat_handle_weapon(void);
void cheat_handle_teleport(struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff);
void cheat_handle_checkpoint(void);
void cheat_handle_unfreeze(struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff);
void cheat_handle_emo(struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff);