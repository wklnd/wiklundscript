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
`String`, `Int`, `Float`, `Bool`, `Array`

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

More info on these modules will come in the future.
See [native/README.md](native/README.md) for writing modules.

### I/O
```ws
import IO.readline

print("Hello, World!")
var input = readline("Prompt: ")
```

### Working with Objects
```ws
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
```ws
object Point {
  x = Int
  y = Int
}

var p = Point
p.x = 10
p.y = 20
print(p.x)  # Output: 10
```

### Arrays

```ws
var items = [1, "two", true, [3, 4]]
print(items)
print(items.length)
print(items[0])
print(items[3][1])
```

---

## Examples

### Example 1: Counting to 10
```ws
repeat 10 times as i
  print(i + 1)
end
```

### Example 2: Temperature Converter
```ws
func celsius_to_fahrenheit c
  var f = (c * 9) / 5 + 32
  return f
end

var temp_c = 25
var temp_f = celsius_to_fahrenheit(temp_c)
print(temp_c + "°C = " + temp_f + "°F")
```

### Example 3: Guessing Game
```ws
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
```ws
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
```ws
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
```ws
print("The answer is " + 42)  # Output: The answer is 42
```

**Nested Conditions**: Use multiple `if/else` blocks
```ws
if condition1
  if condition2
    print("Both are true")
  end
end
```

**Loop Tricks**: Use the iterator variable in calculations
```ws
repeat 10 times as i
  print(i * i)  # Print squares: 0, 1, 4, 9, 16, ...
end
```

**Reusable Code**: Write functions to avoid repetition
```ws
func say msg
  print(">>> " + msg)
end

say("Hello")
say("World")
```

---

## Common Gotchas

**Missing `end`** - All blocks need `end`
```ws
if true        # Missing end!
  print("hi")
```

**Off-by-one in loops** - `repeat N times` starts at 0
```ws
repeat 3 times as i  # i = 0, 1, 2 (not 1, 2, 3)
  print(i)
end
```

**Const can't be reassigned**
```ws
const x = 5
x = 10         # ERROR!
```

---

## Extension
There is a small extension to get syntax highlighting, don't expect too much.

Why? Why not.