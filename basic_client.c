#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  while (42) {

    char input[1000];

    printf("enter input: ");
    fgets(input, 1000, stdin);

    write(to_server, input, strlen(input));

    read(from_server, input, sizeof(input));

    printf("response: %s\n", input);

  }

}
