states 13

start
bracket
return
not

shuttleTR
shuttleFR

shuttleTL
shuttleFL

ignoreR
ignoreLS
ignoreLO

accept +
reject -

alphabet 11 T F & | ! ( ) [ { < X

start X start _ R
start T shuttleTR _ R
start F shuttleFR _ R
start ! not _ R
start ( bracket [ R

bracket X bracket X R
bracket ( bracket ( R
bracket T bracket T R
bracket F bracket F R
bracket | bracket | R
bracket & bracket & R
bracket ! bracket ! R
bracket ) return X L

return X return X L
return T shuttleTL X L
return F shuttleFL X L

not X not _ R
not T shuttleFR _ R
not F shuttleTR _ R
not ! start _ R
not ( bracket { R

shuttleTL X shuttleTL X L
shuttleTL | ignoreLS T L
shuttleTL & return X L
shuttleTL ! shuttleFL X L
shuttleTL ( bracket T R
shuttleTL [ shuttleTR _ R
shuttleTL { shuttleFR _ R
shuttleTL < ignoreR _ R

shuttleFL X shuttleFL X L
shuttleFL | return X L
shuttleFL & ignoreLS F L
shuttleFL ! shuttleTL X L
shuttleFL ( bracket F R
shuttleFL [ shuttleFR _ R
shuttleFL { shuttleTR _ R
shuttleFL < ignoreR _ R

shuttleTR X shuttleTR _ R
shuttleTR | ignoreR T R
shuttleTR & start _ R
shuttleTR _ accept T L

shuttleFR X shuttleFR _ R
shuttleFR | start _ R
shuttleFR & ignoreR F R
shuttleFR _ accept F L

ignoreLS X ignoreLS X L
ignoreLS T ignoreLO X L
ignoreLS F ignoreLO X L

ignoreLO X ignoreLO X L
ignoreLO ! ignoreLO X L
ignoreLO & ignoreLS X L
ignoreLO | ignoreLS X L
ignoreLO ( bracket X R 
ignoreLO [ start _ R
ignoreLO { not _ R
ignoreLO < ignoreR _ R

ignoreR ( bracket < R
ignoreR T ignoreR _ R
ignoreR F ignoreR _ R
ignoreR ! ignoreR _ R
ignoreR & ignoreR _ R
ignoreR | ignoreR _ R
ignoreR ) ignoreR _ R
ignoreR X ignoreR _ R
ignoreR _ accept _ L
