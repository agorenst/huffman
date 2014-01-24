huffman
=======

Simple implementation of Huffman encoding algorithm.

This is an attempt at creating an educational implementation of the Huffman encoding algorithm.
If you're curious about how the Huffman algorithm actually looks like in practice, here's a stab at it.

Where it stands now:
--------------------
It can really compress files now, albeit not very well.
I hope this implementation is useful for students to see:

1. A reasonable implementation of the Huffman encoding algorithm, as seen in huffman.cpp
2. A cute-and-clever way of serializing a Huffman tree, critically exploiting the fact that the tree is full (strictly-binary).
3. A reasonable implementation of how to use that encoding to actually compress a file, as seen in driver.cpp and huffman\_buffers.cpp.

A few "gotchas" in the form of design decisions:

1. In algorithms textbooks the huffman tree is often shown to contain the symbols in the leaves of the tree.
Here, we instead only store indices (integers, really).
This simplifies the generate\_tree implementation, as it needs only take an index->frequency map (i.e., a vector of doubles).
An external index->symbol (such as a vector of strings) can be used by the client.
In our case, our symbols are unsigned characters---thus the index is the symbol, making things fairly terse.
2. A system-dependent fact is that we work in "bytes", which we assume are 8 bit patters, and a 0 byte is literally the bit pattern 00000000.
I implemented this on Ubuntu 12.04LTS, using the standard gcc, g++, on amd64. It should (hopefully) work.
3. What's this "explicit huffman" directory? It is simply a much older implementation, in which I made much "clunkier" decisions, but it may be useful for students to look at.
More of the computation, and especially the tree structure, is concretely realized in the code, hence the name (as it is explicitly written out).

Where are we going from here?

1. The code could always be cleaned up and commented a bit better.
2. Better error-handling, especially in terms of file IO handling.
3. Better formal analysis with more real "asserts".

These improvements already exist to some extent, the idea is to follow-through even more.
At this point, though, I think it's fairly full-featured and actually works, so I'm shelving this for a bit.
Feel free to email me (my same username here, but at outlook.com) for comments/suggestions/questions, and I'd be happy to improve the code.
