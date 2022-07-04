# MaPL
**(Macro Programming Language)**

![Logo](./MaPL.svg)

## Design
MaPL is a C-style scripting language that focuses on a few different design goals:

1. **Runtime performance** - To achieve this goal, MaPL is compiled into bytecode instead of interpreted at runtime. The compiler is able to absorb the expensive parsing and compilation processes so that the runtime can execute the script with better performance.
1. **Compile-time feedback** - The MaPL compiler is responsible for much of the error checking, allowing it to catch errors as early and proactively as possible, and minimize the number of surprises at runtime. A couple of examples:
    * MaPL is strongly typed, so the compiler will emit errors whenever types are mismatched or ambiguous.
    * MaPL requires the API of the host program to be explicitly specified within the script. With this complete understanding of the API surface, the compiler can provide the maximum amount of feedback about API usage.
1. **Small runtime footprint** - The compiler absorbs most of the complexity, so bytecode files tend to be small in size (usually smaller than the script itself), and the runtime is lightweight.
1. **Small in scope** - MaPL scripts are intended to be macros, not entire programs. MaPL intentionally omits language features like the ability to implement entire classes or functions within a script. Instead, think of each MaPL script as the body of a function that can only utilize the existing classes and functions within the host program.

## Language Features

### Types
MaPL supports several data types:

* **char** - Unsigned 8-bit integer.
* **int32** - Signed 32-bit integer.
* **int64** - Signed 64-bit integer.
* **uint32** - Unsigned 32-bit integer.
* **uint64** - Unsigned 64-bit integer.
* **float32** - 32-bit floating point.
* **float64** - 64-bit floating point.
* **bool** - `true` or `false` boolean value.
* **string** - Text.
* **pointers** - There is no primitive type "pointer". Rather, pointers are referred to by their `#type` which is declared in an [API](#APIs).

### Control Flow
MaPL supports several types of C-style control flow.

Loops (`break` and `continue` are supported):

```
while /* boolean expression */ {
    // Repeated logic.
}
```
```
for /* statement */; /* boolean expression */; /* statement */ {
    // Repeated logic.
}
```
```
do {
    // Repeated logic.
} while /* boolean expression */;
```

Conditionals:
```
if /* boolean expression */ {
    // Conditional logic.
} else if /* boolean expression */ {
    // Conditional logic.
} else {
    // Conditional logic.
}
```
```
int32 myVariable = /* boolean expression */ ? 1 : 0;
```
```
Object myObject = possiblyNull() ?? fallbackValue;
```
### Operators
MaPL includes many of the C-style operators:

* **Arithmetic** (numeric types only) - `+`, `-`, `*`, `/`, `%`, `++`, `--`
* **Numeric comparison** (numeric types only) - `<`, `<=`, `>`, `>=`
* **Equality comparison** - `==`, `!=`
* **Logical** (boolean only) - `&&`, `||`, `!`
* **Bitwise** (integral types only) - `&`, `|`, `^`, `~`, `<<`, `>>`

### APIs
MaPL requires explicit declarations for the entire API surface of the host program. APIs are composed of `#type` and `#global` declarations. For example:
```
#type Vehicle {
    float32 speed;
}
#type Car : Vehicle {
    readonly Array<Wheel> wheels;
}
#type Array<T> {
    T [uint32];
    readonly uint32 count;
}
#type Wheel {
    float32 radius;
}
#global Car getCar();
```
The above API would allow a subsequent script to write logic like:
```
Car myCar = getCar();
for uint32 i = 0; i < myCar.wheels.count; i++) {
    myCar.wheels[i].radius += 2;
}
myCar.speed = 45;
```
If you attempted to deviate from the API, by doing something like...
```
myCar.speed = "five";
myCar.fooBar();
```
...the compiler would emit errors regarding the incorrect type of the `speed` value and the usage of the undeclared `fooBar()` function.

### Metadata
MaPL can be used for code generation (or generation of any type of text file). By including metadata tags in MaPL script, the host program can receive the content of those tags as the script executes. For example:
```
<?Start Metadata?>
for int32 i = 0; i < 3; i++ {
    <?Loop Metadata?>
}
<?Start Metadata?>
```
The above script will send callbacks to the host program in the following order:

1. `Start Metadata`
1. `Loop Metadata`
1. `Loop Metadata`
1. `Loop Metadata`
1. `End Metadata`

The host program can take these callbacks and output to file. This is the goal of the ancillary [MaPLGenerator](./Generator) program, included in this repo.
