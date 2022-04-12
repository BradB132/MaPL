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
    |    apiDeclaration
    |    apiImport
    |    METADATA
    |    scope
    ;

imperativeStatement
    :    keyToken=BREAK
    |    keyToken=CONTINUE
    |    keyToken=EXIT
    |    variableDeclaration
    |    assignStatement
    |    unaryStatement
    |    objectExpression
    ;

assignStatement
    :    objectExpression
         keyToken=(
             ASSIGN |
             ADD_ASSIGN |
             SUBTRACT_ASSIGN |
             MULTIPLY_ASSIGN |
             DIVIDE_ASSIGN |
             MOD_ASSIGN |
             BITWISE_AND_ASSIGN |
             BITWISE_OR_ASSIGN |
             BITWISE_XOR_ASSIGN |
             BITWISE_SHIFT_LEFT_ASSIGN |
             BITWISE_SHIFT_RIGHT_ASSIGN
         ) expression
    ;

unaryStatement
    :    objectExpression keyToken=(INCREMENT|DECREMENT)
    ;

// EXPRESSIONS
expression
    :    keyToken=PAREN_OPEN type PAREN_CLOSE expression
    |    expression keyToken=(LOGICAL_AND | LOGICAL_OR) expression
    |    keyToken=(LOGICAL_NEGATION | SUBTRACT | BITWISE_NEGATION) expression
    |    expression keyToken=MOD expression
    |    expression keyToken=(MULTIPLY | DIVIDE) expression
    |    expression keyToken=(ADD | SUBTRACT) expression
    |    expression keyToken=(BITWISE_SHIFT_LEFT | BITWISE_SHIFT_RIGHT) expression
    |    expression keyToken=(BITWISE_AND | BITWISE_XOR | BITWISE_OR) expression
    |    <assoc=right> expression keyToken=TERNARY_CONDITIONAL expression COLON expression
    |    <assoc=right> expression keyToken=NULL_COALESCING expression
    |    expression
         keyToken=(
            LOGICAL_EQUALITY |
            LOGICAL_INEQUALITY |
            LESS_THAN |
            LESS_THAN_EQUAL |
            GREATER_THAN |
            GREATER_THAN_EQUAL
         ) expression
    |    PAREN_OPEN expression keyToken=PAREN_CLOSE
    |    keyToken=LITERAL_TRUE
    |    keyToken=LITERAL_FALSE
    |    keyToken=LITERAL_NULL
    |    keyToken=LITERAL_INT
    |    keyToken=LITERAL_FLOAT
    |    keyToken=LITERAL_STRING
    |    objectExpression
    ;
    
objectExpression
    :    objectExpression keyToken=OBJECT_TO_MEMBER objectExpression
    |    objectExpression keyToken=SUBSCRIPT_OPEN expression SUBSCRIPT_CLOSE
    |    identifier (keyToken=PAREN_OPEN (expression (PARAM_DELIMITER expression)*)? PAREN_CLOSE)?
    ;

// VARIABLES
variableDeclaration
    :    type identifier (ASSIGN expression)?
    ;

type
    :    DECL_CHAR
    |    DECL_INT32
    |    DECL_INT64
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
    :    firstStatement=imperativeStatement? STATEMENT_DELIMITER expression? STATEMENT_DELIMITER lastStatement=imperativeStatement?
    ;

doWhileLoop : LOOP_DO scope LOOP_WHILE expression STATEMENT_DELIMITER ;

conditional : CONDITIONAL expression scope conditionalElse? ;
conditionalElse : CONDITIONAL_ELSE (scope | conditional) ;

// API DECLARATIONS
apiDeclaration
    :    keyToken=API_GLOBAL (apiFunction | apiProperty) STATEMENT_DELIMITER
    |    keyToken=API_TYPE identifier apiInheritance? SCOPE_OPEN
         (
             (
                 apiFunction |
                 apiProperty |
                 apiSubscript
             ) STATEMENT_DELIMITER
         )* SCOPE_CLOSE
    ;

apiInheritance : COLON identifier (PARAM_DELIMITER identifier)* ;
apiFunction : ( API_VOID | type ) identifier PAREN_OPEN apiFunctionParams? PAREN_CLOSE ;
apiFunctionParams : API_VARIADIC_PARAMETERS | type (PARAM_DELIMITER type)* (PARAM_DELIMITER API_VARIADIC_PARAMETERS)? ;
apiProperty : API_READONLY? type identifier ;
apiSubscript : API_READONLY? type SUBSCRIPT_OPEN type SUBSCRIPT_CLOSE ;
apiImport : API_IMPORT LITERAL_STRING ;

// This is a special case where a concept is encoded as both a lexer and parser rule.
// All keywords must be also recognizable as identifiers, and the lexer doesn't have enough context to do this correctly.
identifier
    :    LOOP_WHILE
    |    LOOP_FOR
    |    LOOP_DO
    |    CONDITIONAL
    |    CONDITIONAL_ELSE
    |    BREAK
    |    CONTINUE
    |    EXIT
    |    LITERAL_NULL
    |    API_READONLY
    |    API_VOID
    |    DECL_CHAR
    |    DECL_INT32
    |    DECL_INT64
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
BITWISE_NEGATION: '~';
BITWISE_AND: '&';
BITWISE_AND_ASSIGN: '&=';
BITWISE_OR: '|';
BITWISE_OR_ASSIGN: '|=';
BITWISE_XOR: '^';
BITWISE_XOR_ASSIGN: '^=';
BITWISE_SHIFT_LEFT: '<<';
BITWISE_SHIFT_LEFT_ASSIGN: '<<=';
BITWISE_SHIFT_RIGHT: '>>';
BITWISE_SHIFT_RIGHT_ASSIGN: '>>=';

// KEYWORDS
LOOP_WHILE: 'while' ;
LOOP_FOR: 'for' ;
LOOP_DO: 'do' ;
CONDITIONAL: 'if' ;
CONDITIONAL_ELSE: 'else' ;
BREAK: 'break' ;
CONTINUE: 'continue' ;
EXIT: 'exit' ;
LITERAL_NULL: 'NULL' ;
DECL_CHAR: 'char' ;
DECL_INT32: 'int32' ;
DECL_INT64: 'int64' ;
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
PARAM_DELIMITER: ',' ;
COLON: ':' ;
TERNARY_CONDITIONAL: '?' ;
NULL_COALESCING: '??' ;
STATEMENT_DELIMITER: ';' ;

// API
API_GLOBAL: '#global' ;
API_TYPE: '#type' ;
API_IMPORT: '#import' ;
API_READONLY: 'readonly' ;
API_VOID: 'void' ;
API_VARIADIC_PARAMETERS: '...' ;

// LITERALS
IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]* ;

LITERAL_INT : DIGITS ;
LITERAL_FLOAT
    :    DIGITS ( '.' DIGITS )?
    |    '.' DIGITS
    ;
fragment DIGITS : [0-9]+ ;

LITERAL_STRING : '"' (STRING_ESC|.)*? '"' ;
fragment STRING_ESC : '\\"' | '\\\\' ;

METADATA : '<?' .*? '?>' ;

BLOCK_COMMENT : '/*' .*? '*/' -> skip ;
LINE_COMMENT : '//' .*? '\r'? '\n' -> skip ;

WHITESPACE : [ \t\r\n]+ -> skip ;
