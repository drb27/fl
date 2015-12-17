%{
    int yylex(void);
    void yyerror(const char*);
%}

%token INTEGER

%union
{
    int int_val;
    char* string_val;
}

%type <int_val> INTEGER

%start input

%%

input: /* empty */
     | stmts;

stmt : integer;

stmts: stmt | stmts stmt;

integer: INTEGER '\n';
