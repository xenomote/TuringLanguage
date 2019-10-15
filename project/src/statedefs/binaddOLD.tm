states 20

init
shift1
shift0
shift#
back

identify
seek1
seek0

sub1
sub0
carry
return

check
match1
match1?
match0
match0?

endR

accept +
reject -

alphabet 8 1 0 # X t f T F

init 1 shift1 _ R
init 0 shift0 _ R
init # shift# _ R

shift1 1 shift1 1 R
shift1 0 shift0 1 R
shift1 # shift# 1 R
shift1 _ back 1 R
shift0 1 shift1 0 R
shift0 0 shift0 0 R
shift0 # shift# 0 R
shift0 _ back 0 R
shift# 1 shift1 X R
shift# 0 shift0 X R
shift# # shift# X R
shift# _ back X R

back 1 back t L
back 0 back f L
back _ back _ L
back X return # L

identify 1 seek1 X R
identify 0 seek0 X R
identify # check X R

seek1 1 seek1 1 R
seek1 0 seek1 0 R
seek1 # sub1 # R

seek0 1 seek0 1 R
seek0 0 seek0 0 R
seek0 # sub0 # R

sub1 T sub1 T R
sub1 F sub1 F R
sub1 t return F L
sub1 f carry T R

carry t return f L
carry f carry t R

sub0 T sub0 T R
sub0 F sub0 F R
sub0 t return T L
sub0 f return F L
sub0 _ return _ L

return T return T L
return F return F L
return t return t L
return f return f L
return # return # L
return 1 return 1 L
return 0 return 0 L
return X identify X R 

check X check X R
check 1 check 1 R
check 0 check 0 R
check T match1 X L
check t match1 X L
check F match0 X L
check f match0 X L
check _ endR _ L

match1 X match1 X L
match1 1 match1 1 L
match1 0 match1 0 L
match1 _ match1? _ R

match1? 1 check _ R

match0 X match0 X L
match0 1 match0 1 L
match0 0 match0 0 L
match0 _ match0? _ R

match0? 0 check _ R
match0? X check X R

endR X endR X L
endR 0 endR X L
endR _ accept _ R