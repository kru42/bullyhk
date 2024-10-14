.386
.model flat, c

wrapper_load PROTO C

.CODE
  shim_crash PROC
  int 3
  nop; nop; nop;
  shim_crash ENDP

MAKE_SHIM macro name, ordinal
  .DATA
  PUBLIC shim_p_&name&
  shim_p_&name& DWORD shim_crash
ENDM
INCLUDE shims.inc
PURGE MAKE_SHIM

MAKE_SHIM macro name, ordinal
  .CODE
  shim_l_&name& PROC
    pushad
    call wrapper_load
    popad
    jmp shim_p_&name&
  shim_l_&name& ENDP
ENDM
INCLUDE shims.inc
PURGE MAKE_SHIM

MAKE_SHIM macro name, ordinal
  .CODE
  shim_h_&name& PROC
    jmp shim_p_&name&
  shim_h_&name& ENDP
ENDM
INCLUDE shims.inc
PURGE MAKE_SHIM

END