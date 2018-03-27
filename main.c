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
char *kl;

typedef struct s_p
{
  int file;
  int sk;

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

int core_daemon(t_p *op)
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    int sock;
	int r;
	char buf[2048] = {};
	r = 0;
    write(op->file, "start select\n", 13);

    /* Surveiller stdin (fd 0) en attente d'entrées */
    FD_ZERO(&rfds);
    FD_SET(op->sk, &rfds);

    /* Pendant 5 secondes maxi */
    tv.tv_sec = 15;
    tv.tv_usec = 0;

    retval = select(op->file, &rfds, NULL, NULL, &tv);
    /* Considérer tv comme indéfini maintenant ! */

    if (retval == -1)
{
      write(op->file, "ERR select", 10);
return (-42);
}
    else if (retval)
	{
	write(op->file, "Des données sont disponibles maintenant\n", 41);
	(r = read(op->sk, buf,2048));
	if (r < 0)
	exit_error("[EROR] read fail", op->file);
	else
		print_file(buf, op->file);
	}
        /* FD_ISSET(0, &rfds) est vrai */
    else
{
      write(op->file, "Aucune données durant les 15 secondes\n", 39);
	close (op->file);
	close(op->sk);
	return (-42);
}

    write(op->file, "end select\n", 11);
//	close(op->file);
//	close(op->sk);
    //exit(EXIT_SUCCESS);
return 0;
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
  if ((bind(socketid, (struct sockaddr *)&serv_sock,sizeof(serv_sock))) < 0)
{

close(socketid);
    exit_error("Error while binding the socket.\nExiting...\n", file);
}
  if (listen(socketid, 5) < 0)
    exit_error("Error while listing. Exiting...\n", file);
  write(file, "serv creat\n", 11);

  while (42)
    {
	  write(file, "boucle\n", 7);
      if ((accept_socket = accept(socketid,\
				       (struct sockaddr *)&cli_sock, \
				       (unsigned int *)&cli_size)) < 0)
	exit_error("Error while accepting the socket. Exiting...", file);
      op.file = file;
      op.sk = accept_socket;
            core_daemon(&op);

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


	//2nd fork : PID != SID our process can't take control of a TTY again
	/*	pid2 = fork();
	if (pid2 < 0)
	  {
	    printf("cannot create child, parent exit.\n");
	    exit(EXIT_FAILURE);
	}
	if (pid2 > 0)
          {
            printf("child %d created, parent exit.\n", (int)pid2);
            exit(EXIT_SUCCESS);
	  }

	*/
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
