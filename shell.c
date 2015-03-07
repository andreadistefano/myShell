#include "shell.h"

char ** getInput(char **comando) {
    int j;
    j = 0;
    char input[N];
    char delims[6];
    strcpy(delims, " \n'=");
    char *temp, *temp2;
    fgets(input, sizeof(input), stdin);
    temp2 = (char *) calloc(strlen(input) + 1, sizeof(char));
    strcpy(temp2, input);
    temp2 = strtok(temp2, delims);
    if (temp2 == NULL) {
        return(NULL);
    }
    if (strcmp(temp2, "alias") == 0) {
        return(comando = getInputAlias(input, comando));
    }
    else {
        strcpy(delims, " \n");
    }
    temp = strtok(input, delims);
    if (temp == NULL) {
        return(NULL);
    }
    comando[0] = (char *) calloc(strlen(temp) + 1, sizeof(char));
    strcpy(comando[0], temp);
    j++;
    while ((temp = strtok(NULL, delims)) != NULL) {
        comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
        comando[j] = (char *) calloc(strlen(temp) + 1, sizeof(char));
        strcpy(comando[j], temp);
        j++;
    }
    comando = (char **) realloc(comando, j * sizeof(char *));
    comando[j] = NULL;
    return(comando);
}

char ** getInputAlias(char *input, char **comando) {
    char *pos;
    pos = strchr(input, QUOTE);
    char delims[] = " \n'=";
    char *temp;
    int j;
    j = 0;
    temp = strtok(input, delims);
    comando = (char **) calloc(1, sizeof(char *));
    comando[0] = (char *) calloc(strlen(temp) + 1, sizeof(char));
    strcpy(comando[0], temp);
    j++;
    while ((temp = strtok(NULL, delims)) != NULL) {
        comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
        comando[j] = (char *) calloc(strlen(temp) + 1, sizeof(char));
        strcpy(comando[j], temp);
        if (j == 1) {
            strcpy(delims, " '\n");
            temp = strtok(pos, delims);
            comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
            comando[j + 1] = (char *) calloc(strlen(temp) + 1, sizeof(char));
            strcpy(comando[j + 1], temp);
            j++;
        }
        j++;
    }
    comando = (char **) realloc(comando, j * sizeof(char *));
    comando[j] = NULL;
    return(comando);
}

char ** bangHistory(char **comando, history *h) {
    int i = 0;
    int k = 0;
    int l = 0;
    historyItem *ptr;
    ptr = h->head;
    if (comando[0][1] == '!') {
        if (h->count == 0) {
            return(NULL);
        }
        i = 0;
        ptr = h->tail;
        printf("%d ", ptr->index);
        while (ptr->com[i] != NULL) {
            comando = (char **) realloc(comando, (i + 1) * sizeof(char *));
            comando[i] = (char *) calloc(strlen(ptr->com[i]) + 1, sizeof(char));
            strcpy(comando[i], ptr->com[i]);
            printf("%s ", comando[i]);
            i++;
        }
    }
    else {
        k = 1;
        while (comando[0][k] != '\0') {
            l = l * 10;
            l += comando[0][k] - 48;
            k++;
        }
        if ((h->count == 0) || (l > h->tail->index)) {
            printf("!%d: event not found.\n", l);
            return(NULL);
        }
        while(ptr->index < l) {
            ptr = ptr->next;
        }
        i = 0;
        printf("%d ", l);
        while (ptr->com[i] != NULL) {
            comando = (char **) realloc(comando, (i + 1) * sizeof(char *));
            comando[i] = (char *) calloc(strlen(ptr->com[i]) + 1, sizeof(char));
            strcpy(comando[i], ptr->com[i]);
            printf("%s ", comando[i]);
            i++;
        }
    }
    comando = (char **) realloc(comando, (i + 1) * sizeof(char *));
    comando[i] = NULL;
    printf("\n\n");
    return(comando);
}

void cd(char **comando, struct passwd *pw) {
    const char *homedir = pw->pw_dir;
    if (comando[1] == NULL || (strcmp(comando[1], "~") == 0)) {
        if (chdir(homedir) == -1) {
            perror("");
        }
    }
    else if (chdir(comando[1]) == -1) {
        perror("");
    }
    return;
}

