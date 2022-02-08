grammar MaPL;  // MaPL is short for 'Macro Programming Language'

program
    :    statement* EOF
    ;

statement
    :    imperativeStatement STATEMENT_DELIMITER
    |    whileLoop
    |    forLoop
    |    doWhileLoop
    |    conditional
    |    switchStatement
    |    apiDeclaration
    |    apiImport
    |    METADATA
    |    scope
    ;

imperativeStatement
    :    variableDeclaration
    |    assignStatement
    |    unaryStatement
    |    objectExpression
    |    BREAK
    |    CONTINUE
    |    EXIT
    ;

assignStatement
    :    objectExpression
         (
             ASSIGN |
             ADD_ASSIGN |
             SUBTRACT_ASSIGN |
             MULTIPLY_ASSIGN |
             DIVIDE_ASSIGN |
             EXPONENT_ASSIGN |
             MOD_ASSIGN
         ) expression
    ;

unaryStatement
    :    objectExpression (INCREMENT|DECREMENT)
    ;

// EXPRESSIONS
expression
    :    PAREN_OPEN type PAREN_CLOSE expression
    |    expression (LOGICAL_AND | LOGICAL_OR) expression
    |    expression
         (
            LOGICAL_EQUALITY |
            LOGICAL_INEQUALITY |
            LESS_THAN |
            LESS_THAN_EQUAL |
            GREATER_THAN |
            GREATER_THAN_EQUAL
         ) expression
    |    LOGICAL_NEGATION expression
    |    SUBTRACT expression
    |    expression MOD expression
    |    expression EXPONENT expression
    |    expression (MULTIPLY | DIVIDE) expression
    |    expression (ADD | SUBTRACT) expression
    |    PAREN_OPEN expression PAREN_CLOSE
    |    LITERAL_TRUE
    |    LITERAL_FALSE
    |    LITERAL_NULL
    |    INT
    |    FLOAT
    |    STRING
    |    objectExpression
    ;
    
objectExpression
    :    objectExpression OBJECT_TO_MEMBER objectExpression
    |    objectExpression SUBSCRIPT_OPEN expression SUBSCRIPT_CLOSE
    |    identifier (PAREN_OPEN (expression (ARG_DELIMITER expression)*)? PAREN_CLOSE)?
    ;

// VARIABLES
variableDeclaration
    :    type identifier (ASSIGN expression)?
    ;

type
    :    DECL_INT8
    |    DECL_INT16
    |    DECL_INT32
    |    DECL_INT64
    |    DECL_UINT8
    |    DECL_UINT16
    |    DECL_UINT32
    |    DECL_UINT64
    |    DECL_FLOAT32
    |    DECL_FLOAT64
    |    DECL_BOOL
    |    DECL_STRING
    |    identifier
    ;

// CONTROL FLOW
scope : SCOPE_OPEN statement* SCOPE_CLOSE ;

whileLoop : LOOP_WHILE expression scope ;

forLoop
    :    LOOP_FOR PAREN_OPEN forLoopControlStatements PAREN_CLOSE scope
    |    LOOP_FOR forLoopControlStatements scope
    ;
forLoopControlStatements
    :    imperativeStatement? STATEMENT_DELIMITER expression? STATEMENT_DELIMITER imperativeStatement?
    ;

doWhileLoop : LOOP_DO scope LOOP_WHILE expression STATEMENT_DELIMITER ;

conditional : CONDITIONAL expression scope (CONDITIONAL_ELSE scope)? ;

innerSwitchStatement
    :    SWITCH_CASE
         (
            INT |
            FLOAT |
            STRING |
            LITERAL_TRUE |
            LITERAL_FALSE
         ) SWITCH_DELIMITER statement*
    |    SWITCH_DEFAULT SWITCH_DELIMITER statement*
    ;

switchStatement : SWITCH expression SCOPE_OPEN innerSwitchStatement+ SCOPE_CLOSE ;

// API DECLARATIONS
apiDeclaration
    :    API_GLOBAL (apiFunction | apiProperty) STATEMENT_DELIMITER
    |    API_TYPE identifier apiInheritance? SCOPE_OPEN
         (
             (
                 apiFunction |
                 apiProperty |
                 apiSubscript
             ) STATEMENT_DELIMITER
         )* SCOPE_CLOSE
    ;

apiInheritance : SWITCH_DELIMITER identifier (ARG_DELIMITER identifier)* ;
apiFunction : ( API_VOID | type ) identifier PAREN_OPEN apiFunctionArgs? PAREN_CLOSE ;
apiFunctionArgs : API_VARIADIC_ARGUMENTS | type (ARG_DELIMITER type)* (ARG_DELIMITER API_VARIADIC_ARGUMENTS)? ;
apiProperty : API_READONLY? type identifier ;
apiSubscript : API_READONLY? type SUBSCRIPT_OPEN type SUBSCRIPT_CLOSE ;
apiImport : API_IMPORT STRING ;

