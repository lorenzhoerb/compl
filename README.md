# Typed and Object-Oriented Language Compiler in C

In the "Compiler Construction" course I attended in 2023, the aim was to gain theoretical and practical understanding of compiler workings.

The practical aspect involved implementing a compiler in C for a typed, object-oriented programming language.

## Features
- Support for global variables
- Conditional statements
- Types: `int` and `object`
- Object-oriented features including classes with object variables and methods, and interfaces
- Numerical expressions with basic operations: `+`, `-`, `*`
- AMD64 assembler code generation

## Technologies Used

- Scanner implemented with `flex`
- Parser implemented with `bison`
- Attributed grammar with a symbol table using `ox`
- Code generation utilizing `iBurg`

## Challenges

- **Grammar Parsing:** Designing a parser that accurately captures the language syntax.
- **Type System:** Managing and enforcing type rules throughout the compilation process.
- **Memory Management:** Handling stack and heap memory efficiently.
- **Code Generation:** Generating optimized AMD64 assembler code from the parsed input.

## Example Code

### Example 1
```
int x(object);

class y
int x(object z)

object c <- new y;
object berta <- c;

return y end;

int b;

end;
 ```

### Example 2

```
int mul(object, int);
int pow2(object, int);
class c
 int age;

 int mul(object a, int t)
 return t * 2
 end;

 int pow2(object y, int n)
 return n + mul(y, n)
 end;

end;
```

### Example 3

```
int x(object, int);
class a
 int x(object y, int par)
 int k <- 1;
 cond
 k -> int z<- 1; par <- 2; break;
 -> int z<- 3; int r <- 4; continue;
 end;
 return 0
 end;
end;
```

### Example output

```asm
.data
# define class Person
Person:
 .quad Person_m1


.text
.globl Person

# method implementation of class Person

.type Person_m1, @function
Person_m1:
enter $16, $0

moveq $1, 0(%sbp)
moveq $14, 1(%sbp)
```