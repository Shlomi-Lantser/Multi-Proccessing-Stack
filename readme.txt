# Ex4-OS
Server stack multithreading TCP communication , malloc , calloc & free implementation C++.

## Or Cohen & Shlomi Lantser.

# Description

In this project we implemented a stack of strings to put it on our server , the server supports multithread by POSIX mutex.
the server store the stack that we implemented and every client has it own thread.
The client sending command to the server that changing the stack to all the other clients , the mutex inspectoring that every method of the stack 
will be preformed by only one thread (client).
Every stack change is implemented with our own malloc that using sbrk().

# Diagram of the communication :

![diagramServer](https://user-images.githubusercontent.com/92504985/164740129-20ea15cc-26ea-4e4b-ba84-7d2b8ea435e1.png)


As we can see the clients connecting to the server and the server stores the stack , evey client sending commands to change the stack and the server responding when succeed / not succeed to execute the command.


Files on this project :
                  
* **Server_new** - Implementation of server that handling new connections by threads , each connection has its own thread and its supports many connections.
                  
* **client** - The client connecting to the server and recieve message that approving the connection from the server, we used 127.0.0.1 to connect the server.

* **Stack** - Stack of string that will be stored on the server.

* **malloc** - Our implement for malloc & calloc & free to manipulate the heap.

# Client commands:

* `PUSH [value]` - Pushing a value into the stack (string value).
* `POP` - Pops the top value of the stack and returns it.
* `TOP` - Returns the top value of the stack.
* `EXIT` - To disconnect the server.

                 
## Running guide:

* Clone this ripository
* Open the directory and open it in terminal
* Use the command `make all` to create all the needed files and the executable ones.
* Use the command `./server` to start server receiving and listening to new connections.
* Use the command `./client [ipaddr]` ipaddr is the ip address that using to connect the sever aka : 127.0.0.1
* Use the command `make clean` to clean all the files after you done.


## Video of running:

https://user-images.githubusercontent.com/92504985/164744819-bbd688aa-d728-4c18-8cd1-957829a016d6.mp4

    
                 
