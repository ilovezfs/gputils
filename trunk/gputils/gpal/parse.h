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
# define	BEGIN_TOK	259
# define	ELSE	260
# define	ELSIF	261
# define	END	262
# define	FOR	263
# define	FUNCTION	264
# define	IF	265
# define	IN	266
# define	INOUT	267
# define	IS	268
# define	LOOP	269
# define	MODULE	270
# define	NULL_TOK	271
# define	OF	272
# define	OTHERS	273
# define	PRAGMA	274
# define	PROCEDURE	275
# define	PUBLIC	276
# define	RETURN	277
# define	THEN	278
# define	TO	279
# define	TYPE	280
# define	OUT	281
# define	WHEN	282
# define	WHILE	283
# define	WITH	284
# define	ASM	285
# define	IDENT	286
# define	NUMBER	287
# define	STRING	288
# define	LSH	289
# define	RSH	290
# define	ARROW	291
# define	GREATER_EQUAL	292
# define	LESS_EQUAL	293
# define	EQUAL	294
# define	NOT_EQUAL	295
# define	LOGICAL_AND	296
# define	LOGICAL_OR	297


extern YYSTYPE yylval;

#endif /* not BISON_PARSE_H */
