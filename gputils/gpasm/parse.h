#ifndef BISON_PARSE_H
# define BISON_PARSE_H

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
# define	INCLUDE	264
# define	UPPER	265
# define	HIGH	266
# define	LOW	267
# define	LSH	268
# define	RSH	269
# define	GREATER_EQUAL	270
# define	LESS_EQUAL	271
# define	EQUAL	272
# define	NOT_EQUAL	273
# define	LOGICAL_AND	274
# define	LOGICAL_OR	275
# define	ASSIGN_PLUS	276
# define	ASSIGN_MINUS	277
# define	ASSIGN_MULTIPLY	278
# define	ASSIGN_DIVIDE	279
# define	ASSIGN_MODULUS	280
# define	ASSIGN_LSH	281
# define	ASSIGN_RSH	282
# define	ASSIGN_AND	283
# define	ASSIGN_OR	284
# define	ASSIGN_XOR	285
# define	INCREMENT	286
# define	DECREMENT	287
# define	TBL_NO_CHANGE	288
# define	TBL_POST_INC	289
# define	TBL_POST_DEC	290
# define	TBL_PRE_INC	291
# define	CONCAT	292
# define	VAR	293
# define	DEBUG_FILE	294
# define	DEBUG_LINE	295
# define	VARLAB_BEGIN	296
# define	VAR_BEGIN	297
# define	VAR_END	298


extern YYSTYPE yylval;

#endif /* not BISON_PARSE_H */
