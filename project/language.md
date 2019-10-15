This project will work to define a language which will compile down to the simple state definition format as specified in CS3052 ("state def")

# common issues
there are several common tedious problems encountered when writing in state def, which have trivial or idiomatic solutions. While it is important to fully specify the transition model for the machine, they add little to the actual understanding of the algorithm aside from identifying the pattern used as a whole.

## shuttling
Shuttling occurs when the tape head needs to be placed over a specific position or symbol. The two types are slightly different but are common tedious idioms in state def.

- ### seeking
    this is a case of shuttling where the head must be placed over a specific marker symbol. This is commonly done with a single state that loops to itself for all symbols except the marker, at which point it will cease looping.

- ### positioning
    this is a case of shuttling where the head must be placed a specific number of symbols relative to the current position. The only way to do this is to have a series of states to count out the positions until the head is positioned correctly.

## symbol grouping
Symbol grouping is the case where a group of symbols are used exactly the same way in one or more states. For example, a group of marked symbols may be ignored in several states, or a group of data symbols may be read in one state or erased in another. Currently in state def the transition for each symbol in each state must be specified explicitly, which becomes massively tedious. Therefore it would be useful to refer to a group of symbols with a single name.

- ### groups
    sometimes there is a specific group of symbols which may be grouped logically based on their function, and they may share the same transition in several states, so it would be useful to refer to them as one group.

- ### negated groups
    sometimes there is a specific group of symbols which a state is concerned with, where all other states have the same default behaviour, so it would also be useful to refer to all the symbols not in a group.

- ### negated symbols
    sometimes one specific symbol has a specific behaviour for a given state, where all others have some shared behaviour. In these cases it would be useful to refer to the group of all symbols excluding the given symbol.

- ### all symbols
    sometimes all symbols are treated the same way by a state, so it would be useful to either refer to all symbols explicitly or to have such cases be handled implicitly.

## writing strings
Writing a string of symbols in state def requires defining a state for each symbol in the string and chaining them together, mapping all input symbols to the next state in order to overwrite the tape. It would be useful to simply specify a string of symbols to write out.

## shifting
Shifting happens when a block of symbols must be moved to the left or right as a unit. In state def to achieve this requires a state for every kind of symbol to shift, with a fully connected graph of transitions between them to store the symbol to shift from one position to the next.

# Language

the language used to solve these issues must provide language structures for common idioms, as well as the ability to label common structures within the program.

## basic expressions
the expression for a transition should include some higher level capabilities to abstract over the more time consuming issues mentioned above

### normal expression
a normal transition should specify the symbol to write, the direction to travel and the next state to transfer to:
```
write x, go left, do check
```

### implicit write
it is common to simply replace the symbol just read and move along. Therefore the write may be dropped, meaning the existing symbol is placed back:

```
go left, do check
```

### implicit transition
it is common to need to perform several simple steps sequentially, which in state sef requires a new fully defined state and transition table for each step.
To solve this problem, the transition section may be dropped to form a chain of expressions with only a final transition needed:
```
write x, go right
go right
write y, go right
go right, do check
```

### string write
similar to the above, writing out a string requires several steps and therefore as many states as characters to write. This can be covered by the following syntax:
```
write xyzxyzxyz, go right, do check
```
or for writing the string out backwards (right to left):
```
write xyzxyzxyz backwards, go left, do check
```
In both cases the string write occurs first and leaves the head on the last symbol of the string, and then any subsequent head repositioning and transitions occur. Additionally, multiple repetitions of symbols or strings can be written both forwards and backwards:
```
write x 3 times, go right               // results in xxx
write xyz 3 times, go right             // results in xyzxyzxyz
write xyz backwards 3 times, go right   // results in zyxzyxzyx
```

### seeking and filling
seeking as described above simply requires a direction and an exit condition. It will perform the write step if specified after each move, which can be used to overwrite the tape, as shown below: 
```
go left until x, do check                   // seeks left leaving tape untouched
write y, go left until x, do check          // fills the tape with y
write xyzxyzxyz, go left until q, do check  // repeatedly writes the string
```

### head repositioning
to reposition the head at a constant number of symbols away from the current position, use the following syntax:
```
go right 10 times, do check
```

### symbol groups
sometimes it is useful to refer to an arbitrary group of symbols in multiple places, so it is useful to define such groupings at a global scope
```
data = a b c d e f g
markers = A B C D E F G
```
these groupings can then be used in the conditionals as described below. They may also be defined using the conditional union, complement and intersection as defined later
```
ignored = in data and in markers
special = in data and not in markers
```

### conditionals
at each step, the current symbol can be referred to implicitly in a conditional statement. The conditionals allow branching behaviour based on the current symbol, beginning a new expression block:
```
if x
    write y, go right, do check
```
conditionals may also be chained together, where all conditionals must be exclusive
```
if x
    go left
    write y, go right, do check
or y
    write z, go left, do reset
or z
    go left
```
groupings may be used in conditionals as suggested above
```
if in markers
    write y, go left
or in data
    go right
    write x, go right
```

### negations
all conditional expressions may be negated with the not keyword to express the complement of the conditional group
```
if not x
    go right
or not in markers
    go left
```

### unions
conditional expressions may be joined together using the or keyword to recognise the union of the two smaller conditional groups
```
if in markers or x
    go right
or not in markers or y
    go left
```

### intersections
conditional expressions may be joined together using the and keyword to recognise the intersection of the two smaller conditional groups
```
if in markers and in data
    go right, do check
```

### blocks
all expressions are grouped together into larger named blocks which are referred to by the label at the beginning in order to transition to the block from other parts of the machine