void printHistory(history *h) {
    int i;
    if (h->count == 0) {
        printf("%d %s\n", 0, "history");
        return;
    }
    historyItem *ptr = h->head;
    do {
        i = 0;
        printf("%d ", ptr->index);
        while(ptr->com[i] != NULL) {
            printf("%s ", ptr->com[i]);
            i++;
        }
        printf("\n");
        ptr = ptr->next;
    } while (ptr != NULL);
    printf("%d %s\n", (h->tail->index + 1), "history");
    return;
}

void scalaHistory(history *h) {
    historyItem *ptr = h->head->next;
    h->head = ptr;
    h->count--;
    return;
}

history * newHistory(history *h) {
    h = (history *) calloc (1, sizeof(history));
    h->count = 0;
    h->maxdim = MAXHIST;
    h->head = NULL;
    h->tail = NULL;
    return(h);
}

void insertHistoryItem(history *h, char **comando) {
    int i = 0;
    historyItem *ptr;
    historyItem *item = (historyItem *) calloc(1, sizeof(historyItem));
    if (h->count < h->maxdim) {
        if(h->count == 0) {
            h->head = item;
            h->tail = item;
            item->index = 0;
        }
        else {
            ptr = h->head;
            while(i < h->count - 1) {
                ptr = ptr->next;
                i++;
            }
            item->index = h->tail->index + 1;
            ptr->next = item;
            h->tail = item;
        }
        i = 0;
        while (comando[i] != (char *) NULL) {
            item->com = (char **) realloc(item->com, (i + 1) * sizeof(char *));
            item->com[i] = (char *) calloc (strlen(comando[i]) + 1, sizeof(char));
            strcpy(item->com[i], comando[i]);
            i++;
        }
        item->com = (char **) realloc(item->com, (i + 1) * sizeof(char *));
        item->com[i] = NULL;
        item->next = NULL;
        (h->count)++;
    }
    else {
        scalaHistory(h);
        insertHistoryItem(h, comando);
    }
}

aliasTable * newAliasTable(aliasTable *at) {
    at = (aliasTable *) calloc(1, sizeof(aliasTable));
    at->count = 0;
    at->head = NULL;
    return(at);
}

void gestisciAlias(aliasTable *at, char **comando) {
    aliasItem *ptr = at->head;
    int i;
    int j;
    i = 0;
    if (comando[1] == NULL) {                                           //Stampa tutti gli alias
        while (i < at->count) {
            printf ("alias %s='", ptr->com1);
            j = 0;
            while (ptr->com2[j] != NULL) {
                printf("%s", ptr->com2[j]);
                j++;
                if (ptr->com2[j] == NULL) {
                    printf("'\n");
                    continue;
                }
                else {
                    printf(" ");
                }
            }
            i++;
            ptr = ptr->next;
        }
    }
    else if (isAlias(at, comando[1]) == 0 && comando[2] == NULL) {      // Esce se non esiste un alias per comando[1]
        printf("alias: %s: not found\n", comando[1]);
        return;
    }
    else if (isAlias(at, comando[1]) == 1 && comando[2] == NULL) {      // Stampa solo l'alias che contiene comando[1]
        while (i < at->count) {
            if(strcmp(ptr->com1, comando[1]) == 0) {
                printf ("alias %s='", ptr->com1);
                j = 0;
                while (ptr->com2[j] != NULL) {
                    printf("%s", ptr->com2[j]);
                    j++;
                    if (ptr->com2[j] == NULL) {
                        printf("'\n");
                        continue;
                    }
                    else {
                        printf(" ");
                    }
                }
            }
            i++;
            ptr = ptr->next;
        }
    }
    else {
        at = newAlias(at, comando);
    }
    return;
}

