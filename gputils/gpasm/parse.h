#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
  gpasmVal i;
  char *s;
  struct pnode *p;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	LABEL	257
# define	IDENTIFIER	258
# define	CBLOCK	259
# define	ENDC	260
# define	FILL	261
# define	NUMBER	262
# define	STRING	263
# define	HIGH	264
# define	LOW	265
# define	LSH	266
# define	RSH	267
# define	GREATER_EQUAL	268
# define	LESS_EQUAL	269
# define	EQUAL	270
# define	NOT_EQUAL	271
# define	LOGICAL_AND	272
# define	LOGICAL_OR	273
# define	ASSIGN_PLUS	274
# define	ASSIGN_MINUS	275
# define	ASSIGN_MULTIPLY	276
# define	ASSIGN_DIVIDE	277
# define	ASSIGN_MODULUS	278
# define	ASSIGN_LSH	279
# define	ASSIGN_RSH	280
# define	ASSIGN_AND	281
# define	ASSIGN_OR	282
# define	ASSIGN_XOR	283
# define	INCREMENT	284
# define	DECREMENT	285
# define	TBL_NO_CHANGE	286
# define	TBL_POST_INC	287
# define	TBL_POST_DEC	288
# define	TBL_PRE_INC	289
# define	CONCAT	290
# define	VAR	291
# define	VARLAB_BEGIN	292
# define	VAR_BEGIN	293
# define	VAR_END	294


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
