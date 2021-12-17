#include "pipe_networking.h"


/*=========================
  server_setup
  args:
  creates the WKP (upstream) and opens it, waiting for a
  connection.
  removes the WKP once a connection has been made
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {

  int mkfifo_call = mkfifo(WKP, 0600);

  if (mkfifo_call == -1) {
    printf("WKP: Not Created | %s\n", strerror(errno));
    return -1;
  }
  else printf("WKP: Created\n");

  int open_call = open(WKP, O_RDONLY);

  remove(WKP);
  printf("WKP: Removed\n");

  return open_call;

}

/*=========================
  server_connect
  args: int from_client
  handles the subserver portion of the 3 way handshake
  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {

  srand(time(NULL));
  int random = rand() % HANDSHAKE_BUFFER_SIZE;

  int input[1000];

  int read_call = read(from_client, input, sizeof(input));

  printf("[server] handshake received: -%s-\n", input);

  int open_call = open(input, O_WRONLY);

  sprintf(input, "%d", random);

  write(open_call, input, strlen(input));

  read(from_client, input, sizeof(input));

  int atoi_call = atoi(input);

  if (atoi_call != random+1) {
    printf("[server] handshake received bad ACK: -%s-\n", input);
    exit(0);
  }//bad response
  printf("[server] handshake received: -%s-\n", input);

  return open_call;

}


/*=========================
  server_handshake
  args: int * to_client
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int b, from_client;
  char buffer[HANDSHAKE_BUFFER_SIZE];

  printf("[server] handshake: making wkp\n");
  b = mkfifo(WKP, 0600);
  if ( b == -1 ) {
    printf("mkfifo error %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
  //open & block
  from_client = open(WKP, O_RDONLY, 0);
  //remove WKP
  remove(WKP);

  printf("[server] handshake: removed wkp\n");
  //read initial message
  b = read(from_client, buffer, sizeof(buffer));
  printf("[server] handshake received: -%s-\n", buffer);


  *to_client = open(buffer, O_WRONLY, 0);
  //create SYN_ACK message
  srand(time(NULL));
  int r = rand() % HANDSHAKE_BUFFER_SIZE;
  sprintf(buffer, "%d", r);

  write(*to_client, buffer, sizeof(buffer));
  //rad and check ACK
  read(from_client, buffer, sizeof(buffer));
  int ra = atoi(buffer);
  if (ra != r+1) {
    printf("[server] handshake received bad ACK: -%s-\n", buffer);
    exit(0);
  }//bad response
  printf("[server] handshake received: -%s-\n", buffer);

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {

  int from_server;
  char buffer[HANDSHAKE_BUFFER_SIZE];
  char ppname[HANDSHAKE_BUFFER_SIZE];

  //make private pipe
  printf("[client] handshake: making pp\n");
  sprintf(ppname, "%d", getpid() );
  mkfifo(ppname, 0600);

  //send pp name to server
  printf("[client] handshake: connecting to wkp\n");
  *to_server = open( WKP, O_WRONLY, 0);
  if ( *to_server == -1 ) {
    printf("open error %d: %s\n", errno, strerror(errno));
    exit(1);
  }

  write(*to_server, ppname, sizeof(buffer));
  //open and wait for connection
  from_server = open(ppname, O_RDONLY, 0);

  read(from_server, buffer, sizeof(buffer));
  /*validate buffer code goes here */
  printf("[client] handshake: received -%s-\n", buffer);

  //remove pp
  remove(ppname);
  printf("[client] handshake: removed pp\n");

  //send ACK to server
  int r = atoi(buffer) + 1;
  sprintf(buffer, "%d", r);
  write(*to_server, buffer, sizeof(buffer));

  return from_server;
}
