if _
    reject

go right until 0
write O, go right, do first

first:
    go right until 0 or _
    if _
        accept
    else
        write X, go right, do cross

cross:
    go right until 0 or _
    if _
        go left until O, do first
        
    go right until 0 or _
    if _
        reject
    else
        write X, go right, do cross
