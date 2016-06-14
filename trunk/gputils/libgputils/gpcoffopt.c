/* GNU PIC coff optimizing functions
   Copyright (C) 2005
   Craig Franklin

   Copyright (C) 2015-2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

typedef struct coff_opt_states {
  gp_reloc_type *relocation;
  unsigned int   state;                 /* For COPT_... constants. */
  gp_boolean     protected;

  uint32_t       target_page;
  uint32_t       reloc_page;

  uint32_t       reloc_byte_addr;
  uint32_t       reloc_insn_addr;

  uint32_t       reloc_byte_length;
  uint32_t       reloc_insn_length;
} coff_opt_states_t;

#define COPT_STATE_SIZE                     4

#define COPT_NULL                           0
#define COPT_BRA14E_CURR_PAGE               (1 << 0)
#define COPT_BRA14E_OTHER_PAGE              (1 << 1)
#define COPT_GOTO_CURR_PAGE                 (1 << 2)
#define COPT_GOTO_OTHER_PAGE                (1 << 3)
#define COPT_CALL_CURR_PAGE                 (1 << 4)
#define COPT_CALL_OTHER_PAGE                (1 << 5)
#define COPT_PAGESEL_CURR_PAGE              (1 << 6)
#define COPT_PAGESEL_OTHER_PAGE             (1 << 7)

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

/*------------------------------------------------------------------------------------------------*/

/* Remove any weak symbols in the object. */

void
gp_coffopt_remove_weak(gp_object_type *object)
{
  gp_symbol_type *symbol;

  gp_debug("Removing weak symbols from \"%s\".", object->filename);

  /* Search the symbol table for extern symbols. */
  symbol = object->symbols;
  while (symbol != NULL) {
    if ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF) &&
        (!gp_coffgen_has_reloc(object, symbol))) {
      gp_debug("  removed weak symbol \"%s\"", symbol->name);
      gp_coffgen_del_symbol(object, symbol);
    }

    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Remove any relocatable section that doesn't have a symbol pointed to by a relocation. */

void
gp_coffopt_remove_dead_sections(gp_object_type *object, int pass, gp_boolean enable_cinit_warns)
{
  gp_section_type *section;
  gp_reloc_type   *relocation;
  gp_section_type *rel_sect;
  gp_boolean       section_removed;

  do {
    section_removed = false;
    gp_debug("Removing dead sections pass %i.", pass);

    section = object->sections;
    while (section != NULL) {
      /* Mark all sections as unused. */
      section->is_used = false;
      section = section->next;
    }

    section = object->sections;
    while (section != NULL) {
      /* Mark all sections that relocations point to as used. */
/*    gp_debug("  section_name: %s", section->name);*/
      relocation = section->relocations;
      while (relocation != NULL) {
        if ((rel_sect = relocation->symbol->section) != NULL) {
          if (rel_sect != section) {
/*          gp_debug("    reloc_section_name: %s", rel_sect->name);*/
            rel_sect->is_used = true;
          }
        }
        else {
          if (enable_cinit_warns || (strcmp(relocation->symbol->name, "_cinit") != 0)) {
            gp_warning("Relocation symbol %s has no section.", relocation->symbol->name);
          }
        }
        relocation = relocation->next;
      }
      section = section->next;
    }

    section = object->sections;
    while (section != NULL) {
      /* FIXME: Maybe don't remove if it is in protected memory. */
      if ((!section->is_used) && ((section->flags & (STYP_ABS | STYP_DATA)) == 0)) {
        gp_debug("Removing section \"%s\".", section->name);
        gp_coffgen_del_section_symbols(object, section);
        gp_coffgen_del_section(object, section);
        section_removed = true;
      }
      section = section->next;
    }

    /* take another pass */
    ++pass;
  } while (section_removed);
}

/*------------------------------------------------------------------------------------------------*/

static void
_state_pipe_clear(coff_opt_states_t State_pipe[COPT_STATE_SIZE], size_t Size)
{
  assert(Size > 1);

  memset(State_pipe, 0, sizeof(coff_opt_states_t) * Size);
}

/*------------------------------------------------------------------------------------------------*/

