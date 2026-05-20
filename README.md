# wscript 

**Version:** 0.1.0

## Quick Start

### Running a Script

```bash
./wscript hello.ws
```

### Interactive REPL

```bash
./wscript    # Start the REPL
> print("Hello!")
> exit        # Quit
```


## Language Basics

### Comments

Use `#` to add comments:

```
# This is a comment
print("Hello")  # Comments can go at the end of a line
```

### Variables

Declare variables with `var` (mutable) or `const` (immutable):

```
var x = 10
const message = "Hello"

var name = "Alice"
name = "Bob"  # This works - var can be reassigned

const value = 42
value = 100   # ERROR - const cannot be changed
```

### Data Types

- **String** - Text enclosed in quotes: `"hello"`
- **Int** - Whole numbers: `42`, `0`, `-5`
- **Float** - Decimal numbers: `3.14`, `2.5`
- **Bool** - `true` or `false`

### Print

Output text to the console:

```
print("Hello, World!")
print(42)
print(3.14)
print(true)
```

---

## Operators

### Arithmetic

```
var a = 10 + 5    # 15
var b = 10 - 3    # 7
var c = 4 * 5     # 20
var d = 20 / 4    # 5.0
```

### Comparison

Returns `true` or `false`:

```
var result = 5 == 5      # true
var result = 5 != 3      # true
var result = 10 > 5      # true
var result = 3 < 10      # true
var result = 5 >= 5      # true
var result = 3 <= 10     # true
```

### String Concatenation

Use `+` to combine strings:

```
var greeting = "Hello" + " " + "World"
print(greeting)  # Output: Hello World
```

---

## Control Flow

### If / Else

Make decisions in your code:

```
var age = 18

if age >= 18
  print("You are an adult")
else
  print("You are a minor")
end
```

Nested conditions:

```
var score = 85

if score >= 90
  print("Grade: A")
else
  if score >= 80
    print("Grade: B")
  else
    print("Grade: C")
  end
end
```

---

## Loops

### Repeat N Times

Execute code a specific number of times with a counter:

```
repeat 5 times as i
  print("tests" + i)
end

# Output:
# tests0
# tests1
# tests2
# tests3
# tests4
```

Use the iterator in expressions:

```
repeat 5 as n
  var result = n * 2
  print(result)
end

# Output:
# 0
# 2
# 4
# 6
# 8
```

### Repeat While

Loop while a condition is true:

```
var count = 0

repeat while count < 3
  print(count)
  count = count + 1
end

# Output:
# 0
# 1
# 2
```

---

## Functions

Define reusable blocks of code:

```
func greet name
  print("Hello, " + name)
end

greet("Alice")
greet("Bob")
```

Functions with multiple parameters:

```
func add x y
  var sum = x + y
  print(sum)
end

add(5, 3)  # Output: 8
```

Return values from functions:

```
func multiply x y
  var result = x * y
  return result
end

var answer = multiply(6, 7)
print(answer)  # Output: 42
```

---

## Objects

Create structured data with blueprints:

```
object Person
  name String
  age Int
end

var alice = Person
alice.name = "Alice"
alice.age = 30

print(alice.name)  # Output: Alice
print(alice.age)   # Output: 30
```

Access object fields:

```
object Point
  x Int
  y Int
end

var p = Point
p.x = 10
p.y = 20

print(p.x)  # Output: 10
```
@@```
object Person {
  name = String
  age = Int
}

var alice = Person
alice.name = "Alice"
alice.age = 30

print(alice.name)  # Output: Alice
print(alice.age)   # Output: 30
```

Access object fields:

```
object Point {
  x = Int
  y = Int
}

var p = Point
p.x = 10
p.y = 20

print(p.x)  # Output: 10
```

---

## Examples

### Example 1: Counting to 10

```
repeat 10 as i
  print(i + 1)
end
```

### Example 2: Temperature Converter

```
func celsius_to_fahrenheit c
  var f = (c * 9) / 5 + 32
  return f
end

var temp_c = 25
var temp_f = celsius_to_fahrenheit(temp_c)
print(temp_c + "°C = " + temp_f + "°F")
```

### Example 3: Guessing Game (Print only)

```
var secret = 42
var guess = 40

if guess == secret
  print("You got it!")
else
  if guess < secret
    print("Too low!")
  else
    print("Too high!")
  end
end
```

### Example 4: Multiplication Table

```
func print_table num
  var i = 1
  repeat while i <= 10
    var result = num * i
    print(num + " x " + i + " = " + result)
    i = i + 1
  end
end

print_table(5)
```

### Example 5: Working with Objects

```
object Book {
  title = String
  author = String
  pages = Int
}

var mybook = Book
mybook.title = "The Hobbit"
mybook.author = "Tolkien"
mybook.pages = 310

print(mybook.title)
print("by " + mybook.author)
print(mybook.pages + " pages")

```

---

## Tips & Tricks

**String Concatenation**: Mix types freely with `+`
```
print("The answer is " + 42)  # Output: The answer is 42
```

**Nested Conditions**: Use multiple `if/else` blocks
```
if condition1
  if condition2
    print("Both are true")
  end
end
```

**Loop Tricks**: Use the iterator variable in calculations
```
repeat 10 as i
  print(i * i)  # Print squares: 0, 1, 4, 9, 16, ...
end
```

**Reusable Code**: Write functions to avoid repetition
```
func say msg
  print(">>> " + msg)
end

say("Hello")
say("World")
```

---

## Common Gotchas

❌ **Missing `end`** - All blocks need `end`
```
if true        # Missing end!
  print("hi")
```

❌ **Off-by-one in loops** - `repeat N` starts at 0
```
repeat 3 as i  # i = 0, 1, 2 (not 1, 2, 3)
  print(i)
end
```

❌ **Const can't be reassigned**
```
const x = 5
x = 10         # ERROR!
```

---

why? why not