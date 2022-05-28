/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "stack.cpp"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define PORT "3492" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

stack_ptr stack;

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
	stack = (stack_ptr)mmap(NULL, sizeof(struct Stack), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	stack->top =0;
	stack->data[0] = '\0';
    stack->top++;
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("DEBUG : waiting for connections...\n");

	try
	{
		while (1)
		{ // main accept() loop
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1)
			{
				perror("accept");
				continue;
			}

			inet_ntop(their_addr.ss_family,
					  get_in_addr((struct sockaddr *)&their_addr),
					  s, sizeof s);
			printf("DEBUG : got connection from %s\n", s);
			void *ptr = &new_fd;

			int new_fd = *(int *)ptr;

			if (send(new_fd, "Connected succesfully!\n", 23, 0) == -1)
				perror("send");

			char *file = "lock.txt";
			int fd;
			struct flock fl;
			fd = open(file, O_WRONLY);

			if (!fork())
			{
				while (1) // Recieving commands from the users and handling it.
				{
					char buf[1024];								 // Command reader.
					memset(buf, 0, 1024);						 // Clear the buffer , in case of garbage.
					int bytes_read = recv(new_fd, buf, 1024, 0); // Read the command.

					if (bytes_read == -1) // Checking errors.
					{
						cerr << "ERROR" << endl;
					}
					else if (bytes_read == 0) // Checking for connection.
					{
						cerr << "DEBUG : Client disconnected" << endl;
						close(new_fd);
						break;
					}
					else // Handling the command when the connection recieved.
					{
						string input = buf;
						if (input.substr(0, 4) == "PUSH")
						{
							while (fl.l_type == F_WRLCK)
							{
							}
							cout << "DEBUG : PUSH" << endl;
							memset(&fl, 0, sizeof(fl));
							fl.l_type = F_WRLCK;
							fl.l_pid = getpid();
							fcntl(fd, F_SETLKW, &fl);

							if (input.length() > 5)
							{
								string value = input.substr(5, input.length() - 5); // Get the value to push.
								push(value, stack);									// Push the value to the stack.
								send(new_fd, "DEBUG : Success", 16, 0);				// Send the response.
								fl.l_type = F_UNLCK;
								fcntl(fd, F_SETLKW, &fl);
							}
							else
							{
								send(new_fd, "ERROR : Invalid input", 23, 0);
								fl.l_type = F_UNLCK;
								fcntl(fd, F_SETLKW, &fl);
							}
						}
						else if (input == "POP")
						{
							while (fl.l_type == F_WRLCK)
							{
							}
							memset(&fl, 0, sizeof(fl));
							fl.l_type = F_WRLCK;
							fl.l_pid = getpid();
							fcntl(fd, F_SETLKW, &fl);
							if (top(stack) == "")
							{
								send(new_fd, "ERROR :Stack is empty", 22, 0);
								fl.l_type = F_UNLCK;
								fcntl(fd, F_SETLKW, &fl);
							}
							else // if the stack is not empty the command will be executed.
							{
								string value = pop(stack); // Get the value to pop.
								
								send(new_fd, value.c_str(), value.length(), 0); // Sending the value.
								fl.l_type = F_UNLCK;
								fcntl(fd, F_SETLKW, &fl);
							}
						}
						else if (input == "TOP")
						{
							if (top(stack) == "")
							{
								send(new_fd, "ERROR :Stack is empty", 22, 0);
							}
							else
							{
								while (fl.l_type == F_WRLCK)
								{
								}
								memset(&fl, 0, sizeof(fl));
								fl.l_type = F_WRLCK;
								fl.l_pid = getpid();
								fcntl(fd, F_SETLKW, &fl);

								string value = top(stack); // Get the value to pop.
								
								send(new_fd, value.c_str(), value.length(), 0); // Sending the value.
								fl.l_type = F_UNLCK;
								fcntl(fd, F_SETLKW, &fl);
							}
						}
						else
						{
							while (fl.l_type == F_WRLCK)
							{
							}
							memset(&fl, 0, sizeof(fl));
							fl.l_type = F_WRLCK;
							fl.l_pid = getpid();
							fcntl(fd, F_SETLKW, &fl);
							send(new_fd, "ERROR :Invalid command", 23, 0); // Sending the response.
							fl.l_type = F_UNLCK;
							fcntl(fd, F_SETLKW, &fl);
						}
					}
				}
			}
		}
	}
	catch (exception &e)
	{
		cout << "Exception in server thread" << endl;
		close(new_fd);
		return 0;
	}

	return 0;
}