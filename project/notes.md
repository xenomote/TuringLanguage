wrote out a textual document of requirements.

wrote a first draft of the grammar for the language, as well as writing out some example machines. using the grammar

bison:
    www.gnu.org/software/bison/manual/bison.html

flex:
    ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html

flex and bison tutorial:
    www.aquamentus.com/flex_bison.html

multiline comments:
    www.westes.github.io/flex/manual/How-can-I-match-C_002dstyle-comments_003f.html

whitespace based indentation:
    https://stackoverflow.com/questions/1413204/how-to-use-indentation-as-block-delimiters-with-bison-and-flex

got flex and bison working to parse the grammar

started working on action rules for the parser, created structs for each key program element

filled out different token types, added location tracking and error messages

worked on resolving shift reduce conflicts in grammar and forward declarations of block names

fixed conflicts by adding block and group initialiser keywords, finally got a successful parse

began using PyQt5 to create editor
adapted https://doc.qt.io/qt-5qtwidgets-widgets-codeeditor-example.html into python

used https://www.riverbankcomputing.com/static/Docs/PyQt5/index.html as reference for PyQt5 development

investigated implementing editor as a vscode extension, created syntax highlighting plugin

describes a similar system to the language of this project called a wang-B machine
    https://web.stanford.edu/class/archive/cs/cs103/cs103.1132/lectures/19/Small19.pdf

changed the grammar to be more flexible, more closely reflecting Wang-B machines. Also changed parser to focus more on syntax, AST will be type checked in multiple passes.