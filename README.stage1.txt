a. Reused Code

I have reference the Beej guide for socket programming at http://beej.us/guide/bgnet/output/html/multipage/index.html. Some function names share the similarity as described on the website.

b. Complete: 
Yes, the stage is complete.

c. Portable
No the code will not work if the manager and client were on different machines with different architectures. 
No there is no support added.
For example the size of int might be 32 bit on a 32 bit architecture and it could be 64 bit on a machine running 64 bit architecture. These inconsistencies cause the program to behave differently on different machines. Hence it becomes difficult to support.