object Book {
  title = String
  author = String
  pages = Int
  read = Bool
}

var mybook = Book
mybook.title = "The Hobbit"
mybook.author = "Tolkien"
mybook.pages = 310
mybook.read = true

print(mybook.title)
print("by " + mybook.author)
print(mybook.pages + " pages")
print("Read: " + mybook.read)


func say msg
    print(">>> " +msg)
end

say("Hello")

# End of the fun 