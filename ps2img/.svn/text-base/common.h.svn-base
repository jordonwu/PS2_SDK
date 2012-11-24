/**
 * ps2img - Create, inspect or extract Playstation 2 ROM image files
 * Copyright (c) 2005 Damien Ciabrini (dciabrin), Olivier Parra (yo6)
 *
 *     ps2img is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of
 *     the License, or (at your option) any later version.
 *
 *     ps2img is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with ps2img; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *     02111-1307 USA
 *
 * $Id$
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <time.h>



#define PAD4( x )  (((x) + 0x3) & ~0x3)
#define PAD16( x ) (((x) + 0xF) & ~0xF)


extern char *program_name;
extern int verbose;
/*---------------------------------------------------------------------*/
/*    ROM image layout:                                                */
/*    -------------------------------------------------------------    */
/*    The ROM image file is composed of three sections:                */
/*      * the ROMDIR section: lists all IRX entries present            */
/*        in the image, along with some infos (date, size...)          */
/*      * the EXTINFO section: contains a long description of          */
/*        every ROMDIR entry, for example the IRX version, the IRX     */
/*        description, its creation time.                              */
/*      * the IRX section: contains all the IRX that will              */
/*        replace the IRXs present in your PS2 BIOS.                   */
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/*    The ROMDIR structure ...                                         */
/*---------------------------------------------------------------------*/
typedef struct
{
  char name[10];
  unsigned short extinfo_size;
  int size;
} romdir_t;

/*---------------------------------------------------------------------*/
/*    The EXTINFO structure ...                                        */
/*---------------------------------------------------------------------*/
typedef struct
{
  unsigned short value;
  unsigned char size;
  unsigned char id;
} extinfo_t;

#define EXTINFO_ID_DATE     1
#define EXTINFO_ID_VERSION  2
#define EXTINFO_ID_DESCR    3
#define EXTINFO_ID_NULL     0x7F


/*---------------------------------------------------------------------*/
/*    The IRX entry structure ...                                      */
/*---------------------------------------------------------------------*/
typedef struct
{
  char name[10];
  char flags;
  unsigned date;
  unsigned short version;
  char descr[256];
  int irx_size;
  char *irx_binary;
} entry_t;

#define ENTRY_FLAG_DATE     0x1
#define ENTRY_FLAG_VERSION  0x2
#define ENTRY_FLAG_DESCR    0x4
#define ENTRY_FLAG_NULL     0x8



extern char name_format[];
extern char size_format[];



void read_file (char *irx, char **data, int *size);
int digits_in_number (int num);
char *basename (char *path);
void verbose_set_length_of_size_column (int length);
void verbose_set_length_of_name_column (int length);
void verbose_display_header ();
void verbose_dump_entry_info (entry_t * e);
int time_t_to_hexa (time_t * time);
void fatal (char *format, ...);
void fatal_with_errno (char *format, ...);

#endif
