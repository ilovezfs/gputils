%{
/* Parser for gpasm preprocessor #v() evaluation
   Copyright (C) 2012 Borut Razem

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
#include "gpasm.h"
#include "preprocess.h"

int ppresult = 0;

void
pperror(const char *message)
{
  /* do nothing */
}
%}

/* Bison declarations.  */
%union {
  gpasmVal i;
  char *s;
}

%token HV
%token <i> NUMBER
%token <s> IDENTIFIER

%left <i> LOGICAL_OR
%left <i> LOGICAL_AND
%left <i> '&' '|' '^'
%left <i> '<' '>' EQUAL NOT_EQUAL GREATER_EQUAL LESS_EQUAL
%right <i> LSH RSH
%left <i> '+' '-'
%left <i> '*' '/' '%'
%left <i> UPPER HIGH LOW NEG '!' '~' POS

%type <i> exp
%% /* The grammar follows.  */
hv:
  /* emty */
  |
  HV exp
  { ppresult = $2; }
  ;
 
exp:
  NUMBER
  { $$ = $1; }
  |
  IDENTIFIER
  {
    struct symbol *sym;

    if ((sym = get_symbol(state.stTop, $1)) != NULL) {
      struct variable *var = get_symbol_annotation(sym);
      assert(var != NULL);
      $$ = var->value;
    }
    else {
      /* The default value is 0. */
      $$ = 0;
    }
  }
  |

  exp LOGICAL_OR exp
  { $$ = ($1 || $3); }
  |

  exp LOGICAL_AND exp
  { $$ = ($1 && $3); }
  |

  exp '&' exp
  { $$ = $1 & $3; }
  |
  exp '|' exp
  { $$ = $1 | $3; }
  |
  exp '^' exp
  { $$ = $1 ^ $3; }
  |

  exp '<' exp
  { $$ = $1 ^ $3; }
  |
  exp '>' exp
  { $$ = $1 ^ $3; }
  |
  exp EQUAL exp
  { $$ = ($1 == $3); }
  |
  exp NOT_EQUAL exp
  { $$ = ($1 != $3); }
  |
  exp GREATER_EQUAL exp
  { $$ = ($1 >= $3); }
  |
  exp LESS_EQUAL exp
  { $$ = ($1 <= $3); }
  |

  exp LSH exp
  { $$ = $1 << $3; }
  |
  exp RSH exp
  { $$ = $1 >> $3; }
  |

  exp '+' exp
  { $$ = $1 + $3; }
  |
  exp '-' exp
  { $$ = $1 - $3; }
  |

  exp '*' exp
  { $$ = $1 * $3; }
  |
  exp '/' exp
  { $$ = $1 / $3; }
  |
  exp '%' exp
  { $$ = $1 / $3; }
  |

  UPPER exp
  { $$ = ($2 >> 16) & 0xff; }
  |
  HIGH exp
  { $$ = ($2 >> 8) & 0xff; }
  |
  LOW exp
  { $$ = $2 & 0xff; }
  |
  '-' exp %prec NEG
  { $$ = -$2; }
  |
  '!' exp
  { $$ = !$2; }
  |
  '~' exp
  { $$ = ~$2; }
  |
  '+' exp %prec POS
  { $$ = $2; }
  |

  '(' exp ')'
  { $$ = $2; }
  ;
%%
