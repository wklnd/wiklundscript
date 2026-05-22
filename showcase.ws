import Time.*
import String.*
import Math.*
import Random.*

func section title
  print("")
  print("================================")
  print(doStringUppercase(title))
  print("================================")
end

object Pilot {
  name = String
  rank = String
  score = Int
  shields = Int
}

var pilot = Pilot
pilot.name = "Nova"
pilot.rank = "rookie"
pilot.score = 0
pilot.shields = 100

section("mission start")
print("date: " + getDate())
print("tomorrow: " + getTomorrow())
print("build mood: " + doStringCapitalize("wscript is alive"))
print("mirror signal: " + doStringReverse("orbit"))
print("identity length: " + getStringLength(pilot.name))
print("sleeping for a moment...")
sleep(0.1)
print("awake")

section("pilot profile")
print("name: " + pilot.name)
print("rank: " + pilot.rank)
print("score: " + pilot.score)
print("shields: " + pilot.shields)

if doStringEquals(pilot.name, "Nova")
  print("identity check: ok")
end

section("training run")
var trials = 3
repeat trials times as i
  var roll = getRandomInt(1, 6)
  pilot.score = pilot.score + roll
  pilot.shields = pilot.shields - 3
  print("trial " + (i + 1) + ": roll=" + roll + ", score=" + pilot.score + ", shields=" + pilot.shields)
end

section("signal sweep")
var pulse = 0
repeat while pulse < 3
  print("pulse " + (pulse + 1) + ": " + doStringUppercase("stable"))
  pulse = pulse + 1
end

section("math lab")
var angle = -3.7
var rounded = round(angle)
var floored = floor(angle)
var root = sqrt(49)
var wave = pow(2, 5)
var smallest = min(42, 7, 18, 3, 99)
var biggest = max(42, 7, 18, 3, 99)
var drift = abs(-12)

print("angle: " + angle)
print("rounded: " + rounded)
print("floored: " + floored)
print("sqrt(49): " + root)
print("2^5: " + wave)
print("min: " + smallest)
print("max: " + biggest)
print("abs(-12): " + drift)

section("final checksum")
var checksum = getStringLength(pilot.name) + pilot.score + rounded + floored + root + wave + smallest + biggest + drift
print("checksum: " + checksum)

if checksum > 0
  print("status: green")
else
  print("status: red")
end