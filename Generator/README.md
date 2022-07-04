# MaPL Generator

MaPL Generator is a code generation tool that takes a schema as input and outputs code files. The generator can also take in XML-formatted data, validate it against the schema.

## Architecture
The generator has a few important dependencies:

* **EaSL**
    * EaSL is the schema language used by MaPLGenerator. The implementation for this language is in the [EaSL](./EaSL) directory
* **libXML**
    * An open source XML-parsing library which parses the data which adheres to the EaSL schema.
* **MaPL**
    * MaPL is used to format the output from MaPLGenerator, primarily with its metadata language feature.

## Generation

When the generator runs, it performs the generation in the following phases:

1. **Schema Validation**
    * Verify that the collection of all EaSL files are valid unto themselves.
2. **Data Validation**
    * Verify that all XML data conforms correctly to the EaSL schema.
3. **Script-based generation**
    * Execute any specified MaPL scripts. Scripts have access to an API that exposes all schema and XML data. Scripts can specify which file to output to, and how to format that output.
