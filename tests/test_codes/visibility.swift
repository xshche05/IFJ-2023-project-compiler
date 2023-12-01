var y : Int = 10  // global variable in main body

func foo(over x : Int) {  // void function (no return value)
  let y = "hello" + "\n"  // local variable, scope for entire function
  if (y == "hello\n") {
    let y = 42 // new local variable, scope only in this block
    write(y)        // prints 42
  } else {
    write(y)
  }
  write(y) // prints hello
  
  var i = 0
  while i <= 10 {
    i = i + 1   // local variable of the whole function is used 
    incrGlobal()
    write(i)
    write("Insert non-empty input for quiting, otherwise next iteration.\n")
    let x = readString()  // formal parameter behave in the function body as local variable, but cannot be modified, we can redefine it/hide it
    if let x {
      let inp = length(x)
      if (inp != 0) { write(x, "\n")
        return } else {}  // explicitly stops looping by exiting foo
    } else {}
  }
}

func incrGlobal() { y = y + 1 }

foo(over: 7)
write(y)  // prints global y

// whitespaces and comment possible since now end-mark here
    
