/* GNU PIC coff optimizing functions
   Copyright (C) 2005
   Craig Franklin

   Copyright (C) 2015-2016 Molnar Karoly

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"
#include "libgputils.h"

#define COPT_NULL                           0
#define COPT_BRA14E_CURR_PAGE               (1 << 0)
#define COPT_BRA14E_OTHER_PAGE              (1 << 1)
#define COPT_GOTO_CURR_PAGE                 (1 << 2)
#define COPT_GOTO_OTHER_PAGE                (1 << 3)
#define COPT_CALL_CURR_PAGE                 (1 << 4)
#define COPT_CALL_OTHER_PAGE                (1 << 5)
#define COPT_PAGESEL_CURR_PAGE              (1 << 6)
#define COPT_PAGESEL_OTHER_PAGE             (1 << 7)

#define COPT_BANKSEL                        (1 << 8)

/* Only PIC14E and PIC14EX. */
#define COPT_BRA14E_MASK                    (COPT_BRA14E_CURR_PAGE | COPT_BRA14E_OTHER_PAGE)

#define COPT_GOTO_MASK                      (COPT_GOTO_CURR_PAGE | COPT_GOTO_OTHER_PAGE)

#define COPT_CALL_MASK                      (COPT_CALL_CURR_PAGE | COPT_CALL_OTHER_PAGE)

/* Only PIC14E and PIC14EX. */
#define COPT_REL_BRANCH_CURR_PAGE_MASK      COPT_BRA14E_CURR_PAGE
#define COPT_REL_BRANCH_OTHER_PAGE_MASK     COPT_BRA14E_OTHER_PAGE
#define COPT_REL_BRANCH_MASK                COPT_BRA14E_MASK

#define COPT_ABS_BRANCH_CURR_PAGE_MASK      (COPT_GOTO_CURR_PAGE | COPT_CALL_CURR_PAGE)
#define COPT_ABS_BRANCH_OTHER_PAGE_MASK     (COPT_GOTO_OTHER_PAGE | COPT_CALL_OTHER_PAGE)
#define COPT_ABS_BRANCH_MASK                (COPT_ABS_BRANCH_CURR_PAGE_MASK | COPT_ABS_BRANCH_OTHER_PAGE_MASK)

#define COPT_BRANCH_CURR_PAGE_MASK          (COPT_REL_BRANCH_CURR_PAGE_MASK | COPT_ABS_BRANCH_CURR_PAGE_MASK)
#define COPT_BRANCH_OTHER_PAGE_MASK         (COPT_REL_BRANCH_OTHER_PAGE_MASK | COPT_ABS_BRANCH_OTHER_PAGE_MASK)
#define COPT_BRANCH_MASK                    (COPT_BRANCH_CURR_PAGE_MASK | COPT_BRANCH_OTHER_PAGE_MASK)

#define COPT_PAGESEL_MASK                   (COPT_PAGESEL_CURR_PAGE | COPT_PAGESEL_OTHER_PAGE)

/* Number of reloc_properties_t type in an array. */
#define RELOC_PIPE_LENGTH                   4

typedef struct reloc_properties {
  gp_reloc_t        *relocation;
  const gp_symbol_t *label;             /* If exists so label which is linked here to. */
  const insn_t      *instruction;       /* The actual instruction. */
  unsigned int       state;             /* For COPT_... constants. */
  gp_boolean         protected;

  uint32_t           target_page;
  uint32_t           reloc_page;

  uint32_t           reloc_byte_addr;
  uint32_t           reloc_insn_addr;

  uint32_t           reloc_byte_length;
  uint32_t           reloc_insn_length;

  uint32_t           ram_bank;
} reloc_properties_t;

static reloc_properties_t   reloc_pipe[RELOC_PIPE_LENGTH];

static gp_section_t       **section_array;
static unsigned int         num_sections;

static gp_symbol_t        **register_array;
static unsigned int         num_registers;

static gp_boolean           first_banksel = false;

/*------------------------------------------------------------------------------------------------*/

/* Remove any weak symbols in the object. */