static void
_state_pipe_shift(coff_opt_states_t State_pipe[COPT_STATE_SIZE], size_t Size, gp_boolean Up)
{
  size_t i;

  assert(Size > 1);

  /* Step down to the last index. */
  --Size;

  if (Up) {
    /* Move upward.

       State_pipe[last - 1] --> State_pipe[last]  -- The oldest relocation.
         .
         .
         .
       State_pipe[first]    --> State_pipe[first+1]
       State_pipe[first]    <-- 0                 -- The current relocation. */
    for (i = Size; i > 0; --i) {
      memcpy(&State_pipe[i], &State_pipe[i - 1], sizeof(coff_opt_states_t));
    }

    memset(&State_pipe[0], 0, sizeof(coff_opt_states_t));
  }
  else {
    /* Move backward.

       State_pipe[first + 1] --> State_pipe[first] -- The current relocation.
         .
         .
         .
       State_pipe[last]      --> State_pipe[last - 1]
       State_pipe[last]      <-- 0                 -- The oldest relocation. */
    for (i = 0; i < Size; ++i) {
      memcpy(&State_pipe[i], &State_pipe[i + 1], sizeof(coff_opt_states_t));
    }

    memset(&State_pipe[Size], 0, sizeof(coff_opt_states_t));
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_state_pipe_delete_state(coff_opt_states_t State_pipe[COPT_STATE_SIZE], size_t Size, size_t State_num)
{
  assert(Size > 1);
  assert(State_num < Size);

  --Size;
  while (State_num < Size) {
    memcpy(&State_pipe[State_num], &State_pipe[State_num + 1], sizeof(coff_opt_states_t));
    ++State_num;
  }

  memset(&State_pipe[Size], 0, sizeof(coff_opt_states_t));
}

/*------------------------------------------------------------------------------------------------*/

static uint32_t
_page_addr_from_insn_addr(proc_class_t Class, uint32_t Insn_addr)
{
  return gp_processor_page_addr(Class, Insn_addr);
}

/*------------------------------------------------------------------------------------------------*/

static uint32_t
_page_addr_from_byte_addr(proc_class_t Class, uint32_t Byte_addr)
{
  return gp_processor_page_addr(Class, gp_processor_byte_to_org(Class, Byte_addr));
}

/*------------------------------------------------------------------------------------------------*/

static int
_reloc_addr_cmp(const void *P0, const void *P1)
{
  const gp_reloc_type *r0 = *(const gp_reloc_type **)P0;
  const gp_reloc_type *r1 = *(const gp_reloc_type **)P1;

  if (r0->address < r1->address) {
    return -1;
  }
  else if (r0->address > r1->address) {
    return 1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static gp_reloc_type **
_reloc_make_array(gp_section_type *Section, unsigned int *Num_relocations)
{
  gp_reloc_type  *current;
  gp_reloc_type **reloc_array;
  unsigned int    i;
  unsigned int    n_relocations;

  if ((n_relocations = Section->num_reloc) == 0) {
    return NULL;
  }

  reloc_array = (gp_reloc_type **)GP_Calloc(n_relocations, sizeof(gp_reloc_type *));

  i       = 0;
  current = Section->relocations;
  while (current != NULL) {
    reloc_array[i] = current;
    ++i;
    current        = current->next;
  }

  assert(n_relocations == i);

  qsort(reloc_array, n_relocations, sizeof(gp_reloc_type *), _reloc_addr_cmp);
  *Num_relocations = n_relocations;
  return reloc_array;
}

/*------------------------------------------------------------------------------------------------*/

static void
_reloc_decrease_addresses(proc_class_t Class, gp_reloc_type *Relocation, uint32_t Relocation_page,
                          uint32_t Insn_offset, uint32_t Byte_offset)
{
  gp_reloc_type  *reloc;
  gp_symbol_type *symbol;

  reloc = Relocation;
  while (reloc != NULL) {
    if (reloc->address >= Byte_offset) {
      reloc->address -= Byte_offset;
      symbol = reloc->symbol;

      /* Prevents the modification of symbols on the other pages. */
      if ((_page_addr_from_insn_addr(Class, symbol->value) == Relocation_page) &&
          /* Prevents the multiple modifications of symbol. */
          (!symbol->opt_flags)) {
        symbol->value     -= Insn_offset;
        symbol->opt_flags  = true;
      }
    }

    reloc = reloc->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_section_decrease_addresses(proc_class_t Class, gp_section_type *First_section, uint32_t Relocation_page,
                            uint32_t Start_address, uint32_t Insn_offset, uint32_t Byte_offset)
{
  gp_section_type *section;
  gp_symbol_type  *symbol;

  section = First_section;
  while (section != NULL) {
    /* Prevents the modification of sections on other pages. */
    if ((_page_addr_from_byte_addr(Class, section->address) == Relocation_page) &&
        (section->address >= Start_address)) {
        symbol = section->symbol;
      b_memory_move(section->data, section->address, section->address - Byte_offset, section->size);
      section->address       -= Byte_offset;
      /* The first symbol of list the name of section. */
      /* Prevents the multiple modifications of symbol. */
      if (!symbol->opt_flags) {
        symbol->value     -= Insn_offset;
        symbol->opt_flags  = true;
      }
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_linenum_decrease_addresses(proc_class_t Class, gp_section_type *First_section,
                            uint32_t Relocation_page, uint32_t Start_address, uint32_t Byte_offset)
{
  gp_section_type *section;
  gp_linenum_type *linenum;

  section = First_section;
  while (section != NULL) {
    linenum = section->line_numbers;
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

static void
_label_clear_opt_flags(gp_symbol_type **Label_array, unsigned int Num_labels)
{
  unsigned int i;

  for (i = 0; i < Num_labels; ++i) {
    Label_array[i]->opt_flags = false;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_label_decrease_addresses(proc_class_t Class, gp_symbol_type **Label_array, unsigned int Num_labels,
                          uint32_t Relocation_page, uint32_t Start_address, uint32_t Insn_offset)
{
  unsigned int    i;
  gp_symbol_type *label;

  for (i = 0; i < Num_labels; ++i) {
    label = Label_array[i];

    /* Prevents the modification of labels (symbols) on the other pages. */
    if ((_page_addr_from_insn_addr(Class, label->value) == Relocation_page) &&
        (label->value >= Start_address)) {
      /* Prevents the multiple modifications of symbol. */
      if (!label->opt_flags) {
        label->value     -= Insn_offset;
        label->opt_flags  = true;
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_reloc_analyze(proc_class_t Class, gp_section_type *Section, gp_reloc_type *Relocation,
               unsigned int Num_pages, coff_opt_states_t Opt_state[COPT_STATE_SIZE])
{
  const gp_symbol_type *symbol;
  uint32_t              reloc_byte_addr;
  uint32_t              reloc_insn_addr;
  uint32_t              value;
  uint32_t              reloc_page;
  uint32_t              target_page;
  uint32_t              reloc_byte_length;

  symbol          = Relocation->symbol;
  reloc_byte_addr = Section->address   + Relocation->address;
  value           = Relocation->offset + (uint32_t)symbol->value;

  reloc_insn_addr = gp_processor_byte_to_org(Class, reloc_byte_addr);
  reloc_page      = gp_processor_page_addr(Class, reloc_insn_addr);
  target_page     = gp_processor_page_addr(Class, value);

  /* No relocation. */
  if ((Opt_state[1].relocation == NULL) ||
      /* A relocation which is not interesting. */
      (Opt_state[1].state == COPT_NULL) ||
      /* A relocation which is too far away. Meanwhile there is at least one other instruction. */
      ((Opt_state[1].reloc_insn_addr + Opt_state[1].reloc_insn_length) != reloc_insn_addr)) {
    /* Clears the contents of status pipe. */
    _state_pipe_clear(Opt_state, COPT_STATE_SIZE);
  }

  Opt_state[0].relocation      = Relocation;
  Opt_state[0].state           = COPT_NULL;
  Opt_state[0].protected       = false;
  Opt_state[0].target_page     = target_page;
  Opt_state[0].reloc_page      = reloc_page;
  Opt_state[0].reloc_byte_addr = reloc_byte_addr;
  Opt_state[0].reloc_insn_addr = reloc_insn_addr;

  reloc_byte_length = 0;

  switch (Relocation->type) {
  case RELOCT_ALL:
    break;

  case RELOCT_CALL:
    Opt_state[0].state = (reloc_page == target_page) ? COPT_CALL_CURR_PAGE : COPT_CALL_OTHER_PAGE;
    reloc_byte_length  = 2;
    break;

  case RELOCT_GOTO:
    Opt_state[0].state = (reloc_page == target_page) ? COPT_GOTO_CURR_PAGE : COPT_GOTO_OTHER_PAGE;
    reloc_byte_length  = 2;
    break;

  case RELOCT_LOW:
  case RELOCT_HIGH:
  case RELOCT_UPPER:
  case RELOCT_P:
  case RELOCT_BANKSEL:
  case RELOCT_IBANKSEL:
  case RELOCT_F:
  case RELOCT_TRIS:
  case RELOCT_TRIS_3BIT:
  case RELOCT_MOVLR:
  case RELOCT_MOVLB:
  case RELOCT_GOTO2:
  case RELOCT_FF1:
  case RELOCT_FF2:
  case RELOCT_LFSR1:
  case RELOCT_LFSR2:
    break;

  case RELOCT_BRA:
    Opt_state[0].state = (reloc_page == target_page) ? COPT_BRA14E_CURR_PAGE : COPT_BRA14E_OTHER_PAGE;
    reloc_byte_length  = 2;
    break;

  case RELOCT_CONDBRA:
  case RELOCT_ACCESS:
    break;

  case RELOCT_PAGESEL_WREG:
    Opt_state[0].state = (reloc_page == target_page) ? COPT_PAGESEL_CURR_PAGE : COPT_PAGESEL_OTHER_PAGE;
    reloc_byte_length  = Class->pagesel_byte_length(Num_pages, true);
    break;

  case RELOCT_PAGESEL_BITS:
  case RELOCT_PAGESEL_MOVLP:
    Opt_state[0].state = (reloc_page == target_page) ? COPT_PAGESEL_CURR_PAGE : COPT_PAGESEL_OTHER_PAGE;
    reloc_byte_length  = Class->pagesel_byte_length(Num_pages, false);
    break;

  /* unimplemented relocations */
  case RELOCT_PAGESEL:
  case RELOCT_SCNSZ_LOW:
  case RELOCT_SCNSZ_HIGH:
  case RELOCT_SCNSZ_UPPER:
  case RELOCT_SCNEND_LOW:
  case RELOCT_SCNEND_HIGH:
  case RELOCT_SCNEND_UPPER:
  case RELOCT_SCNEND_LFSR1:
  case RELOCT_SCNEND_LFSR2:
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

  Opt_state[0].reloc_byte_length = reloc_byte_length;
  Opt_state[0].reloc_insn_length = gp_processor_byte_to_org(Class, reloc_byte_length);
}

/*------------------------------------------------------------------------------------------------*/

static void
_instruction_destroy(gp_section_type *Section, uint32_t Byte_address, uint32_t Byte_length)
{
  b_memory_delete_area(Section->data, Byte_address, Byte_length);
  Section->size -= Byte_length;
}

/*------------------------------------------------------------------------------------------------*/

static void
_destroy_pagesel_and_update(proc_class_t Class, gp_section_type *First_section, gp_section_type *Section,
                            coff_opt_states_t Opt_state[COPT_STATE_SIZE],
                            gp_symbol_type **Label_array, unsigned int Num_labels, unsigned int Insn_index)
{
  unsigned int i;
  uint32_t     start_page;
  uint32_t     byte_addr_curr;
  uint32_t     byte_length_curr;
  uint32_t     insn_addr_curr;
  uint32_t     insn_length_curr;
  uint32_t     byte_addr_next;
  uint32_t     insn_addr_next;

  byte_addr_curr   = Opt_state[Insn_index].reloc_byte_addr;
  byte_length_curr = Opt_state[Insn_index].reloc_byte_length;
  insn_addr_curr   = Opt_state[Insn_index].reloc_insn_addr;
  insn_length_curr = Opt_state[Insn_index].reloc_insn_length;
  byte_addr_next   = byte_addr_curr + byte_length_curr;
  insn_addr_next   = insn_addr_curr + insn_length_curr;
  start_page       = Opt_state[Insn_index].reloc_page;

  _instruction_destroy(Section, byte_addr_curr, byte_length_curr);
  gp_coffgen_del_linenum_by_address_area(Section, byte_addr_curr, byte_addr_next - 1);
  _linenum_decrease_addresses(Class, First_section, start_page, byte_addr_next, byte_length_curr);

  _section_decrease_addresses(Class, First_section, start_page, byte_addr_next, insn_length_curr,
                              byte_length_curr);

  _label_clear_opt_flags(Label_array, Num_labels);

  _reloc_decrease_addresses(Class, Opt_state[Insn_index].relocation->next, start_page, insn_length_curr,
                            byte_length_curr);

  _label_decrease_addresses(Class, Label_array, Num_labels, start_page, insn_addr_next, insn_length_curr);
  gp_coffgen_del_reloc(Section, Opt_state[Insn_index].relocation);

  /* Decrease the address of instruction in newer (younger) states. */
  for (i = 0; i < Insn_index; ++i) {
    Opt_state[i].reloc_byte_addr -= byte_length_curr;
    Opt_state[i].reloc_insn_addr -= insn_length_curr;
  }

  _state_pipe_delete_state(Opt_state, COPT_STATE_SIZE, Insn_index);
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_pagesel_remove(proc_class_t Class, gp_section_type *First_section, gp_section_type *Section,
                coff_opt_states_t Opt_state[COPT_STATE_SIZE],
                gp_symbol_type **Label_array, unsigned int Num_labels, gp_boolean Completion)
{
  unsigned int saturation;

  saturation  = (Opt_state[0].relocation != NULL);
  saturation += (Opt_state[1].relocation != NULL);
  saturation += (Opt_state[2].relocation != NULL);
  saturation += (Opt_state[3].relocation != NULL);

  if (saturation == 0) {
    /* The State Pipe is empty. */
    return false;
  }

  if (Completion && (Opt_state[0].state == COPT_PAGESEL_CURR_PAGE)) {
    /* This is the last relocation on chain. */
    /*
        [0] pagesel current_page  <-- UNNECESSARY
    */
    _destroy_pagesel_and_update(Class, First_section, Section, Opt_state, Label_array, Num_labels, 0);
    _state_pipe_shift(Opt_state, COPT_STATE_SIZE, false);
    return true;
  }

  if (saturation == 4) {
    /* The State Pipe is full. */
    if ((Opt_state[3].state == COPT_CALL_OTHER_PAGE) &&
        (Opt_state[2].state == COPT_PAGESEL_CURR_PAGE) &&
        (Opt_state[1].state == COPT_PAGESEL_CURR_PAGE) &&
        (Opt_state[0].state == COPT_CALL_CURR_PAGE)) {
      /*
         [3] call    function_on_other_page 
         [2] pagesel current_page  <------- UNNECESSARY
         [1] pagesel current_page
         [0] call    function_on_current_page 
      */
      _destroy_pagesel_and_update(Class, First_section, Section, Opt_state, Label_array, Num_labels, 2);
    }
    else if ((Opt_state[3].state == COPT_CALL_OTHER_PAGE) &&
             (Opt_state[2].state == COPT_PAGESEL_CURR_PAGE) &&
             (Opt_state[1].state == COPT_PAGESEL_OTHER_PAGE) &&
             (Opt_state[0].state == COPT_CALL_OTHER_PAGE)) {
      /*
         [3] call    function_on_other_page 
         [2] pagesel current_page  <------- UNNECESSARY
         [1] pagesel other_page
         [0] call    function_on_other_page 
      */
      _destroy_pagesel_and_update(Class, First_section, Section, Opt_state, Label_array, Num_labels, 2);
    }
  } /* if (saturation == 4) */

  if (saturation >= 3) {
    /* The saturation of State Pipe at least 3/4. */
    if ((Opt_state[2].state == COPT_CALL_OTHER_PAGE) &&
        (Opt_state[1].state == COPT_PAGESEL_CURR_PAGE) &&
        (Opt_state[0].state == COPT_PAGESEL_CURR_PAGE)) {
      /*
         [2] call    function_on_other_page 
         [1] pagesel current_page  <------- clear PROTECTED
         [0] pagesel current_page  <------- set PROTECTED
      */
      Opt_state[1].protected = false;
      Opt_state[0].protected = true;
    }
  } /* if (saturation >= 3) */

  if (saturation >= 2) {
    /* The saturation of State Pipe at least 1/2. */
    if ((Opt_state[1].state == COPT_CALL_OTHER_PAGE) &&
        (Opt_state[0].state == COPT_PAGESEL_CURR_PAGE)) {
      /*
         [1] call    function_on_other_page 
         [0] pagesel current_page  <------- set PROTECTED
      */
      Opt_state[0].protected = true;
    }
    else if ((Opt_state[1].state == COPT_PAGESEL_CURR_PAGE) && (!Opt_state[1].protected) &&
             (Opt_state[0].state & COPT_ABS_BRANCH_CURR_PAGE_MASK)) {
      /*
         [1] pagesel current_page  <------ UNNECESSARY if not PROTECTED
         [0] goto    label_on_current_page 

         OR

         [1] pagesel current_page  <--------- UNNECESSARY if not PROTECTED
         [0] call    function_on_current_page
      */
      _destroy_pagesel_and_update(Class, First_section, Section, Opt_state, Label_array, Num_labels, 1);
    }
    else if ((Opt_state[1].state & COPT_PAGESEL_MASK) &&
             (Opt_state[0].state & COPT_REL_BRANCH_MASK)) {
      /*
         [1] pagesel current_or_other_page  <----- UNNECESSARY
         [0] bra     label_on_current_or_other_page
      */
      gp_warning("Strange relocation = %s (%u) with = %s (%u) in section \"%s\" at symbol \"%s\".",
                 gp_coffgen_reloc_type_to_str(Opt_state[1].relocation->type), Opt_state[1].relocation->type,
                 gp_coffgen_reloc_type_to_str(Opt_state[0].relocation->type), Opt_state[0].relocation->type,
                 Section->name, Opt_state[0].relocation->symbol->name);
      _destroy_pagesel_and_update(Class, First_section, Section, Opt_state, Label_array, Num_labels, 1);
    }
  } /* if (saturation >= 2) */

  return true;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffopt_remove_unnecessary_pagesel(gp_object_type *Object)
{
  proc_class_t        class;
  gp_section_type    *first_section;
  gp_symbol_type    **label_array;
  unsigned int        num_labels;
  gp_reloc_type     **relocation_array;
  unsigned int        num_relocations;
  gp_section_type    *section;
  unsigned int        num_pages;
  coff_opt_states_t   opt_state[COPT_STATE_SIZE];
  unsigned int        i;

  class = Object->class;

  if ((class != PROC_CLASS_PIC12)  && (class != PROC_CLASS_PIC12E) &&
      (class != PROC_CLASS_PIC12I) && (class != PROC_CLASS_SX)     &&
      (class != PROC_CLASS_PIC14)  && (class != PROC_CLASS_PIC14E) &&
      (class != PROC_CLASS_PIC14EX)) {
    return;
  }

  gp_debug("Removing unnecessary pagesel instructions.");
  _state_pipe_clear(opt_state, COPT_STATE_SIZE);
  num_pages     = gp_processor_num_pages(Object->processor);
  first_section = Object->sections;

  section = first_section;
  while (section != NULL) {
    if (gp_has_data(section)) {
      num_labels  = 0;
      label_array = gp_symbol_make_label_array(section->symbol, NULL, &num_labels);

      if (label_array != NULL) {
        /* Remove the unnecessary pagesel instructions. */
        relocation_array = _reloc_make_array(section, &num_relocations);
        if (relocation_array != NULL) {
          i = 0;
          do {
            _reloc_analyze(class, section, relocation_array[i], num_pages, opt_state);
            ++i;

            _pagesel_remove(class, first_section, section, opt_state, label_array, num_labels,
                            (i >= num_relocations));

            _state_pipe_shift(opt_state, COPT_STATE_SIZE, true);
          } while (i < num_relocations);

          free(relocation_array);
        }

        free(label_array);
      }
    }

    section = section->next;
  } /* while (section != NULL) */
}
