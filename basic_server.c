#include "pipe_networking.h"

int main() {

  int to_client;
  int from_client;

  // from_client = server_handshake( &to_client );

  while (42) {

    from_client = server_setup();


    int f = fork();

    if (f == 0) {

      to_client = server_connect(from_client);

      while (42) {

        char input[1000];

        int read_call = read(from_client, input, sizeof(input));

        if (read_call != 0) {

          printf("[subserver] disc");

          close(to_client);
          close(from_client);

          return -1;
        }

        else {
          int i;
          for (i = 0; i < strlen(input); i++) {
            input[i] = toupper(input[i]);
          }

          write(to_client, input, strlen(input));
        }

      }

    }

    else {
      return -1;
    }

  }

  return 0;

}
