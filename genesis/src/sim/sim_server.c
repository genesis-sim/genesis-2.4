#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <setjmp.h>

static int Debug = 0;
extern jmp_buf main_context;

/*
** Server
**
**	Run this GENESIS as a server.  Supports multiple network
**	connections which can execute GENESIS server commands.
**	Each GENESIS server command runs to completion before
**	handling the next request.  GENESIS server requests:
**
**	EXEC cmd args
**
**		Execute the given GENESIS command returning the
**		command return value.
**
**	BYE
**
**		Close the connection.
**
**	QUIT
**
**		Terminate the GENESIS server.
**
**	INCLUDE filename
**
**		Include the named file (which must reside and be
**		readable on the server machine.
**
**	PARSE
**
**		Read GENESIS script input from the connection.
**
**	UPLOAD filename size
**
**		Upload GENESIS script input and save in the named
**		file.
**
** Currently Implemented:
**
**	BYE, QUIT, EXEC
*/

#define	SERVER_WELCOME	"GENESIS Server 0.2, Ready\n"

/*
** Server result codes
*/

/* general */
#define	RES_OK		0	/* command processed successfully */
#define	RES_NOTIMP	101	/* known server command but not implemented */
#define	RES_UNKCMD	102	/* unknown server command */
#define	RES_LIMIT	103	/* request exceeded a serve limitation */
#define	RES_CMDERR	104	/* server command error */
#define	RES_PATHERR	105	/* path is not acceptable */

/* EXEC command */
#define	RES_CMDNOTFOUND	201	/* EXEC can't find the genesis command */

/* BYE and QUIT commands */
#define	RES_GOODBYE	301	/* client is ending the session */
#define	RES_SHUTDOWN	302	/* client is shutting down the server */

/* INCLUDE command */
#define	RES_SCRIPTNOTFOUND	401 /* script file not found */
#define	RES_SCRIPTERROR		402 /* error running script */

/* Upload commands */
#define	RES_UPLOADERR	501	/* Error uploading data */
#define	RES_OPENERR	502	/* Open failed on file */
#define	RES_SENDDATA	503	/* File open ready to receive */

/* internal codes */
#define RES_CLOSE	901	/* socket closed by the client */


static struct
  {
    int		code;
    char*	desc;
  } CodeTable[] =
  {
    { RES_OK,		"Ok" },
    { RES_NOTIMP,	"Server command not implemented" },
    { RES_UNKCMD,	"Unknown server command" },
    { RES_CMDERR,	"Server command syntax error" },
    { RES_LIMIT,	"Request exceeds a server limitation" },
    { RES_PATHERR,	"Invalid filename path" },
    { RES_CMDNOTFOUND,	"Unknown GENESIS command or function" },
    { RES_SCRIPTNOTFOUND, "Script file not found" },
    { RES_SCRIPTERROR,	"Error running script" },
    { RES_UPLOADERR,	"Error while uploading" },
    { RES_OPENERR,	"Error opening upload file" },
    { RES_SENDDATA,	"Send data" },
    { RES_GOODBYE,	"Bye" },
    { RES_SHUTDOWN,	"Shutting down GENESIS server" },
    { -1, "Error" }
  };


/*
** Module variables
*/

static int	ServerListen;	/* socket we're listening on */
static fd_set	ServerConnSet;	/* fd_set of open connections and the
				   ServerListen socket */


Server(port)

short	port;

{
	struct sockaddr		sa;
	struct sockaddr_in 	*sin;

	ServerListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerListen == -1)
	  {
	    perror("Server(socket)");
	    exit(1);
	  }

	FD_ZERO(&ServerConnSet);
	FD_SET(ServerListen, &ServerConnSet);

	/* set up sockaddr sa to listen on given port */
	sin = (struct sockaddr_in *) &sa;
	BZERO(sin, sizeof(struct sockaddr_in));
	sin->sin_family = AF_INET;
	sin->sin_port = htons(port);

	if (bind(ServerListen, &sa, sizeof(struct sockaddr_in)) == -1)
	   {
	    perror("Server(bind)");
	    exit(1);
	   }

	if (listen(ServerListen, 5) == -1)
	  {
	    perror("Server(listen)");
	    exit(1);
	  }

	while (1)
	  {
	    ServerJob();
	    ExecuteJobs();
	    ExecuteCleanups();
	  }
}


/*
** ServerJob
**
**	Check for activity on network sockets.  Accept new connections
**	and read and execute server commands on active connections.
*/

ServerJob()

