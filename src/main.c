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


#include "host.h"
#include "sys.h"
#include <SDL_main.h>

#ifdef CHOCOLATE_QUAKE_PS3
#include <unistd.h>
#endif


int main(int argc, char* argv[]) {
    quakeparms_t* parms = Sys_Init(argc, argv);
    SYS_TRACE("main: Sys_Init returned (parms=%p), calling Host_Init\n",
              (void*) parms);
    Host_Init(parms);
    SYS_TRACE("main: Host_Init returned, entering frame loop\n");

    double old_time = Sys_FloatTime();
    while (true) {
#ifdef CHOCOLATE_QUAKE_PS3
        // PS3: if the XMB overlay is open (PS button), park the loop so
        // we don't fight the OS for the framebuffer or waste CPU.
        if (Sys_XmbMenuOpen()) {
            usleep(16000);
            continue;
        }
#endif
        double new_time = Sys_FloatTime();
        double dt = new_time - old_time;
        Host_Frame((float) dt);
        old_time = new_time;
    }
}
