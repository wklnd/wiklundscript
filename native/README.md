# Native Modules

This folder contains C shared libraries that wscript can load with `import`.

## How imports work

Use this syntax in a `.ws` file:

```ws
import DateTime.getCurrentDate
import Random.getRandomInt
```

The interpreter looks for a shared library named:

- `native/libDateTime.so`
- `native/libRandom.so`

Then it looks for a symbol named:

- `DateTime_getCurrentDate`
- `Random_getRandomInt`

## Native function ABI

Native functions use this signature:

```c
const char *Module_function(size_t argc, const char **argv)
```

Rules:

- `argc` is the number of script arguments.
- `argv` contains the arguments as strings.
- Return a heap-allocated C string.
- The interpreter converts the returned string to `Int`, `Float`, or `String`.

Example return values:

- `"42"` becomes an `Int`
- `"3.14"` becomes a `Float`
- Anything else becomes a `String`

## Building

Build the native libraries from this folder:

```bash
make -C native
```

Or from the repo root:

```bash
make
```

## Example

`native/Random.c` exposes:

```c
const char *Random_getRandomInt(size_t argc, const char **argv)
```

And this WScript code calls it:

```ws
import Random.getRandomInt

var value = getRandomInt(1, 100)
print(value)
```

## Creating a new module

1. Add a new `.c` file in `native/`.
2. Export a function named `ModuleName_functionName`.
3. Add a `libModuleName.so` build rule to `native/Makefile` and, if needed, the root `Makefile`.
4. Import it from WScript with `import ModuleName.functionName`.



# String Library


getStringLength(string)
returns the length of the string

example: 
const string = "Hello"
print("String length: " + getStringLength(string))

