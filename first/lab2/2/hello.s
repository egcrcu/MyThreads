.data

msg:
  .ascii "Hello, World!\n"
  
.text
  .global _start
  
_start:
  movq $1, %rax # номер системного вызова
  movq $1, %rdi # файловый дескриптор
  movq $msg, %rsi # переменная msg со строкой
  movq $15, %rdx # длина строки
  syscall
  
  movq $60, %rax # номер системного вызова
  xorq %rdi, %rdi # зануление %rdi
  syscall
  # exit(0)
