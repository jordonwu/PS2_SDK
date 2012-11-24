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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "elf.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/*---------------------------------------------------------------------*/
/*    get_entry_extinfo_size                                           */
/*    -------------------------------------------------------------    */
/*    Compute the size this entry would take in the extinfo section    */
/*---------------------------------------------------------------------*/
static int get_entry_extinfo_size( entry_t * entry )
{
  int size = 0;
  if ( entry->flags & ENTRY_FLAG_DATE )
    size += sizeof( extinfo_t ) + 4;
  if ( entry->flags & ENTRY_FLAG_VERSION )
    size += sizeof( extinfo_t );
  if ( entry->flags & ENTRY_FLAG_DESCR )
    size += sizeof( extinfo_t ) + PAD4( strlen( entry->descr ) + 1 );
  if ( entry->flags & ENTRY_FLAG_NULL )
    size += sizeof( extinfo_t ) + 4;
  return size;
}


/*---------------------------------------------------------------------*/
/*    create_romdir_section ...                                        */
/*    -------------------------------------------------------------    */
/*    Build a raw ROMDIR section, given a list of ROM entries.         */
/*    The raw data is eventually saved to disk.                        */
/*---------------------------------------------------------------------*/
static romdir_t *create_romdir_section( entry_t * entry, int nb_entries )
{
  int i;

  // Create directory entries
  // total_entries = number of real entries plus 1 dummy at the end
  int total_entries = nb_entries + 1;
  romdir_t *romdir;
  if ( ( romdir = malloc( sizeof( romdir_t ) * total_entries ) ) == NULL )
    fatal_with_errno( "Cannot allocate %d bytes of memory",
                      sizeof( romdir_t ) * total_entries );

  memset( romdir, 0, total_entries * sizeof( romdir_t ) );

  // Copy files names into ROMDIR 
  for ( i = 0; i < nb_entries; i++ )
    strcpy( romdir[i].name, entry[i].name );

  // Set user file sizes into ROMDIR
  for ( i = 3; i < nb_entries; i++ ) {
    romdir[i].size = entry[i].irx_size;
  }

  // Compute the third entries size and extinfo sizes
  // the size of the RESET entry is always 0
  romdir[0].size = 0;
  // The size of the ROMDIR entry is the total size of the ROMDIR section
  romdir[1].size = total_entries * sizeof( romdir_t );
  // The size of the EXTINFO entry is the total size of the EXTINFO section
  romdir[2].size = 0;
  // Fill file attributes and size of user entries  
  for ( i = 0; i < nb_entries; i++ ) {
    // get the extinfo_size of this entry
    romdir[i].extinfo_size = get_entry_extinfo_size( &entry[i] );
    // Update EXTINFO section's size
    romdir[2].size += romdir[i].extinfo_size;
  }
  // dummy size for verbose mode
  entry[0].irx_size = romdir[0].size;
  entry[1].irx_size = romdir[1].size;
  entry[2].irx_size = romdir[2].size;

  return romdir;
}


/*---------------------------------------------------------------------*/
/*    create_extinfo_section ...                                       */
/*    -------------------------------------------------------------    */
/*    Build a raw EXTINFO section, given a list of ROM entries and     */
/*    a ROMDIR section. The raw data is eventually saved to disk.      */
/*---------------------------------------------------------------------*/
static char *create_extinfo_section( char *bytes, entry_t * entry,
                                     int nb_entries )
{
  int i;
  int off = 0;
  extinfo_t *inf;

  for ( i = 0; i < nb_entries; i++ ) {
    if ( entry[i].flags & ENTRY_FLAG_DATE ) {
      inf = ( extinfo_t * ) ( bytes + off );
      inf->value = 0;
      inf->size = 4;
      inf->id = EXTINFO_ID_DATE;
      off += sizeof( extinfo_t );
      *( unsigned * ) ( bytes + off ) = entry[i].date;
      off += inf->size;
    }

    if ( entry[i].flags & ENTRY_FLAG_VERSION ) {
      inf = ( extinfo_t * ) ( bytes + off );
      inf->value = entry[i].version;
      inf->size = 0;
      inf->id = EXTINFO_ID_VERSION;
      off += sizeof( extinfo_t );
    }

    if ( entry[i].flags & ENTRY_FLAG_DESCR ) {
      inf = ( extinfo_t * ) ( bytes + off );
      inf->value = 0;
      inf->size = PAD4( strlen( entry[i].descr ) + 1 );
      inf->id = EXTINFO_ID_DESCR;
      off += sizeof( extinfo_t );
      memset( bytes + off, 0, inf->size );
      strcpy( bytes + off, entry[i].descr );
      off += inf->size;
    }

    if ( entry[i].flags & ENTRY_FLAG_NULL ) {
      inf = ( extinfo_t * ) ( bytes + off );
      inf->value = 0;
      inf->size = 4;
      inf->id = EXTINFO_ID_NULL;
      off += sizeof( extinfo_t );
      memset( bytes + off, 0, inf->size );
      off += inf->size;
    }
  }

  return bytes;
}


