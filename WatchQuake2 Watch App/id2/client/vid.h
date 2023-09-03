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
// vid.h -- video driver defs

#ifndef VID_H
#define VID_H

typedef struct vrect_s
{
	int				x,y,width,height;
} vrect_t;

typedef uint8_t pixel_t;

typedef struct
{
// from r_local
    pixel_t                 *buffer;                // invisible buffer
    pixel_t                 *colormap;              // 256 * VID_GRADES size
    pixel_t                 *alphamap;              // 256 * 256 translucency map
    int                             rowbytes;               // may be > width if displayed in a window
// original
	int		width;		
	int		height;
} viddef_t;

extern	viddef_t	viddef;				// global video state

// Video module initialisation etc
void	VID_Init (void);
void	VID_Shutdown (void);
void	VID_CheckChanges (void);

void	VID_MenuInit( void );
void	VID_MenuDraw( void );
const char *VID_MenuKey( int );

#endif
