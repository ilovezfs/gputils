#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
  int i;
  char *s;
  struct pnode *p;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	SYMBOL	257
# define	LIBPATH	258
# define	LKRPATH	259
# define	PATH	260
# define	NUMBER	261


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
