# Milestone 3: GoLF Semantic Checking

## What is this

The purpose of the second milestone is to implement semantic checking for GoLF. You can find my
implementation here: https://gitlab.cpsc.ucalgary.ca/joseph.vanlierop/cpsc-411

## Who am I

Name: Joseph (Joey) Van Lierop  
UCID: 30096849  
Email address: joseph.vanlierop@ucalgary.ca

## Where do I need feedback

I chose to use strings as types because that's what Aycock did in his video series, but I am interested in hearing about
how you structured yours using formal types. I would also like to hear how I could make my code more readable, since its
starting to approach spaghetti. Some resources online use the visitor pattern which seems pretty verbose, but maybe
useful? Not sure, would love to hear your thoughts if you know about it.

## What grade have I earned

I think that I have earned an 8/8, but I think its debatable. I have also put in a lot of work to match the reference
compiler's output 1:1, however I think my code could use some refactoring to make more resilient and less reliant on
magic values.

### Tool (milestone) properties

- Good error and warning messages output.
    - My error messages are inspired by rust. I not only include an error message, but also the exact location of the
      error itself. One bug I have found is that it doesn't properly align code indented by tabs, but I am working on a
      fix.
  ```shell
  --> ../test/check.t12:7:6
      |
    7 | func main() {
      |      ^~~~ error: "main" redefined
  ```
    - Predictable:
        - I handle all legal input cases properly, and any unknown inputs are verbosely errored. After parsing, an AST
          is
          printed using proper indentation and any relevant information.

      ```shell
        program
            func sym=0x9d7250 @ (4, 1)
                newid [main] @ (4, 6)
                sig
                  formals
                  typeid [$void]
                block
                  var sym=0x9d72e0 @ (5, 2)
                    newid [int] @ (5, 6)
                    typeid [int] sym=0x9d6b90 @ (5, 10)
                  funccall sig=void @ (6, 8)
                    id [printi] sig=f(int) sym=0x9d6f80 @ (6, 2)
                    actuals
                      id [int] sig=int sym=0x9d72e0 @ (6, 9)
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
      I/O, semantic analysis, symbol table, and records
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
- Easy to build, and building without errors or warnings

### Relationship with the environment.

- Consistent with the environment
    - Warnings and errors are sent to `stderr`, while all other output its sent to `stdout`.
- No extraneous garbage in the output
    - Although I am formatting the error messages, no escape codes are added and the actual formatting is quite simple.
- Easy to run
    - The executable is very simple to run. All that needs to be provided is the file to parse:
  ```
  $ ./golf test/check.t7
  ```
- Meta-documentation
    - A README is included in the root of the repository with an additional SPECIFICATION document.
