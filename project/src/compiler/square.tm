check = 0, _
second = X, _

mark, go right
go right until _
write X, go left 
go left until marked
accept

/*
if 0
    mark, go right
    do first
reject

// just marked/crossed first 0
first: 
    go right until check 

    // the only 0 seen was the first
    if _
        accept
    
    // continue to cross off 0s
    write X, go right
    do cross

// start on even
cross:
    // check for even number of 0s
    go right until check
    if _
    	go left until marked
        do first
    write X, go right
    go right until check
    if _
        reject
    write X, go right
    do cross
*/