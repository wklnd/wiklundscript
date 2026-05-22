# Native Modules

This directory contains the native libraries that wscript can load with `import`.

## Important: This is not complete, and probably will always be behind. 

## ABI

All native functions use this signature:

```c
NativeValue Module_function(size_t argc, const char **argv)
```

Use `native/native.h` for the shared `NativeType` enum and `NativeValue` tagged union.

## Import Pattern

Use native functions like this from a `.ws` file:

```ws
import Time.getDate
import IO.readline
import String.reverse
import Random.getRandomInt
import Math.floor
```

The interpreter loads `native/libName.so` and resolves symbols named `Name_function`.

`DateTime` remains as a compatibility module, but `Time` is the preferred home for date and sleep helpers.

## Time

Date and sleep helpers.

### `getDate()`

Returns today in `YYYY-MM-DD` format.

```ws
import Time.getDate

print(getDate())
```

### `getTomorrow()`

Returns tomorrow in `YYYY-MM-DD` format.

```ws
import Time.getTomorrow

print(getTomorrow())
```

### `sleep(seconds)`

Pauses execution for the given number of seconds.

```ws
import Time.sleep

print("waiting...")
sleep(0.5)
print("done")
```

Notes:

- Fractional seconds are supported.
- A non-positive value returns immediately.

## DateTime

Compatibility aliases for older scripts.

### `getCurrentDate()`

Alias for `getDate()`.

```ws
import DateTime.getCurrentDate

print(getCurrentDate())
```

### `getTomorrow()`

Alias for `Time.getTomorrow()`.

```ws
import DateTime.getTomorrow

print(getTomorrow())
```

## IO

Terminal input helpers.

### `readline(prompt)`

Prints a prompt, then reads one line from standard input.

```ws
import IO.readline

var name = readline("Name: ")
print("Hello, " + name)
```

Notes:

- The trailing newline is removed.
- If input ends, the function returns an empty string.

### `parseInt(text)`

Converts text to an `Int`.

```ws
import IO.parseInt

var value = parseInt("42")
print(value)
```

Notes:

- Parse before the first assignment if you want the variable to be an `Int`.
- Variables keep the type they were first assigned.

## Math

Numeric helpers for calculations.

### `floor(value)`

Rounds down to the nearest whole number.

```ws
print(floor(3.9))
print(floor(-3.1))
```

### `sqrt(value)`

Returns the square root.

```ws
print(sqrt(16))
```

### `abs(value)`

Returns the absolute value.

```ws
print(abs(-12))
print(abs(-2.5))
```

### `round(value)`

Rounds to the nearest whole number.

```ws
print(round(3.2))
print(round(3.8))
```

### `pow(base, exponent)`

Raises a number to a power.

```ws
print(pow(2, 3))
```

### `min(values...)`

Returns the smallest value from the arguments.

```ws
print(min(5, 2, 9, 1))
```

### `max(values...)`

Returns the largest value from the arguments.

```ws
print(max(5, 2, 9, 1))
```

### `random()`

Returns a random float in the range $0 \le x < 1$.

```ws
print(random())
```

Notes:

- `floor` and `round` return `Int` values.
- `sqrt` and `pow` return `Float` values.
- `min` and `max` preserve whole-number results when all inputs are whole numbers.

## Random

Pseudo-random number helpers.

### `getRandomInt(min, max)`

Returns a random integer between `min` and `max`, inclusive.

```ws
import Random.getRandomInt

print(getRandomInt(1, 100))
```

If `min` and `max` are swapped, the function normalizes them.

### `getRandomFloat(min, max)`

Returns a random float between `min` and `max`.

```ws
import Random.getRandomFloat

print(getRandomFloat(0, 1))
```

Notes:

- The random generator is seeded once per process.

## String

Basic string operations.

### `getStringLength(text)`

Returns the number of characters in a string.

```ws
import String.getStringLength

var length = getStringLength("Hello")
print(length)
print(length + 2)
```

### `doStringConcat(values...)`

Concatenates all arguments into one string.

```ws
import String.doStringConcat

print(doStringConcat("Hello", " ", "world"))
```

### `doStringReverse(text)`

Returns the text in reverse order.

```ws
import String.doStringReverse

print(doStringReverse("wscript"))
```

### `doStringCapitalize(text)`

Uppercases the first character and lowercases the rest.

```ws
import String.doStringCapitalize

print(doStringCapitalize("hello WORLD"))
```

### `doStringUppercase(text)`

Converts text to uppercase.

```ws
import String.doStringUppercase

print(doStringUppercase("hello"))
```

### `doStringLowercase(text)`

Converts text to lowercase.

```ws
import String.doStringLowercase

print(doStringLowercase("Hello"))
```

### `doStringEquals(left, right)`

Returns `true` if both strings are exactly equal.

```ws
import String.doStringEquals

print(doStringEquals("abc", "abc"))
```

## Adding A New Module

1. Add a new `.c` file in `native/`.
2. Include `native.h`.
3. Export functions as `ModuleName_functionName`.
4. Add a shared-library rule to `native/Makefile` and the root `Makefile`.


