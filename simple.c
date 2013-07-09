/*
 * Simple World. A simple playground.
 *
 * (C) Copyright 2013 Pablo Martin Medrano <pablo@odkq.com> 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3
 * of the License.
 */
#include <string.h>
#include <graphic.h>

int main(int argc, char **argv)
{
	argc = gl_start(argc, argv);
	gl_loop();
	gl_end();
	return 0;
}

