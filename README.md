#### Text Justify utility (Linux & Windows) 

<pre> 

This  is  small but quite  useful  utility for formatting texts with fixed-width
fonts (such as  some README files, for example).  It takes a text file as  input
and outputs a file with the  same contents using fixed line length justified (by
inserting spaces) using predefined (equal to 80) or supplied line width. 

</pre> 

#### Usage and Examples 

<pre> 

`Syntax: ./justify [-f -u -wLINE_WIDTH] infile.txt outfile.txt` 

Examples: 

`./justify -w70 README.md README_70.md`

`./justify -u -w70 README.md` (updates README.md to use 70 as line width)

`./justify -f -w70 README.md README_70.md` (overwrite README_70.md if present)

It  inserts  spaces to justify text (to make every but the very last line in any
paragraph use the same number  of characters. Note that it will leave unmodified
lines where the very first non-space or -tab symbol is back-quote. It works with
both ASCII and UTF-8 files to support international characters. 

It  also  respects parargaph  first-line  indent by saving  the  spaces/tabs (it
replaces  tabs  with spaces),  so  the  indented text  on the  first  line  of a
paragraph stays indented (see example #2). 

Included are two  examples. The first one is unformatted  Engish text news story
(sample.txt), and corresponding sample_70.txt  file  after formatting with 70 as
line width. 

The second  text  is  thunder.txt (Vladimir Nabokov's  short story) in  Russian,
originally justified with  line width of 63.  The output, thunder_80.txt is  the
same  text with  line  width  set to 80.  Notice how paragraph indentations  are
preserved. 

</pre> 

#### Possible Enhancements 

<pre> 

Make the application respect the formatting  tags (such  as markdown tabs). This 
can work either by using  an additional flag argument or by checking  input file
extention. 

</pre> 

#### License 

[MIT](https://choosealicense.com/licenses/mit/) 
