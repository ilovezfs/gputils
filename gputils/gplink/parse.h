typedef union {
  int i;
  char *s;
  struct pnode *p;
} YYSTYPE;
#define	SYMBOL	257
#define	LIBPATH	258
#define	LKRPATH	259
#define	PATH	260
#define	NUMBER	261


extern YYSTYPE yylval;
