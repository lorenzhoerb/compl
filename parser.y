
%{
#include <stdio.h>
#include <stdlib.h>
#include "symt.h"
#include "gentree.h"
#include "assembly.h"

int yylex(void);
int yyerror(char* s);
%}

%token ASSIGN ARROW MINUS PLUS MULTIPLY GREATER_THAN HASH  RIGHT_PAREN LEFT_PAREN NEW OR ';' ','
%token OBJECT INT CLASS END RETURN COND CONTINUE BREAK NOT NULLKEY
%token NUM ID
%start start

%{
extern void invoke_burm(NODEPTR_TYPE root);
%}

@attributes { char* id; unsigned lineNr; } ID
@attributes { unsigned n; } NUM
@attributes { struct symbol_table *symtab; struct symbol_table *symtab_in; struct symbol_table *symtab_out; struct selector_list *sl;} class 
@attributes { struct symbol_table *symtab; int returnType;} guarded guarded_list cond 
@attributes { struct symbol_table *symtab;} expr_list
@attributes { struct symbol_table *symtab; char *className;} method
@attributes { int bt;} type
@attributes { int bt; struct s_node *n;} notexpr
@attributes { struct type_list *tl;} type_list
@attributes { struct symbol_table *symtab; int bt; struct s_node *n;} term addexpr expr multexpr orexpr 

@attributes {struct type_list *tl; struct symbol_table *symtab; struct symbol_table *symtab_out;} pars
@attributes { struct symbol_table *symtab; int bt; struct symbol_table *symtab_out;} par

@attributes { struct symbol_table *symtab; struct symbol_table *symtab_out;} selector
@attributes { struct symbol_table *symtab; struct symbol_table *out; struct selector_list *sl;} program
@attributes { struct symbol_table *symtab;} start
@attributes { struct symbol_table *symtab; struct symbol_table *up; char *className;} member_list 

@attributes { struct symbol_table *symtab; struct symbol_table *symtab_out; int returnType; struct s_node *n;} stat 

@attributes { struct symbol_table *symtab; int returnType;} return

@attributes { struct symbol_table *symtab; int returnType; struct s_node *n;} stats
@attributes {struct symbol_table *symtab; int returnType; } m_stat_list  g_stat_list

@traversal @preorder codegen
@traversal @postorder codegenpost

%%
start: program
	@{
		@i @start.symtab@ = @program.symtab@;
		@i @program.out@ = @start.symtab@;
		@i @program.sl@ = get_selectors(@start.symtab@);
	@}
	;

program: 
	  /* empty */
	@{
		@i @program.symtab@ = symtab_init();
	@}
	| program selector ';' 
	@{
		@i @selector.symtab@ = @program.1.symtab@;
		@i @program.0.symtab@ = @selector.symtab_out@;

		@i @program.1.out@ = @program.0.out@;
		@i @program.1.sl@ = @program.0.sl@;
	@}
	| program class ';'
	@{
		@i @class.symtab@ = @program.1.symtab@;
		@i @program.0.symtab@ = @class.symtab_out@;
		@i @class.symtab_in@ = @program.out@;
		@i @program.1.out@ = @program.0.out@;

		@i @program.1.sl@ = @program.0.sl@;
		@i @class.0.sl@ = @program.0.sl@;
	@}
	;

selector: type ID LEFT_PAREN OBJECT type_list RIGHT_PAREN
	@{
		@i @selector.symtab_out@ = symtab_insert(@selector.symtab@, @ID.id@, METHOD, complex_type_init(@type.bt@, types_add_first(@type_list.tl@, OBJECT_T)), @ID.lineNr@);
	@}
	;

class: CLASS ID member_list END
	@{
		@i @class.symtab_out@ = symtab_insert(@class.symtab@, @ID.id@, CLASS_NAME, NULL, @ID.lineNr@);
		@i @member_list.symtab@ = symtab_namespace(@class.symtab_in@);
		@i @member_list.className@ = @ID.id@;
		@codegen defineClassSection(@ID.id@, @class.0.sl@->selectors, @class.0.sl@->size);
	@}
	;

