states 9

start

mark1
mark0

return

check

match1
match0

accept +
reject -

alphabet 5 1 0 X T F

start _ accept _ R
start 1 mark1 _ R
start 0 mark0 _ R

mark1 1 mark1 T R
mark1 0 mark1 F R
mark1 1 return X L

mark0 1 mark0 T R
mark0 0 mark0 F R
mark0 0 return X L

return 1 return 1 L
return 0 return 0 L
return X return X L
return T return T L
return F return F L
return _ check _ R

check _ accept _ R
check X check _ R
check T match1 _ R
check F match0 _ R

match1 X match1 X R
match1 T match1 T R
match1 F match1 F R
match1 1 return X L

match0 X match0 X R
match0 T match0 T R
match0 F match0 F R
match0 0 return X L