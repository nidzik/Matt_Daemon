#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <sys/select.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <sys/time.h>
char *kl;
int fdmax;

typedef struct s_p
{
	int file;
	int sk1;
	int sk2;
	int sk3;

}t_p;

void exit_error(char *err, int file)
{
	int i = errno;
	write(file, err, strlen(err));
	write(file, strerror(i), strlen(strerror(i)));
	close(file);
	exit(EXIT_FAILURE);
}

void	print_file(char *cmd, int file)
{
	write (file, cmd, strlen(cmd));
}


t_p *create_server(int file)
{
	struct sockaddr_in	serv_sock;
	struct sockaddr_in	cli_sock;
	struct protoent	*prot;
	int cli_size = 0;
	int socketid;
	int accept_socket;
	int port;
	t_p op;
	fd_set rfds;
	fd_set rfdss;
	int tmp_sock;
	int retval;

	retval = 0;
	op.sk1 = -1;
	op.sk2 = -1;
	op.sk3 = -1;
	port = atoi(kl);
	prot = getprotobyname("tcp");
	if (prot == NULL)
		write(file, "prot fail\n", 10);
	socketid = 0;
	accept_socket = 0;
	bzero(&serv_sock, sizeof(serv_sock));
	serv_sock.sin_family = AF_INET;
	serv_sock.sin_port = htons(port);
	serv_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	if ((socketid = socket(AF_INET, SOCK_STREAM,\
						   prot->p_proto)) < 0)
		exit_error("socket error server side.\nExiting...\n", file);
	fdmax = socketid;
	if ((bind(socketid, (struct sockaddr *)&serv_sock,sizeof(serv_sock))) < 0)
	{

		close(socketid);
		exit_error("Error while binding the socket.\nExiting...\n", file);
	}
	if (listen(socketid, 5) < 0)
		exit_error("Error while listing. Exiting...\n", file);
	write(file, "serv creat\n", 11);
	fdmax = socketid;
	//        FD_ZERO(&rfds);
	//	FD_SET(socketid, &rfds);

	int nb = 0;
	int i = 0;

	FD_ZERO(&rfdss);
	FD_SET(socketid, &rfdss);

	while (42)
    {
		rfds = rfdss;
		if (op.sk1 != -1)
		{
			FD_SET(op.sk1, &rfds);
		}
		if (op.sk2 != -1)
			FD_SET(op.sk2, &rfds);
		if (op.sk3 != -1)
			FD_SET(op.sk3, &rfds);
		retval = select(fdmax+1, &rfds, NULL, NULL, NULL);
		/* Considérer tv comme indéfini maintenant ! */
		//	  write(file, "retval = ", 9);
		//	  write(file, "retval = ", 9);
		if (retval == -1)
			exit_error("[ERROR] select fail. ", file);
		for (int i = 0; i <= fdmax ; ++i)
	    {
			if (FD_ISSET(i, &rfds))
			{
				if (i == socketid)
				{
					// ##newconnection...
				  //					if (op.sk1 == -1 || op.sk2 == -1 || op.sk3 == -1)
				  //{
						if ((tmp_sock = accept(socketid,			\
											   (struct sockaddr *)&cli_sock, \
											   (unsigned int *)&cli_size)) < 0)
							exit_error("Error while accepting the socket. Exiting...", file);
						write(file, "[LOG]  new socket created\n", 26);
						if (tmp_sock > fdmax)
						{
							fdmax = tmp_sock;
						}
						nb++;
						if (nb == 1)
							op.sk1 = tmp_sock;
						else if (nb == 2)
							op.sk2 = tmp_sock;
						else if (nb == 3)
							op.sk3 = tmp_sock;
						if (op.sk1 == -1 || op.sk2 == -1 || op.sk3 == -1) 
						  FD_SET(tmp_sock, &rfds);
						
					else
						write(file, "[ERROR]  no socket avalibles \n", 30);
					

				}
				else
				{
					//#knowed socket read..
//		     write(file, "socket knowed\n", 14);
					char buff[1024] = {};
					if (read(i, buff, 1024) == 0)
					{
						write(file, "[ERROR] sk disconected\n", 23);
						close(i);
						FD_CLR(i, &rfds);
						if (i == op.sk1) op.sk1 = -1;
						if (i == op.sk2) op.sk2 = -1;
						if (i == op.sk3) op.sk3 = -1;
					}
					write(file, "[CMD]   ", 8);
					write(file, buff,strlen(buff));

//		      FD_SET(i, &rfds);
				}
			}
	    }
	  
    }
	return (&op);
}

int main(int ac, char **av)
{
	kl = av[1];
	pid_t pid, pid2;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
	{
	    printf("cannot create child, parent exit.\n");
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0)
	{
	    printf("child %d created, parent exit.\n", (int)pid);
	    exit(EXIT_SUCCESS);
	}

	//handle sig
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	//create mask, open full access
	mode_t mask;
	mask = umask(0);
	
	// open logfile
	int file;
	file = open(".logm", mask | O_CREAT);
	write(file, "log arythm\n", 11);

	// set sid
	pid_t sid;
	if ((sid = setsid()) < 0)
	{
	    write(file, "ERR sid\n", 8);
	    close(file);
	    exit(EXIT_FAILURE);
	}
	else if (pid == sid)
	{
	    write(file, "ERR sid == pid\n", 15);
	    close(file);
	    exit(EXIT_FAILURE);	    
	}
	else
	{
	    write(file, "sid ok\n", 7);
	    //	    fprintf(STDIN_FILENO, "hello");
	    //	    fprintf((FILE *)file, "sid = %d, pid = %d\n",(int)sid, (int)pid);
	    //	    close(file);
	    //	    exit(EXIT_FAILURE);
	}
	pid2 = fork();
	if (pid2 > 0)
		exit(EXIT_SUCCESS);
	/* Change the current working directory */
	if ((chdir("/")) < 0)
	{
	    write(file, "ERR chdir / \n", 13);
	    close(file);
	    exit(EXIT_FAILURE);
	}

	// close all fds
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	write(file, "BC\n", 3);

	while (1)
	{
		write(file, "OP\n", 3);
		create_server(file);
		//core_daemon(&op)
		sleep(10);
	}
	return (0);
}
