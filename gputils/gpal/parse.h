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
# define	IF	266
# define	IN	267
# define	INOUT	268
# define	IS	269
# define	LOOP	270
# define	MODULE	271
# define	NULL_TOK	272
# define	OF	273
# define	OTHERS	274
# define	PRAGMA	275
# define	PROCEDURE	276
# define	PUBLIC	277
# define	RETURN	278
# define	THEN	279
# define	TO	280
# define	TYPE	281
# define	OUT	282
# define	WHEN	283
# define	WHILE	284
# define	WITH	285
# define	ASM	286
# define	IDENT	287
# define	NUMBER	288
# define	STRING	289
# define	LSH	290
# define	RSH	291
# define	ARROW	292
# define	GREATER_EQUAL	293
# define	LESS_EQUAL	294
# define	EQUAL	295
# define	NOT_EQUAL	296
# define	LOGICAL_AND	297
# define	LOGICAL_OR	298


extern YYSTYPE yylval;

#endif /* not BISON_PARSE_H */
