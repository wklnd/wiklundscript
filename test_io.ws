import IO.readline
import String.*

print("--- IO Test ---")

var name = readline("Enter your name: ")
print("Hello, " + name)
name = doStringUppercase(name)
print("Your name in uppercase is: " + name)

var age = readline("Enter your age: ")
print("You are " + age + " years old")


