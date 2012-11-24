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
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include "common.h"

char name_format[] = "%-4s ";
char size_format[] = "%4d ";
char header_format[] = "NAME      DATE     VER %4s DESCRIPTION";


/*---------------------------------------------------------------------*/
/*    read_file ...                                                    */
/*    -------------------------------------------------------------    */
/*    Read and store the contents of a file in memory.                 */
/*---------------------------------------------------------------------*/
void read_file( char *irx, char **data, int *size )
{
  FILE *f;
  if ( ( f = fopen( irx, "r" ) ) == NULL )
    fatal_with_errno( "Cannot open file %s", irx );

  if ( fseek( f, 0, SEEK_END ) == -1 )
    fatal_with_errno( "Cannot seek in file %s", irx );

  if ( ( *size = ftell( f ) ) == -1 )
    fatal_with_errno( "Cannot determine size of file %s", irx );

  if ( ( *data = malloc( *size ) ) == NULL )
    fatal_with_errno( "Cannot allocate %d bytes for loading file %s",
                      *size, irx );

  if ( fseek( f, 0, SEEK_SET ) == -1 )
    fatal_with_errno( "Cannot seek in file %s", irx );

  if ( fread( *data, *size, 1, f ) != 1 )
    fatal_with_errno( "Cannot read file %s", irx );

  if ( fclose( f ) == -1 )
    fatal_with_errno( "Cannot close file %s", irx );
}


/*---------------------------------------------------------------------*/
/*    basename                                                         */
/*    -------------------------------------------------------------    */
/*    Extract the name of a file from a path.                          */
/*---------------------------------------------------------------------*/
char *basename( char *path )
{
  char *old = path;
  while ( path = strchr( old, '/' ) )
    old = path + 1;
  return old;
}

int digits_in_number( int num )
{
  int d = 0;
  while ( num /= 10 )
    d++;
  return d + 1;
}

void verbose_set_length_of_name_column( int length )
{
  if ( length < 4 )
    length = 4;
  name_format[2] = '0' + ( length % 10 );
}

void verbose_set_length_of_size_column( int length )
{
  if ( length < 4 )
    length = 4;
  size_format[1] = '0' + ( length % 10 );
  header_format[24] = '0' + ( length % 10 );
}

void verbose_display_header(  )
{
  char buffer[80];
  snprintf( buffer, sizeof( buffer ), header_format, "SIZE" );
  printf( "%s\n", buffer );
  memset( buffer, '-', strlen( buffer ) );
  printf( "%s\n", buffer );
}


void verbose_dump_entry_info( entry_t * e )
{
  printf( "%-9s ", e->name );
  if ( e->flags & ENTRY_FLAG_DATE )
    printf( "%-8X ", e->date );
  else
    printf( "-        " );

  if ( e->flags & ENTRY_FLAG_VERSION )
    printf( "%-3X ", e->version );
  else
    printf( "-   " );

  printf( size_format, e->irx_size );

  if ( e->flags & ENTRY_FLAG_DESCR )
    printf( "%s\n", e->descr );
  else
    printf( "-\n" );
}


int time_t_to_hexa( time_t * time )
{
  char conv[11];
  int date_hexa;
  struct tm m;
  localtime_r( time, &m );
  snprintf( conv, sizeof( conv ), "0x%04d%02d%02d",
            m.tm_year + 1900, m.tm_mon + 1, m.tm_mday );
  sscanf( conv, "%x", &date_hexa );
  return date_hexa;
}

void fatal( char *format, ... )
{
  va_list ap;
  fprintf( stderr, "%s: ", program_name );
  va_start( ap, format );
  vfprintf( stderr, format, ap );
  va_end( ap );
  fprintf( stderr, "\n" );
  exit( 1 );
}

void fatal_with_errno( char *format, ... )
{
  va_list ap;
  fprintf( stderr, "%s: ", program_name );
  va_start( ap, format );
  vfprintf( stderr, format, ap );
  va_end( ap );
  fprintf( stderr, ": %s\n", strerror( errno ) );
  exit( 1 );
}
