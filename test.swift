var a : Int? = 0
var b : Int? = 0

for i in a!/10...b!/10 {
    write(i)
}
write("\n")
for i in 0...5 {
    write(i)
}
write("\n")
for i in 0..<5 {
    write(i)
}
write("\n")
for i in 0...5 {
    if i == 3 {
        continue
    }
    write(i)
}
write("\n")
for i in 0...5 {
    if i == 3 {
        break
    }
    write(i)
}
write("\n")
for i in 0...5 {
    let i = i + 1
    write(i)
    for i in 0...5 {
        write(i)
    }
}
write("\n")

/*
0
012345
01234
01245
012
101234520123453012345401234550123456012345
*/