/*---------------------------------------------------------------------*/
/*    init_entry_from_irx                                              */
/*    -------------------------------------------------------------    */
/*    Load an IRX into memory and fill a ROM file entry element.       */
/*---------------------------------------------------------------------*/
void init_entry_from_irx( entry_t * entry, char *irx )
{
  struct stat st;
  read_file( irx, &entry->irx_binary, &entry->irx_size );

  if ( stat( irx, &st ) == -1 )
    fatal_with_errno( "Cannot stat file %s", irx );

  irx = basename( irx );
  if ( strlen( irx ) > 9 )
    fatal( "invalid ROM file entry %s: name too long" );
  else
    strcpy( entry->name, irx );

  entry->date = time_t_to_hexa( &st.st_mtime );

  entry->flags = ENTRY_FLAG_DATE | ENTRY_FLAG_VERSION | ENTRY_FLAG_DESCR;

  char *boot_elf = ( char * ) entry->irx_binary;
  Elf32_Ehdr *eh = ( Elf32_Ehdr * ) boot_elf;
  Elf32_Shdr *esh = ( Elf32_Shdr * ) ( boot_elf + eh->e_shoff );
  char *sh_str_table = boot_elf + esh[eh->e_shstrndx].sh_offset;
  int i;

  // search for the start of section .iopmod in the elf
  // don't search in the first section descriptor as it is a dummy
  for ( i = 1; i < eh->e_shnum; i++ ) {
    if ( strcmp( sh_str_table + esh[i].sh_name, ".iopmod" ) == 0 ) {
      char *iopmodsec = ( char * ) ( boot_elf + esh[i].sh_offset );
      // We skip the first 24 bytes of the section. it contains:
      // [4 bytes] a magic word
      // [4 bytes] the IRX's start address
      // [4 bytes] the value of the GP register
      // [4 bytes] the size of the .text section
      // [4 bytes] the size of the .data section
      // [4 bytes] the size of the .bss section
      entry->version = ( *( iopmodsec + 24 ) << 8 ) + *( iopmodsec + 25 );
      strcpy( entry->descr, iopmodsec + 26 );
      return;
    }
  }
  fatal( "Invalid IRX %s: .iopmod section not found.", irx );
}


/*---------------------------------------------------------------------*/
/*    make_romdir_description                                          */
/*    -------------------------------------------------------------    */
/*    Compute a special descriptor for the ROMDIR entry.               */
/*    The descriptor contains the mandatory fields:                    */
/*          * The date and time of creation,                           */
/*          * The name of a dummy config file                          */
/*          * The name of the ROM image,                               */
/*          * The name of the user that created the image,             */
/*          * The host and path where the image is being built.        */
/*---------------------------------------------------------------------*/
void make_romdir_description( char *img_name, entry_t * entry, time_t * time )
{
  char path[200];
  char host[200];
  char *home = getenv( "HOME" );

  // get hostname and current path
  gethostname( host, sizeof( host ) );
  if ( getcwd( path, sizeof( path ) ) == NULL )
    fatal_with_errno( "Could not get current working directory" );

  // wipe homedir off from path if any
  char *loc = strstr( path, home );
  if ( loc != NULL )
    loc = loc + strlen( home );
  else
    loc = path;

  struct tm m;
  localtime_r( time, &m );

  // generate the 4 elements description string
  snprintf( entry->descr, sizeof( entry->descr ),
            "%x-%02d%02d%02d,dummyconf,%s,%s@%s%s",
            time_t_to_hexa( time ), m.tm_hour, m.tm_min, m.tm_sec,
            basename( img_name ), getenv( "USERNAME" ), host, loc );
}


