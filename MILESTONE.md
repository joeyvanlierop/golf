# Milestone 1: GoLF Scanner

## What is this

The purpose of this first milestone is to implement a lexical analyzer (scanner) for GoLF. You can find my
implementation here: https://gitlab.cpsc.ucalgary.ca/joseph.vanlierop/cpsc-411

## Who am I

Name: Joseph (Joey) Van Lierop  
UCID: 30096849  
Email address: joseph.vanlierop@ucalgary.ca

## Where do I need feedback

I am wondering if the project as a whole is structured well. I am a bit concerned that some files (specifically the
lexer.* files) are too convoluted, and I am wondering if I should try to extract some functionality to other classes.
Maybe the specific matching functions (`number()`, `identifier()`, etc) should be moved to a different location?

## What grade have I earned

I would argue that I have earned an 8/8. Besides the objective evidence that I will point out below, I have also
investigated far too many sources about lexical analysis. I have read through every medium article, university lecture,
and free course that I could find. Every time that I search up some variation of "lexical analysis", the entirety of my
search results are purple. But anyways, onto the evidence:

### Tool (milestone) properties

- Good error and warning messages output.
    - My error messages are inspired by rust. I not only include an error message, but also the exact location of the
      error itself.
  ```shell
  --> ../test/scan.t24:3:16
    |
  3 |     var test = "abc
    |                ^~~~~ error: string contains newline
  ```
- Predictable:
    - I handle all legal input cases properly, and any unknown inputs are verbosely ignored. All non-ascii characters
      are skipped and not printed to the output to avoid any text decoding errors.

  ```shell
  --> ../test/scan.t7:1:1
    |
  1 | ?
    | ^ warning: skipping unknown character '?'
  
  --> ../test/scan.t7:2:1
    |
  2 | $
    | ^ warning: skipping unknown character '$'
  
  var      [var] @ (4, 0)
  id       [a] @ (4, 4)
  =        [=] @ (4, 6)
  string   [testing] @ (4, 9)
  ;        [] @ (4, 21)
  eof      [] @ (6, 0)
  ```

### Development practices

- Committing work regularly to your CPSC Gitlab repo
    - Around 40 commits were made over the past two weeks. The project was frequently iterated on, and there was no need
      for last minute crunch. You can view the commit log
      here: https://gitlab.cpsc.ucalgary.ca/joseph.vanlierop/cpsc-411/-/commits/main
- Good commit messages
    - I tend to follow a commit message convention where the messages are imperative and simple.

### Code qualities

- Code is readable
    - The code was frequently refactored both manually and automatically. The manual refactoring involved removing
      unnecessary complications and duplication, while the automatic refactoring involved reformatting and adhering to
      modern style conventions.
- Code is modular and extensible
    - I split up code in order to adhere to the single-responsibility principle. Tokens, logging, lexing, and I/O are
      all handled individually which makes it very easy to refactor and implement new functionality.
- Code is consistent
    - I configured my IDE to enforce my specific style conventions.
- Code is consistent with respect to the implementation language
    - I tried to follow as many C++ idioms as possible. No external libraries were used in this case.
- Code is well documented
    - The important aspects of the project are well documented in a consistent structure.
- Speed
    - The code executes the test cases just as fast as the reference compiler
- Easy to build, and building without errors or warnings
  ```shell
  joseph.vanlierop@csx:~/Documents/cpsc-411$ make clean && make
  rm *.o golf
  g++ -c src/golf.cpp
  g++ -c src/lexer.cpp
  g++ -c src/token.cpp
  g++ -c src/logger.cpp
  g++ -c src/filereader.cpp
  g++ -g golf.o lexer.o token.o logger.o filereader.o -o golf
  ```

### Relationship with the environment.

- Consistent with the environment
    - Warnings and errors are sent to `stderr`, while all other output its sent to `stdout`.
- No extraneous garbage in the output
    - Although I am formatting the error messages, no escape codes are added and the actual formatting is quite simple.
- Easy to run
    - The executable is very simple to run. All that needs to be provided is the file to lex:
  ```
  $ ./golf test/scan.t7
  ```
- Meta-documentation
  - A README is included in the root of the repository.
