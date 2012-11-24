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

#include <stdio.h>
#include <string.h>

#include "common.h"



/*---------------------------------------------------------------------*/
/*    write_file ...                                                   */
/*    -------------------------------------------------------------    */
/*    Store a block of memory to a file.                               */
/*---------------------------------------------------------------------*/
void write_file( const char *irx, unsigned char *data, int size )
{
  FILE *f;

  if ( ( f = fopen( irx, "w" ) ) == NULL )
    fatal_with_errno( "Cannot create file %s", irx );

  if ( fwrite( data, size, 1, f ) != 1 )
    fatal_with_errno( "Cannot write to file %s", irx );

  if ( fclose( f ) == -1 )
    fatal_with_errno( "Cannot close file %s", irx );

}


/*---------------------------------------------------------------------*/
/*    verbose_print_extract_message                                    */
/*    -------------------------------------------------------------    */
/*    Print the `extract' message with proper padding                  */
/*---------------------------------------------------------------------*/
void verbose_print_extract_message( const char *irx, int size )
{
  printf( "Extracting " );
  printf( name_format, irx );
  printf( "(%d bytes)\n", size );
}


/*---------------------------------------------------------------------*/
/*    verbose_print_delete_message                                     */
/*    -------------------------------------------------------------    */
/*    Print the `delete' message with proper padding                   */
/*---------------------------------------------------------------------*/
void verbose_print_delete_message( const char *irx, int size )
{
  printf( "Deleting " );
  printf( name_format, irx );
  printf( "(%d bytes)\n", size );
}


/*---------------------------------------------------------------------*/
/*    fill_entry_descriptors                                           */
/*    -------------------------------------------------------------    */
/*    Build an in-memory representation of a ROM image, given a        */
/*    raw contents of a ROM image and its name.                        */
/*---------------------------------------------------------------------*/
void
fill_entry_descriptors( char *image_file, char *img, int img_size,
                        entry_t ** res_entries, int *res_nb_entries )
{
  int elf_offset;
  int max_size = 0;
  int max_name = 0;
  int i;

  // Check file integrity
  if ( ( img_size < ( 16 * 3 ) ) ||
       img[0] != 'R' ||
       img[1] != 'E' || img[2] != 'S' || img[3] != 'E' || img[4] != 'T' )
    fatal( "%s is not a valid Playstation 2 ROM image\n", image_file );

  // Get number of ROMDIR entries
  romdir_t *romdir = ( romdir_t * ) img;
  int offset = 0;
  entry_t *entries;
  int nb_entries = 0;
  while ( romdir[nb_entries].name[0] ) {
    nb_entries++;
    offset += 16;
    if ( offset > img_size )
      fatal( "%s is not a valid Playstation 2 ROM image: "
             "ROMDIR section ended prematuraly\n", image_file );
  }

  // Alloc the resulting entries
  entries = malloc( sizeof( entry_t ) * nb_entries );

  // fill the resulting entries w.r.t the EXTINFO and ROMDIR sections
  unsigned char *extinfo = img + ( ( nb_entries + 1 ) * sizeof( romdir_t ) );
  for ( i = 0; i < nb_entries; i++ ) {
    //print("%p [%p]\n",extinfo,img);
    if ( extinfo >= ( unsigned char * ) ( img + img_size ) )
      fatal( "%s is not a valid Playstation 2 ROM image: "
             "EXTINFO section ended prematuraly\n", image_file );

    // fill entry infos
    strcpy( entries[i].name, romdir[i].name );
    if ( max_name < strlen( entries[i].name ) )
      max_name = strlen( entries[i].name );

    entries[i].flags = 0;
    int size = romdir[i].extinfo_size;
    while ( size ) {

      if ( extinfo >= ( unsigned char * ) ( img + img_size ) )
        fatal( "%s is not a valid Playstation 2 ROM image: "
               "EXTINFO section ended prematuraly\n", image_file );
      extinfo_t *ei = ( extinfo_t * ) extinfo;

      switch ( ei->id ) {
      case EXTINFO_ID_DATE:
        entries[i].date = *( unsigned int * ) ( ei + 1 );
        entries[i].flags |= ENTRY_FLAG_DATE;
        break;
      case EXTINFO_ID_VERSION:
        entries[i].version = ei->value;
        entries[i].flags |= ENTRY_FLAG_VERSION;
        break;
      case EXTINFO_ID_DESCR:
        strcpy( entries[i].descr, ( char * ) ( ei + 1 ) );
        entries[i].flags |= ENTRY_FLAG_DESCR;
        break;
      case EXTINFO_ID_NULL:
        strcpy( entries[i].descr, ( char * ) ( ei + 1 ) );
        entries[i].flags |= ENTRY_FLAG_NULL;
        break;
      default:
        fatal( "%s is not a valid Playstation 2 archive: "
               "invalid EXTINFO id for IRX %s\n",
               image_file, entries[i].name );
      }

      size -= ( sizeof( extinfo_t ) + ei->size );
      extinfo += ( sizeof( extinfo_t ) + ei->size );
    }
  }

  // The IRX files are located right after sizeof(ROMDIR) + sizeof(EXTINFO)
  elf_offset = PAD16( romdir[1].size + romdir[2].size );
  entries[0].irx_size = romdir[0].size;
  entries[1].irx_size = romdir[1].size;
  entries[2].irx_size = romdir[2].size;
  for ( i = 3; i < nb_entries; i++ ) {
    entries[i].irx_binary = ( char * ) ( elf_offset + ( int ) img );
    entries[i].irx_size = romdir[i].size;
    elf_offset += PAD16( romdir[i].size );
    if ( max_size < romdir[i].size )
      max_size = romdir[i].size;
  }
  verbose_set_length_of_size_column( digits_in_number( max_size ) );
  verbose_set_length_of_name_column( max_name );
  *res_entries = entries;
  *res_nb_entries = nb_entries;
}


