/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <signal.h>
#include <net/if.h>
/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#define BUFFSIZE 1518
#define ETHERTYPE_LEN 2
#define MAC_ADDR_LEN 6


typedef unsigned char MacAddress[MAC_ADDR_LEN];
extern int errno;
// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

  unsigned char buff1[BUFFSIZE]; // buffer de recepcao
  extern int errno;

  int sockd;
  int on;
  struct ifreq ifr;




  struct fluxo {
    int          cont;
  	int          ip_org;
  	int          ip_dest;
  	int          porta_o;
  	int          porta_d;
  	int          protocolo;
    int          preenchido;
    int          banido;
  };

struct fluxo f[15];


void preenche()
{
  int i;
  for(i=0;i<15;i++)
  {
    f[i].ip_org = 0;
    f[i].ip_dest = 0;
    f[i].protocolo = 0;
    f[i].porta_o = 0;
    f[i].porta_d = 0;
    f[i].cont = 0;
    f[i].preenchido = 0;
    f[i].banido = 0;
  }

}

int verificafluxo(int ip, int ip2, int t, int p, int portadest)
{
  int i=0;
  int flag=0;

  //printf("%x ", ip);
  //printf("%x ", ip2);
  //printf("%d ", t);
  //printf("%d ", p);
  //printf("%d \n", portadest);

  //printf("%x | %x | %d | %d | %d\n", f[0].ip_org, f[0].ip_dest, f[0].protocolo, f[0].porta_o, f[0].porta_d);
  //printf("entrei verifica\n" );

  while(f[i].preenchido != 0){
    //printf("entrei verifica while %d\n",i );

    //if((ip == f[i].ip_org) && (ip2 == f[i].ip_dest) && (t == f[i].protocolo) && (p == f[i].porta_o) && (portadest == f[i].porta_d))
    //{
    if((ip == f[i].ip_org) && (ip2 == f[i].ip_dest) && (t == f[i].protocolo) && (portadest == f[i].porta_d))
      {
      //printf("entrei verifica if1 %d\n",i );
        f[i].cont = f[i].cont +1;
        flag = 1;
        //printf("%d", flag);
    }
    i++;
  }
  //printf("entrei verifica fora while %d\n",i );
  if(flag == 0)
  {
    //printf("entrei verifica if2 %d\n",i );
      f[i].ip_org = ip;
      f[i].ip_dest = ip2;
      f[i].protocolo = t;
      f[i].porta_o = p;
      f[i].porta_d = portadest;
      //printf("%d\n", portadest);
      //printf("%d\n", f[i].porta_d);
      f[i].cont = 1;
      f[i].preenchido = 1;
      //printf("%x | %x | %d | %d | %d\n", f[i].ip_org, f[i].ip_dest, f[i].protocolo, f[i].porta_o, f[i].porta_d);
  }

  return flag;

}


void printaStruct()
{
  int i=0;
  system("clear");
  printf("fluxo | IP origem | IP destino | Protocolo | Porta Orig | Porta Dest | Qtde. Pacotes | banido\n");
  while(f[i].preenchido != 0)
  {
    printf("  %d   |  %x  |  %x  |     %d     |    %d    |   %d    |      %d      |   %d     \n\n",i, f[i].ip_org, f[i].ip_dest, f[i].protocolo, f[i].porta_o, f[i].porta_d, f[i].cont, f[i].banido);
    i++;
  }

}


int testabanido(int ip, int ip2, int t, int p, int portadest)
{
  int i=0;
  while(f[i].preenchido != 0){
    //printf("  %d   |  %x  |  %x  |     %d     |    %d    |   %d    |      %d      |   %d     \n\n",i, ip, ip2, t, p, portadest, 707, -1);
    //printf("  %d   |  %x  |  %x  |     %d     |    %d    |   %d    |      %d      |   %d     \n\n",i, f[i].ip_org, f[i].ip_dest, f[i].protocolo, f[i].porta_o, f[i].porta_d, f[i].cont, f[i].banido);
    if((ip == f[i].ip_org) && (ip2 == f[i].ip_dest) && (t == f[i].protocolo) && (portadest == f[i].porta_d) && (f[i].banido == 1))
    {
      //printf("entrei\n");
      return 1;
    }
    i++;
  }
  return 0;
}

int main(int argc,char *argv[])
{

  int retValue = 0;
  char buffer[BUFFSIZE], dummyBuf[50];
  struct sockaddr_ll destAddr;
  short int etherTypeT = htons(0x0800);

  /* Configura MAC Origem e Destino */
  MacAddress destMac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  MacAddress localMac = {0x00, 0x00, 0x00, 0x0a, 0x00, 0x04};



    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
       printf("Erro na criacao do socket.\n");
       exit(1);
    }

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);
  int p=0;
  int portadest=0;

  //char ifname[IFNAMSIZ];
  //strcpy(ifname, argv[1]);
  //memset(&ifr, 0, sizeof (struct ifreq));
	//strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	//if (ioctl(sockd, SIOCGIFINDEX, &ifr) < 0) {
		//perror("SIOCGIFINDEX");
		//exit(1);
	//}



  	struct ifreq if_idx;
  	struct ifreq if_mac;
  	struct sockaddr_ll socket_address;
  	char ifname[IFNAMSIZ];
  	int frame_len = 0;

	strcpy(ifname, argv[1]);
  /* Obtem o indice da interface de rede */
  	memset(&if_idx, 0, sizeof (struct ifreq));
  	strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
  	if (ioctl(sockd, SIOCGIFINDEX, &if_idx) < 0) {
  		perror("SIOCGIFINDEX");
  		exit(1);
  	}

  	/* Obtem o endereco MAC da interface local */
  	memset(&if_mac, 0, sizeof (struct ifreq));
  	strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
  	if (ioctl(sockd, SIOCGIFHWADDR, &if_mac) < 0) {
  		perror("SIOCGIFHWADDR");
  		exit(1);
  	}

  	/* Indice da interface de rede */
  	socket_address.sll_ifindex = if_idx.ifr_ifindex;

  	/* Tamanho do endereco (ETH_ALEN = 6) */
  	socket_address.sll_halen = ETH_ALEN;

  	/* Endereco MAC de destino */
  	memcpy(socket_address.sll_addr, destMac, MAC_ADDR_LEN);





