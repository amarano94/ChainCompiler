# ChainCompiler
Clang patch files and documentation for supporting Chain language for intermittent systems

Language spec for additions to C-standard to support Chain:

Additional Statements:
ChOut {F <- v}, T;
NextTask T;
MultiOut {F <- v}, {T1, ..., Tn};
ModEnter M, T;
ModLeave;
ModPut {F <- v}, M;
ModOut {F <- v};
task T, f {}
channel (T1, T2), {F:type, ...}
module M, Tin, Tout, {T1, ..., Tn}

Additional Expressions:
ChIn F, T
ChSync F, {T1, ..., Tn}
ModGet F, M
ModIn F

Additional Keywords:
origin
self

Otherwise this modified Clang/LLVM compiler will use the standards of C for compilation
of Chain. Any notable differences to the Clang toolchain in order to incorporate these
additions will be noted in this document.

The reasoning behind this delineation is that those features marked as statements are
all stand-alone in their functionality and will always appear in the code as such.
The functions marked as expressions can appear either as stand-alone functionality or
will produce a value that can be saved to a variable or used in a declaration, which are
key components of an expression. The keywords origin and self are modifiers to statements
or expressions and do not contain functionality on their own. As of now, task is being
included as a statement, as it always precludes a function definition which is in itself
a statement. However, because the task keyword is modifying an already present function
definition, it is possible that it would be better suited to parsing as a keyword like
origin and self.

About this repo:
This repo will contain only the files modified of Clang and LLVM (ver. 3.9.1) necessary to
incorporate Chain, and can be used as a patch to Clang, up-to-date as of December 2016. 
This date will be changed as the working codebase is periodically modified to reflect the
most recent changes to Clang and LLVM.

All original file paths will be included with each file uploaded here for ease of reference.
This is mostly to keep the repo from getting too bloated, as LLVM and Clang together 
consist of many Gigabytes of data which are not necessary to store here.

To Use:
Install LLVM with Clang on a machine of your choice, using the steps outlined at:

https://clang.llvm.org/get_started.html

After building, the patch can be applied and the toolchain can be rebuilt and tested using
the tests located in the upper level tests directory in this repo.

A full list of compilation flags and options can be found at:

https://clang.llvm.org/docs/ClangCommandLineReference.html#compilation-flags

Of note is the section entitled 'Actions' where flags that determine up to what stage of
compilation is executed, and will be used heavily during this modification process.

-E, --preprocess   : only runs the preprocessor (lexer)
-fsyntax-only      : runs up through the parser
-emit-llvm         : uses the LLVM representation for assembler and object files


The Lexer:


The Parser:


The AST:


LLVM, or, adding proper behavior at the back-end: 

TBA
