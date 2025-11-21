#include <strings.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * A function which parses the arguments for the type of subcommand the binary is running
 *
 * @param argc the argument count
 * @param argv an array of all the arguments
 * @return false on an invalid arg else true
*/
bool parseArgs(int argc, char **argv) {
  for (unsigned i = 0; i < argc; i++) {
  }
  return true;
}

/**
  * A function which parses the type of subcommand the binary should run
  *
  * @param argc the amount of args (the argument count)
  * @param argv an array of all the arguments
  * @return a boolean which is true if we found a valid subcommand else false
*/
bool parseType(int argc, char **argv) {
  for (unsigned i = 0; i < argc; i++) {
    if (strcasecmp(argv[i], "send") == 0) return true;
    if (strcasecmp(argv[i], "request") == 0) return true;
  }
  return false;
}

int main(int argc, char **argv) {
  bool succ = parseType(argc, argv);
  if (!succ) {
    printf("Failed to get a valid subcommand of lambc");
    return 1;
  } else {
    printf("Completed operation");
  }
  return 0;
}
