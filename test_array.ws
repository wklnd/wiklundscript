object Pilot {
  name = String
  rank = String
  score = Int
}

var ace = Pilot
ace.name = "Nova"
ace.rank = "captain"
ace.score = 900

var values = [1, "two", 3.5, true, ace, [4, 5], 0 - 7]

print(values)
print(values.length)
print(values[0])
print(values[1])
print(values[2])
print(values[3])
var crew = values[4]
print(crew.name)
print(values[5][1])
print(values[6])

if values[6] == -7
  print("negative indexing data is fine")
end

print(values[10])  # out of bounds should return null