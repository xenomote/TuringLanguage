check = 0, _

mark, go right
go right until _
write X, go left
if marked
    reject
else
    go left until marked
    unmark, go right
    go right until _
    accept

/*
second = X, _

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
    write x, go right
    do cross

// start on even
cross:
    // check for even number of 0s
    go right until check
    if _
    	go left until marked
        do first
    go right

    //check for an odd number of 0s
    go right until check
    if _
        reject
    write X, go right
    do cross
*/