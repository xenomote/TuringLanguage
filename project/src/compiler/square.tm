start = 0, _
second = X, _


if _
    reject
go right until 0
mark, go right
do first


first:
    go right until start  
    if X
        accept
    or _
        reject
    else
        write Y, go right
        do cross

cross:
    go left until start
    if _
    	go right until marked
        do first
    go right until start
    if _
        reject
    else 
        write X, go right
        do cross