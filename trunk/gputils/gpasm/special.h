/* Implements special instruction mnemonics
   Copyright (C) 2001 Craig Franklin

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

#ifndef __SPECIAL_H__
#define __SPECIAL_H__

extern gpasmVal do_addcf(gpasmVal r, 
                         char *name, 
                         int arity, 
                         struct pnode *parms);
extern gpasmVal do_adddcf(gpasmVal r, 
                          char *name, 
                          int arity, 
                          struct pnode *parms);
extern gpasmVal do_b(gpasmVal r, 
                     char *name, 
                     int arity, 
                     struct pnode *parms);
extern gpasmVal do_bc(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms);
extern gpasmVal do_bdc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms);                      
extern gpasmVal do_bz(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms);
extern gpasmVal do_bnc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms);
extern gpasmVal do_bnz(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms);
extern gpasmVal do_bndc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_bnz(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms);
extern gpasmVal do_clrc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_clrdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);                        
extern gpasmVal do_clrz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_lcall(gpasmVal r, 
                         char *name, 
                         int arity, 
                         struct pnode *parms);
extern gpasmVal do_lgoto(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_movfw(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_negf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_setc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_setdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_setdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_setz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_skpc(gpasmVal r, 
                        char *name, 
                        int arity, 
                        struct pnode *parms);
extern gpasmVal do_setdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_skpz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);
extern gpasmVal do_skpnc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_skpdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_skpndc(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms);
extern gpasmVal do_skpnz(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_subcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_subdcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms);
extern gpasmVal do_tstf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms);

#endif
