# EaSL
**(Easy Schema Language)**

![Logo](./EaSL_logo.svg)

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

The EaSL directory contains an [example schema](./exampleSchema.easl) and [example XML](./exampleXML.xml).

EaSL is intended for use in [MaPLGenerator](../).