member_list:
	  /* empty */
	@{ 
		@i @member_list.up@ = @member_list.symtab@;
	@}
	| member_list type ID ';'
	@{
		@i @member_list.1.symtab@ = symtab_insert(@member_list.0.symtab@, @ID.id@, OBJ_VAR, complex_type_init(@type.bt@, NULL), @ID.lineNr@);

		@i @member_list.0.up@ = @member_list.1.up@;
		@i @member_list.1.className@ = @member_list.0.className@;
	@}
	| member_list method ';'
	@{
		@i @member_list.1.symtab@ = @member_list.0.symtab@;
		@i @member_list.0.up@ = @member_list.1.up@;
		@i @method.symtab@ = @member_list.1.up@;

		@i @member_list.1.className@ = @member_list.0.className@;
		@i @method.className@ = @member_list.0.className@;
	@}
	;

method: type ID LEFT_PAREN pars RIGHT_PAREN m_stat_list
	@{
		@i @pars.symtab@ = symtab_namespace(@method.symtab@);

		@i @m_stat_list.symtab@ = @pars.symtab_out@;
		@i @m_stat_list.returnType@ = @type.bt@;

		@codegen {
			symtab_check_method_impl(@method.symtab@, @ID.id@, complex_type_init(@type.bt@, @pars.tl@), @ID.lineNr@);
			implementMethod(@method.className@, @ID.id@);
		}
	@}
	;

pars:
	par
	@{
		@i @par.symtab@ = @pars.symtab@;
		@i @pars.symtab_out@ = @par.symtab_out@;

		@i @pars.tl@ = types_add(types_init(), @par.bt@);
	@}
	| pars ',' par
	@{
		@i @pars.1.symtab@ = @pars.0.symtab@;
		@i @par.0.symtab@ = @pars.1.symtab_out@;

		@i @pars.0.symtab_out@ = @pars.1.symtab_out@;

		/*@i @pars.0.tl@ = @pars.1.tl@;
		@codegen types_add(@pars.0.tl@, @par.bt@);*/

		@i @pars.0.tl@ = types_add(@pars.1.tl@, @par.bt@);
	@}
	;

par: type ID
	@{
		@i @par.symtab_out@ = symtab_insert(@par.symtab@, @ID.id@, PARAMETER, complex_type_init(@type.bt@, NULL), @ID.lineNr@);
		@i @par.bt@ = @type.bt@;
	@}
	;

m_stat_list:
	return END
	@{
		@i @return.returnType@ = @m_stat_list.returnType@;
		@i @return.symtab@ = @m_stat_list.symtab@;
	@}
	| stat ';' m_stat_list
	@{
		@i @stat.symtab@ = @m_stat_list.0.symtab@;
		@i @stat.returnType@ = @m_stat_list.returnType@;
		@i @m_stat_list.1.symtab@ = @stat.symtab_out@;
		@i @m_stat_list.1.returnType@ = @m_stat_list.0.returnType@;
		@codegen if(@stat.n@ != NULL) invoke_burm(@stat.n@);
	@}
	;

g_stat_list:
	escape
	| stat ';' g_stat_list
	@{
		@i @stat.returnType@ = @g_stat_list.returnType@; 
		@i @g_stat_list.1.returnType@ = @g_stat_list.0.returnType@;
		@i @stat.symtab@ = @g_stat_list.0.symtab@;
		@i @g_stat_list.1.symtab@ = @stat.symtab_out@;
		
	@}
	;

stat: 
	return
	@{
		@i @return.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;

		@i @return.returnType@ = @stat.returnType@;
		@i @stat.n@ = NULL;
	@}
	| cond
	@{
		@i @cond.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;

		@i @cond.returnType@ = @stat.returnType@;
		@i @stat.n@ = NULL;
	@}
	| type ID ASSIGN expr
	@{
		@i @stat.symtab_out@ = symtab_insert(@stat.symtab@, @ID.id@, VAR, complex_type_init(@type.bt@, NULL), @ID.lineNr@);

		@codegen symtab_check_assign(@stat.symtab@, @ID.id@, @expr.bt@, @ID.lineNr@);
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.n@ = NULL;
	@}
	| ID ASSIGN expr
	@{
		@codegen symtab_check_assign(@stat.symtab@, @ID.id@, @expr.bt@, @ID.lineNr@);
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;
		@i @stat.n@ = NULL;
	@}
	| expr
	@{
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;
		@i @stat.n@ = @expr.n@;
	@}
	;

