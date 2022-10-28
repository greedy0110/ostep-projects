What is the input?
- one or more files.

What is the output?
- print the compressed string to standard output

How separate things?
- read a file as the text
- compress the text
  - *RLE
- print it

Trouble Shooting
- How to read the text from the compressed file? Since it is stored as a binary file ... Can I handle it just as if it is normal ASCII?
  - I don't think it's going to be a problem. Since I can parse ...
- the thing is that "How to unzip '10a4b'"
  - To do so, have to figure '10' as an integer 10.
- How to determine the size of compreesed buffer?
  - the worst case : the size of the compressed buffer could be double by of the original source. (e.g. 'abc' -> '1a1b1c')