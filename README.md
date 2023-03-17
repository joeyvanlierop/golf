# The GoLF Language

This is a stack-based, multi-pass compiler written in C++ for a programming language called GoLF. This was created as part of a term project for a course at the University of Calgary (CPSC 411) taught by John Aycock. If you are reading this and he is still teaching the course, enroll in it. Right now. Seriously.

## Features

You can read through the whole specification [here](./SPECIFICATION.md). Otherwise, here are some short snippets of what you can do in GoLF:

- GoLF has built-in functions:

  - `printb(b bool)`: To print a boolean to the console,
  - `printc(c int)`: To print a character to the console,
  - `printb(i int)`: To print a number to the console,
  - `printb(s string)`: To print a string to the console,
  - `getchar()`: To receive user input,
  - `len(s string)`: To get the length of a string,
  - `halt()`: To halts the execution of the program,

- GoLF has support for arithmetic operators: `+`, `-`, `/`, `*`, `%`

- GoLF has support for comparison operators: `==`, `<`, `<=`, `>=`, `>`

- GoLF has support for binary operators: `&&`, `||`

- GoLF has support for unary operators: `!`, `-`

- GoLF supports the `break` statement in loops

## Building

In order to build and compile, you need to install `Make` plus the toolchain of your choice, e.g. `g++` or `clang`. After you setup `Make` with your toolchain of choice, clone the repository and build the project as follows:

```shell
git clone https://github.com/joeyvanlierop/golf.git
cd golf/
make
```

## Hello World

To run a "hello world" program, create a file named `hello-world.golf` on your computer, and place the following inside it:

```go
func main() {
    prints("Hello, world!\n")
}
```

Then, in a terminal window, navigate to the unzipped folder and run:

```shell
$ golf hello-world.golf
```

## The Classic Fibonacci Number Calculator:

```go
func main() {
    var i int
    i = 0

    // Anything larger than 47 overflows a 32-bit int...
    for i <= 47 {
        prints("fib(")
        printi(i)
        prints(") = ")
        printi(fib(i))
        prints("\n")
        i = i + 1
    }
}

func fib(n int) int {
    if n == 0 { return 0 }
    if n == 1 { return 1 }
    return fib(n-1) + fib(n-2)
}
```

<!-- ### A Little Greeting Loop

```go
while true {
    var name = input("Who are we greeting? ");
    print("Hello there, " + name + "!\n");

    if input("Greet again? (y/n): ") != "y" {
        break;
    }
}
``` -->

## Lifecycle of a GoLF Program

GoLF programs get executed in four separate steps: lexing, parsing, semantic analysis, and code generation.

- [**Lexing**](./src/lexer.cpp): The lexer matches strings in the source code and groups them into tokens.

- [**Parsing**](./src/parser.cpp): The parser organizes tokens into an abstract syntax tree of the source code.

- **Semantic Analysis**: (TODO)

- **Code Generation**: (TODO)
