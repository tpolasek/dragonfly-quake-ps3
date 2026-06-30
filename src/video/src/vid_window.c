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
// vid_window.c -- handles window creation, management and resizing.


#include "vid_window.h"
#include "config.h"
#include "cvar.h"
#include "input.h"
#include "sys.h"
#include "vid_buffers.h"

/*
================================================================================

PS3 NATIVE WINDOW

================================================================================
*/

static cvar_t _windowed_mouse = {"_windowed_mouse", "0", true};

static void VID_RegisterCvars(void) {
    Cvar_RegisterVariable(&_windowed_mouse);
}

void VID_InitWindow(void) {
    VID_RegisterCvars();
    VID_PS3_Init();
}

void VID_ShutdownWindow(void) {
    VID_PS3_Shutdown();
}

static void VID_UpdateScreen(vrect_t* rect) {
    if (!rect) return;
    VID_UpdateAndPresent(rect);
}

void VID_UpdateWindow(vrect_t* rect) {
    VID_UpdateScreen(rect);
}

void VID_ResizeScreen(void) {
    VID_ReallocBuffers();
}

// Mouse stubs — no mouse on PS3.
static void VID_CenterMouse(void) {}
static void VID_ReleaseMouse(void) { IN_DeactivateMouse(); }
static void VID_GrabMouse(void) { IN_ActivateMouse(); }
qboolean VID_WindowedMouse(void) { return false; }
void VID_ToggleMouseGrab(void) {}
void VID_HandlePause(qboolean pause) { (void)pause; }
void VID_MinimizeWindow(void) {}

// Mode stubs — single fixed mode on PS3.
static void VID_SetWindowed(void) {}
static void VID_SetFullscreen(void) {}

//==============================================================================
