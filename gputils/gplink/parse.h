#ifndef BISON_PARSE_H
# define BISON_PARSE_H

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
# define	LEXEOF	261
# define	NUMBER	262


extern YYSTYPE yylval;

#endif /* not BISON_PARSE_H */
