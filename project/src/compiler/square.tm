start = 0, _

if _
    reject
go right until 0
mark, go right, do first

first:
    go right until start
    if _
        accept
    else
        write X, go right, do cross

cross:
    go right until start
    if _
        go left until marked, do first
    go right until start
    if _
        reject
    else 
        write X, go right, do cross