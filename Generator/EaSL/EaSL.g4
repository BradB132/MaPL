grammar EaSL;  // EaSL is short for 'Easy Schema Language'

schema
    :    definition* EOF
    ;

definition
    :    classDefinition
    |    enumDefinition
    ;

classDefinition
    :    CLASS identifier (COLON identifier)? DEFINITION_OPEN attribute* DEFINITION_CLOSE
    ;

attribute
    :    READONLY? type identifier sequenceDescriptor? (DEFAULTS_TO defaultValue)? STATEMENT_DELIMITER
    ;

sequenceDescriptor
    :    SEQUENCE_OPEN (sequenceLength (SEQUENCE_DELIMITER sequenceLength)?)? SEQUENCE_CLOSE
    ;
sequenceLength : LITERAL_INT | SEQUENCE_WILDCARD ;

enumDefinition
    :    ENUM identifier DEFINITION_OPEN (identifier SEQUENCE_DELIMITER?)+ DEFINITION_CLOSE
    ;

defaultValue
    :    LITERAL_NULL
    |    LITERAL_TRUE
    |    LITERAL_FALSE
    |    LITERAL_STRING
    |    LITERAL_INT
    |    LITERAL_FLOAT
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
    |    DECL_UID
    |    DECL_UID_REF
    |    identifier
    ;

// This is a special case where a concept is encoded as both a lexer and parser rule.
// All keywords must be also recognizable as identifiers, and the lexer doesn't have enough context to do this correctly.
identifier
    :    LITERAL_NULL
    |    DECL_CHAR
    |    DECL_INT32
    |    DECL_INT64
    |    DECL_UINT32
    |    DECL_UINT64
    |    DECL_FLOAT32
    |    DECL_FLOAT64
    |    DECL_BOOL
    |    DECL_STRING
    |    DECL_UID
    |    DECL_UID_REF
    |    LITERAL_TRUE
    |    LITERAL_FALSE
    |    READONLY
    |    IDENTIFIER
    ;

// TYPES
DECL_CHAR: 'char' ;
DECL_INT32: 'int32' ;
DECL_INT64: 'int64' ;
DECL_UINT32: 'uint32' ;
DECL_UINT64: 'uint64' ;
DECL_FLOAT32: 'float32' ;
DECL_FLOAT64: 'float64' ;
DECL_BOOL: 'bool' ;
DECL_STRING: 'string' ;
DECL_UID: 'UID' ;
DECL_UID_REF: 'UIDRef' ;

// DEFINITIONS
CLASS: 'class' ;
ENUM: 'enum' ;
READONLY: 'readonly' ;
DEFAULTS_TO: '=' ;
DEFINITION_OPEN: '{' ;
DEFINITION_CLOSE: '}' ;
SEQUENCE_OPEN: '[' ;
SEQUENCE_CLOSE: ']' ;
SEQUENCE_DELIMITER: ',' ;
SEQUENCE_WILDCARD: '*' ;
COLON: ':' ;
STATEMENT_DELIMITER: ';' ;

// LITERALS
LITERAL_NULL: 'NULL' ;
LITERAL_TRUE: 'true' ;
LITERAL_FALSE: 'false' ;
LITERAL_INT : DIGITS ;
LITERAL_FLOAT : DIGITS? '.' DIGITS ;
fragment DIGITS : [0-9]+ ;
LITERAL_STRING : '"' (STRING_ESC|.)*? '"' ;
fragment STRING_ESC : '\\"' | '\\\\' ;

IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]* ;

BLOCK_COMMENT : '/*' .*? '*/' -> skip ;
LINE_COMMENT : '//' .*? '\r'? '\n' -> skip ;

WHITESPACE : [ \t\r\n]+ -> skip ;
