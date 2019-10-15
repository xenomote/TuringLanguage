states 15

first

shuttleA1
shuttleA0

second1
second0

shuttleB2
shuttleB1
shuttleB0

match2
match1
match0

carry
return

accept +
reject -

alphabet 6 1 0 C X # ~

first 1 shuttleA1 _ R
first 0 shuttleA0 _ R
first # second0 ~ R

shuttleA0 1 shuttleA0 1 R
shuttleA0 0 shuttleA0 0 R
shuttleA0 # second0 # R

shuttleA1 1 shuttleA1 1 R
shuttleA1 0 shuttleA1 0 R
shuttleA1 # second1 # R

second0 X second0 X R
second0 1 shuttleB1 X R
second0 0 shuttleB0 X R
second0 # match0 # R

second1 X second1 X R
second1 1 shuttleB2 X R
second1 0 shuttleB1 X R
second1 # match1 # R

shuttleB0 1 shuttleB0 1 R
shuttleB0 0 shuttleB0 0 R
shuttleB0 # match0 # R

shuttleB1 1 shuttleB1 1 R
shuttleB1 0 shuttleB1 0 R
shuttleB1 # match1 # R

shuttleB2 1 shuttleB2 1 R
shuttleB2 0 shuttleB2 0 R
shuttleB2 # match2 # R

match0 X match0 X R
match0 0 return X L
match0 _ accept _ L

match1 X match1 X R
match1 1 return X L

match2 X match2 X R
match2 1 carry C R
match2 0 return C R

carry 0 carry 1 R
carry 1 return 0 L

return C carry X R
return # return # L
return X return X L
return 1 return 1 L
return 0 return 0 L
return _ first _ R
return ~ second0 ~ R