aliasTable * newAlias(aliasTable *at, char **comando) {
    int i;
    int j;
    aliasItem *ptr = at->head;
    if (isAlias(at, comando[1]) == 1) {
        while (strcmp(ptr->com1, comando[1]) != 0) {
            ptr = ptr->next;
        }
        i = 2;
        j = 0;
        while (comando[i] != NULL) {
            ptr->com2 = (char **) realloc(ptr->com2, (i + 1) * sizeof(char *));
            ptr->com2[j] = (char *) calloc(strlen(comando[i]) + 1, sizeof(char));
            strcpy(ptr->com2[j], comando[i]);
            i++;
            j++;
        }
        ptr->com2 = (char **) realloc(ptr->com2, (i + 1) * sizeof(char *));
        ptr->com2[i] = NULL;
    }
    else {
        aliasItem *item = (aliasItem *) calloc(1, sizeof(aliasItem));
        item->com1 = (char *) calloc(strlen(comando[1]) + 1, sizeof(char));
        strcpy(item->com1, comando[1]);
        item->next = NULL;
        i = 0;
        if (at->count == 0) {
            at->head = item;
        }
        else {
            while (i < at->count - 1) {
                ptr = ptr->next;
                i++;
            }
            ptr->next = item;
        }
        at->count++;
        item->com2 = (char **) calloc(1, sizeof(char *));
        i = 2;
        j = 0;
        while (comando[i] != NULL) {
            item->com2 = (char **) realloc(item->com2, (i + 1) * sizeof(char *));
            item->com2[j] = (char *) calloc(strlen(comando[i]) + 1, sizeof(char));
            strcpy(item->com2[j], comando[i]);
            i++;
            j++;
        }
        item->com2 = (char **) realloc(item->com2, (i + 1) * sizeof(char *));
        item->com2[i] = NULL;
    }
    return(0);
}

char ** checkAlias(char **comando, aliasTable *at) {
    if (isAlias(at, comando[0]) == 0) {
        return(comando);
    }
    aliasItem *aliasptr = at->head;
    int i, j;
    i = 1;
    j = 0;
    char **temp;
    temp = (char **) calloc(1, sizeof(char *));
    if (comando[i] != NULL) {
        while (comando[i] != NULL) {
            temp = (char **) realloc(temp, (j + 1) * sizeof(char *));
            temp[j] = (char *) calloc (strlen(comando[i]) + 1, sizeof(char));
            strcpy(temp[j], comando[i]);
            i++;
            j++;
        }
        temp = (char **) realloc(temp, (j + 1) * sizeof(char *));
        temp[j] = NULL;
    }
    else {
        temp = NULL;
    }
    i = 0;
    j = 0;
    while (i < at->count) {
        if (strcmp(comando[0], aliasptr->com1) == 0) {
            while (aliasptr->com2[j] != NULL) {
                comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
                comando[j] = (char *) calloc(strlen(aliasptr->com2[j]) + 1, sizeof(char));
                strcpy(comando[j], aliasptr->com2[j]);
                j++;
            }
            if (temp != NULL) {
                i = 0;
                while(temp[i] != NULL) {
                    comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
                    comando[j] = (char *) calloc(strlen(temp[i]) + 1, sizeof(char));
                    strcpy(comando[j], temp[i]);
                    i++;
                    j++;
                }
            }
            comando = (char **) realloc(comando, (j + 1) * sizeof(char *));
            comando[j] = NULL;
            return(comando);
        }
        aliasptr = aliasptr->next;
        i++;
    }
    return(comando);
}

aliasTable * unalias(aliasTable *at, char **comando) {
    aliasItem *aliasptr = at->head;
    aliasItem *tempptr;
    int i = 0;
    if (i == 0) {
            if (strcmp(aliasptr->com1, comando[1]) == 0) {
                at->head = at->head->next;
                (at->count)--;
                return(at);
            }
            else {
                i++;
            }
        }
    while (i < at->count) {
        if (strcmp(aliasptr->next->com1, comando[1]) == 0) {
            tempptr = aliasptr->next;
            aliasptr->next = aliasptr->next->next;
            free(tempptr);
            (at->count)--;
            return(at);
        }
        aliasptr = aliasptr->next;
        i++;
    }
    printf("unalias: %s: not found\n", comando[1]);
    return(at);
}

int isAlias (aliasTable *at, char *com) {
    aliasItem *aliasptr = at->head;
    int i = 0;
    while (i < at->count) {
        if (strcmp(aliasptr->com1, com) == 0) {
            return(1);
        }
        else {
            aliasptr = aliasptr->next;
            i++;
        }
    }
    return(0);
}