// This is a special case where a concept is encoded as both a lexer and parser rule.
// All keywords must be also recognizable as identifiers, and the lexer doesn't have enough context to do this correctly.
identifier
    :    LOOP_WHILE
    |    LOOP_FOR
    |    LOOP_DO
    |    CONDITIONAL
    |    CONDITIONAL_ELSE
    |    SWITCH
    |    SWITCH_CASE
    |    SWITCH_DEFAULT
    |    BREAK
    |    CONTINUE
    |    EXIT
    |    LITERAL_NULL
    |    API_READONLY
    |    API_VOID
    |    DECL_INT8
    |    DECL_INT16
    |    DECL_INT32
    |    DECL_INT64
    |    DECL_UINT8
    |    DECL_UINT16
    |    DECL_UINT32
    |    DECL_UINT64
    |    DECL_FLOAT32
    |    DECL_FLOAT64
    |    DECL_BOOL
    |    DECL_STRING
    |    LITERAL_TRUE
    |    LITERAL_FALSE
    |    IDENTIFIER
    ;

// OPERATORS
ASSIGN: '=' ;
ADD: '+' ;
ADD_ASSIGN: '+=' ;
SUBTRACT: '-' ;
SUBTRACT_ASSIGN: '-=' ;
DIVIDE: '/' ;
DIVIDE_ASSIGN: '/=' ;
MULTIPLY: '*' ;
MULTIPLY_ASSIGN: '*=' ;
EXPONENT: '^' ;
EXPONENT_ASSIGN: '^=' ;
MOD: '%' ;
MOD_ASSIGN: '%=' ;
INCREMENT: '++' ;
DECREMENT: '--' ;
LOGICAL_EQUALITY: '==' ;
LOGICAL_INEQUALITY: '!=' ;
LOGICAL_AND: '&&' ;
LOGICAL_OR: '||' ;
LOGICAL_NEGATION: '!' ;
LESS_THAN: '<' ;
LESS_THAN_EQUAL: '<=' ;
GREATER_THAN: '>' ;
GREATER_THAN_EQUAL: '>=' ;

// KEYWORDS
LOOP_WHILE: 'while' ;
LOOP_FOR: 'for' ;
LOOP_DO: 'do' ;
CONDITIONAL: 'if' ;
CONDITIONAL_ELSE: 'else' ;
SWITCH: 'switch' ;
SWITCH_CASE: 'case' ;
SWITCH_DEFAULT: 'default' ;
BREAK: 'break' ;
CONTINUE: 'continue' ;
EXIT: 'exit' ;
LITERAL_NULL: 'NULL' ;
DECL_INT8: 'int8' ;
DECL_INT16: 'int16' ;
DECL_INT32: 'int32' ;
DECL_INT64: 'int64' ;
DECL_UINT8: 'uint8' ;
DECL_UINT16: 'uint16' ;
DECL_UINT32: 'uint32' ;
DECL_UINT64: 'uint64' ;
DECL_FLOAT32: 'float32' ;
DECL_FLOAT64: 'float64' ;
DECL_BOOL: 'bool' ;
DECL_STRING: 'string' ;
LITERAL_TRUE: 'true' ;
LITERAL_FALSE: 'false' ;

// PUNCTUATION
PAREN_OPEN: '(' ;
PAREN_CLOSE: ')' ;
SCOPE_OPEN: '{' ;
SCOPE_CLOSE: '}' ;
SUBSCRIPT_OPEN: '[' ;
SUBSCRIPT_CLOSE: ']' ;
OBJECT_TO_MEMBER: '.' ;
ARG_DELIMITER: ',' ;
SWITCH_DELIMITER: ':' ;
STATEMENT_DELIMITER: ';' ;

// API
API_GLOBAL: '#global' ;
API_TYPE: '#type' ;
API_IMPORT: '#import' ;
API_READONLY: 'readonly' ;
API_VOID: 'void' ;
API_VARIADIC_ARGUMENTS: '...' ;

// LITERALS
IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]* ;

INT : DIGITS ;
FLOAT
    :    DIGITS ( '.' DIGITS )?
    |    '.' DIGITS
    ;
fragment DIGITS : [0-9]+ ;

STRING : '"' (STRING_ESC|.)*? '"' ;
fragment STRING_ESC : '\\"' | '\\\\' ;

METADATA : '<?' .*? '?>' ;

BLOCK_COMMENT : '/*' .*? '*/' -> skip ;
LINE_COMMENT : '//' .*? '\r'? '\n' -> skip ;

WHITESPACE : [ \t\r\n]+ -> skip ;
