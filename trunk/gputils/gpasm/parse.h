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
# define	UPPER	264
# define	HIGH	265
# define	LOW	266
# define	LSH	267
# define	RSH	268
# define	GREATER_EQUAL	269
# define	LESS_EQUAL	270
# define	EQUAL	271
# define	NOT_EQUAL	272
# define	LOGICAL_AND	273
# define	LOGICAL_OR	274
# define	ASSIGN_PLUS	275
# define	ASSIGN_MINUS	276
# define	ASSIGN_MULTIPLY	277
# define	ASSIGN_DIVIDE	278
# define	ASSIGN_MODULUS	279
# define	ASSIGN_LSH	280
# define	ASSIGN_RSH	281
# define	ASSIGN_AND	282
# define	ASSIGN_OR	283
# define	ASSIGN_XOR	284
# define	INCREMENT	285
# define	DECREMENT	286
# define	TBL_NO_CHANGE	287
# define	TBL_POST_INC	288
# define	TBL_POST_DEC	289
# define	TBL_PRE_INC	290
# define	CONCAT	291
# define	VAR	292
# define	VARLAB_BEGIN	293
# define	VAR_BEGIN	294
# define	VAR_END	295


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
