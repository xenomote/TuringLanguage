states 9

next

carry1
mark1

carry0
mark0

returnN
returnY

accept +
reject -

alphabet 6 1 0 # X T F

next 1 carry1 X R
next 0 carry0 X R
next # accept # R

carry1 1 carry1 1 R
carry1 0 carry1 0 R
carry1 # carry1 # R
carry1 T mark1 1 R
carry1 F carry1 0 R
carry1 _ returnN _ L

carry0 1 carry0 1 R
carry0 0 carry0 0 R
carry0 # carry0 # R
carry0 F mark0 0 R
carry0 T carry0 1 R
carry0 _ returnN _ L

mark1 1 carry1 T R
mark1 0 carry1 F R
mark1 F carry1 F R
mark1 T mark1 T R
mark1 _ returnN _ L

mark0 1 carry0 T R
mark0 0 carry0 F R
mark0 T carry0 T R
mark0 F mark0 F R
mark0 _ returnN _ L

returnN 1 returnN 1 L
returnN 0 returnN 0 L
returnN T returnY T L
returnN F returnY F L

returnY 1 returnY 1 L
returnY 0 returnY 0 L
returnY T returnY T L
returnY F returnY F L
returnY # returnY # L
returnY X next X R