/*---------------------------------------------------------------------*/
/*    verbose_print_add_message                                        */
/*    -------------------------------------------------------------    */
/*    Print the `add' message with proper padding                      */
/*---------------------------------------------------------------------*/
void verbose_print_add_message( const char *irx, int size )
{
  printf( "Adding " );
  printf( name_format, irx );
  printf( "(%d bytes)\n", size );
}

/*---------------------------------------------------------------------*/
/*    Padding buffer...                                                */
/*---------------------------------------------------------------------*/
char zeros_buffer[16];


/*---------------------------------------------------------------------*/
/*    create_image                                                     */
/*    -------------------------------------------------------------    */
/*    Build a raw ROM image file, given a list of IRX files.           */
/*    The created image is then saved to disk.                         */
/*---------------------------------------------------------------------*/
void create_image( char *image_name, char *irx_args[], int num_irx )
{
  int nb_entries = num_irx + 3;
  entry_t *entry;
  int i;
  int size = 0;
  if ( ( entry = malloc( sizeof( entry_t ) * nb_entries ) ) == NULL )
    fatal_with_errno( "Cannot allocate %d bytes of memory",
                      sizeof( entry_t ) * nb_entries );

  for ( i = 0; i < num_irx; i++ ) {
    init_entry_from_irx( &entry[i + 3], irx_args[i] );
    if ( size < entry[i + 3].irx_size )
      size = entry[i + 3].irx_size;
  }
  verbose_set_length_of_size_column( digits_in_number( size ) );

  // Get current time for meta entries
  time_t curtime;
  time( &curtime );

  // Init first meta-entry
  strcpy( entry[0].name, "RESET" );
  entry[0].flags = ENTRY_FLAG_DATE;
  entry[0].date = time_t_to_hexa( &curtime );

  // Init second meta-entry
  strcpy( entry[1].name, "ROMDIR" );
  entry[1].flags = ENTRY_FLAG_DESCR;
  make_romdir_description( image_name, &entry[1], &curtime );

  // Init third meta-entry
  strcpy( entry[2].name, "EXTINFO" );
  entry[2].flags = ENTRY_FLAG_NULL;

  // Dump filesystem info
  if ( verbose ) {
    printf( "Creating ROM image %s with the following entries:\n",
            image_name );
    verbose_display_header(  );
  }
  // Create ROMDIR
  romdir_t *romdir = create_romdir_section( entry, nb_entries );

  // Create EXTINFO
  char *bytes;
  if ( ( bytes = malloc( romdir[2].size ) ) == NULL )
    fatal_with_errno( "Cannot allocate %d bytes of memory", romdir[2].size );
  char *extinfo = create_extinfo_section( bytes, entry, nb_entries );

  // Create IMG file
  int off = 0;
  FILE *f;
  if ( ( f = fopen( image_name, "w" ) ) == NULL )
    fatal_with_errno( "Could not create ROM image %s\n", image_name );

  // Write ROMDIR
  fwrite( romdir, romdir[1].size, 1, f );
  off += romdir[1].size;
  if ( verbose ) {
    verbose_dump_entry_info( &entry[0] );
    verbose_dump_entry_info( &entry[1] );
  }
  // Write EXTINFO
  fwrite( extinfo, romdir[2].size, 1, f );
  off += romdir[2].size;
  if ( verbose )
    verbose_dump_entry_info( &entry[2] );

  // Write files
  for ( i = 3; i < nb_entries; i++ ) {
    // Pad with zeroes if necessary
    if ( off & 0xF ) {
      int toWrite = 0x10 - ( off & 0xF );
      fwrite( zeros_buffer, toWrite, 1, f );
      off += toWrite;
    }
    fwrite( entry[i].irx_binary, 1, entry[i].irx_size, f );
    off += entry[i].irx_size;

    if ( verbose )
      verbose_dump_entry_info( &entry[i] );
  }

  fclose( f );
}


