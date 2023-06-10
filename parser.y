
%{
#include <stdio.h>
#include <stdlib.h>
#include "symt.h"
#include "gentree.h"
#include "assembly.h"
#include "clist.h"

int yylex(void);
int yyerror(char* s);

%}

%token ASSIGN ARROW MINUS PLUS MULTIPLY GREATER_THAN HASH  RIGHT_PAREN LEFT_PAREN NEW OR SEMICOLON ','
%token OBJECT INT CLASS END RETURN COND CONTINUE BREAK NOT NULLKEY
%token NUM ID
%start start

%{

enum VAR_TYPE resolveType(enum sym_kind kind);

%}

%{
extern void invoke_burm(NODEPTR_TYPE root, unsigned parCount);
%}

@attributes { char* id; unsigned lineNr; } ID
@attributes { unsigned n; } NUM
@attributes { struct symbol_table *symtab; struct symbol_table *symtab_in; struct symbol_table *symtab_out; struct selector_list *sl;} class 
@attributes { struct symbol_table *symtab; int returnType; unsigned varCount;} guarded guarded_list cond 
@attributes { struct symbol_table *symtab;} expr_list
@attributes { struct symbol_table *symtab; char *className; struct clist *usedMethodsIn; struct clist *usedMethodsOut;} method
@attributes { int bt;} type
@attributes { int bt; struct s_node *n;} notexpr
@attributes { struct type_list *tl;} type_list
@attributes { struct symbol_table *symtab; int bt; struct s_node *n;} term addexpr expr multexpr orexpr 

@attributes {struct type_list *tl; struct symbol_table *symtab; struct symbol_table *symtab_out; int regPointer_in; int regPointer_out;} pars
@attributes { struct symbol_table *symtab; int bt; struct symbol_table *symtab_out; int regPointer_in; int regPointer_out;} par

@attributes { struct symbol_table *symtab; struct symbol_table *symtab_out;} selector
@attributes { struct symbol_table *symtab; struct symbol_table *out; struct selector_list *sl;} program
@attributes { struct symbol_table *symtab;} start
@attributes { struct symbol_table *symtab; struct symbol_table *up; char *className; struct clist *usedMethods; unsigned objVarOffset;} member_list 

@attributes { struct symbol_table *symtab; struct symbol_table *symtab_out; int returnType; struct s_node *n; unsigned varCount;} stat 

@attributes { struct symbol_table *symtab; int returnType; struct s_node *n;} return

@attributes { struct symbol_table *symtab; int returnType; struct s_node *n;} stats
@attributes {struct symbol_table *symtab; int returnType; unsigned varCount; } g_stat_list
@attributes {struct symbol_table *symtab; int returnType; unsigned varCount; unsigned parCount;} m_stat_list

@traversal @preorder sematic
@traversal @preorder codegen

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
	| program selector SEMICOLON 
	@{
		@i @selector.symtab@ = @program.1.symtab@;
		@i @program.0.symtab@ = @selector.symtab_out@;

		@i @program.1.out@ = @program.0.out@;
		@i @program.1.sl@ = @program.0.sl@;
	@}
	| program class SEMICOLON
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
		@codegen defineClassSection(@ID.id@, @member_list.up@, @member_list.usedMethods@);
	@}
	;

