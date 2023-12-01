var y : Int = 10

func foo(x x : Int) {
  let y = "hello" + "\n"
  if (y == "hello\n") {
    let y = 42
    write(y)
  } else {
    write(y)
  }
  write(y)
  
  var i = 0
  while i <= 10 {
    i = i + 1
    incrGlobal()
    write(i)
    write("Insert non-empty input for quiting, otherwise next iteration.\n")
    let x = readString()
    if let x {
      let inp = length(x)
      if (inp != 0) { write(x, "\n")
        return } else {}
    }
  }
}

func incrGlobal() { y = y + 1 }

foo(x: 7)
write(y)  // prints global y

// whitespaces and comment possible since now end-mark here
    
