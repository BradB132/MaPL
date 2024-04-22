# MaPL Generator

MaPL Generator is a code generation tool that takes a schema as input and outputs code files. The generator can also take in XML-formatted data and validate it against the schema.

## Architecture
The generator has a few important dependencies:

* **EaSL**
    * EaSL is the schema language used by MaPLGenerator. The implementation for this language is in the [EaSL](./EaSL) directory
* **MaPL**
    * MaPL is used to format the output from MaPLGenerator, primarily with its metadata language feature.
* **tinyxml**
    * An open source XML-parsing library which parses the data which adheres to the EaSL schema.

## Generation

When the generator runs, it performs the generation in the following phases:

1. **Schema Validation**
    * Verify that the collection of all EaSL files are valid unto themselves.
1. **Data Validation**
    * Verify that all XML data conforms correctly to the EaSL schema(s).
1. **Script-based generation**
    * Execute any specified MaPL scripts. Scripts have access to an [API](./Executable/MaPLGeneratorAPI.mapl) that exposes all schema and XML data. Scripts can specify which file to output to, and how to format that output.

## Command line interface

MaPLGenerator takes in 3 types of paths:

1. Schema paths (requires at least 1).
1. XML paths (optional).
1. Template script paths (requires at least 1).

In addition, MaPLGenerator accepts flag arguments. This mechanism allows users to pass important information directly from the command line into the code-generation template. The required flags are specified in the templates themselves (each template can query for whatever extra information it needs). MaPLGenerator will alert you if your template requests a flag that you haven't specified. Each flag must be prefixed with `--` and specify a key and value. For example: `--myFeatureEnabled=false`.

A template can be invoked via the command line like this:
```
cd MaPL/Generator/Executable

./MaPLGenerator ../EaSL/exampleSchema.easl ../Templates/MaPL/main.mapl  --scriptOutputDir=../path/to/output
```

## Examples
Example code-generation templates can be found in the [Templates](./Templates) directory.

## Building the generator

The easiest way to build the generator is to invoke `cmake`:
```
cd MaPL/Generator
cmake .
make
```

The executable can also be built using the existing Xcode project if you're on macOS.
