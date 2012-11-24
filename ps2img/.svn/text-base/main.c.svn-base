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
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include"common.h"

char *version_string = "0.1";

#define OP_EXTRACT 1
#define OP_CREATE  2
#define OP_LIST    3
#define OP_DELETE  4
#define OP_ADD     5

extern void create_image( char *image_name, char *irx_args[], int num_irx );
extern void extract_image( char *image_name, char *irx_args[], int num_irx );
extern void delete_entries_from_image( char *image_name, char *irx_args[],
                                       int num_irx );
extern void add_entries_to_image( char *image_name, char *irx_args[],
                                  int num_irx );
extern void list_image_entries( char *image_name );

char *program_name;
int verbose;

static struct option long_options[] = {
  {"help", no_argument, NULL, 'H'},
  {"version", no_argument, NULL, 'V'},
  {"extract", no_argument, NULL, 'x'},
  {"create", no_argument, NULL, 'c'},
  {"delete", no_argument, NULL, 'd'},
  {"append", no_argument, NULL, 'a'},
  {"list", no_argument, NULL, 't'},
  {"verbose", no_argument, NULL, 'v'},
  {"file", required_argument, NULL, 'f'},
  {0, no_argument, 0, 0}
};

void error_invalid_operation_mode(  )
{
  fatal( "You may not specify more than one `-adxct' option\n"
         "Try `%s --help' for more information.\n", program_name );
}

void error_no_operation_mode(  )
{
  fatal( "You must specify one `-adxct' option\n"
         "Try `%s --help' for more information.\n", program_name );
}

void error_no_image_given(  )
{
  fatal( "You must specify an archive file with `-f' \n"
         "Try `%s --help' for more information.\n", program_name );
}

void error_create_empty_archive(  )
{
  fatal( "Refusing to create an empty archive\n"
         "Try `%s --help' for more information.\n", program_name );
}

void dump_version_and_exit(  )
{
  printf( "%s %s\n", basename( program_name ), version_string );
  printf( "Create, inspect or extract Playstation 2 ROM image files\n" );
  printf( "Copyright (C) 2005 brAun / yo6\n" );
  exit( 0 );
}

void dump_help_and_exit(  )
{
  printf
    ( "ps2img creates, inspects or extracts Playstation 2 ROM image files\n"
      "\n" "Usage: ps2img [OPTION]... -f IMAGE [IRX]...\n" "\n" "Examples:\n"
      "  ps2img -cf rom.img bar gee # Create image rom.img from IRXs bar and gee.\n"
      "  ps2img -tf rom.img         # List all IRXs in image rom.img.\n"
      "  ps2img -xvf rom.img        # Extract all IRXs in image rom.img verbosely.\n"
      "\n"
      "If a long option shows an argument as mandatory, then it is mandatory\n"
      "for the equivalent short option also.  Similarly for optional arguments.\n"
      "\n" "Main operation mode:\n"
      "  -t, --list                  List the contents of an ROM image\n"
      "  -x, --extract               Extract IRXs from an ROM image\n"
      "  -c, --create                Create a new ROM image\n"
      "  -a, --append                Append IRXs to the end of a ROM image\n"
      "  -d, --delete                Delete IRXs from the ROM image\n"
      "  -f, --file=FILE             Use FILE as the ROM image\n" "\n"
      "Informative output:\n"
      "  -H, --help                  Print this help, then exit\n"
      "  -V, --version               Print ps2img program version number\n"
      "  -v, --verbose               Verbosely list files processed\n" "\n"
      "Report bugs to <damien.ciabrini@bar.org>.\n" );
  exit( 0 );
}

int main( int argc, char *argv[] )
{
  char *img_file = NULL;
  int opt_index;
  char c;
  int operation_mode = 0;

  program_name = argv[0];

  while ( ( c =
            getopt_long( argc, argv, "adxctvf:", long_options,
                         NULL ) ) != -1 ) {
    switch ( c ) {
    case 'a':
      if ( operation_mode )
        error_invalid_operation_mode(  );
      operation_mode = OP_ADD;
      break;
    case 'd':
      if ( operation_mode )
        error_invalid_operation_mode(  );
      operation_mode = OP_DELETE;
      break;
    case 'x':
      if ( operation_mode )
        error_invalid_operation_mode(  );
      operation_mode = OP_EXTRACT;
      break;
    case 'c':
      if ( operation_mode )
        error_invalid_operation_mode(  );
      operation_mode = OP_CREATE;
      break;
    case 't':
      if ( operation_mode )
        error_invalid_operation_mode(  );
      operation_mode = OP_LIST;
      break;
    case 'f':
      img_file = optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'V':
      dump_version_and_exit(  );
      break;
    case 'H':
      dump_help_and_exit(  );
      break;
    default:
      break;
      //printf("%-3d (%c)\n",c,c);
    }
  }

  if ( !img_file )
    error_no_image_given(  );

  switch ( operation_mode ) {
  case OP_EXTRACT:
    extract_image( img_file, &argv[optind], argc - optind );
    break;
  case OP_CREATE:
    if ( optind == argc )
      error_create_empty_archive(  );
    else
      create_image( img_file, &argv[optind], argc - optind );
    break;
  case OP_DELETE:
    delete_entries_from_image( img_file, &argv[optind], argc - optind );
    break;
  case OP_ADD:
    add_entries_to_image( img_file, &argv[optind], argc - optind );
    break;
  case OP_LIST:
    list_image_entries( img_file );
    break;
  default:
    error_no_operation_mode(  );
  }

  return 0;
}
