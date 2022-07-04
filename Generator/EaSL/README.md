# EaSL
**(Easy Schema Language)**

![Logo](./EaSL.svg)

## Design
EaSL is a schema language intended for use with MaPLGenerator. EaSL has a straightforward syntax that allows specification of:

* Multiple interrelated schemas (each with a separate namespace).
* Multiple classes and/or enumerations within each namespace.
* Multiple attributes within each class.
* Attributes with data types matching all MaPL primitives.
* Attributes with data types for specifying relationships between objects.
* Sequences of attributes:
    * Primitive values.
    * One-to-many object relationships.
    * Child objects.