cond: COND guarded_list END
	@{
		@i @guarded_list.symtab@ = @cond.symtab@;
		
		@i @guarded_list.returnType@ = @cond.returnType@;
	@}
	;

guarded_list: 
	/* empty */
	| guarded_list guarded ';' 
	@{
		@i @guarded.symtab@ = symtab_namespace(@guarded_list.0.symtab@);
		@i @guarded_list.1.symtab@ = @guarded_list.0.symtab@;

		@i @guarded_list.1.returnType@ = @guarded_list.0.returnType@;
		@i @guarded.returnType@ = @guarded_list.0.returnType@;
	@}
	;

guarded:
	expr ARROW g_stat_list
	@{
		@i @g_stat_list.symtab@ = @guarded.symtab@;
		@i @expr.symtab@ = @guarded.symtab@;

		@i @g_stat_list.returnType@ = @guarded.returnType@;

		@codegen {
			if(@expr.bt@ != INT_T) {
				fprintf(stderr, "Type of expression in guarded must be type int\n");
				exit(3);
			}
		};
	@}
	| ARROW g_stat_list
	@{
		@i @g_stat_list.symtab@ = @guarded.symtab@;

		@i @g_stat_list.returnType@ = @guarded.returnType@;
	@}
	;

escape:
	CONTINUE
	| BREAK
	;

return: RETURN expr
	@{
		@i @expr.symtab@ = @return.symtab@;

		@codegen {
			is_return_valid(@return.returnType@, @expr.bt@);
			genReturn();
		}
	@}
	;

expr:
	notexpr term
	@{
		@i @term.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;

		@i @expr.n@ = newOperatorNode(OP_UNARY, @notexpr.n@, @term.n@);

		@codegen {
			if(@term.bt@ != INT_T)
			fprintf(stderr, "Invalid type for not operator\n"); exit(3);
		};
	@}
	| addexpr
	@{
		@i @addexpr.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;
		@i @expr.n@ = @addexpr.n@;
	@}
	| multexpr
	@{
		@i @multexpr.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;
		@i @expr.n@ = @multexpr.n@;
	@}
	| orexpr
	@{
		@i @orexpr.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;
		@i @expr.n@ = @orexpr.n@;
	@}
	| term GREATER_THAN term
	@{
		@i @term.0.symtab@ = @expr.symtab@;
		@i @term.1.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;
		@i @expr.n@ = newOperatorNode(OP_GREATER, @term.0.n@, @term.1.n@);
		@codegen check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| term HASH term
	@{
		@i @term.0.symtab@ = @expr.symtab@;
		@i @term.1.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;

		@i @expr.n@ = newOperatorNode(OP_HASH, @term.0.n@, @term.1.n@);

		@codegen {
			if(@term.0.bt@ != @term.1.bt@) {
				fprintf(stderr, "Error: Invalid types for # operator. Both types must be the same\n");
				exit(3);
			}
		};
	@}
	// check type of hash and return the type
	| NEW ID 
	@{
		@codegen symtab_check_new(@expr.symtab@, @ID.id@, @ID.lineNr@);

		@i @expr.bt@ = OBJECT_T;

		@i @expr.n@ = NULL;
	@}
	| term
	@{
		@i @term.symtab@ = @expr.symtab@;

		@i @expr.bt@ = @term.bt@;

		@i @expr.n@ = @term.n@;
	@}
	;

notexpr:
	MINUS
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = newNumNode(-1);
	@}
	| NOT
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = NULL;
	@}
	| MINUS notexpr
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = newOperatorNode(OP_NEG, @notexpr.1.n@, NULL);
	@}
	| NOT notexpr
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = NULL;
	@}
	;

