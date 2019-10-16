states 19

next

add
sub
addP
subP
nez

inc
dec
incP
decP
cmp

loop

stop
seekL
seekR
jump

return

accept +
reject -

alphabet 18 [ ( { 1 0 # ~ + a - s > l < r } ] )

next + add a R
next - sub s R
next > addP r R
next < subP l R
next [ nez ( R
next ] loop ] L
next # accept # R
next ~ accept ~ R

add + add + R
add - add - R
add ] add ] R
add [ add [ R
add < add < R
add > add > R
add 1 add 1 R
add 0 add 0 R
add ~ add ~ R
add # inc # R
add _ inc # R

sub + sub + R
sub - sub - R
sub ] sub ] R
sub [ sub [ R
sub < sub < R
sub > sub > R
sub 1 sub 1 R
sub 0 sub 0 R
sub ~ sub ~ R
sub # dec # R
sub _ reject # R

addP + addP + R
addP - addP - R
addP ] addP ] R
addP [ addP [ R
addP < addP < R
addP > addP > R
addP 1 addP 1 R
addP 0 addP 0 R
addP ~ addP ~ R
addP # incP ~ R
addP _ return # L

subP + subP + R
subP - subP - R
subP ] subP ] R
subP [ subP [ R
subP < subP < R
subP > subP > R
subP 1 subP 1 R
subP 0 subP 0 R
subP ~ subP ~ R
subP # decP ~ L
subP _ return # L

nez + nez + R
nez - nez - R
nez > nez > R
nez < nez < R
nez ] nez ] R
nez [ nez [ R
nez 1 nez 1 R
nez 0 nez 0 R
nez ~ nez ~ R
nez # cmp # R

cmp 1 return 1 L
cmp 0 cmp 0 R
cmp ~ stop ~ L
cmp _ stop _ L

inc 1 inc 0 R
inc 0 return 1 L
inc _ return 1 L
inc ~ reject ~ L

dec 0 dec 1 R
dec 1 return 0 L
dec _ reject _ L
dec ~ reject ~ L

incP 1 incP 1 R
incP 0 incP 0 R
incP ~ return # L

decP 1 decP 1 L
decP 0 decP 0 L
decP ~ return # L

loop + loop + L
loop - loop - L
loop > loop > L
loop < loop < L
loop 1 loop 1 L
loop 0 loop 0 L
loop # loop # L
loop [ loop [ L
loop ] loop ] L
loop ( nez ( R

stop + stop + L
stop - stop - L
stop > stop > L
stop < stop < L
stop 1 stop 1 L
stop 0 stop 0 L
stop # stop # L
stop ~ stop ~ L
stop [ stop [ L
stop ] stop ] L
stop ( seekR ( R

seekR [ seekR [ R
seekR + seekR + R
seekR - seekR - R
seekR > seekR > R
seekR < seekR < R
seekR 1 seekR 1 R
seekR 0 seekR 0 R
seekR # seekR # R
seekR { seekR { R
seekR } seekR } R
seekR ) seekR ] R
seekR ] seekL ) L

seekL + seekL + L
seekL - seekL - L
seekL > seekL > L
seekL < seekL < L
seekL 1 seekL 1 L
seekL 0 seekL 0 L
seekL # seekL # L
seekL { seekL { L
seekL } seekL } L
seekL ] seekL } L
seekL [ seekR { R
seekL ( jump [ R

jump + jump + R
jump - jump - R
jump > jump > R
jump < jump < R
jump { jump [ R
jump } jump ] R
jump ) next ] R

return 1 return 1 L
return 0 return 0 L
return # return # L
return ~ return ~ L
return + return + L
return - return - L
return < return < L
return > return > L
return ] return ] L
return [ return [ L
return ( next ( R
return a next + R
return s next - R
return l next < R
return r next > R
