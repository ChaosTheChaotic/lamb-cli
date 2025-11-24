#ifndef KEYS_H
#define KEYS_H

int store_pwd(const char *restrict pwd);

char *get_pwd();

int clear_pwd();

int prompt_pwd(const char *restrict prompt, char *restrict pbuf);

#endif
