#ifndef PARSER_H
#define PARSER_H

#include "subcmd.h"

bool parse_type(int argc, char** argv, SubCommand* result);
bool parse_args(int argc, char** argv, SubCommand* subcmd);

#endif
