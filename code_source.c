//Rostom BACCAR 3MIC C
/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>
//FONCTIONS DE CREATION DES MESSAGES A ENVOYER
void construire_message(char*message,char motif,int lg){
    int i;
    for (i=0;i<lg;i++){
        message[i]=motif;
    }
}

void afficher_message(char*message,int lg){
    int i;
    for (i=0;i<lg;i++){
        printf("%c",message[i]);
    }
}
int main (int argc, char **argv){
    int c;
    extern char *optarg;
    extern int optind;
    int nb_message=-1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    int source=-1; /* 0=puits, 1=source */
    int tp=-1; /*0=tcp,1=udp */
    int lg=30; /*valeur par defaut de la longueur des messages lus*/
    int port=atoi(argv[argc-1]); //stockage du numéro de port
    port=htons(port);
    char*destinataire=(argv[argc-2]); //stockage du destinataire
    while ((c=getopt(argc,argv,"pn:sul:"))!=-1){
        switch (c){
        case 'p':
            if (source==1){
                printf("usage: cmd [-p|-s][-n ##]\n");
                exit(1);
            }
            source=0;
            break;
        case 's':
            if (source==0){
                printf("usage: cmd [-p|-s]][-n ##]\n");
                exit(1);
            }
            source=1;
            break;
        case 'n':
            nb_message=atoi(optarg);
            break;
        case'l' :
            lg=atoi(optarg); //si non renseignée, la variable lg reste initialisée à 30
            break;
        case 'u':
            if (tp==1){
                printf("usage: cmd [-p|-s]][-n ##]\n");
                exit(1);
            }
            tp=1;
            break;
            printf("usage: cmd [-p|-s][-n ##]\n");
            break;
        default:
            printf("usage: cmd [-p|-s][-n ##]\n");
            break;
        }
    }
    if (source==-1){
        printf("usage: cmd [-p|-s][-n ##]\n");
        exit(1);
    }
    if (source==1){ 
/*---------------------SOURCE----------------------*/
//CODE SOURCE UDP & TCP
		printf("on est dans la source\n");
    if (tp==1){ 
/*---------------------UDP SOURCE----------------------*/
    //Communication par socket UDP: EMETTEUR
    //Création d'un socket local
    int sock;                   //SOURCE UDP
    if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){ 
        printf("échec de la création du socket\n");
        exit(1); 
        }
    //Pas besoin de création d'une adresse externe pour le socket local et d'un bind car
    //on est émetteur
    struct hostent*hp;
    struct sockaddr_in adr_distant;
    //Affectation domaine et n° de port
    memset((char*)& adr_distant,0,sizeof(adr_distant)); //Reset de la structure: facultatif
    adr_distant.sin_family=AF_INET; // domaine Internet
    adr_distant.sin_port=port; //n° de port. Si valeur=0: 1er num port disponible
            //Affectation adresse IP
    if ((hp=gethostbyname(destinataire))==NULL){
        printf("erreur gethostbyname\n");
        exit(1);
        }
    memcpy(((char*)&adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);
    //Envoi du message à un socket distant dont on connait l'adresse (adresse IP etnum port)
            printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=udp, dest=%s\n",lg,port,nb_message,destinataire);
            char *message=(char*)malloc(sizeof(char));
            char motif='a';
        for (int i=0;i<nb_message;i++){ //boucle permettant de construire un message différent à chaque itération par le changement du motif à chaque fois (motif incrémenté)
            construire_message(message,motif,lg); //on envoie message par message
            if(sendto(sock,message,lg,0,(struct sockaddr*) &(adr_distant),sizeof(adr_distant))==-1){
                printf("échec du sendto()\n");
                exit(1);
            }
            printf("SOURCE: Envoi n°%d (%d) [%d ",i+1,lg,i+1);
            afficher_message(message,lg);
            printf("]\n"); 
            motif++; 
            if(motif>'z'){ //si motif depasse 'z' on reboucle sur 'a'
                motif='a';
            }
        }
        printf("SOURCE: FIN\n");
        //Destruction du socket
        if (close(sock)==-1){
            printf("échec de la destruction du socket\n");
            }
    }
    else {                   
/*---------------------TCP SOURCE----------------------*/
    //Création d'un socket local
    int sock;
		if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1){
			printf("échec de la création du socket\n");
			exit(1);
		}
            //Construction de l'adresse du socket distant (auquel on souhaite s'adresser)
            //Déclarations
            struct hostent *hp;
            struct sockaddr_in adr_distant;
            //Affectation domaine et n° de port
            memset((char *)&adr_distant,0,sizeof(adr_distant)); //Reset de la structure: facultatif
            adr_distant.sin_family=AF_INET; // domaine Internet
            adr_distant.sin_port=port; //n° de port. Si valeur=0: 1er num port disponible
            //Affectation adresse IP
            if ((hp=gethostbyname(destinataire))==NULL){
                printf("erreur gethostbyname\n");
                exit(1);
            }
            memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);
            //Connexion au serveur
            if (connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
                printf("échec du connect\n");
                exit(1);
            }
        printf("SOURCE: lg_mesg_emis=%d, port =%d, nb_envois=%d, TP=tcp, dest=%s\n",lg,port,nb_message,destinataire);
        
    //construction du message
        char *message=(char*)malloc(sizeof(char)*lg);
        char motif='a';
        for(int i=0; i<nb_message; i++){ //boucle permettant de construire un message différent à chaque itération par le changement du motif à chaque fois (motif incrémenté)
            construire_message(message,motif,lg);
            if (write(sock,message,lg)==-1){ //on envoie message par message
                    printf("échec du write\n");
                    exit(1);
            }
            printf("SOURCE: Envoi n°%d (%d) [%d ",i+1,lg,i+1);
            afficher_message(message,lg);
            printf("]\n");
            motif++;
            if (motif >='z'){ //si motif depasse 'z' on reboucle sur 'a'
                motif='a';
            }
        }
        printf("SOURCE: fin\n");
            //Primitive shutdown: pour ne plus émettre et ne plus recevoir
            if(shutdown(sock,2)==-1 ){
                printf("échec du shutdown\n");
            }
            //destruction du socket créé
            if(close(sock==-1)){
                printf("échec de la destruction du socket\n");
                exit(1);
            }
        }
    }
    else {
/*---------------------PUITS----------------------*/
        printf("on est dans le puits\n"); 
        //CODE PUITS UDP & TCP
    if (tp==1){
           /*---------------------UDP PUITS----------------------*/
            //Communication par socket UDP: RECEPTEUR
            //Construire l'adresse d'un socket local (au récepteur) + association
            //Déclarations
            struct sockaddr_in adr_local;
            //Affectation domaine et n° de port
            memset((char *)& adr_local,0,sizeof(adr_local));  //Reset de la structure: facultatif
            adr_local.sin_family=AF_INET; // domaine Internet
            adr_local.sin_port=port; //n° de port
            //Affectation adresse IP
            adr_local.sin_addr.s_addr=INADDR_ANY; //si la machine possède plus d'une carte réseau:
            //affectation aléatoire (ex ethernet ou wifi)
            //Création d'un socket local
            int sock;
            if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){ 
                printf("échec de la création du socket\n");
                exit(1);
            }
            //Association de cette adresse créée au socket créé
            int lg_adr_local=sizeof(adr_local);
            if (bind(sock,(struct sockaddr*)&adr_local,lg_adr_local)==-1){
                printf("échec du bind\n");
                exit(1);
            }
            //Reception du message
            printf("PUITS: lg_mesg-lu=%d, port =%d, nb_receptions=%d, TP=udp\n",lg,adr_local.sin_port,nb_message);
            char * message_recu=(char*)malloc(sizeof(char)*lg); //adresse où le message reçu sera stocké
            unsigned int lg_message_recu=sizeof(adr_local); //longueur du message reçu
            int i=0; //num message
            while(1){
                if (recvfrom(sock,message_recu,lg,0,(struct sockaddr*)&adr_local,&lg_message_recu)==-1){
                    printf("échec du receive\n");
                }
                printf("PUITS: Reception n°%d (%d) [%d ",i+1,lg,i+1);
                afficher_message(message_recu,lg);
                printf("]\n");
                i++;
            }
            //Destruction du socket
            if (close(sock)==-1){
                printf("échec de la destruction du socket\n");
            }
    }
    else {
/*---------------------TCP PUITS----------------------(SANS SOUS-TRAITANCE)*/

            //Communication par socket UDP: RECEPTEUR
            //Construire l'adresse d'un socket local (au récepteur) + association
            //Déclarations
            struct sockaddr_in adr_local;
            //Affectation domaine et n° de port
            memset((char *)& adr_local,0,sizeof(adr_local));  //Reset de la structure: facultatif
            adr_local.sin_family=AF_INET; // domaine Internet
            adr_local.sin_port=port; //n° de port
            //Affectation adresse IP
            adr_local.sin_addr.s_addr=INADDR_ANY; //si la machine possède plus d'une carte réseau:
            //affectation aléatoire (ex ethernet ou wifi)
            //Création d'un socket local
            int sock;
            if((sock=socket(AF_INET,SOCK_STREAM,0))==-1){ 
                printf("échec de la création du socket\n");
                exit(1);
            }
            //Association de cette adresse créée au socket créé
            int lg_adr_local=sizeof(adr_local);
            if (bind(sock,(struct sockaddr *)&adr_local,lg_adr_local)==-1){
                printf("échec du bind\n");
                exit(1);
            }
    //Réception du message
        printf("PUITS: lg_mesg-lu=%d, port =%d, nb_receptions=%d,TP=tcp\n",lg,port,nb_message);
        //Primitive listen: permet de dimenssionner la file d'attente des demandes de connexion
        if (listen(sock,5)==-1){ 
            printf("échec du listen\n");
            exit(1);
        }
        //Acceptation de la demande de connexion
        struct sockaddr_in padr_client;
        unsigned int plg_adr_client=sizeof(padr_client);
        int acpt;
        if((acpt=accept(sock,(struct sockaddr *)&padr_client,&plg_adr_client))==-1){
            printf("échec du accept\n");
            exit(1);
        }
        char* message=(char*)malloc(sizeof(char)*lg);
        int i=0; //num message
        int rcv; 
        //Réception messages
        while((rcv=recv(acpt,message,lg,0))){
            if (rcv==-1){
                printf("échec du recieve\n");
                exit(1);
            }
            printf("PUITS: Reception n°%d (%d) [%d ",i+1,lg,i+1);
            afficher_message(message,lg);
            printf("]\n");
            i++;
        }
            //Primitive shutdown: pour ne plus émettre et ne plus recevoir
            if(shutdown(sock,2)==-1){
                printf("échec du shutdown\n");
            }
            //destruction du socket créé
            if(close(sock==-1)){
                printf("échec de la destruction du socket\n");
                exit(1);
            }
        }   
    }
}