member_list:
	  /* empty */
	@{ 
		@i @member_list.up@ = @member_list.symtab@;

		@i @member_list.objVarOffset@ = 0;

		@i @member_list.usedMethods@ = clist_init();
	@}
	| member_list type ID SEMICOLON
	@{
		@i @member_list.0.objVarOffset@ = @member_list.1.objVarOffset@ + 1;
		@i @member_list.1.symtab@ = symtab_insert_obj_var(@member_list.0.symtab@, @ID.id@, @type.bt@, @member_list.1.objVarOffset@, @ID.lineNr@);

		@i @member_list.0.up@ = @member_list.1.up@;
		@i @member_list.1.className@ = @member_list.0.className@;

		@i @member_list.0.usedMethods@ = @member_list.1.usedMethods@;
	@}
	| member_list method SEMICOLON
	@{
		@i @member_list.1.symtab@ = @member_list.0.symtab@;
		@i @member_list.0.up@ = @member_list.1.up@;
		@i @method.symtab@ = @member_list.1.up@;

		@i @member_list.0.objVarOffset@ = @member_list.1.objVarOffset@;

		@i @member_list.1.className@ = @member_list.0.className@;
		@i @method.className@ = @member_list.0.className@;

		@i @method.usedMethodsIn@ = @member_list.1.usedMethods@;
		@i @member_list.0.usedMethods@ = @method.usedMethodsOut@;
	@}
	;

method: type ID LEFT_PAREN pars RIGHT_PAREN m_stat_list
	@{
		@i @pars.symtab@ = symtab_namespace(@method.symtab@);
		@i @pars.regPointer_in@ = 0;
		@i @m_stat_list.parCount@ = @pars.regPointer_out@ + 1;

		@i @m_stat_list.symtab@ = @pars.symtab_out@;
		@i @m_stat_list.returnType@ = @type.bt@;

		@i @method.usedMethodsOut@ = clist_add(@method.usedMethodsIn@, @ID.id@);

		@sematic {
			symtab_check_method_impl(@method.symtab@, @ID.id@, complex_type_init(@type.bt@, @pars.tl@), @ID.lineNr@);
		}
		@codegen {
			implementMethod(@method.className@, @ID.id@, @m_stat_list.varCount@);
		}
	@}
	;

pars:
	par
	@{
		@i @par.symtab@ = @pars.symtab@;
		@i @pars.symtab_out@ = @par.symtab_out@;

		@i @par.regPointer_in@ = @pars.regPointer_in@;
		@i @pars.regPointer_out@ = @par.regPointer_in@;

		@i @pars.tl@ = types_add(types_init(), @par.bt@);
	@}
	| pars ',' par
	@{
		@i @pars.1.symtab@ = @pars.0.symtab@;
		@i @par.0.symtab@ = @pars.1.symtab_out@;

		@i @pars.1.regPointer_in@ = @pars.0.regPointer_in@;
		@i @par.regPointer_in@ = @pars.1.regPointer_out@ + 1;
		@i @pars.0.regPointer_out@ = @par.regPointer_out@;


		@i @pars.0.symtab_out@ = @par.0.symtab_out@;

		/*@i @pars.0.tl@ = @pars.1.tl@;
		@codegen types_add(@pars.0.tl@, @par.bt@);*/

		@i @pars.0.tl@ = types_add(@pars.1.tl@, @par.bt@);
	@}
	;

par: type ID
	@{
		@i @par.regPointer_out@ = @par.regPointer_in@;
		@i @par.symtab_out@ = symtab_insert_param(@par.symtab@, @ID.id@, @type.bt@, @par.regPointer_out@, @ID.lineNr@);
		@i @par.bt@ = @type.bt@;
	@}
	;

m_stat_list:
	return END
	@{
		@i @return.returnType@ = @m_stat_list.returnType@;
		@i @return.symtab@ = @m_stat_list.symtab@;
		@i @m_stat_list.varCount@ = 0;
		@codegen if(@return.n@ != NULL) invoke_burm(@return.n@, @m_stat_list.parCount@);
	@}
	| stat SEMICOLON m_stat_list
	@{
		@i @stat.symtab@ = @m_stat_list.0.symtab@;
		@i @stat.returnType@ = @m_stat_list.returnType@;
		@i @m_stat_list.1.symtab@ = @stat.symtab_out@;
		@i @m_stat_list.1.returnType@ = @m_stat_list.0.returnType@;

		@i @m_stat_list.varCount@ = @stat.varCount@ + @m_stat_list.1.varCount@;
		@i @m_stat_list.1.parCount@ = @m_stat_list.0.parCount@;

		@codegen if(@stat.n@ != NULL) invoke_burm(@stat.n@, @m_stat_list.parCount@);
	@}
	;

