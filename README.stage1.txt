What did I learn ?

* network system calls usage and implementation
* writing modular code
* socket programming principles
* efficient use of resources


a. Reused Code

I have reference the Beej guide for socket programming at http://beej.us/guide/bgnet/output/html/multipage/index.html. Some function names share the similarity as described on the website.

b. Complete: 
Yes, the stage is complete.

c. Portable
Yes it will work, if the packet format is independent of the CPU
and the program converts between the network format and the CPU format.
For example it you write an short integer in x86 its little endian
in network format it is typically big endian so the program needs to use the htons() method to convert between the two and ntohs() while reading the packet.