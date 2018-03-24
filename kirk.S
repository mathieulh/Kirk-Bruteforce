## Sample code to measure latency of end indication by watching interrupt flag
   .set noreorder
   .set noat

   .section .text, "ax", @progbits
   .ent   dlap2
   .globl   dlap2

## uses buf_src, buf_dest 
## return v0, db_lap
##   v0:result value same as pre-IPL
##   db_lap:latency of process end indication (interrupt flag)
## unsigned int dlap2();
## extern unsigned int db_lap;
dlap2:
   sync
   nop
   sync
   nop

   ## initialize
   lui   $t0, 0xbde0
   li   $v0, 0x1            ## command code
   sw   $v0, 0x10($t0)      ## function code
   li   $v1, 0x1fffffff
   la   $v0, buf_src
   and   $v0, $v1
   sw   $v0, 0x2c($t0)      ## source buffer
   la   $v0, buf_dest
   and   $v0, $v1
   sw   $v0, 0x30($t0)      ## destination buffer

   ## start decrypt procedure
   li   $v0, 1
   sw   $v0, 0x0c($t0)      ## start

   ## measure latency
   li   $t1, 0         ## counter
   lw   $v1, 0x1c($t0)
dlap2_loop:
   andi   $v1, 0x0011
   addiu   $t1, 1
   beqz   $v1, dlap2_loop      ## loop until end indication
   lw   $v1, 0x1c($t0)

   la   $v0, db_lap
   sw   $t1, ($v0)      ## store counter

   andi   $v0, $v1, 0x0010   ## unknown rare hardware error indication?? usually zero
   bnez   $v0, dlap2_1
   sw   $v1, 0x28($t0)      ## clear interrupt status
   b   dlap2_fin
   lw   $v0, 0x14($t0)      ## return value

dlap2_1:
   li   $v0, 0x02
   sw   $v0, 0x0c($t0)      ## unknown command
dlap2_loop2:
   lw   $v1, 0x1c($t0)
   andi   $v1, 0x0002
   beqz   $v1, dlap2_loop2
   nop
   sw   $v1, 0x28($t0)      ## clear status
   b   dlap2_fin
   li   $v0, -1         ## return -1

dlap2_fin:
   jr   $ra
   nop

   .end   dlap2

   .section .sbss, "wa"
   .align   6      ## 0x40
   .globl   buf_src
   .globl   buf_dest
   .extern   db_lap
buf_src:   .space   0x1000
buf_dest:   .space   0x1000
