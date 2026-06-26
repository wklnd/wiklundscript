import Time.*
import String.*
import Math.*
import Random.*
import color.*

func section title
  print("")
  print("================================")
  print(doStringUppercase(title))
  print("================================")
end

import Time.*
import String.*
import Math.*
import Random.*
import color.*

func section title
  print("")
  print(colorize("================================", "dim"))
  print(colorize(">>> " + doStringUppercase(title) + " <<<", "bold"))
  print(colorize("================================", "dim"))
end

object ConsoleDemo {
  app = String
  build = String
  mode = String
  latency = Float
  packets = Int
  errors = Int
}

var demo = ConsoleDemo
demo.app = "wscript"
demo.build = "2026.06"
demo.mode = "tech demo"
demo.latency = 12.4
demo.packets = 0
demo.errors = 0

section("boot sequence")
print(colorize("> loading modules...", "cyan"))
print(colorize("> time api: ready", "green"))
print(colorize("> string api: ready", "green"))
print(colorize("> math api: ready", "green"))
print(colorize("> random api: ready", "green"))
print(colorize("> color api: glowing", "magenta"))
print(colorize("> renderer: ANSI terminal", "yellow"))
print(colorize("> theme: neon control room", "blue"))
print(colorize("> build: " + demo.build, "white"))
print(colorize("> app: " + demo.app, "cyan"))
sleep(0.1)
print(bold(green("boot complete")))
print(colorize("boot complete", "underline"))

print(colorize("=== palette smoke test ===", "bold"))
print(colorize("red", "red"))
print(colorize("green", "green"))
print(colorize("yellow", "yellow"))
print(colorize("blue", "blue"))
print(colorize("magenta", "magenta"))
print(colorize("cyan", "cyan"))
print(colorize("white", "white"))
print(colorize("black", "black"))
print(colorize("dim", "dim"))
print(colorize("underline", "underline"))
print(colorize("bold", "bold"))

section("live dashboard")
print(colorize("mode: " + demo.mode, "cyan"))
print(colorize("status: online", "green"))
print(colorize("status light: bright", "yellow"))
print(colorize("transport: stable", "blue"))
print(colorize("console name: " + doStringCapitalize(demo.app), "magenta"))
print(colorize("session id length: " + getStringLength(demo.build), "white"))
print(colorize("echo test: " + doStringReverse("packet"), "dim"))

if doStringEquals(demo.app, "wscript")
  print(colorize("app check: ok", "green"))
  print(colorize("app check: extra crisp", "bold"))
end

section("throughput loop")
var frames = 3
repeat frames times as i
  var tick = getRandomInt(10, 99)
  demo.packets = demo.packets + tick
  demo.latency = demo.latency + 0.5
  print(colorize("frame " + (i + 1) + ": packets=" + demo.packets + ", tick=" + tick, "cyan"))
  print(colorize("frame " + (i + 1) + ": latency=" + demo.latency, "yellow"))
  print(colorize("frame " + (i + 1) + ": vibing", "magenta"))
end

section("string lab")
print(colorize("reverse: " + doStringReverse("terminal"), "blue"))
print(colorize("capitalize: " + doStringCapitalize("neon console"), "green"))
print(colorize("uppercase: " + doStringUppercase("signal strong"), "yellow"))
print(colorize("lowercase: " + doStringLowercase("LOUD OUTPUT"), "cyan"))
print(colorize("concat: " + doStringConcat("tech", " ", "demo", " ", "mode"), "magenta"))
print(colorize("length: " + getStringLength("hardware"), "white"))

section("math benchmark")
var angle = -3.7
var rounded = round(angle)
var floored = floor(angle)
var root = sqrt(49)
var wave = pow(2, 5)
var smallest = min(42, 7, 18, 3, 99)
var biggest = max(42, 7, 18, 3, 99)
var drift = abs(-12)

print(colorize("angle: " + angle, "cyan"))
print(colorize("rounded: " + rounded, "green"))
print(colorize("floored: " + floored, "yellow"))
print(colorize("sqrt(49): " + root, "magenta"))
print(colorize("2^5: " + wave, "blue"))
print(colorize("min: " + smallest, "white"))
print(colorize("max: " + biggest, "red"))
print(colorize("abs(-12): " + drift, "dim"))
print(colorize("math benchmark: finished", "bold"))

section("final readout")
var checksum = getStringLength(demo.app) + demo.packets + rounded + floored + root + wave + smallest + biggest + drift
demo.errors = 0

print(colorize("packets: " + demo.packets, "cyan"))
print(colorize("latency: " + demo.latency, "yellow"))
print(colorize("errors: " + demo.errors, "green"))
print(colorize("checksum: " + checksum, "white"))

if checksum > 0
  print(colorize("status: good", "green"))
  print(colorize("status: good", "bold"))
  print(colorize("demo vibe: polished", "magenta"))
else
  print(colorize("status: bad", "red"))
  print(colorize("status: bad", "bold"))
  print(colorize("demo vibe: loud", "yellow"))
end

print(colorize("tech demo complete", "bold"))
print(colorize("tech demo complete", "cyan"))
print(colorize("tech demo complete", "magenta"))