void
gp_coffopt_remove_weak(gp_object_t *Object)
{
  gp_symbol_t *symbol;

  gp_debug("Removing weak symbols from \"%s\".", Object->filename);

  /* Search the symbol table for extern symbols. */
  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if (gp_coffgen_is_external_symbol(symbol) && (!gp_coffgen_symbol_has_reloc(symbol, COFF_SYM_RELOC_ALL))) {
      gp_debug("  removed weak symbol \"%s\"", symbol->name);
      /* It is not allowed to deleted because the gplink/cod.c will need this. */
      gp_coffgen_move_reserve_symbol(Object, symbol);
    }

    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Remove any relocatable section that doesn't have a symbol pointed to by a relocation. */

void
gp_coffopt_remove_dead_sections(gp_object_t *Object, int Pass, gp_boolean Enable_cinit_wanings)
{
  gp_section_t *section;
  gp_section_t *section_next;
  gp_boolean    section_removed;

  do {
    section_removed = false;
    gp_debug("Removing dead sections pass %i.", Pass);

    gp_coffgen_check_relocations(Object, Enable_cinit_wanings);
    section = Object->section_list.first;
    while (section != NULL) {
      section_next = section->next;

      if (FlagIsClr(section->opt_flags, OPT_FLAGS_PROTECTED_SECTION)) {
        gp_debug("Removing section \"%s\".", section->name);
        /* It is not allowed to deleted because the gplink/cod.c will need these. */
        gp_coffgen_move_reserve_section_symbols(Object, section);
        /* It is not allowed to deleted because the gplink/cod.c will need this. */
        gp_coffgen_move_reserve_section(Object, section);
        section_removed = true;
      }
      section = section_next;
    }

    /* take another pass */
    ++Pass;
  } while (section_removed);
}


/*------------------------------------------------------------------------------------------------*/

/* Deletes all states from relocation pipe. */

static void
_reloc_pipe_clear(void)
{
  memset(reloc_pipe, 0, sizeof(reloc_pipe));
}

/*------------------------------------------------------------------------------------------------*/

/* Moves the contents of relocation tube forward or backward. */

static void
_reloc_pipe_shift(gp_boolean Forward)
{
  size_t i;

  if (Forward) {
    /* Moves forward.

       reloc_pipe[last - 1] --> reloc_pipe[last]  -- The oldest relocation.
         .
         .
         .
       reloc_pipe[first]    --> reloc_pipe[first+1]
       reloc_pipe[first]    <-- 0                 -- The current relocation. */
    for (i = RELOC_PIPE_LENGTH - 1; i > 0; --i) {
      memcpy(&reloc_pipe[i], &reloc_pipe[i - 1], sizeof(reloc_properties_t));
    }

    memset(&reloc_pipe[0], 0, sizeof(reloc_properties_t));
  }
  else {
    /* Moves backward.

       reloc_pipe[first + 1] --> reloc_pipe[first] -- The current relocation.
         .
         .
         .
       reloc_pipe[last]      --> reloc_pipe[last - 1]
       reloc_pipe[last]      <-- 0                 -- The oldest relocation. */
    for (i = 0; i < (RELOC_PIPE_LENGTH - 1); ++i) {
      memcpy(&reloc_pipe[i], &reloc_pipe[i + 1], sizeof(reloc_properties_t));
    }

    memset(&reloc_pipe[RELOC_PIPE_LENGTH - 1], 0, sizeof(reloc_properties_t));
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Deletes one state from the relocation pipe. */

static void
_reloc_pipe_delete_state(size_t State_index)
{
  assert(State_index < RELOC_PIPE_LENGTH);

  while (State_index < (RELOC_PIPE_LENGTH - 1)) {
    memcpy(&reloc_pipe[State_index], &reloc_pipe[State_index + 1], sizeof(reloc_properties_t));
    ++State_index;
  }

  memset(&reloc_pipe[RELOC_PIPE_LENGTH - 1], 0, sizeof(reloc_properties_t));
}

/*------------------------------------------------------------------------------------------------*/

/* Make page address from an instruction address. */

static uint32_t
_page_addr_from_insn_addr(proc_class_t Class, uint32_t Insn_addr)
{
  return gp_processor_page_addr(Class, Insn_addr);
}

/*------------------------------------------------------------------------------------------------*/

/* Make page address from an byte address. */

static uint32_t
_page_addr_from_byte_addr(proc_class_t Class, uint32_t Byte_addr)
{
  return gp_processor_page_addr(Class, gp_processor_insn_from_byte_c(Class, Byte_addr));
}

/*------------------------------------------------------------------------------------------------*/

/* Decrease relocation addresses in a given list. */

static void
_reloc_decrease_addresses(proc_class_t Class, gp_reloc_t *Relocation, uint32_t Relocation_page,
                          uint32_t Insn_offset, uint32_t Byte_offset)
{
  gp_reloc_t         *reloc;
  gp_symbol_t        *symbol;
  const gp_section_t *section;

  if (Relocation == NULL) {
    return;
  }

  reloc = Relocation;
  while (reloc != NULL) {
    if (reloc->address >= Byte_offset) {
      reloc->address -= Byte_offset;
      symbol  = reloc->symbol;
      section = symbol->section;

      /* Prevents the modification of symbols on the other pages. */
      if (FlagIsSet(section->flags, STYP_ROM_AREA) &&
          (_page_addr_from_insn_addr(Class, symbol->value) == Relocation_page)) {
          /* Prevents the multiple modifications of symbol. */
        if (FlagIsClr(symbol->opt_flags, OPT_FLAGS_GPCOFFOPT_MODULE)) {
          symbol->value -= Insn_offset;
          FlagSet(symbol->opt_flags, OPT_FLAGS_GPCOFFOPT_MODULE);
        }
      }
    }

    reloc = reloc->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_label_arrays_make(proc_class_t Class)
{
  gp_section_t *section;
  unsigned int  i;

  if ((section_array == NULL) || (num_sections == 0)) {
    return;
  }

  for (i = 0; i < num_sections; ++i) {
    section = section_array[i];
    section->num_labels  = 0;
    section->label_array = gp_symbol_make_label_array(section, Class->org_to_byte_shift,
                                                      &(section->num_labels));
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_label_arrays_free(void)
{
  gp_section_t *section;
  unsigned int  i;

  if ((section_array == NULL) || (num_sections == 0)) {
    return;
  }

  for (i = 0; i < num_sections; ++i) {
    section = section_array[i];

    if (section->label_array != NULL) {
      free(section->label_array);
      section->label_array = NULL;
    }

    section->num_labels  = 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Sets or clears section optimize flag in a given list. */

static void
_label_clear_opt_flag(void)
{
  gp_section_t *section;
  gp_symbol_t  *label;
  unsigned int  i;
  unsigned int  j;

  if ((section_array == NULL) || (num_sections == 0)) {
    return;
  }

  for (i = 0; i < num_sections; ++i) {
    section = section_array[i];
    for (j = 0; j < section->num_labels; ++j) {
      label = section->label_array[j];
      /* This will be modifiable. */
      FlagClr(label->opt_flags, OPT_FLAGS_GPCOFFOPT_MODULE);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Decrease label addresses in a given list. */

static void
_label_array_decrease_addresses(proc_class_t Class, gp_section_t *Section, uint32_t Start_address,
                                uint32_t Insn_offset)
{
  unsigned int  i;
  gp_symbol_t  *label;

  for (i = 0; i < Section->num_labels; ++i) {
    label = Section->label_array[i];

    /* Prevents the multiple modifications of symbol. */
    if (label->value >= Start_address) {
      if (FlagIsClr(label->opt_flags, OPT_FLAGS_GPCOFFOPT_MODULE)) {
        label->value -= Insn_offset;
        FlagSet(label->opt_flags, OPT_FLAGS_GPCOFFOPT_MODULE);
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Decrease section addresses in a given list. */

static void
_sections_decrease_start_address(proc_class_t Class, const gp_section_t *Section, uint32_t Insn_offset,
                                 uint32_t Byte_offset)
{
  gp_section_t  *section;
  gp_symbol_t   *symbol;
  unsigned int   i;
  uint32_t       byte_address;
  uint32_t       insn_address;
  gp_symvalue_t  value_prev;

  if ((section_array == NULL) || (num_sections < 1)) {
    return;
  }

  for (i = 0; i < num_sections; ++i) {
    section = section_array[i];
    /* Prevents the modification of sections on other pages. */
    if (section->address > Section->address) {
      byte_address = section->address - Byte_offset;
      insn_address = gp_processor_insn_from_byte_c(Class, byte_address);
      gp_mem_b_move(section->data, section->address, byte_address, section->size);
      section->address = byte_address;

      symbol = section->symbol;
      if (symbol != NULL) {
        value_prev     = symbol->value;
        symbol->value -= Insn_offset;
        assert((gp_symvalue_t)insn_address == symbol->value);
      }

      _label_array_decrease_addresses(Class, section, value_prev, Insn_offset);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Decrease line number addresses in a given list. */

static void
_linenum_decrease_addresses(proc_class_t Class, gp_section_t *First_section,
                            uint32_t Relocation_page, uint32_t Start_address, uint32_t Byte_offset)
{
  gp_section_t *section;
  gp_linenum_t *linenum;

  section = First_section;
  while (section != NULL) {
    linenum = section->line_number_list.first;
    while (linenum != NULL) {
      /* Prevents the modification of linenumbers on other pages. */
      if ((_page_addr_from_byte_addr(Class, linenum->address) == Relocation_page) &&
          (linenum->address >= Start_address)) {
        linenum->address -= Byte_offset;
      }
      linenum = linenum->next;
    }
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Destroys an instruction from data memory of given section. */

static void
_destroy_insn(proc_class_t Class, gp_section_t *Section, uint32_t Byte_address, uint32_t Byte_length,
              const char *Symbol_name)
{
  gp_mem_b_delete_area(Section->data, Byte_address, Byte_length);
  Section->size -= Byte_length;
}

/*------------------------------------------------------------------------------------------------*/

/* Destroys a Pagesel directive and updates all related addresses. */

static void
_destroy_insn_and_update_addr(proc_class_t Class, gp_section_t *First_section, gp_section_t *Section,
                              unsigned int Insn_index)
{
  unsigned int  i;
  uint32_t      start_page;
  uint32_t      byte_addr_curr;
  uint32_t      byte_length_curr;
  uint32_t      insn_addr_curr;
  uint32_t      insn_length_curr;
  uint32_t      byte_addr_next;
  const char   *sym_name;

  byte_addr_curr   = reloc_pipe[Insn_index].reloc_byte_addr;
  byte_length_curr = reloc_pipe[Insn_index].reloc_byte_length;
  insn_addr_curr   = reloc_pipe[Insn_index].reloc_insn_addr;
  insn_length_curr = reloc_pipe[Insn_index].reloc_insn_length;
  byte_addr_next   = byte_addr_curr + byte_length_curr;
  start_page       = reloc_pipe[Insn_index].reloc_page;
  sym_name         = (reloc_pipe[Insn_index].relocation->symbol != NULL) ?
                                        reloc_pipe[Insn_index].relocation->symbol->name : NULL;

  _destroy_insn(Class, Section, byte_addr_curr, byte_length_curr, sym_name);
  gp_symbol_delete_by_value(Section->label_array, &Section->num_labels, insn_addr_curr);

  gp_coffgen_del_linenum_by_address_area(Section, byte_addr_curr, byte_addr_next - 1);
  _linenum_decrease_addresses(Class, First_section, start_page, byte_addr_next, byte_length_curr);

  /* Enable modification of address only in program memory. */
  _label_clear_opt_flag();

  _sections_decrease_start_address(Class, Section, insn_length_curr, byte_length_curr);

  _reloc_decrease_addresses(Class, reloc_pipe[Insn_index].relocation->next, start_page, insn_length_curr,
                            byte_length_curr);

  gp_coffgen_del_reloc(Section, reloc_pipe[Insn_index].relocation);

  /* Decrease the address of instruction in newer (younger) states. */
  for (i = 0; i < Insn_index; ++i) {
    reloc_pipe[i].reloc_byte_addr -= byte_length_curr;
    reloc_pipe[i].reloc_insn_addr -= insn_length_curr;
  }

  _reloc_pipe_delete_state(Insn_index);
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_insn_isReturn(proc_class_t Class, const gp_section_t *Section, unsigned int Byte_addr)
{
  uint16_t      data;
  const insn_t *instruction;

  if (Class->find_insn == NULL) {
    return false;
  }

  if (Class->i_memory_get(Section->data, Byte_addr, &data, NULL, NULL) != W_USED_ALL) {
    return false;
  }

  instruction = Class->find_insn(Class, data);
  if (instruction == NULL) {
    return false;
  }

  switch (instruction->icode) {
    case ICODE_RETFIE:
    case ICODE_RETI:
    case ICODE_RETIW:
    case ICODE_RETLW:
    case ICODE_RETP:
    case ICODE_RETURN:
      return true;

    default:
      return false;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Analyze and add a new relocation state unto the relocation pipe. */

static void
_pagesel_reloc_analyze(proc_class_t Class, gp_section_t *Section, gp_reloc_t *Relocation,
                       unsigned int Num_pages)
{
  const gp_symbol_t *symbol;
  uint16_t           data;
  uint32_t           reloc_byte_addr;
  uint32_t           reloc_insn_addr;
  uint32_t           reloc_byte_length;
  uint32_t           value;
  uint32_t           reloc_page;
  uint32_t           target_page;

  symbol          = Relocation->symbol;
  reloc_byte_addr = Section->address        + Relocation->address;
  value           = (uint32_t)symbol->value + Relocation->offset;

  if (Class->i_memory_get(Section->data, reloc_byte_addr, &data, NULL, NULL) != W_USED_ALL) {
    gp_error("No instruction at 0x%0*X in program memory!", Class->addr_digits, reloc_byte_addr);
    assert(0);
  }

  reloc_insn_addr = gp_processor_insn_from_byte_c(Class, reloc_byte_addr);
  reloc_page      = gp_processor_page_addr(Class, reloc_insn_addr);
  target_page     = gp_processor_page_addr(Class, value);

  /* No relocation. */
  if ((reloc_pipe[1].relocation == NULL) ||
      /* A relocation which is not interesting. */
      (reloc_pipe[1].state == COPT_NULL) ||
      /* A relocation which is too far away. Meanwhile there is at least one other instruction. */
      ((reloc_pipe[1].reloc_insn_addr + reloc_pipe[1].reloc_insn_length) != reloc_insn_addr)) {
    /* Clears the contents of status pipe. */
    _reloc_pipe_clear();
  }

  reloc_pipe[0].relocation      = Relocation;
  reloc_pipe[0].label           = gp_symbol_find_by_value(Section->label_array, Section->num_labels, reloc_insn_addr);
  reloc_pipe[0].instruction     = (Class->find_insn != NULL) ? Class->find_insn(Class, data) : NULL;
  reloc_pipe[0].state           = COPT_NULL;
  reloc_pipe[0].protected       = ((reloc_pipe[0].label != NULL) && (reloc_pipe[0].label->reloc_count_all_section > 1)) ? true : false;
  reloc_pipe[0].target_page     = target_page;
  reloc_pipe[0].reloc_page      = reloc_page;
  reloc_pipe[0].reloc_byte_addr = reloc_byte_addr;
  reloc_pipe[0].reloc_insn_addr = reloc_insn_addr;

  reloc_byte_length = 0;

  switch (Relocation->type) {
    case RELOC_ALL:
      break;

    case RELOC_CALL:
      /* call function */
      reloc_pipe[0].state = (reloc_page == target_page) ? COPT_CALL_CURR_PAGE : COPT_CALL_OTHER_PAGE;
      reloc_byte_length   = 2;
      break;

    case RELOC_GOTO:
      /* goto label */
      reloc_pipe[0].state = (reloc_page == target_page) ? COPT_GOTO_CURR_PAGE : COPT_GOTO_OTHER_PAGE;
      reloc_byte_length   = 2;
      break;

    case RELOC_LOW:
      break;

    case RELOC_HIGH: {
      /* high(value) */
      if ((reloc_pipe[0].instruction != NULL) && (reloc_pipe[0].instruction->icode == ICODE_MOVLP)) {
        /* movlp high(value) */
        reloc_pipe[0].state = (reloc_page == target_page) ? COPT_PAGESEL_CURR_PAGE : COPT_PAGESEL_OTHER_PAGE;
      }
      reloc_byte_length = 2;
      break;
    }

    case RELOC_UPPER:
    case RELOC_P:
    case RELOC_BANKSEL:
    case RELOC_IBANKSEL:
    case RELOC_F:
    case RELOC_TRIS:
    case RELOC_TRIS_3BIT:
    case RELOC_MOVLR:
    case RELOC_MOVLB:
    case RELOC_GOTO2:
    case RELOC_FF1:
    case RELOC_FF2:
    case RELOC_LFSR1:
    case RELOC_LFSR2:
      break;

    case RELOC_BRA:
      /* bra label */
      reloc_pipe[0].state = (reloc_page == target_page) ? COPT_BRA14E_CURR_PAGE : COPT_BRA14E_OTHER_PAGE;
      reloc_byte_length   = 2;
      break;

    case RELOC_CONDBRA:
    case RELOC_ACCESS:
      break;

    case RELOC_PAGESEL_WREG:
      /* PIC12, PIC12E, PIC12I

       movlw value
       movwf STATUS

        OR

       PIC14

       movlw value
       movwf PCLATH */
      reloc_pipe[0].state = (reloc_page == target_page) ? COPT_PAGESEL_CURR_PAGE : COPT_PAGESEL_OTHER_PAGE;
      reloc_byte_length   = Class->pagesel_byte_length(Num_pages, true);
      break;

    case RELOC_PAGESEL_BITS:
      /* PIC12, PIC12E, PIC12I

       bcf STATUS, x
       bsf STATUS, x

        OR

       PIC14

       bcf PCLATH, x
       bsf PCLATH, x */
    case RELOC_PAGESEL_MOVLP:
      /* PIC14E, PIC14EX

       movlp value */
      reloc_pipe[0].state = (reloc_page == target_page) ? COPT_PAGESEL_CURR_PAGE : COPT_PAGESEL_OTHER_PAGE;
      reloc_byte_length   = Class->pagesel_byte_length(Num_pages, false);
      break;

    /* unimplemented relocations */
    case RELOC_PAGESEL:
    case RELOC_SCNSZ_LOW:
    case RELOC_SCNSZ_HIGH:
    case RELOC_SCNSZ_UPPER:
    case RELOC_SCNEND_LOW:
    case RELOC_SCNEND_HIGH:
    case RELOC_SCNEND_UPPER:
    case RELOC_SCNEND_LFSR1:
    case RELOC_SCNEND_LFSR2:
    default: {
        if (symbol->name != NULL) {
          gp_error("Unimplemented relocation = %s (%u) in section \"%s\" at symbol \"%s\".",
                   gp_coffgen_reloc_type_to_str(Relocation->type),
                   Relocation->type, Section->name, symbol->name);
        }
        else {
          gp_error("Unimplemented relocation = %s (%u) in section \"%s\".",
                   gp_coffgen_reloc_type_to_str(Relocation->type),
                   Relocation->type, Section->name);
        }
        assert(0);
      }
  }

  reloc_pipe[0].reloc_byte_length = reloc_byte_length;
  reloc_pipe[0].reloc_insn_length = gp_processor_insn_from_byte_c(Class, reloc_byte_length);
}

/*------------------------------------------------------------------------------------------------*/

/* If possible according to the rules, then removes a Pagesel directive. */

static gp_boolean
_pagesel_remove(proc_class_t Class, gp_section_t *First_section, gp_section_t *Section,
                gp_boolean Completion)
{
  unsigned int saturation;
  unsigned int byte_addr_next;

  saturation  = (reloc_pipe[0].relocation != NULL);
  saturation += (reloc_pipe[1].relocation != NULL);
  saturation += (reloc_pipe[2].relocation != NULL);
  saturation += (reloc_pipe[3].relocation != NULL);

  if (saturation == 0) {
    /* The State Pipe is empty. */
    return false;
  }

  if (Completion) {
    /* This is the last relocation on chain (a code section). */
    if ((reloc_pipe[0].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[0].protected)) {
      byte_addr_next = reloc_pipe[0].reloc_byte_addr + reloc_pipe[0].reloc_byte_length;

      if (_insn_isReturn(Class, Section, byte_addr_next)) {
        /*
          reloc_pipe[0]   pagesel current_page  <-- UNNECESSARY if not PROTECTED
          byte_addr_next: return (or these: retfie, retlw, reti, retp)
        */
        _destroy_insn_and_update_addr(Class, First_section, Section, 0);
        _reloc_pipe_shift(false);
        return true;
      }
    }
  }

  if (saturation >= 2) {
    /* The saturation of State Pipe at least 1/2. */
    if ((reloc_pipe[1].state == COPT_CALL_CURR_PAGE) &&
        (reloc_pipe[0].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[0].protected)) {
      /*
        reloc_pipe[1] call    function_on_current_page 
        reloc_pipe[0] pagesel current_page  <--------- UNNECESSARY if not PROTECTED
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 0);
    }
    else if ((reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[0].state == COPT_PAGESEL_OTHER_PAGE)) {
      /*
        reloc_pipe[1] pagesel current_page  <------- UNNECESSARY if not PROTECTED
        reloc_pipe[0] pagesel other_page  
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
    else if ((reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[0].state & COPT_ABS_BRANCH_CURR_PAGE_MASK)) {
      /*
        reloc_pipe[1] pagesel current_page  <------ UNNECESSARY if not PROTECTED
        reloc_pipe[0] goto    label_on_current_page 

          OR

        reloc_pipe[1] pagesel current_page  <--------- UNNECESSARY if not PROTECTED
        reloc_pipe[0] call    function_on_current_page
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
    else if ((reloc_pipe[1].state & COPT_PAGESEL_MASK) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[0].state & COPT_REL_BRANCH_MASK)) {
      /*
        The 'bra' is a relative jump, no need to Pagesel.

        reloc_pipe[1] pagesel current_or_other_page  <----- UNNECESSARY if not PROTECTED
        reloc_pipe[0] bra     label_on_current_or_other_page
      */
      gp_warning("Strange relocation = %s (%u) with = %s (%u) in section \"%s\" at symbol \"%s\".",
                 gp_coffgen_reloc_type_to_str(reloc_pipe[1].relocation->type), reloc_pipe[1].relocation->type,
                 gp_coffgen_reloc_type_to_str(reloc_pipe[0].relocation->type), reloc_pipe[0].relocation->type,
                 Section->name, reloc_pipe[0].relocation->symbol->name);
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
  } /* if (saturation >= 2) */

  if (saturation >= 3) {
    /* The saturation of State Pipe at least 3/4. */
    if ((reloc_pipe[2].state == COPT_CALL_OTHER_PAGE) &&
        (reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) &&
        (reloc_pipe[0].state == COPT_PAGESEL_CURR_PAGE)) {
      /*
        reloc_pipe[2] call    function_on_other_page 
        reloc_pipe[1] pagesel current_page  <------- clear PROTECTED
        reloc_pipe[0] pagesel current_page  <------- set PROTECTED
      */
      reloc_pipe[1].protected = false;
      reloc_pipe[0].protected = true;
    }
    else if ((reloc_pipe[2].state == COPT_CALL_OTHER_PAGE) &&
             (reloc_pipe[1].state == COPT_PAGESEL_OTHER_PAGE) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[2].target_page == reloc_pipe[1].target_page) &&
             (reloc_pipe[0].state == COPT_CALL_OTHER_PAGE) &&
             (reloc_pipe[1].target_page == reloc_pipe[0].target_page)) {
      /*
        reloc_pipe[2] call    function_on_other_page 
        reloc_pipe[1] pagesel other_page  <--------- UNNECESSARY if not PROTECTED
        reloc_pipe[0] call    function_on_other_page 
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
    else if ((reloc_pipe[2].state == COPT_CALL_CURR_PAGE) &&
             (reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[0].state == COPT_PAGESEL_CURR_PAGE)) {
      /*
        reloc_pipe[2] call    function_on_current_page 
        reloc_pipe[1] pagesel current_page  <--------- UNNECESSARY if not PROTECTED
        reloc_pipe[0] pagesel current_page
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
    else if ((reloc_pipe[2].state == COPT_CALL_CURR_PAGE) &&
             (reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[1].protected) &&
             (reloc_pipe[0].state == COPT_CALL_CURR_PAGE)) {
      /*
        reloc_pipe[2] call    function_on_current_page 
        reloc_pipe[1] pagesel current_page  <--------- UNNECESSARY if not PROTECTED
        reloc_pipe[0] call    function_on_current_page 
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 1);
    }
  } /* if (saturation >= 3) */

  if (saturation == 4) {
    /* The State Pipe is full. */
    if ((reloc_pipe[3].state == COPT_CALL_OTHER_PAGE) &&
        (reloc_pipe[2].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[2].protected) &&
        (reloc_pipe[1].state == COPT_PAGESEL_CURR_PAGE) &&
        (reloc_pipe[0].state == COPT_CALL_CURR_PAGE)) {
      /*
        reloc_pipe[3] call    function_on_other_page 
        reloc_pipe[2] pagesel current_page  <------- UNNECESSARY if not PROTECTED
        reloc_pipe[1] pagesel current_page
        reloc_pipe[0] call    function_on_current_page 
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 2);
    }
    else if ((reloc_pipe[3].state == COPT_CALL_OTHER_PAGE) &&
             (reloc_pipe[2].state == COPT_PAGESEL_CURR_PAGE) && (!reloc_pipe[2].protected) &&
             (reloc_pipe[1].state == COPT_PAGESEL_OTHER_PAGE) &&
             (reloc_pipe[0].state == COPT_CALL_OTHER_PAGE)) {
      /*
        reloc_pipe[3] call    function_on_other_page 
        reloc_pipe[2] pagesel current_page  <------- UNNECESSARY if not PROTECTED
        reloc_pipe[1] pagesel other_page
        reloc_pipe[0] call    function_on_other_page 
      */
      _destroy_insn_and_update_addr(Class, First_section, Section, 2);
    }
  } /* if (saturation == 4) */

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Deletes the unnecessary Pagesel directives from an object. */

void
gp_coffopt_remove_unnecessary_pagesel(gp_object_t *Object)
{
  proc_class_t  class;
  gp_section_t *first_section;
  gp_section_t *section;
  gp_reloc_t   *reloc_curr;
  gp_reloc_t   *reloc_next;
  unsigned int  num_pages;
  unsigned int  i;

  class = Object->class;

  /* Only case of PIC12 and PIC14 families. */
  if ((class != PROC_CLASS_PIC12)  && (class != PROC_CLASS_PIC12E) &&
      (class != PROC_CLASS_PIC12I) && (class != PROC_CLASS_SX)     &&
      (class != PROC_CLASS_PIC14)  && (class != PROC_CLASS_PIC14E) &&
      (class != PROC_CLASS_PIC14EX)) {
    return;
  }

  section_array  = NULL;
  num_sections   = 0;
  register_array = NULL;
  num_registers  = 0;

  gp_debug("Removing unnecessary pagesel instructions.");
  _reloc_pipe_clear();
  num_pages     = gp_processor_num_pages(Object->processor);
  first_section = Object->section_list.first;

  section = first_section;
  while (section != NULL) {
    _reloc_pipe_clear();

    if (gp_coffgen_section_has_data(section)) {
      num_sections  = 0;
      section_array = gp_coffgen_make_section_array(Object, &num_sections,
                              gp_processor_page_addr(class, gp_processor_insn_from_byte_c(class, section->address)),
                              STYP_ROM_AREA);
      _label_arrays_make(class);

      if (section->label_array != NULL) {
        reloc_curr = section->relocation_list.first;
        if (reloc_curr != NULL) {
          i = 0;
          do {
            reloc_next = reloc_curr->next;
            _pagesel_reloc_analyze(class, section, reloc_curr, num_pages);
            reloc_curr = reloc_next;
            _pagesel_remove(class, first_section, section, (reloc_curr == NULL));
            _reloc_pipe_shift(true);
            ++i;
          } while (reloc_curr != NULL);
        }
      }

      _label_arrays_free();

      if (section_array != NULL) {
        free(section_array);
      }
    }

    section = section->next;
  } /* while (section != NULL) */
}

/*------------------------------------------------------------------------------------------------*/

/* Analyze and add a new relocation state unto the relocation pipe. */

static gp_boolean
_banksel_reloc_analyze(proc_class_t Class, pic_processor_t Processor, gp_section_t *Section,
                       gp_reloc_t *Relocation, unsigned int Num_pages)
{
  const gp_symbol_t *symbol;
  uint16_t           data;
  uint32_t           reloc_byte_addr;
  uint32_t           reloc_insn_addr;
  uint32_t           reloc_byte_length;
  uint32_t           reloc_page;
  uint32_t           value;
  uint32_t           ram_bank;
  gp_boolean         need_clear;
  gp_boolean         there_is_banksel;

  symbol          = Relocation->symbol;
  reloc_byte_addr = Section->address + Relocation->address;
  reloc_insn_addr = gp_processor_insn_from_byte_c(Class, reloc_byte_addr);
  value           = (uint32_t)symbol->value + Relocation->offset;

  reloc_page      = gp_processor_page_addr(Class, reloc_insn_addr);

  if (Class->i_memory_get(Section->data, reloc_byte_addr, &data, NULL, NULL) != W_USED_ALL) {
    gp_error("No instruction at 0x%0*X in program memory!", Class->addr_digits, reloc_byte_addr);
    assert(0);
  }

  reloc_byte_length = 0;
  ram_bank          = 0;
  need_clear        = false;
  there_is_banksel  = false;

  switch (Relocation->type) {
    case RELOC_ALL:
      break;

    case RELOC_CALL:
    case RELOC_GOTO:
      need_clear = true;
      break;

    case RELOC_LOW:
    case RELOC_HIGH:
    case RELOC_UPPER:
    case RELOC_P:
      break;

    case RELOC_BANKSEL:
      ram_bank          = gp_processor_bank_addr(Processor, value);
      reloc_byte_length = Class->banksel_byte_length(Num_pages, false);
      there_is_banksel  = true;
      break;

    case RELOC_IBANKSEL:
      break;

    case RELOC_F:
    case RELOC_TRIS:
    case RELOC_TRIS_3BIT:
    case RELOC_MOVLR:
      break;

    case RELOC_MOVLB:
      ram_bank          = gp_processor_bank_addr(Processor, value);
      reloc_byte_length = 2;
      there_is_banksel  = true;
      break;

    case RELOC_GOTO2:
      need_clear = true;
      break;

    case RELOC_FF1:
    case RELOC_FF2:
    case RELOC_LFSR1:
    case RELOC_LFSR2:
      break;

    case RELOC_BRA:
    case RELOC_CONDBRA:
      need_clear = true;
      break;

    case RELOC_ACCESS:
    case RELOC_PAGESEL_WREG:
    case RELOC_PAGESEL_BITS:
    case RELOC_PAGESEL_MOVLP:
      break;

    /* unimplemented relocations */
    case RELOC_PAGESEL:
    case RELOC_SCNSZ_LOW:
    case RELOC_SCNSZ_HIGH:
    case RELOC_SCNSZ_UPPER:
    case RELOC_SCNEND_LOW:
    case RELOC_SCNEND_HIGH:
    case RELOC_SCNEND_UPPER:
    case RELOC_SCNEND_LFSR1:
    case RELOC_SCNEND_LFSR2:
    default: {
        if (symbol->name != NULL) {
          gp_error("Unimplemented relocation = %s (%u) in section \"%s\" at symbol \"%s\".",
                   gp_coffgen_reloc_type_to_str(Relocation->type),
                   Relocation->type, Section->name, symbol->name);
        }
        else {
          gp_error("Unimplemented relocation = %s (%u) in section \"%s\".",
                   gp_coffgen_reloc_type_to_str(Relocation->type),
                   Relocation->type, Section->name);
        }
        assert(0);
      }
  }

  if (need_clear) {
    _reloc_pipe_clear();
    return false;
  }

  if (there_is_banksel) {
    _reloc_pipe_shift(true);

    reloc_pipe[0].relocation        = Relocation;
    reloc_pipe[0].label             = gp_symbol_find_by_value(Section->label_array, Section->num_labels, reloc_insn_addr);
    reloc_pipe[0].instruction       = (Class->find_insn != NULL) ? Class->find_insn(Class, data) : NULL;
    reloc_pipe[0].state             = COPT_BANKSEL;
    reloc_pipe[0].protected         = ((reloc_pipe[0].label != NULL) && (reloc_pipe[0].label->reloc_count_all_section > 1)) ? true : false;
    reloc_pipe[0].reloc_page        = reloc_page;
    reloc_pipe[0].reloc_byte_addr   = reloc_byte_addr;
    reloc_pipe[0].reloc_insn_addr   = reloc_insn_addr;
    reloc_pipe[0].reloc_byte_length = reloc_byte_length;
    reloc_pipe[0].reloc_insn_length = gp_processor_insn_from_byte_c(Class, reloc_byte_length);
    reloc_pipe[0].ram_bank          = ram_bank;

    if (!first_banksel) {
      /* This is the first Banksel directive of section. Absolutely must protect it. */
      reloc_pipe[0].protected = true;
      first_banksel           = true;
    }
    return true;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

/* If possible according to the rules, then removes a Pagesel directive. */

static gp_boolean
_banksel_remove(proc_class_t Class, gp_section_t *First_section, gp_section_t *Section)
{
  unsigned int saturation;

  saturation  = (reloc_pipe[0].relocation != NULL);
  saturation += (reloc_pipe[1].relocation != NULL);
  saturation += (reloc_pipe[2].relocation != NULL);
  saturation += (reloc_pipe[3].relocation != NULL);

  if (saturation == 0) {
    /* The State Pipe is empty. */
    return false;
  }

  if (saturation >= 2) {
    if ((reloc_pipe[1].state == COPT_BANKSEL) &&
        (reloc_pipe[0].state == COPT_BANKSEL) &&
        (reloc_pipe[1].ram_bank == reloc_pipe[0].ram_bank)) {
      if (!reloc_pipe[1].protected) {
        /*
          reloc_pipe[1] banksel Z <--------- UNNECESSARY if not PROTECTED
          reloc_pipe[0] banksel Z
        */
        _destroy_insn_and_update_addr(Class, First_section, Section, 1);
      }
      else if (!reloc_pipe[0].protected) {
        /*
          reloc_pipe[1] banksel Z
          reloc_pipe[0] banksel Z <--------- UNNECESSARY if not PROTECTED
        */
        _destroy_insn_and_update_addr(Class, First_section, Section, 0);
      }
    }
  } /* if (saturation >= 2) */

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Deletes the unnecessary Banksel directives from an object. */

void
gp_coffopt_remove_unnecessary_banksel(gp_object_t *Object)
{
  proc_class_t     class;
  pic_processor_t  processor;
  gp_section_t    *first_section;
  gp_section_t    *section;
  gp_reloc_t      *reloc_curr;
  gp_reloc_t      *reloc_next;
  unsigned int     num_banks;
  unsigned int     i;
  gp_boolean       may_remove;

  class     = Object->class;
  processor = Object->processor;

  if ((class != PROC_CLASS_PIC12)   && (class != PROC_CLASS_PIC12E) &&
      (class != PROC_CLASS_PIC12I)  && (class != PROC_CLASS_SX)     &&
      (class != PROC_CLASS_PIC14)   && (class != PROC_CLASS_PIC14E) &&
      (class != PROC_CLASS_PIC14EX) && (class != PROC_CLASS_PIC16)  &&
      (class != PROC_CLASS_PIC16E)) {
    return;
  }

  section_array  = NULL;
  num_sections   = 0;
  register_array = NULL;
  num_registers  = 0;

  gp_debug("Removing unnecessary banksel instructions.");
  num_registers  = 0;
  register_array = gp_symbol_make_register_array(Object, &num_registers);

  if (register_array == NULL) {
    return;
  }

  _reloc_pipe_clear();
  num_banks     = gp_processor_num_banks(Object->processor);
  first_section = Object->section_list.first;

  section = first_section;
  while (section != NULL) {
    first_banksel = false;
    _reloc_pipe_clear();

    if (gp_coffgen_section_has_data(section)) {
      num_sections  = 0;
      section_array = gp_coffgen_make_section_array(Object, &num_sections,
                              gp_processor_page_addr(class, gp_processor_insn_from_byte_c(class, section->address)),
                              STYP_ROM_AREA);
      _label_arrays_make(class);
      reloc_curr = section->relocation_list.first;
      if (reloc_curr != NULL) {
        i = 0;
        while (reloc_curr != NULL) {
          reloc_next = reloc_curr->next;
          may_remove = _banksel_reloc_analyze(class, processor, section, reloc_curr, num_banks);

          if (may_remove) {
            _banksel_remove(class, first_section, section);
          }

          reloc_curr = reloc_next;
          ++i;
        }

      }

      _label_arrays_free();

      if (section_array != NULL) {
        free(section_array);
      }
    } /* if (gp_coffgen_section_has_data(section)) */

    section = section->next;
  } /* while (section != NULL) */

  free(register_array);
}