addexpr: 
	term PLUS term
	@{
		@i @term.0.symtab@ = @addexpr.symtab@;
		@i @term.1.symtab@ = @addexpr.symtab@;

		@i @addexpr.bt@ = INT_T;
		@codegen check_binop_types(@term.0.bt@, @term.1.bt@);
		@i @addexpr.n@ = newOperatorNode(OP_ADD, @term.0.n@, @term.1.n@);
	@}
	| addexpr PLUS term
	@{
		@i @term.symtab@ = @addexpr.symtab@;
		@i @addexpr.1.symtab@ = @addexpr.0.symtab@;

		@i @addexpr.bt@ = INT_T;
		@codegen check_binop_types(@addexpr.1.bt@, @term.bt@);
		@i @addexpr.0.n@ = newOperatorNode(OP_ADD, @addexpr.1.n@, @term.n@);
	@}
	;

multexpr: 
	term MULTIPLY term
	@{
		@i @term.0.symtab@ = @multexpr.symtab@;
		@i @term.1.symtab@ = @multexpr.symtab@;

		@i @multexpr.bt@ = INT_T;

		@i @multexpr.n@ = newOperatorNode(OP_MULT, @term.0.n@, @term.1.n@);

		@codegen check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| multexpr MULTIPLY term
	@{
		@i @term.symtab@ = @multexpr.symtab@;
		@i @multexpr.1.symtab@ = @multexpr.0.symtab@;

		@i @multexpr.bt@ = INT_T;

		@i @multexpr.0.n@ = newOperatorNode(OP_MULT, @multexpr.1.n@, @term.n@);

		@codegen check_binop_types(@multexpr.1.bt@, @term.bt@);
	@}
	;

orexpr: 
	term OR term
	@{
		@i @term.0.symtab@ = @orexpr.symtab@;
		@i @term.1.symtab@ = @orexpr.symtab@;

		@i @orexpr.bt@ = INT_T;
		@i @orexpr.n@ = newOperatorNode(OP_OR, @term.0.n@, @term.1.n@);

		@codegen check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| orexpr OR term
	@{
		@i @term.symtab@ = @orexpr.symtab@;
		@i @orexpr.1.symtab@ = @orexpr.0.symtab@;

		@i @orexpr.bt@ = INT_T;
		@i @orexpr.n@ = newOperatorNode(OP_OR, @orexpr.1.n@, @term.n@);
		@codegen check_binop_types(@orexpr.1.bt@, @term.bt@);
	@}
	;

term:
	LEFT_PAREN expr RIGHT_PAREN
	@{
		@i @expr.symtab@ = @term.symtab@;

		@i @term.bt@ = @expr.bt@;
		@i @term.n@ = @expr.n@;
	@}
	| NUM
	@{
		@i @term.bt@ = INT_T;
		@i @term.n@ = newNumNode(@NUM.n@);
	@}
	| NULLKEY
	@{
		@i @term.bt@ = OBJECT_T;
		@i @term.n@ = NULL;
	@}
	| ID 
	@{
		@i @term.bt@ = symtab_lookup_return_type(@term.symtab@, @ID.id@);
		@i @term.n@ = NULL;
	@}
	// TODO: remove NULL from ID
	| ID LEFT_PAREN expr expr_list RIGHT_PAREN
	@{
		@i @expr.symtab@ = @term.symtab@;
		@i @expr_list.symtab@ = @term.symtab@;
		@i @term.bt@ = symtab_lookup_return_type(@term.symtab@, @ID.id@);
		@i @term.n@ = NULL;
	@}
	;

expr_list:
	/* empty */
	| expr_list ',' expr
	@{
		@i @expr_list.1.symtab@ = @expr_list.0.symtab@;
		@i @expr.0.symtab@ = @expr_list.0.symtab@;
	@}
	;

type_list: 
	/* empty */
	@{
		@i @type_list.tl@ = types_init();
	@}
	| type_list ',' type
	@{
		@i @type_list.0.tl@ = types_add(@type_list.1.tl@, @type.bt@);
	@}
	;

type:
	INT
	@{
		@i @type.bt@ = INT_T;
	@}
	| OBJECT
	@{
		@i @type.bt@ = OBJECT_T;
	@}
	;
%%

int yyerror(char *e)
{
	printf("Parser error: '%s'\n", e);
	exit(2);
}

int main(void)
{
	yyparse();
	return 0;
}
