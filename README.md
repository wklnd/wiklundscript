# wscript
A simple scripting language with native module support.

## Usage
```bash
./wscript script.ws       # Run a script
./wscript                 # Start interactive REPL
```

## Syntax

### Variables
```ws
var x = 10          # Mutable
const pi = 3.14     # Immutable
```

### Types

String, Int, Float, Bool

### Operators

Arithmetic: `+`, `-`, `*`, `/`  
Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`  
String concatenation: `+`

### Control Flow

```ws
if condition
  # ...
else
  # ...
end

repeat 5 times as i
  # ...
end

repeat while condition
  # ...
end
```

### Functions

```ws
func add x y
  return x + y
end

var result = add(3, 5)
```

### Objects

```ws
object Point {
  x = Int
  y = Int
}

var p = Point
p.x = 10
p.y = 20
```

### Native Modules

Import C libraries:
```ws
import IO.readline
import String.doStringReverse
import Random.getRandomInt
import Math.floor
import Time.getDate
import Time.sleep

var name = readline("Name: ")
var reversed = doStringReverse(name)
var number = getRandomInt(1, 100)
var whole = floor(3.9)
var today = getDate()
sleep(1.1)
print("the pause is over")
```

More info on these modules / libraries will come in the feature. 

See [native/README.md](native/README.md) for writing modules.

### I/O

```ws
import IO.readline

print("Hello, World!")
var input = readline("Prompt: ")
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
var guessText = readline("Guess the secret number: ")
var guess = parseInt(guessText)

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

## Extension. 
there is a small extension to get syntax highlighting, don't expect to much.

why? why not