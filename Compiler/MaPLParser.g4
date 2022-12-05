parser grammar MaPLParser;  // MaPL is short for 'Macro Programming Language'
options { tokenVocab = MaPLLexer; }

program
    :    statement* EOF
    ;

statement
    :    imperativeStatement STATEMENT_DELIMITER
    |    whileLoop
    |    forLoop
    |    doWhileLoop
    |    conditional
    |    apiGlobal
    |    apiType
    |    apiImport
    |    metadataStatement
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

metadataStatement
    :    metadataTokens+=METADATA_OPEN (metadataTokens+=METADATA_INTERPOLATION expression metadataTokens+=SCOPE_CLOSE)* metadataTokens+=METADATA_CLOSE
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
    |    keyToken=(LOGICAL_NEGATION | SUBTRACT | BITWISE_NEGATION) expression
    |    expression keyToken=MOD expression
    |    expression keyToken=(MULTIPLY | DIVIDE) expression
    |    expression keyToken=(ADD | SUBTRACT) expression
    |    expression (keyToken=BITWISE_SHIFT_LEFT | bitwiseShiftRight) expression
    |    expression keyToken=(BITWISE_AND | BITWISE_XOR | BITWISE_OR) expression
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
    |    expression keyToken=(LOGICAL_AND | LOGICAL_OR) expression
    |    <assoc=right> expression keyToken=TERNARY_CONDITIONAL expression COLON expression
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
    |    pointerType
    ;

pointerType : identifier (LESS_THAN type (PARAM_DELIMITER type)* GREATER_THAN)? ;

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
apiGlobal : API_GLOBAL (apiFunction | apiProperty) STATEMENT_DELIMITER;
apiType
    :    API_TYPE typeName=identifier (LESS_THAN generics+=identifier (PARAM_DELIMITER generics+=identifier)* GREATER_THAN)? apiInheritance? SCOPE_OPEN
         (
             (
                 apiFunction |
                 apiProperty |
                 apiSubscript
             ) STATEMENT_DELIMITER
         )* SCOPE_CLOSE
    ;

apiInheritance : COLON pointerType (PARAM_DELIMITER pointerType)* ;
apiFunction : ( API_VOID | type ) identifier PAREN_OPEN apiFunctionParams? PAREN_CLOSE ;
apiFunctionParams : API_VARIADIC_PARAMETERS | type identifier (PARAM_DELIMITER type identifier)* (PARAM_DELIMITER API_VARIADIC_PARAMETERS)? ;
apiProperty : API_READONLY? type identifier ;
apiSubscript : API_READONLY? type SUBSCRIPT_OPEN type SUBSCRIPT_CLOSE ;
apiImport : API_IMPORT LITERAL_STRING ;

// LEXER WORKAROUNDS:
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

// The lexer cannot have enough context to correctly distinguish between the following statements:
//   `Array<Array<int32>> a = NULL;` vs `int32 i = 4 >> 1;`
// The fix is to recognize this concept via a parser rule instead of a lexer rule, and confirm there's no whitespace during compilation (ie, after parsing).
bitwiseShiftRight: GREATER_THAN GREATER_THAN;
