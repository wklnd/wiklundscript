import Math.*

var whole = floor(3.9)
var root = sqrt(16)
var roll = random()
var magnitude = abs(-12)
var rounded = round(3.8)
var power = pow(2, 3)
var smallest = min(5, 2, 9, 1)
var largest = max(5, 2, 9, 1)

print(whole)
print(root)
print(roll)
print(magnitude)
print(rounded)
print(power)
print(smallest)
print(largest)

if whole == 3
  print("floor ok")
end

if root == 4
  print("sqrt ok")
end

if roll >= 0
  if roll < 1
    print("random ok")
  end
end

if magnitude == 12
  print("abs ok")
end

if rounded == 4
  print("round ok")
end

if power == 8
  print("pow ok")
end

if smallest == 1
  print("min ok")
end

if largest == 9
  print("max ok")
end