#ifndef BISON_PARSE_H
# define BISON_PARSE_H

#ifndef YYSTYPE
typedef union {
  int i;
  struct {
    tree *start;
    tree *end;
  } r;
  char *s;
  tree *t;
  enum node_dir d;
  enum node_op o;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	ARRAY	257
# define	CASE	258
# define	CONSTANT	259
# define	BEGIN_TOK	260
# define	ELSE	261
# define	ELSIF	262
# define	END	263
# define	FOR	264
# define	FUNCTION	265
# define	GOTO	266
# define	IF	267
# define	IN	268
# define	INOUT	269
# define	IS	270
# define	LOOP	271
# define	MODULE	272
# define	NULL_TOK	273
# define	OF	274
# define	OTHERS	275
# define	PRAGMA	276
# define	PROCEDURE	277
# define	PUBLIC	278
# define	RETURN	279
# define	THEN	280
# define	TO	281
# define	TYPE	282
# define	OUT	283
# define	WHEN	284
# define	WHILE	285
# define	WITH	286
# define	ASM	287
# define	IDENT	288
# define	LABEL	289
# define	NUMBER	290
# define	STRING	291
# define	LSH	292
# define	RSH	293
# define	ARROW	294
# define	GREATER_EQUAL	295
# define	LESS_EQUAL	296
# define	EQUAL	297
# define	NOT_EQUAL	298
# define	LOGICAL_AND	299
# define	LOGICAL_OR	300


extern YYSTYPE yylval;

#endif /* not BISON_PARSE_H */
