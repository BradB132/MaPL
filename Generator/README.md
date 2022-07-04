# MaPL Generator

MaPL Generator is a code generation tool that takes a schema as input and outputs code files. The generator can also take in XML-formatted data and validate it against the schema.

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

## Command line interface

MaPLGenerator takes in 3 types of paths:

1. Schema paths (requires at least 1).
1. XML paths (optional).
1. Script paths (requires at least 1).

In addition, MaPLGenerator accepts flag arguments. Each flag must be prefixed with `--` and specify a key and value. For example: `--myFeatureEnabled=false`. This mechanism allows users to pass important information directly from the command line into the code-generation script, allowing output to be altered without editing the scripts themselves.

## Building the generator

The easiest way to build the generator is to invoke:
```
cd MaPL/Generator
xcodebuild
```
Then check the [Executable](./Executable) directory for a symlink to the executable, as well as the MaPL API that can be used for accessing the schema and XML data.