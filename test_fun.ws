# test_fun.ws - demonstrates the `fun` alias for `func`

func greet name
  print("hello " + name)
end

func countdown max
  repeat max times as i
      print(i)
  end
end

greet("world")

countdown(10)