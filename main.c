
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>


sem_t attendre_voiture;
sem_t attendre_camion;
pthread_mutex_t pont;

// Nombre de camions et voitures bloqués en attente d'accès au pont
int nb_camion=0;
int nb_voiture=0;

// Poids encore disponible sur le pont
int seuil=0;


void attendre(double poids)
{
    int delai_alea = (int) (poids*rand()/(RAND_MAX+1.0));
    if (delai_alea<1) delai_alea = 1;
    int milli_seconds = 1000 * delai_alea;
    clock_t start_time = clock();
}

void acceder_au_pont(int tonnes)
{
	pthread_mutex_lock(&pont);

	if(seuil+tonnes <= 15)
		{
        // Passage véhicule
		seuil=seuil+tonnes;
		if (tonnes==5)
			sem_post(&attendre_voiture);
		else	sem_post(&attendre_camion);
		}
	else	{
        // Vehicule bloquée
		if (tonnes==5)
			nb_voiture++;
			// Camion bloqué
		else	nb_camion++;
		}

	pthread_mutex_unlock(&pont);

	if(tonnes==5)
		sem_wait(&attendre_voiture);
	else	sem_wait(&attendre_camion);
}



void quitter_le_pont(int tonnes)
{
	pthread_mutex_lock(&pont);


	// Liberation de la ressource
	seuil=seuil-tonnes;


		// Il y a un camion bloqué et on peut le libérer?
	if ( (seuil==0) && (nb_camion!=0) )
		{
		nb_camion--;
		sem_post(&attendre_camion);
		seuil=15;
		}
	else
		   // Libérer les voitures bloquées jusqu'à atteindre
		   // le seuil de la capacité du pont, et s'il n'y a pas
		   // un camion bloqué

		while ( (seuil < 15) && (nb_voiture>0) && (nb_camion==0) )
			{
			nb_voiture--;
			seuil=seuil+5;
			sem_post(&attendre_voiture);
			}

	pthread_mutex_unlock(&pont);
}


//  pthread_self() function returns the ID of the thread in which it is invoked
void* voiture(void* arg)
{
	acceder_au_pont(5);
	printf("Voiture %d : je traverse le pont\n",pthread_self());
	attendre(5.0);
	printf("Voiture %d : je quitte le pont\n",pthread_self());
	quitter_le_pont(5);
	pthread_exit(NULL);
}

void* camion(void* arg)
{
	acceder_au_pont(15);
	printf("Camion %d : je traverse le pont\n",pthread_self());
	attendre(5.0);
	printf("Camion %d : je quitte le pont\n",pthread_self());
	quitter_le_pont(15);


	pthread_exit(NULL);
}
void attendre(double max);




int main(int argc, char* argv[])
{
	int i;
	pthread_t id;

	sem_init(&attendre_voiture,0,0);
	sem_init(&attendre_camion,0,0);
	pthread_mutex_init(&pont,0);

        for(i =0; i< 5; i++)
                {
                pthread_create(&id, NULL, camion, NULL);

                pthread_create(&id, NULL, voiture, NULL);
                pthread_create(&id, NULL, voiture, NULL);
                pthread_create(&id, NULL, voiture, NULL);
                pthread_create(&id, NULL, voiture, NULL);
                }

	pthread_exit(NULL);

}



