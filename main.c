#include "shell.h"

extern int errno;

int main () {
	
	// Dichiarazione delle variabili
	int i, pid, status;
	char *pwd;
	char **comando;
	struct passwd *pw = getpwuid(getuid());
	char hostname[HOST_NAME_MAX+1];
	gethostname(hostname, sizeof(hostname));
	history *h;
	h = newHistory(h);
	aliasTable *at;
	at = newAliasTable(at);
	
	// Ciclo infinito
	while (1) {
		
		// Stampa del prompt
		pwd = (char *) get_current_dir_name();
		printf("%s@%s:%s $ > ", pw->pw_name, hostname, pwd);
		
		// Allocazione dell'array dinamico
		comando = (char **) calloc(1, sizeof(char *));
			
		// Parsing dell'input
		if ((comando = getInput(comando)) == NULL) {
			continue;
		}

		// Gestione di !
		if  (comando[0][0] == '!') {
			if ((comando = bangHistory(comando, h)) == NULL) {
				continue;
			}
		}
		
		// Controllo alias
		comando = checkAlias(comando, at);
			
		// Comandi di uscita
		if (strcmp(comando[0], "exit") == 0 || strcmp(comando[0], "quit") == 0 || strcmp(comando[0], "logout") == 0) {
			exit(0);
		}
		
		// Comando cd
		else if (strcmp(comando[0], "cd") == 0) {
			cd(comando, pw);
		}
		
		// Comando history
		else if (strcmp(comando[0], "history") == 0) {
			printHistory(h);
		}
		
		// Comando alias
		else if (strcmp(comando[0], "alias") == 0) {
			gestisciAlias(at, comando);
		}
		
		else if (strcmp(comando[0], "unalias") == 0) {
			at = unalias(at, comando);
		}
		
		// Esecuzione altri comandi
		else {
			i = fork();
			if (i < 0) {
				perror("");
				exit(1);
			}
			if (i == 0) {
				execvp(comando[0], comando);
				perror("");
				exit(1);
			}
			pid = wait(&status);
		}
		
		// History
		insertHistoryItem(h, comando);
	}
}