int t,ip, ip2;
unsigned char ipban[3];
int fluxoban;
int cont=0;
int op = 3;
int b;
int banir=0;
int vf;
preenche();

	// recepcao de pacotes
while (1)
{
  if(cont<5)
  {

   		t = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		// impress�o do conteudo - exemplo Endereco Destino e Endereco Origem
		//printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff1[0],buff1[1],buff1[2],buff1[3],buff1[4],buff1[5]);
		//printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n\n", buff1[6],buff1[7],buff1[8],buff1[9],buff1[10],buff1[11]);
    if((buff1[12]==8)&&(buff1[13]==0))
    {
      ip2=(buff1[30]<<24)+(buff1[31]<<16)+(buff1[32]<<8)+(buff1[33]);
      ip=(buff1[26]<<24)+(buff1[27]<<16)+(buff1[28]<<8)+(buff1[29]);
      p=(buff1[34]<<8)+(buff1[35]);
      portadest=(buff1[36]<<8)+(buff1[37]);


      /*
      printf("IP Origem:  %d.%d.%d.%d \n\n", buff1[26],buff1[27],buff1[28],buff1[29]);
      printf("IP Destino:  %d.%d.%d.%d \n\n", buff1[30],buff1[31],buff1[32],buff1[33]);
      printf("Protocolo:  %d \n", buff1[23]);
      //p=(buff1[34]<<8)+(buff1[35]);
      printf("Porta Origem:  %d \n", p);
      //portadest=(buff1[36]<<8)+(buff1[37]);
      printf("Porta Destino:  %d \n\n", portadest);
      */
      //printf("IP: %d.%d.%d.%d \n\n", buff1[30],buff1[31],buff1[32],buff1[33]);
      //printf("IP: %x.%x.%x.%x \n\n", buff1[30],buff1[31],buff1[32],buff1[33]);
      //ip=(buff1[30]<<24)+(buff1[31]<<16)+(buff1[32]<<8)+(buff1[33]);
      //ip2=(buff1[26]<<24)+(buff1[27]<<16)+(buff1[28]<<8)+(buff1[29]);
      //printf("%d\n", ip);

      //if(((buff1[30]==ipban[0])&&(buff1[31]==ipban[1])&&(buff1[32]==ipban[2])&&(buff1[33]==ipban[3])))
      //{
        //banir = 1;
      //}
      if((ip != 0xa00000a) && (ip2 != 0xa00000a))
      {

        //if(((buff1[30]==ipban[0])&&(buff1[31]==ipban[1])&&(buff1[32]==ipban[2])&&(buff1[33]==ipban[3]))||(porta==portadest))
        //{
        banir = testabanido(ip, ip2, buff1[23], p, portadest);
        //printf("%d\n", banir);
        if(banir == 1){
          printf("BARRADO\n");
        }
        else
        {
          verificafluxo(ip, ip2, buff1[23], p, portadest);
          printaStruct();

          printf("EVIEI\n\n");
          buff1[0] = 0x00;
          buff1[1] = 0x00;
          buff1[2] = 0x00;
          buff1[3] = 0xaa;
          buff1[4] = 0x00;
          buff1[5] = 0x00;
        //if((retValue = sendto(sockd, buff1, 64, 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0) {
           //printf("ERROR! sendto() \n");
           //exit(1);
        //}

      	 buff1[6] = 0x00;
         buff1[7] = 0x00;
         buff1[8] = 0x00;
         buff1[9] = 0xaa;
         buff1[10] = 0x00;
         buff1[11] = 0x04;

         //printf("%d\n", t);
         //printf("%d\n", sizeof(buff1));
         //printf("%x %x %x \n", buff1[42],buff1[43],buff1[44]);
         //printf("%d \n", buff1[23]);
         if (sendto(sockd, buff1, t, 0, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0)
		     {
			        perror("send");
			        close(sockd);
			        exit(1);
		     }


        }
        }
        cont++;

    //}
  }
}
if(cont>=5)
{
  printf("Quer banir algum fluxo?(1 para sim e 0 para nao):");
  scanf("%d", &op);
  if(op == 0)
  {
    cont = 0;
  }
  if(op == 1)
  {
    printf("Qual fluxo deseja banir: ");
    scanf("%d", &fluxoban);
    //printf("%d\n", fluxoban);
    f[fluxoban].banido = 1;
    //printf("%d\n", f[fluxoban].banido);
    //for(b=0;b<4;b++){
      //printf("%d.", ipban[b]);
    //}
    cont = 0;
  }

  }
}

}