{
	fd_set		readfds;
	struct timeval	tv;
	int		fd;

	/*
	** we will loop here handling active and newconnections until
	** they are all satisfied.
	*/

	BCOPY(&ServerConnSet, &readfds, sizeof(fd_set));
	BZERO(&tv, sizeof(struct timeval));
if (Debug) fprintf(stderr, "ServerJob(select)\n");
	while (select(getdtablesize(), &readfds, NULL, NULL, &tv) > 0)
	  {
	    if (FD_ISSET(ServerListen, &readfds))
	      {
		struct sockaddr	sa;
		int addrlen = sizeof(struct sockaddr);
		int newconn;

if (Debug) fprintf(stderr, "SeverJob(accept)\n");
		newconn = accept(ServerListen, &sa, &addrlen);
		if (newconn == -1)
		    perror("Server(accept)");
		else
		  {
		    FD_SET(newconn, &ServerConnSet);

		    write(newconn, SERVER_WELCOME, strlen(SERVER_WELCOME));
		    ServerLog(newconn, "Connected");
		  }

		FD_CLR(ServerListen, &readfds);
	      }

	    for (fd = FD_SETSIZE - 1; fd >=0; fd--)
		if (FD_ISSET(fd, &readfds))
		  {
		    int		result;
		    void*	resultdata;
		    int		resultdatalen;

if (Debug) fprintf(stderr, "SeverJob(ServerCommand)\n");
		    result = ServerCommand(fd, &resultdata, &resultdatalen);
		    if (result == RES_CLOSE)
		      {
			ServerLog(fd, "Closed");
			close(fd);
			FD_CLR(fd, &ServerConnSet);
		      }
		    else
			ServerReply(fd, result, resultdata, resultdatalen);

		    switch (result)
		      {

		      case RES_GOODBYE:
			ServerLog(fd, "Bye");
			GoodBye(fd);
			break;

		      case RES_SHUTDOWN:
			ServerLog(fd, "Shutdown");
			Shutdown();
			exit(0);
		      }
		  }

	    BCOPY(&ServerConnSet, &readfds, sizeof(fd_set));
	    BZERO(&tv, sizeof(struct timeval));
	  }
}


int ServerArgv(cmd, argv, maxargs)

char*	cmd;
char*	argv[];
int	maxargs;

{
	extern char* strchr();
	int	argc;

	argv[0] = cmd;
	cmd = strchr(cmd, ' ');
	if (cmd != NULL)
	    *cmd++ = '\0';

	argc = 1;
	while (cmd != NULL)
	  {
	    if (argc >= maxargs)
		return maxargs+1;

	    if (*cmd == '\'' || *cmd == '"')
	      {
		char	quote = *cmd++;

		argv[argc] = cmd;
		cmd = strchr(cmd, quote);
		if (cmd != NULL)
		    *cmd++ = '\0';
	      }
	    else
	      {
		argv[argc] = cmd;
		cmd = strchr(cmd, ' ');
	      }

	    if (cmd != NULL)
		*cmd++ = '\0';

	    argc++;
	  }

	return argc;
}


GoodBye(fd)

int	fd;

{
	close(fd);
	FD_CLR(fd, &ServerConnSet);
}


Shutdown()

{
	int	fd;

	for (fd = FD_SETSIZE - 1; fd >=0; fd--)
	    if (FD_ISSET(fd, &ServerConnSet))
		GoodBye(fd);
}


static int DoInclude(includecmd)

char*	includecmd;

{
	extern FILE*	SearchForScript();

	int	argc;
	char*	argv[100];
	char*	pathname;
	FILE*	fs;

	argc = ServerArgv(includecmd, argv, 100);
	if (argc > 100)
	    return RES_LIMIT;

	pathname = argv[0];
	fs = SearchForScript(pathname,"r");
	if(fs == NULL){
    	    if (IsSilent() < 2) {
		Error();
	   	 printf("can't open script file %s\n", pathname);
	    }

	    return RES_SCRIPTNOTFOUND;
	}

	if(setjmp(main_context) == 0){
	    ParseInit();
	    SetScript(NULL, fs, argc, argv, FILE_TYPE);
	    Interpreter();
	} else {
	    ClearScriptStack();
	    RemoveSimulationJob();

	    return RES_SCRIPTERROR;
	}

	return RES_OK;
}


static DoUpload(fd, filename, size)

int	fd;
char*	filename;
int	size;

