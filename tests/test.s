.data
# define class c1
c1:
	 .quad c1_a


.text
.globl c1

# method implementation of class c1

.type c1_a, @function
c1_a:

ret

