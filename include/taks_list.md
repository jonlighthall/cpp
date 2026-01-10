I need to add the ability to read in numbers saved in scientific notation. the
decimal calculator will need to be updated accordingly. there should also be a
warning if the decimal precision of either file exceeds single precision. I'm
not sure how to handle very small numbers written in scientific notation and how
to pass that to the decimal counter. do we need to track both number of decimal
places and significant figures? usually if one file is in scientific notation,
the other one will be too, so the printed decimal precision of the numbers in
scientific notation should be the number of decimals? or is that inconsistent?

well, this makes me think. this is the whole point of the program. in the
example files that you generated, the printed numbers are identical to within
printed precision. should we use the un-rounded difference compared to the
threshold? should we add an un-rounded difference to the print table? what would
be the most rigorous and valid method to do this mathematically and in terms of
computer science? in this case, if we're asking for any non-zero differences to
be printed, how should we handle this? also if we're comparing files generated
by different computers, with different compilers and cpu architectures, and now
different programming languages, how should we handle these tiny differences? The
main purpose of this program is validating outputs from a program written in a
legacy language (fortran 77), to a ported version of the program written in a
modern language (like c++11). If the user, for example, where to change the
printed format of the outputs to increase or reduce the printed precision, this
difference analysis program should be able to report that the files are
numerically identical, even if they have different precisions. If we do a good
enough job on designing this algorithm, not only will it be used by developers
to validate new installations of this critical simulation, but I would also like
to write a memo report explaining it's function, so I would like our solution to
be rigorous.