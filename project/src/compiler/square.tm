if _
    reject
go right until 0 
mark, go right, do first

first:
    go right until 0 or _
    if _
        accept
    else
        write X, go right, do cross
  accept
cross:
    go right until 0 or _
    if _
        go left until marked, do first    
    go right until 0 or _
    if _
        reject
    else 
        write X, go right, do cross