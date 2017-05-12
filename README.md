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
This repo will contain the files modified of Clang (ver. 3.9.1) including the changes to
incorporate Chain, and can be used as a patch to Clang, up-to-date as of May 2017. 
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

Steps outlining how to add new expressions and statements to Clang are described in
llvm/tools/clang/docs/InternalsManual.rst under "How to add an expression or statement".
This document will be included in this repo.
 
In more detail the steps necessary to add statements and expressions to the Clang frontend
are explained below:

Statements:

References to the new statement must be created in clang/include/clang subdirectories.
The easiest way to see statement definitions is to grep for a statement that is already
defined (like WhileStmt) and mimic the places where new definitions are located.

Basic/TokenKinds.def contains all lexer keywords, new statements need to be added there.

Basic/StmtNodes.td holds the definitions of new statement keywords as well as new
expression keywords.

Serialization/ASTBitCodes.h contains the records of statements and subexpressions that
will be present in the AST after parsing.

AST/Stmt.h Contains the definition of each statement class. This is where the statement itself
is defined, and contains any special functions necessary for the correct compilation of
the statement.

AST/RecursiveASTVisitor.h Constains statements like DEF_TRAVERSE_STMT(...). It is necessary
for correct compilation of your statement to add one here or else the statement cannot be
traversed through for sub-statements or sub-expressions.

The rest of the functionality will be placed in the clang/lib subdirectories, where
.cpp files that match the headers in the include directories implement function definitions:

Parse/ParseStmt.cpp You will use the keywords that you have added to the lexer in
clang/include/clang/Basic/TokenKinds.def to determine which parts of the statement are
sub-expressions or statements and use them as arguments to ActOnYourStatement, which you
call here.

Sema/TreeTransform.h This is the brunt of the semantic analysis and rebuilding of statements
and expressions to AST. It is necessary to call or implement (though implementation may be
somewhere else) the following functions specific to your statement:
    ActOnYourStatement
    TransformYourStatement
    RebuildYourStatement

Sema/SemaStmt.cpp This will use your statement class and contains the implementation of
ActOnYourStatement

CodeGen/CGStmt.cpp This 'emits' your statement as a form useful for backend analysis.
You will implement the function EmitYourStatement.

Serialization/ASTReaderStmt.cpp You will implement the reader version of VisitYourStatement
where you create an empty context of your statement.

Serialization/ASTWriterStmt.cpp You will implement the writer version of VisitYourStatement.

AST/StmtPrinter.cpp This prints out your new AST after creating it in codegen, there is no
compiler functionality but it is useful for debugging and getting output during compilation.

AST/Stmt.cpp This contains the definitions of the statement class you created in Stmt.h listed
above. All class functions are defined here.

AST/ExprConstant.cpp You may or may not need to modify here; this transforms your statement
into an expression.

AST/StmtProfile, you will need to call VisitYourStatment here.

This is what you need to add a statement to the frontend (Lexing through basic 
CodeGen). I unfortunately didn't get to do much in the way of semantic analysis or anything
beyond that, but the Sema directories in include and lib hold the functions for 
Semantic analysis. Additionally, the file clang/lib/Analysis/CFG.cpp visits each 
statement to do analysis.

Expressions:

Adding an expression in essence is similar to adding a statement, but traversing and parsing
and expression is more complex, because parsing an expression with subexpressions requires
different steps than parsing a statement with subexpressions.

The lexing step is the same, as is adding the new keyword to StmtNodes.td.
ASTBitCodes.h also contains the expression records with the statement records.

Parser.h in clang/include/clang/Parse includes a function for each expression of the form ExprResult ParseChInExpression();
This is different than the statements because the statements didn't contain explicit parsing, it was implied by the function
VisitXXXStmt where the passed value was an internal sub-expression or statement. For expressions, the parsing is done 
explicitly in order to account for unique sub-expressions or other unusual behavior. However, it is extremely difficult to
overload functionality of existing symbols for new parsing purposes in C, because those other definitions are hard-coded
(For example, using {} as sub-expression demarcation instead of (), because {} is already hard-coded as a block identifier
by the compiler). Be wary when trying to overload already-used symbols, you'd be better using symbols that aready have that
functionality (i.e. () for sub-expression demarcation).

clang/include/clang/AST/Expr.h is where representations of expressions are defined, also in classes as statements were. 
Like with the statements, a DEF_TRAVERSE_STMT must be included for the expression in RecursiveASTVisitor.h.

In the lib directories, there are similarities to adding statements as well.
Sema/SemaStmt.cpp also contains VisitYourExpr functions, and you must add one for your expression.
Sema/TreeTransform.h needs a ActOnYourExpr, RebuildYourExpr functions.

Sema/SemaExpr.cpp contains information on expressions where SemaStmt.cpp contains information on statements. 
This file contains implementation details of ActOnYourExpr.

Again, ASTStmtReader and ASTStmtWriter both need a version of VisitYourExpr.

Parse/ParseExpr.cpp is the expression mirror of ParseStmt.cpp and will contain the implementation of your ParseYourExpression
function. It will call ActOnYourExpr.

There is also space in StmtPrinter to write a VisitYourExpr function for debug print output.

AST/Expr.cpp is the expression version of Stmt.cpp and will contain the implementation details of your expression class.

Unlike statements, expressions need to be classified in lib/AST/ExprClassificiation.cpp. In a large switch statement, each
type of expression returns with a call to ClassifyInternal of the subexpression until the entire expression is classified.
lib/AST/ExprConstant.cpp is also used to check for constant expressions/expression folding, and a case must be added in there
as well for your expression.

Semantic analysis for expressions is located in clang/lib/Sema like with statements.