{
	FILE*	fs;
	char	buf[BUFSIZ];
	int	nbytes;

	/*
	** This is intended to be a security measure to prevent
	** overwriting of random files.  But the security holes
	** of unrestricted access to GENESIS commands makes this
	** fairly meaningless.  Might be useful against accedental
	** overwrites of user's own files.
	**
	** For now its disabled.
	if (!CleanPath(filename))
	  {
	    printf("Upload files must be relative to working directory\n");
	    return RES_PATHERR;
	  }
	*/

	fs = fopen(filename, "w");
	if (fs == NULL)
	  {
	    perror(filename);
	    return RES_OPENERR;
	  }

	ServerReply(fd, RES_SENDDATA, NULL, 0);

	do
	  {
	    nbytes = size < sizeof(buf) ? size : sizeof(buf);

	    nbytes = read(fd, buf, nbytes);
	    if (nbytes == -1)
	      {
		perror("reading upload data");
		fclose(fs);
		unlink(filename);
		return RES_UPLOADERR;
	      }

	    if (nbytes > 0)
		if (fwrite(buf, 1, nbytes, fs) != nbytes)
		  {
		    perror(filename);
		    fclose(fs);
		    unlink(filename);
		    return RES_UPLOADERR;
		  }

	    size -= nbytes;
	  }
	while (size > 0);

	fclose(fs);
	return RES_OK;
}


/*
** GenCmd
** 
** Parse the EXEC command into an arg vector and dispatch to
** GENESIS.  Return the command/function result as a string.
**
** NOTE: the code demands that the command name and args be
** separated by a single space.  Whitespace may be included in
** a command argument using either single or double quotes.
*/

int GenCmd(cmd, resultstr, resultlen)

char*	cmd;
char**	resultstr;
int*	resultlen;

{
	extern char* ExecuteStrFunction();

	char*	argv[100];
	int	argc;

	argc = ServerArgv(cmd, argv, 100);
	if (argc > 100)
	    return RES_LIMIT;

	cmd = argv[0];

	/*
	** ExecuteStrFunction and other similar commands
	** do not return a status to indicate that a given
	** command or function is not defined.  (It prints
	** its own error message; Yuk!)  We could handle the
	** unknown command/function by checking both the
	** script symbol table and the command table.  But
	** not for now.  If necessary, put the code here.
	*/

	*resultstr = ExecuteStrFunction(argc, argv);
	*resultlen = strlen(*resultstr);

	return RES_OK;
}

int ExecuteServerCmd(fd, cmd, resultstr, resultlen)

char*	cmd;
char**	resultstr;
int*	resultlen;

{
	if (strncmp(cmd, "EXEC ", 5) == 0)
	  {
	    return GenCmd(cmd+5, resultstr, resultlen);
	  }
	else if (strncmp(cmd, "INCLUDE ", 8) == 0)
	  {
	    return DoInclude(cmd+8);
	  }
	else if (strncmp(cmd, "UPLOAD ", 7) == 0)
	  {
	    char*	filename;
	    int		size;

	    filename = cmd+7;
	    cmd = strchr(filename, ' ');
	    if (cmd != NULL)
	      {
		*cmd++ = '\0';
		if (sscanf(cmd, "%d", &size) == 1)
		    return DoUpload(fd, filename, size);
	      }

	    return RES_CMDERR;
	  }
	else if (strncmp(cmd, "PARSE ", 6) == 0)
	  {
	    return RES_NOTIMP;
	  }
	else if (strncmp(cmd, "BYE", 3) == 0)
	  {
	    return RES_GOODBYE;
	  }
	else if (strncmp(cmd, "QUIT", 4) == 0)
	  {
	    return RES_SHUTDOWN;
	  }

	return RES_UNKCMD;
}

int ServerCommand(fd, resultstr, resultlen)

int	fd;
char**	resultstr;
int*	resultlen;

{
    char	buf[2048];
    int		nread;
    int		cmdlen;

    *resultlen = 0;
    *resultstr = NULL;

    cmdlen = 0;
    nread = read(fd, buf, sizeof(buf));
    while (nread > 0)
      {
	cmdlen += nread;
	if (cmdlen >= sizeof(buf))
	    break;

	if (buf[cmdlen-1] == '\n')
	    break;

	nread = read(fd, buf+cmdlen, sizeof(buf)-cmdlen);
      }

    if (nread <= 0)
	return RES_CLOSE;

    buf[cmdlen-1] = '\0';
    if (cmdlen > 1 && buf[cmdlen-2] == '\r')
	buf[cmdlen-2] = '\0';

    return ExecuteServerCmd(fd, buf, resultstr, resultlen);
}


ServerReply(fd, result, resultstr, resultlen)

int	fd;
int	result;
char*	resultstr;
int	resultlen;

{
	char	buf[100];
	int	len;
	int	i;

	for (i = 0 ; CodeTable[i].code != -1; i++)
	    if (CodeTable[i].code == result)
		break;

	sprintf(buf, "%03d %d %s\n", result, resultlen, CodeTable[i].desc);
	len = strlen(buf);
	write(fd, buf, len);

	if (resultlen > 0)
	    write(fd, resultstr, resultlen);
}


ServerLog(fd, msg)

int	fd;
char*	msg;

{
	fprintf(stderr, "fd = %d: %s\n", fd, msg);
}
