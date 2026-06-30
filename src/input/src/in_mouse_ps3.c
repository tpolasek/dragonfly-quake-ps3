/*
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) Henrique Barateli, <henriquejb194@gmail.com>, et al.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
// in_mouse_ps3.c -- no-op mouse stub for PS3.
//
// The DualShock 3 has no pointer; IN_JoyMove owns all gameplay input via
// PSL1GHT's io/pad.h. We provide the symbols in_main.c calls so the input
// subsystem links without the desktop mouse code.


#include "in_mouse.h"
#include "quakedef.h"


void IN_InitMouse(void) {
}

void IN_MouseMove(usercmd_t* cmd) {
    (void) cmd;
}

void IN_ActivateMouse(void) {
}

void IN_DeactivateMouse(void) {
}

void IN_ShowMouse(void) {
}

void IN_HideMouse(void) {
}
