lexer grammar MaPLLexer;  // MaPL is short for 'Macro Programming Language'

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
DECL_CHAR: 'char' ;
DECL_INT32: 'int32' ;
DECL_INT64: 'int64' ;
DECL_UINT32: 'uint32' ;
DECL_UINT64: 'uint64' ;
DECL_FLOAT32: 'float32' ;
DECL_FLOAT64: 'float64' ;
DECL_BOOL: 'bool' ;
DECL_STRING: 'string' ;

// PUNCTUATION
PAREN_OPEN: '(' ;
PAREN_CLOSE: ')' ;
SCOPE_OPEN: '{' -> pushMode(DEFAULT_MODE);
SCOPE_CLOSE: '}' -> popMode ;
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
LITERAL_NULL: 'NULL' ;
LITERAL_TRUE: 'true' ;
LITERAL_FALSE: 'false' ;
LITERAL_INT : DIGITS ;
LITERAL_FLOAT : DIGITS? '.' DIGITS ;
fragment DIGITS : [0-9]+ ;
LITERAL_STRING : '"' (STRING_ESC|.)*? '"' ;
fragment STRING_ESC : '\\"' | '\\\\' ;

METADATA_OPEN: '<?' -> pushMode(METADATA);

IDENTIFIER: [_a-zA-Z][_a-zA-Z0-9]* ;

BLOCK_COMMENT: '/*' .*? '*/' -> skip ;
LINE_COMMENT: '//' .*? '\r'? '\n' -> skip ;

WHITESPACE: [ \t\r\n]+ -> skip ;

// This value is no longer used in the parser, but is still used in the compiler.
// Backtick character used as placeholder because it's not needed elsewhere in MaPL.
BITWISE_SHIFT_RIGHT: '`';

mode METADATA;
METADATA_INTERPOLATION: '${' -> pushMode(DEFAULT_MODE);
METADATA_CLOSE: '?>' -> popMode;
// All metadata characters will be picked up by
// the compiler during its parse tree traversal.
METADATA_CHAR: . -> skip ;
