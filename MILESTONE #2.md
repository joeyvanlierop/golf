# Milestone 1: GoLF Scanner

## What is this

The purpose of this first milestone is to implement a lexical analyzer (scanner) for GoLF. You can find my
implementation here: https://gitlab.cpsc.ucalgary.ca/joseph.vanlierop/cpsc-411

## Who am I

Name: Joseph (Joey) Van Lierop  
UCID: 30096849  
Email address: joseph.vanlierop@ucalgary.ca

## Where do I need feedback

In order to make the grammar work properly, I restructured it in such a way that it doesn't clearly match the original (
visually), but it does match the functionality (at least as far as I could tell from rigorous testing). Is this an
acceptable approach, or should I have tried to more closely restructure the original grammar?

Sorry if my question isn't super clear. I can try to clarify it during tutorial (T04)

## What grade have I earned

I would argue that I have earned an 8/8. Besides the objective evidence that I will point out below, I have also put in
a lot of work to match the reference compiler's output 1:1.

### Tool (milestone) properties

- Good error and warning messages output.
    - My error messages are inspired by rust. I not only include an error message, but also the exact location of the
      error itself. One bug I have found is that it doesn't properly align code indented by tabs, but I am working on a
      fix.
  ```shell
  --> ../test/parse.t0:2:9
    |
  2 |     if a; {
    |         ^ error: expected {, got ;
  ```
- Predictable:
    - I handle all legal input cases properly, and any unknown inputs are verbosely errored. After parsing, an AST is
      printed using proper indentation and any relevant information.

  ```shell
    program
        func @ (3, 0)
          newid [foo] @ (3, 5)
          sig
            formals
            typeid [$void]
          block
            funccall
              id [garbled] @ (4, 1)
              actuals
                id [a] @ (4, 9)
                id [b] @ (5, 2)
                id [c] @ (6, 2)
  ```

### Development practices

- Committing work regularly to your CPSC Gitlab repo
    - Around 40 commits were made over the past two weeks. To be honest, I crunched a little towards the end but I'd
      rather enjoy my reading break than hate my life. You can view the commit log
      here: https://gitlab.cpsc.ucalgary.ca/joseph.vanlierop/cpsc-411/-/commits/main
- Good commit messages
    - I tend to follow a commit message convention where the messages are imperative and simple. Sometimes I do a little
      trolling, but then again don't we all

### Code qualities

- Code is readable
    - The code was frequently refactored both manually and automatically. The manual refactoring involved removing
      unnecessary complications and duplication, while the automatic refactoring involved reformatting and adhering to
      modern style conventions.
- Code is modular and extensible
    - I split up code in order to adhere to the single-responsibility principle. Tokens, logging, lexing, parsing, and
      I/O are
      all handled individually which makes it very easy to refactor and implement new functionality. I also split up the
      parsing functionality in a way that I am quite proud of. Although it might be a little jumbled to read at first, I
      found it quite scalable and pleasant once I got used to it.
- Code is consistent
    - I configured my IDE to enforce my specific style conventions.
- Code is consistent with respect to the implementation language
    - I tried to follow as many C++ idioms as possible. No external libraries were used in this case.
- Code is well documented
    - The important aspects of the project are well documented in a consistent structure.
- Speed
    - The code executes the test cases just as fast as the reference compiler
- Easy to build, and building without errors or warnings (ok there are 2 warnings, but they are only there because the compiler doesn't have fancy static analysis)
  ```shell
    joseph.vanlierop@csx2:~/Documents/cpsc-411$ make clean && make
    rm *.o golf
    g++ -c src/golf.cpp
    g++ -c src/lexer.cpp
    g++ -c src/token.cpp
    g++ -c src/logger.cpp
    g++ -c src/filereader.cpp
    g++ -c src/ast.cpp
    g++ -c src/parser.cpp
    src/parser.cpp: In member function ‘Token Parser::consume(TokenType)’:
    src/parser.cpp:62:1: warning: control reaches end of non-void function [-Wreturn-type]
       62 | }
          | ^
    src/parser.cpp: In member function ‘AST* Parser::operand()’:
    src/parser.cpp:513:1: warning: control reaches end of non-void function [-Wreturn-type]
      513 | }
          | ^
    g++ -g golf.o lexer.o token.o logger.o filereader.o ast.o parser.o -o golf
  ```

### Relationship with the environment.

- Consistent with the environment
    - Warnings and errors are sent to `stderr`, while all other output its sent to `stdout`.
- No extraneous garbage in the output
    - Although I am formatting the error messages, no escape codes are added and the actual formatting is quite simple.
- Easy to run
    - The executable is very simple to run. All that needs to be provided is the file to parse:
  ```
  $ ./golf test/parse.t7
  ```
- Meta-documentation
    - A README is included in the root of the repository.