/*---------------------------------------------------------------------*/
/*    inner_add_entries ...                                            */
/*    -------------------------------------------------------------    */
/*    Add entries to an image loaded in memory. In-place process.      */
/*---------------------------------------------------------------------*/
void inner_add_entries( char *image_name, entry_t entries[], int num_entries )
{
  // Read the entire file
  char *img;
  int size, i, j;
  read_file( image_name, &img, &size );

  romdir_t *romdir_entry = ( romdir_t * ) img;

  int old_romdir_size = romdir_entry[1].size;
  int old_extinfo_size = romdir_entry[2].size;
  int old_irx_size = size - old_romdir_size - PAD16( old_extinfo_size );

  int new_romdir_size = old_romdir_size;
  int new_extinfo_size = old_extinfo_size;
  int new_irx_size = old_irx_size;

  if ( verbose ) {
    int max_name = 0;
    entry_t *old_entry;
    int nb_old_entries;
    fill_entry_descriptors( image_name, img, size, &old_entry,
                            &nb_old_entries );

    // find out the size of the name column
    for ( j = 0; j < num_entries; j++ ) {
      for ( i = 0; i < nb_old_entries; i++ ) {
        if ( strcmp( old_entry[i].name, entries[j].name ) == 0 ) {
          if ( max_name < strlen( old_entry[i].name ) )
            max_name = strlen( old_entry[i].name );
          break;
        }
      }
    }
    verbose_set_length_of_name_column( max_name );
  }
  // compute the new size of each section
  for ( i = 0; i < num_entries; i++ ) {
    new_romdir_size += sizeof( romdir_t );
    new_extinfo_size =
      PAD4( new_extinfo_size ) + get_entry_extinfo_size( &entries[i] );
    new_irx_size = PAD16( new_irx_size ) + entries[i].irx_size;
  }

  // realloc the size of the new image
  int new_total_size =
    new_romdir_size + PAD16( new_extinfo_size ) + new_irx_size;
  img = realloc( img, new_total_size );
  romdir_entry = ( romdir_t * ) img;

  // update the irx section
  memmove( img + new_romdir_size + PAD16( new_extinfo_size ),
           img + old_romdir_size + PAD16( old_extinfo_size ), old_irx_size );
  int offset = new_romdir_size + PAD16( new_extinfo_size ) + old_irx_size;
  for ( i = 0; i < num_entries; i++ ) {
    memset( img + offset, 0, PAD16( offset ) - offset );
    offset = PAD16( offset );
    memmove( img + offset, entries[i].irx_binary, entries[i].irx_size );
    offset += entries[i].irx_size;
  }

  // update the extinfo section
  memmove( img + new_romdir_size, img + old_romdir_size, old_extinfo_size );
  offset = new_romdir_size + old_extinfo_size;
  for ( i = 0; i < num_entries; i++ ) {
    memset( img + offset, 0, PAD4( offset ) - offset );
    offset = PAD4( offset );
    create_extinfo_section( img + offset, &entries[i], 1 );
    offset += get_entry_extinfo_size( &entries[i] );
  }
  // pad up to the next section
  memset( img + offset, 0, PAD16( offset ) - offset );

  // update the romdir section
  offset = old_romdir_size - sizeof( romdir_t );
  for ( i = 0; i < num_entries; i++ ) {
    romdir_t romdir_entry;
    memset( &romdir_entry, 0, sizeof( romdir_t ) );
    strcpy( romdir_entry.name, entries[i].name );
    romdir_entry.extinfo_size = get_entry_extinfo_size( &entries[i] );
    romdir_entry.size = entries[i].irx_size;
    memcpy( img + offset, &romdir_entry, sizeof( romdir_t ) );
    offset += sizeof( romdir_t );
    if ( verbose )
      verbose_print_add_message( entries[i].name, entries[i].irx_size );
  }
  // write the last romdir entry (dummy)
  memset( img + offset, 0, sizeof( romdir_t ) );

  // Update the overall size of the modified sections
  romdir_entry[1].size = new_romdir_size;
  romdir_entry[2].size = new_extinfo_size;

  // done ! save the result to disk
  write_file( image_name, img, new_total_size );
}


/*---------------------------------------------------------------------*/
/*    add_entries_to_image                                             */
/*    -------------------------------------------------------------    */
/*    Add some IRXs to an existing image. In-place process.            */
/*---------------------------------------------------------------------*/
void add_entries_to_image( char *image_name, char *irx_args[], int num_irx )
{
  entry_t entries[num_irx];
  int i;
  for ( i = 0; i < num_irx; i++ ) {
    init_entry_from_irx( &entries[i], irx_args[i] );
  }
  inner_add_entries( image_name, entries, num_irx );
}
