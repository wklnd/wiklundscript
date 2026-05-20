import String.getStringLength
import String.doStringConcat
import String.doStringReverse

var text = "Hello, World!"
var reversed = doStringReverse(text)
var combined = doStringConcat("Hello", " ", "WS")
var len = getStringLength(text)

print("Original: " + text)
print("Reversed: " + reversed)
print("Combined: " + combined)
print("Length: " + len)


if len == 13 
    print("The text is 13 characters long.")
else 
    print("The text is not 13 characters long.")
endss