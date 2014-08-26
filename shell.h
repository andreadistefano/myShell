#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>

#define N 1024
#define MAXHIST 20
#define QUOTE 39

typedef struct historyItem {
	int index;
	char **com;
	struct historyItem *next;
} historyItem;

typedef struct history {
	int count;
	int maxdim;
	historyItem *head;
	historyItem *tail;
} history;

typedef struct aliasItem {
	char *com1;					//Comando che viene inserito
	char **com2;				//Comando che viene eseguito
	struct aliasItem *next;
} aliasItem;

typedef struct aliasTable {
	int count;
	aliasItem *head;
} aliasTable;

// Prototipi delle funzioni
char ** getInput(char **);
char ** getInputAlias(char *, char **);

void cd(char **, struct passwd *);

char ** bangHistory(char **, history *);
void printHistory(history *);
void scalaHistory(history *);
history * newHistory(history *);
void insertHistoryItem(history *, char **);

aliasTable * newAliasTable(aliasTable *);
aliasTable * newAlias(aliasTable *, char **);
void gestisciAlias(aliasTable *, char **);
char ** checkAlias(char **, aliasTable *); 
aliasTable * unalias(aliasTable *, char **);
int isAlias (aliasTable *, char *);