g_stat_list:
	escape
	@{
		@i @g_stat_list.varCount@ = 0;
	@}
	| stat SEMICOLON g_stat_list
	@{
		@i @stat.returnType@ = @g_stat_list.returnType@; 
		@i @g_stat_list.1.returnType@ = @g_stat_list.0.returnType@;
		@i @stat.symtab@ = @g_stat_list.0.symtab@;
		@i @g_stat_list.1.symtab@ = @stat.symtab_out@;

		@i @g_stat_list.varCount@ = @stat.varCount@ + @g_stat_list.1.varCount@;
	@}
	;

stat: 
	return
	@{
		@i @return.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;

		@i @return.returnType@ = @stat.returnType@;
		@i @stat.n@ = @return.n@;

		@i @stat.varCount@ = 0;
	@}
	| cond
	@{
		@i @cond.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;

		@i @cond.returnType@ = @stat.returnType@;
		@i @stat.n@ = NULL;
		@i @stat.varCount@ = @cond.varCount@;
	@}
	| type ID ASSIGN expr
	@{
		@i @stat.symtab_out@ = symtab_insert_local_var(@stat.symtab@, @ID.id@, @type.bt@, @ID.lineNr@);
		@sematic symtab_check_assign(@stat.symtab@, @ID.id@, @expr.bt@, @ID.lineNr@);
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.n@ = newOperatorNode(OP_ASSIGN, newIdNode(@ID.id@, symtab_lookup_var_offset(@stat.symtab_out@, @ID.id@), LOC), @expr.n@);
		@i @stat.varCount@ = 1;
	@}
	| ID ASSIGN expr
	@{
		@sematic symtab_check_assign(@stat.symtab@, @ID.id@, @expr.bt@, @ID.lineNr@);
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;
		@i @stat.n@ = newOperatorNode(OP_ASSIGN, newIdNode(@ID.id@, symtab_lookup_var_offset(@stat.symtab_out@, @ID.id@), LOC), @expr.n@);

		@i @stat.varCount@ = 0;
	@}
	| expr
	@{
		@i @expr.symtab@ = @stat.symtab@;
		@i @stat.symtab_out@ = @stat.symtab@;
		@i @stat.n@ = @expr.n@;

		@i @stat.varCount@ = 0;
	@}
	;

cond: COND guarded_list END
	@{
		@i @guarded_list.symtab@ = @cond.symtab@;
		
		@i @guarded_list.returnType@ = @cond.returnType@;

		@i @cond.varCount@ = @guarded_list.varCount@;
	@}
	;

guarded_list: 
	/* empty */
	@{
		@i @guarded_list.varCount@ = 0;
	@}
	| guarded_list guarded SEMICOLON 
	@{
		@i @guarded.symtab@ = symtab_namespace(@guarded_list.0.symtab@);
		@i @guarded_list.1.symtab@ = @guarded_list.0.symtab@;

		@i @guarded_list.1.returnType@ = @guarded_list.0.returnType@;
		@i @guarded.returnType@ = @guarded_list.0.returnType@;

		@i @guarded_list.varCount@ = @guarded_list.1.varCount@ + @guarded.varCount@;
	@}
	;

