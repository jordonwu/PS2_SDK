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

/*---------------------------------------------------------------------*/
/*    The following definitions have been extracted from the           */
/*    corresponding header in the GNU C Library (C) FSF                */
/*---------------------------------------------------------------------*/

#ifndef __ELF_H__
#define __ELF_H__

#define ELF_MAGIC "\177ELF"
#define ELF_PT_LOAD 1

// Check: should work on 64 bits architectures
typedef unsigned char u8;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;

/* The ELF file header.  This appears at the start of every ELF file.  */
typedef struct {
  unsigned char e_ident[16];    /* Magic number and other info */
  Elf32_Half e_type;            /* Object file type */
  Elf32_Half e_machine;         /* Architecture */
  Elf32_Word e_version;         /* Object file version */
  Elf32_Addr e_entry;           /* Entry point virtual address */
  Elf32_Off e_phoff;            /* Program header table file offset */
  Elf32_Off e_shoff;            /* Section header table file offset */
  Elf32_Word e_flags;           /* Processor-specific flags */
  Elf32_Half e_ehsize;          /* ELF header size in bytes */
  Elf32_Half e_phentsize;       /* Program header table entry size */
  Elf32_Half e_phnum;           /* Program header table entry count */
  Elf32_Half e_shentsize;       /* Section header table entry size */
  Elf32_Half e_shnum;           /* Section header table entry count */
  Elf32_Half e_shstrndx;        /* Section header string table index */
} Elf32_Ehdr;

/* Section header.  */
typedef struct {
  Elf32_Word sh_name;           /* Section name (string tbl index) */
  Elf32_Word sh_type;           /* Section type */
  Elf32_Word sh_flags;          /* Section flags */
  Elf32_Addr sh_addr;           /* Section virtual addr at execution */
  Elf32_Off sh_offset;          /* Section file offset */
  Elf32_Word sh_size;           /* Section size in bytes */
  Elf32_Word sh_link;           /* Link to another section */
  Elf32_Word sh_info;           /* Additional section information */
  Elf32_Word sh_addralign;      /* Section alignment */
  Elf32_Word sh_entsize;        /* Entry size if section holds table */
} Elf32_Shdr;

#endif
