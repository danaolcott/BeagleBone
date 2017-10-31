PCB:
Built using the original expressPCB, but the ording did not work.  
Therefore, opened the project file in new expressPCB Plus, 
remove the silkscreen layer, and order from there.  The 
"as submitted" file is in the backback_no_ss file.  

Submitted 10.21.17


CHANGES
_____________________________


10.30.17
Discovered buttonRight does not work due to conflict
with GPIO117.  Some limited research shows this pin is
used for the external oscillator output?

Change: Use GPIO61.  Add jumper from the via near center
button to GPIO61 pin (4 pin connector containing power, 
ground, pin61, pin.. ??).  Cut the trace at GPIO117 on the
bottom of the board.



