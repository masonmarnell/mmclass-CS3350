.section .text
.global extract_fields
.type extract_fields @function


extract_fields:

   and x6, x0, x0       //set x6 to 0
   beq x11, x6, OPCODE  //if mode==0, jump
   addi x6, x6, 1       //set x6 to 1
   beq x11, x6, RD      //if mode==1, jump
   addi x6, x6, 1       //set x6 to 2
   beq x11, x6, RS1

   ret

   OPCODE:
   and x7, x0, x0           //clearing x7
   addi x7, x7, 0b1111111   //pretty sure we can use binary like this
   and x10, x10, x7         //ANDing with just the opcode

   ret
   RD:
   and x7, x0, x0           //clearing x7

   //lui x7, 1                    //Ended up not working
   //addi x7, x7, 0b11111000000   //Ended up not working
   li x7, 0b111110000000          //using this pseudoinstruction solved my issue

   and x10, x10, x7               //ANDing with just the rd
   srli x10, x10, 7              //Shifting to get rid of the 0s

   ret
   RS1:
   and x7, x0, x0           //clearing x7
   lui x7, 0b11111000       //loading 1s into the rs1 field position
   and x10, x10, x7         //ANDing with just the rs1
   srli x10, x10, 15        //Shifting to get rid of the 0s

   ret