/*---------------------------------------------------------------------*/
/*    extract_image ...                                                */
/*    -------------------------------------------------------------    */
/*    Extract some IRX files from a ROM image.                         */
/*    If irx_args == NULL, extract all the IRX present in the image    */
/*---------------------------------------------------------------------*/
void extract_image( char *image_name, char *irx_args[], int num_irx )
{
  // Read the entire file
  char *img;
  int size, i, j;
  read_file( image_name, &img, &size );

  // Fill our entry descriptors
  entry_t *entry;
  int nb_entries;
  fill_entry_descriptors( image_name, img, size, &entry, &nb_entries );

  if ( verbose ) {
    int max_name = 0;
    // find out the size of the name column
    for ( j = 0; j < num_irx; j++ ) {
      for ( i = 0; i < nb_entries; i++ ) {
        if ( strcmp( entry[i].name, irx_args[j] ) == 0 ) {
          if ( max_name < strlen( entry[i].name ) )
            max_name = strlen( entry[i].name );
          break;
        }
      }
    }
    verbose_set_length_of_name_column( max_name );
  }

  if ( num_irx ) {
    // Extract only the selected IRX to files
    for ( i = 0; i < num_irx; i++ ) {
      int j;
      for ( j = 3; j < nb_entries; j++ ) {
        if ( strcmp( entry[j].name, irx_args[i] ) == 0 ) {
          if ( verbose )
            verbose_print_extract_message( entry[j].name, entry[j].irx_size );
          write_file( entry[j].name, entry[j].irx_binary, entry[j].irx_size );
          break;
        }
      }
      if ( j == nb_entries )
        fatal( "Entry %s not found in ROM image %s", irx_args[i],
               image_name );
    }
  } else {
    // Extract all IRX to files
    for ( i = 3; i < nb_entries; i++ ) {
      if ( verbose )
        verbose_print_extract_message( entry[i].name, entry[i].irx_size );
      write_file( entry[i].name, entry[i].irx_binary, entry[i].irx_size );
    }
  }
}


/*---------------------------------------------------------------------*/
/*    list_image_entries ...                                           */
/*    -------------------------------------------------------------    */
/*    Dump the contents of a ROM image to the screen.                  */
/*---------------------------------------------------------------------*/
void list_image_entries( char *image_name )
{
  // Read the entire file
  char *img;
  int size, i;
  read_file( image_name, &img, &size );

  // Fill our entry descriptors
  entry_t *entry;
  int nb_entries;
  fill_entry_descriptors( image_name, img, size, &entry, &nb_entries );

  verbose_display_header(  );
  for ( i = 0; i < nb_entries; i++ ) {
    verbose_dump_entry_info( &entry[i] );
  }
}


