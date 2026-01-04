#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    int fd_PadreAFiglio[2]; //file descriptor -> 0 = leggere, 1 = scrivere
    int fd_FiglioAPadre[2];

    pid_t ritorno_fork1; // da padre a figlio
    pid_t ritorno_fork2; // da figlio a padre
    char *n;

    if (pipe(fd_PadreAFiglio) == -1 || pipe(fd_FiglioAPadre) == -1)
    {
        perror("Errore nella creazione delle pipe");
        exit(-1);
    }

    printf("OH OH OH! Sono Babbo Natale e ho bisogno del tuo aiuto "
           "per sapere quanti bambini buoni e cattivi ci sono al mondo. "
           "Inserisci il numero dei bambini: \t");
    fgets(n, 50, stdin);
    int numero = atoi(n);

    //Primo figlio -> elfo 1, bambini buoni
    ritorno_fork1 = fork();

    //figlio
    if(ritorno_fork1  == 0)
    {
        close(fd_FiglioAPadre[0]);  // chiudo lettura in daFiglioAPadre perché è elfo
                                    // che deve scrivere a Babbo
        close(fd_PadreAFiglio[1]);  // chiudo scrittura in daPadreAFiglio perché è Babbo
                                    // che legge il n° di bambini

        int nRicevuto, bambiniBuoni = 0;

        read(fd_PadreAFiglio[0], &nRicevuto, sizeof(nRicevuto)); //si lavora con indirizzi

        for(int i = 1; i <= nRicevuto; i ++)
        {
            if(i%2 != 0)
            {
                bambiniBuoni += 1;
            }
        }

        write(fd_FiglioAPadre[1], &bambiniBuoni, sizeof(bambiniBuoni));

        close(fd_FiglioAPadre[1]);  // per sicurezza chiudo la scrittura dopo che ho fatto tutto

        exit(0);
    }

    //Secondo figlio -> elfo 2, bambini cattivi
    ritorno_fork2 = fork();
    if(ritorno_fork2  == 0)
    {
        close(fd_FiglioAPadre[0]);
        close(fd_PadreAFiglio[1]);

        int nRicevuto, bambiniCattivi = 0;

        read(fd_PadreAFiglio[0], &nRicevuto, sizeof(nRicevuto));

        for(int i = 1; i <= nRicevuto; i ++)
        {
            if(i%2 == 0)
            {
                bambiniCattivi += 1;
            }
        }

        write(fd_FiglioAPadre[1], &bambiniCattivi, sizeof(bambiniCattivi));

        close(fd_FiglioAPadre[1]);

        exit(0);
    }
    //processo padre
    else
    {
        close(fd_FiglioAPadre[1]);  //chiudo scrittura perché padre deve leggere
        close(fd_PadreAFiglio[0]);  //chiudo lettura perché Babbo deve dire il n° di bambini

        write(fd_PadreAFiglio[1], &numero, sizeof(numero)); //scrivo 2 volte perché ci sono
        write(fd_PadreAFiglio[1], &numero, sizeof(numero)); //2 processi figli

        int ris1, ris2; //non so quale elfo scrive per primo, però in teoria ris1
                        //dovrebbe rappresentare i bambini buoni
                        //si potrebbe sistemare con 3 pipe per distinguere i figli,
                        //o 4 per scrivere ai figli senza confusione

        read(fd_FiglioAPadre[0], &ris1, sizeof(ris1)); //dispari = buono
        read(fd_FiglioAPadre[0], &ris2, sizeof(ris2)); //pari = cattivo

        wait(NULL);

        printf("\nBabbo Natale deve consegnare %d regali e %d carbone", ris1, ris2);
    }


    return 0;
}