guarded:
	expr ARROW g_stat_list
	@{
		@i @g_stat_list.symtab@ = @guarded.symtab@;
		@i @expr.symtab@ = @guarded.symtab@;

		@i @g_stat_list.returnType@ = @guarded.returnType@;

		@i @guarded.varCount@ = @g_stat_list.varCount@;

		@sematic {
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

		@i @guarded.varCount@ = @g_stat_list.varCount@;
	@}
	;

escape:
	CONTINUE
	| BREAK
	;

return: RETURN expr
	@{
		@i @expr.symtab@ = @return.symtab@;
		@i @return.n@ = newOperatorNode(OP_RETURN, @expr.n@, NULL);

		@sematic {
			is_return_valid(@return.returnType@, @expr.bt@);
			printLocalVars(@return.symtab@);
		}
		@codegen {
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

		@sematic {
			if(@term.bt@ != INT_T) {
				fprintf(stderr, "Invalid type for not operator\n"); exit(3);
			}
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
		@sematic check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| term HASH term
	@{
		@i @term.0.symtab@ = @expr.symtab@;
		@i @term.1.symtab@ = @expr.symtab@;

		@i @expr.bt@ = INT_T;

		@i @expr.n@ = newOperatorNode(OP_HASH, @term.0.n@, @term.1.n@);

		@sematic {
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
		@i @notexpr.n@ = newNumNode(-1);
	@}
	| MINUS notexpr
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = newOperatorNode(OP_NEG, @notexpr.1.n@, NULL);
	@}
	| NOT notexpr
	@{
		@i @notexpr.bt@ = INT_T;
		@i @notexpr.n@ = newOperatorNode(OP_NOT, @notexpr.1.n@, NULL);
	@}
	;

addexpr: 
	term PLUS term
	@{
		@i @term.0.symtab@ = @addexpr.symtab@;
		@i @term.1.symtab@ = @addexpr.symtab@;

		@i @addexpr.bt@ = INT_T;
		@sematic check_binop_types(@term.0.bt@, @term.1.bt@);
		@i @addexpr.n@ = newOperatorNode(OP_ADD, @term.0.n@, @term.1.n@);
	@}
	| addexpr PLUS term
	@{
		@i @term.symtab@ = @addexpr.symtab@;
		@i @addexpr.1.symtab@ = @addexpr.0.symtab@;

		@i @addexpr.bt@ = INT_T;
		@sematic check_binop_types(@addexpr.1.bt@, @term.bt@);
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

		@sematic check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| multexpr MULTIPLY term
	@{
		@i @term.symtab@ = @multexpr.symtab@;
		@i @multexpr.1.symtab@ = @multexpr.0.symtab@;

		@i @multexpr.bt@ = INT_T;

		@i @multexpr.0.n@ = newOperatorNode(OP_MULT, @multexpr.1.n@, @term.n@);

		@sematic check_binop_types(@multexpr.1.bt@, @term.bt@);
	@}
	;

orexpr: 
	term OR term
	@{
		@i @term.0.symtab@ = @orexpr.symtab@;
		@i @term.1.symtab@ = @orexpr.symtab@;

		@i @orexpr.bt@ = INT_T;
		@i @orexpr.n@ = newOperatorNode(OP_OR, @term.0.n@, @term.1.n@);

		@sematic check_binop_types(@term.0.bt@, @term.1.bt@);
	@}
	| orexpr OR term
	@{
		@i @term.symtab@ = @orexpr.symtab@;
		@i @orexpr.1.symtab@ = @orexpr.0.symtab@;

		@i @orexpr.bt@ = INT_T;
		@i @orexpr.n@ = newOperatorNode(OP_OR, @orexpr.1.n@, @term.n@);
		@sematic check_binop_types(@orexpr.1.bt@, @term.bt@);
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
		@i @term.n@ = newNumNode(0);
	@}
	| ID 
	@{
		@i @term.bt@ = symtab_lookup_return_type(@term.symtab@, @ID.id@);
		@i @term.n@ = newIdNode(@ID.id@, symtab_lookup_var_offset(@term.symtab@, @ID.id@), resolveType(symtab_lookup_kind(@term.symtab@, @ID.id@)));

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

enum VAR_TYPE resolveType(enum sym_kind kind) {
	switch(kind) {
		case VAR:
			return VAR;
		case PARAMETER:
			return PAR;
		case OBJ_VAR:
			return OBJ;
		default:
			fprintf(stderr, "Error: could not resolve type sym_kind: '%d'\n", kind);
			exit(3);
	}
}

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
