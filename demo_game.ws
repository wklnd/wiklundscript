import Graphics

window(800, 500, "wscript ball demo")
fps(144)

x = 400
y = 250
vx = 4
vy = 3
size = 24

while running()
  clear("black")

  x = x + vx
  y = y + vy

  if x < size
    vx = 0 - vx
    x = size
  if x > 800 - size
    vx = 0 - vx
    x = 800 - size
  if y < size
    vy = 0 - vy
    y = size
  if y > 500 - size
    vy = 0 - vy
    y = 500 - size

  circle(x, y, size, "red")
  text("press escape to quit", 10, 10, 18, "gray")

  if keyDown("escape")
    vx = 0
    vy = 0

  update()
