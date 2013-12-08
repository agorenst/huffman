huffman
=======

Simple implementation of Huffman encoding algorithm.
====================================================

Source Files
============

There are 3 source files:
huffman.h and huffman.cpp implement the huffman algorithm.
huffman_encode.cpp has the "main" function and is essentially just a driver for exercising the algorithms in the main implementation.

The Makefile probably needs editing: it is custom-made for my very odd set-up.

Assuming you can change the compiler to something that works for you, the usage is quite simple:
./huffman_encode < frequency_file > encoding_map
That is, pipe frequency_file into stdin, and pipe the stdout into encoding_map.
Of course leaving the stdout to the terminal will also work: it is all plaintext.

Input files are simply a white-space delimited <word> <double> repeating sequence.
There are several frequency files, all in /inputs.
huckfinnfreq is a slighty-modified (dealing with whitespace characters is a pain) frequency file derived from the project gutenberg plaintext version of Huckleberry Finn.
The "wiki" files are from the wikipedia page on Huffman encodings.

I have attempted to comment the code thoroughly and keep the code clean for read-ability.

Goal Of Code
============

The goal is to be more educative than anything else.
Ideally, it is a playground to explore
1) Basic efficiency-related questions about the Huffman algorithm;
2) Some fun/interesting C++ features.
3) The fun and interesting Huffman algorithm!

Efficiency Exploration
----------------------

How do we achieve 1)?
C++ allows for both high-level and low-level implementation details.
For instance, the complexity of Huffman encoding is rather well-known, but what about memory allocations?
That can obviously vary based on how you implement the algorithm.
I cannot claim to be particularly knowledgeable about true low-level optimizations, but casual experiments seem to show that each "symbol" adds about 12 memory allocations and about 500 bytes.
This is just linear extrapolation from some of the input test files and valgrind results.

Exploring C++
-------------

How do we achieve 2)?
There is the question of the "right" implementation---Perl people may not like that idea.
However, for such a tight and well-defined algorithm as the Huffman algorithm, it is easy to explore and examine different formulations.
Would it be better to have all the code in the .h file?
Is there possibly a "better" encapsulation option?
(Argument for yes: all the structs etc. can be put in the huffman.cpp file easily, simply be wrapping build_tree and build_code into a single function that just takes an iterator pair.)
Can we do something better re: templates for the build_tree function? If you know the answer, do let me know...
Can we make code terser or "better-expressed" in some places or others?

Huffman Exploration
-------------------

How do we achieve 3)?
For the uninitiated, the algorithm can be quite unintuitive.
Running the algorithm by hand is the best way of learning how it works, but it may also be useful to see the algorithm run on larger inputs.
Lastly, with working code (and hopefully not-to-complex code) it should be easy to manipulate the code, and so easily "explore" how the algorithm works in that hands-on way.

Not Goals
---------

What do we *not* achieve?
1) Error-checking is extremely important in the real world, I am sure.
Here, however, I assume all the input is well-formed and so on.
It is simply a conceptual overhead which I think detracts from the 
2) In a similar vein, there are no unit tests or the like.
I will never claim that my implementation is correct, but it seems to work.
Moreover, the algorithm is so tiny, unit tests seem overwhelming---again we sacrifice this for the sake of a smaller code base.
3) Actually implementing a compression *program* requires a bit more effort: this can produce the mapping from strings to "binary" strings, but writing to binary files is tricky, among other facets.


Target Audience
===============

Who is the target audience for this program?
This code was based off of something I wrote to help me grade homework questions concerning the Huffman algorithm as a TA for an undergrad algorithms course.
So I *hope* that undergrads (or really, anyone just learning about the Huffman algorithm) would find this code useful.
