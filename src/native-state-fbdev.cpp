/*
 * Copyright © 2012 Linaro Limited
 * Copyright © 2013 Canonical Ltd
 *
 * This file is part of the glmark2 OpenGL (ES) 2.0 benchmark.
 *
 * glmark2 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * glmark2 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * glmark2.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *  Simon Que
 *  Jesse Barker
 *  Alexandros Frantzis
 *  Wladimir J. van der Laan
 */
#include "native-state-fbdev.h"
#include "log.h"
#include "util.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <EGL/eglvivante.h>

/******************
 * Public methods *
 ******************/

bool
NativeStateFBDEV::init_display()
{
    if (native_display != 0)
        return true;

    /* always fb0 for now */
    native_display = fbGetDisplayByIndex(0);
    if(native_display == 0)
    {
        Log::error("Error: Cannot open display %i\n", 0);
        return false;
    }

    signal(SIGINT, &NativeStateFBDEV::quit_handler);

    return true;
}

void*
NativeStateFBDEV::display()
{
    return native_display;
}

bool
NativeStateFBDEV::create_window(WindowProperties const& /*properties*/)
{
    native_window = fbCreateWindow(native_display, -1, -1, 0, 0);
    if(native_window == 0)
    {
        Log::error("Error: Cannot create window\n");
        return false;
    }
    return true;
}

void*
NativeStateFBDEV::window(WindowProperties& properties)
{
    fbGetWindowGeometry(native_window, 0, 0, &properties.width, &properties.height);
    properties.fullscreen = true;
    properties.visual_id = 0;
    return native_window;
}

void
NativeStateFBDEV::visible(bool /*visible*/)
{
}

bool
NativeStateFBDEV::should_quit()
{
    return should_quit_;
}

void
NativeStateFBDEV::flip()
{
}

/*******************
 * Private methods *
 *******************/

volatile std::sig_atomic_t NativeStateFBDEV::should_quit_(false);

void
NativeStateFBDEV::quit_handler(int /*signo*/)
{
    should_quit_ = true;
}

void
NativeStateFBDEV::cleanup()
{
    if(native_window != 0) {
        fbDestroyWindow(native_window);
        native_window = 0;
    }
    if(native_display != 0) {
        fbDestroyDisplay(native_display);
        native_display = 0;
    }
}
