grammar EaSL;  // EaSL is short for 'Easy Schema Language'

schema
    :    namespace definition* EOF
    ;

namespace
    :    NAMESPACE identifier STATEMENT_END
    ;

definition
    :    classDefinition
    |    enumDefinition
    ;

classDefinition
    :    ANNOTATION* CLASS identifier (COLON identifier)? DEFINITION_OPEN attribute* DEFINITION_CLOSE
    ;

attribute
    :    ANNOTATION* type identifier sequenceDescriptor? (DEFAULTS_TO defaultValue)? STATEMENT_END
    ;

sequenceDescriptor
    :    SEQUENCE_OPEN (sequenceLength (SEQUENCE_DELIMITER sequenceLength)?)? SEQUENCE_CLOSE
    ;
sequenceLength : LITERAL_INT | SEQUENCE_WILDCARD ;

enumDefinition
    :    ANNOTATION* ENUM enumName=identifier DEFINITION_OPEN (enumValue+=identifier SEQUENCE_DELIMITER?)+ DEFINITION_CLOSE
    ;

defaultValue
    :    literalValue (SEQUENCE_DELIMITER literalValue)*
    ;
    
literalValue
    :    literalToken=LITERAL_NULL
    |    literalToken=LITERAL_TRUE
    |    literalToken=LITERAL_FALSE
    |    literalToken=LITERAL_STRING
    |    literalToken=LITERAL_INT
    |    literalToken=LITERAL_FLOAT
    ;

type
    :    typeToken=DECL_CHAR
    |    typeToken=DECL_INT32
    |    typeToken=DECL_INT64
    |    typeToken=DECL_UINT32
    |    typeToken=DECL_UINT64
    |    typeToken=DECL_FLOAT32
    |    typeToken=DECL_FLOAT64
    |    typeToken=DECL_BOOL
    |    typeToken=DECL_STRING
    |    typeToken=DECL_UID
    |    typeToken=REFERENCE REFERENCE_OPEN identifier REFERENCE_CLOSE
    |    identifier
    ;

// This is a special case where a concept is encoded as both a lexer and parser rule.
// All keywords must be also recognizable as identifiers, and the lexer doesn't have enough context to do this correctly.
identifier
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
    |    REFERENCE
    |    NAMESPACE
    |    CLASS
    |    ENUM
    |    LITERAL_NULL
    |    LITERAL_TRUE
    |    LITERAL_FALSE
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
REFERENCE: 'reference' ;
REFERENCE_OPEN: '<';
REFERENCE_CLOSE: '>';

// DEFINITIONS
NAMESPACE: 'namespace' ;
CLASS: 'class' ;
ENUM: 'enum' ;
DEFAULTS_TO: '=' ;
DEFINITION_OPEN: '{' ;
DEFINITION_CLOSE: '}' ;
SEQUENCE_OPEN: '[' ;
SEQUENCE_CLOSE: ']' ;
SEQUENCE_DELIMITER: ',' ;
SEQUENCE_WILDCARD: '*' ;
COLON: ':' ;
STATEMENT_END: ';' ;
ANNOTATION: '@' IDENTIFIER_FRAGMENT;

// LITERALS
LITERAL_NULL: 'NULL' ;
LITERAL_TRUE: 'true' ;
LITERAL_FALSE: 'false' ;
LITERAL_INT : '-'? DIGITS ;
LITERAL_FLOAT : '-'? DIGITS? '.' DIGITS ;
fragment DIGITS : [0-9]+ ;
LITERAL_STRING : '"' (STRING_ESC|.)*? '"' ;
fragment STRING_ESC : '\\"' | '\\\\' ;

IDENTIFIER: IDENTIFIER_FRAGMENT;
fragment IDENTIFIER_FRAGMENT : [_a-zA-Z][_a-zA-Z0-9]* ;

BLOCK_COMMENT : '/*' .*? '*/' -> skip ;
LINE_COMMENT : '//' .*? '\r'? '\n' -> skip ;

WHITESPACE : [ \t\r\n]+ -> skip ;