/*---------------------------------------------------------------------*/
/*    delete_entries_from_image                                        */
/*    -------------------------------------------------------------    */
/*    Delete some IRXs from an existing image. In-place process.       */
/*---------------------------------------------------------------------*/
void
delete_entries_from_image( char *image_name, char *irx_args[], int num_irx )
{
  // Read the entire file
  char *img;
  int size, i, j;
  read_file( image_name, &img, &size );

  // Fill our entry descriptors
  entry_t *entry;
  int nb_entries;
  fill_entry_descriptors( image_name, img, size, &entry, &nb_entries );

  romdir_t *romdir_entry = ( romdir_t * ) img;

  int romdir_original = 0;
  int romdir_updated = 0;
  int extinfo_original = 0;
  int extinfo_updated = 0;
  int irx_original = 0;
  int irx_updated = 0;

  int new_romdir_size = 0;
  int new_extinfo_size = 0;
  int new_irx_size = 0;

  char *romdir_section = img;
  char *extinfo_section = romdir_section + romdir_entry[1].size;
  char *irx_section = extinfo_section + PAD16( romdir_entry[2].size );

  if ( verbose ) {
    int max_name = 0;
    // find out the size of the name column
    for ( j = 0; j < num_irx; j++ ) {
      for ( i = 0; i < nb_entries; i++ ) {
        if ( strcmp( entry[i].name, irx_args[j] ) == 0 ) {
          if ( max_name < strlen( entry[i].name ) )
            max_name = strlen( entry[i].name );
          break;
        }
      }
    }
    verbose_set_length_of_name_column( max_name );
  }
  // mark irx entries to delete
  for ( j = 0; j < num_irx; j++ ) {
    for ( i = 0; i < nb_entries; i++ ) {
      if ( strcmp( entry[i].name, irx_args[j] ) == 0 ) {
        if ( verbose )
          verbose_print_delete_message( entry[i].name, entry[i].irx_size );
        entry[i].name[0] = -1;
        break;
      }
    }
    if ( i == nb_entries )
      fatal( "Entry %s not found in ROM image %s", irx_args[j], image_name );
  }

  // Update all section at once
  for ( i = 0; i < nb_entries; i++ ) {
    // pad the previous section with zeros, if any
    memset( irx_section + irx_updated, 0,
            PAD16( irx_updated ) - irx_updated );
    irx_original = PAD16( irx_original );
    irx_updated = PAD16( irx_updated );
    new_irx_size = PAD16( new_irx_size );
    if ( entry[i].name[0] == -1 ) {
      // this entry must be deleted
      if ( i > 2 )
        irx_original += romdir_entry[i].size;
      extinfo_original += romdir_entry[i].extinfo_size;
      romdir_original += sizeof( romdir_t );
    } else {
      // this entry is kept back
      if ( i > 2 ) {
        memmove( irx_section + irx_updated, irx_section + irx_original,
                 romdir_entry[i].size );
        irx_original += romdir_entry[i].size;
        irx_updated += romdir_entry[i].size;
        new_irx_size += romdir_entry[i].size;
      }
      memmove( extinfo_section + extinfo_updated,
               extinfo_section + extinfo_original,
               romdir_entry[i].extinfo_size );
      extinfo_original += romdir_entry[i].extinfo_size;
      extinfo_updated += romdir_entry[i].extinfo_size;
      new_extinfo_size += romdir_entry[i].extinfo_size;

      memmove( romdir_section + romdir_updated,
               romdir_section + romdir_original, sizeof( romdir_t ) );
      romdir_original += sizeof( romdir_t );
      romdir_updated += sizeof( romdir_t );
      new_romdir_size += sizeof( romdir_t );
    }
  }
  // copy the last romdir entry (dummy)
  memset( romdir_section + romdir_updated, 0, sizeof( romdir_t ) );
  romdir_original += sizeof( romdir_t );
  romdir_updated += sizeof( romdir_t );
  new_romdir_size += sizeof( romdir_t );

  // Update the overall size of the previous sections
  romdir_entry[1].size = new_romdir_size;
  romdir_entry[2].size = new_extinfo_size;

  // pad the extinfo section
  memset( extinfo_section + new_extinfo_size, 0,
          PAD16( new_extinfo_size ) - new_extinfo_size );
  new_extinfo_size = PAD16( new_extinfo_size );


  // decal the EXTINFO and IRX sections to their new position
  memmove( img + new_romdir_size, extinfo_section, new_extinfo_size );
  memmove( img + new_romdir_size + new_extinfo_size, irx_section,
           new_irx_size );

  // done ! save the result to disk
  write_file( image_name, img,
              new_romdir_size + new_extinfo_size + new_irx_size );

}
