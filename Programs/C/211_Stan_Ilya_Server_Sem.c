#include<stdio.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<string.h>
//Server Shashki
void doskaOut1(char doska[65]);// Output of doska
int checkFerzWhite(char doska[65]);// 1 = white Ferz; changes the W on P
int bothOnField(char doska[65]);// Checks, if someone won; 0 => someone won
int canEatW(char doska[65]);// checks, if White can eat; 1 => can eat
int noBBetween(char doska[65],char turn[6]);// checks, if there is no Blacks on the way of P
void mov(char doska[65],char turn[6]);// moves a figure
int Eat(char doska[65],char turn[6]);// eats a peace and checks, if the eating move is correct
int canEat1(char doska[65],int coll, int ln);// checks, if Whites can eat with figure in [ln][coll]
int endFindWhite(char doska[65]);// checks, if there are any whites on field
int TwoInRaw(char doska[65],char turn[6]);// for correct eating with P: can't jump over two figures standing next to each other
int noWBetween(char doska[65], char turn[6]);// same as noBBetween, but for White
int abs(int x);

int main(){
	printf("Welcome to a game of shashki!\nThe basics:\n1) W and B are usual shashki,\nand P and K (Princess and King) are damki.\n2) The turn should be written, like: A1-B2.\n3) All the pointers on the doska should point at the '-' symbols.\nSo, to eat the shashka, you need to point the next cell\nafter the shashka.\n4) To finish the game, the player should enter \"End\", as a turn.\n5) This program does not cover all the endgames.\nSo, player2 and player1 should negotiate in that case\nand use the End command.\n__________________________\n");
	printf("Welcome, player1!\n");
	key_t key;
	int semid,shmid;
	struct sembuf sops;
	char *shmaddr;
	char doska[65];
	for(int i = 0; i<64; i++){// init (different inits in the comments below)
		if(i<=23)
			if((i+i/8)%2==1)
				doska[i] = 'B';
			else
				doska[i] = '_';
		else if(i<=39)
			if((i+i/8)%2==1)
				doska[i] = '-';
			else
				doska[i] = '_';
		else
			if((i+i/8)%2==1)
				doska[i] = 'W';
			else
				doska[i] = '_';
	}
	doska[64] = '\0';
	

//_________________________________________________________
	
	key = ftok("a.txt",'S');
	semid = semget(key,1,0666|IPC_CREAT);
	shmid = shmget(key,65,0666 | IPC_CREAT);
	shmaddr = shmat(shmid,NULL,0);
	semctl(semid,0,SETVAL,(int)0);
	sops.sem_num = 0;
	sops.sem_flg = 0;
	char turn[6];
	char initColl, initLn, nextColl, nextLn,tmpColl,tmpLn;
	
//______________________________________________________________________________ game

	while(bothOnField(doska)){
		printf("__________________________\n\nPlayer1\n\n");
		doskaOut1(doska);
		printf("\nEnter your move, please:\n");
MOV:		scanf("%s",turn);
		if(!strcmp(turn,"End")){
			printf("Immediate finish of the game!\n");
			sops.sem_op = 10;
			semop(semid,&sops,1);
			goto FIN;
		}
		initColl = turn[0]-'A'; initLn = 8-turn[1]+'0';
		nextColl = turn[3]-'A'; nextLn = 8-turn[4]+'0';
		//printf("%d %d %d %d\n",initColl,initLn,nextColl,nextLn);
		//check and make the move
		if(initColl == nextColl || initLn == nextLn || initColl < 0 || initLn < 0 || nextColl > 7 || nextLn > 7 || nextColl < 0 || nextLn < 0 || initColl > 7 || initLn > 7 || doska[initColl + initLn * 8] == '_' || doska[nextColl + nextLn * 8] == '_' || doska[initColl + initLn * 8] == '-' || (doska[initColl + initLn * 8] == 'W' && (((nextLn >= initLn) && abs(nextLn-initLn)==1) || abs(nextLn-initLn) > 2)) || doska[nextColl + nextLn * 8] != '-' || doska[initColl + initLn * 8] == 'B' || doska[initColl + initLn * 8] == 'K' || TwoInRaw(doska,turn)){
			printf("Incorrect move, try again!\n");
			goto MOV;
		}
		if(!canEatW(doska)){//nothing to eat
			if(doska[initColl + initLn * 8] == 'P'){
				if((abs(nextColl - initColl) != abs(nextLn - initLn)) && !noBBetween(doska,turn)){
					printf("Incorrect move, try again!\n");
					goto MOV;
				}
				else{
					mov(doska,turn);
				}
			}
			else if(doska[initColl + initLn * 8] == 'W'){
				if((abs(nextColl - initColl) != abs(nextLn - initLn)) || abs(nextLn - initLn) != 1){
					printf("Incorrect move, try again!\n");
					goto MOV;
				}
				else{
					mov(doska,turn);
				}
			}
			goto NEXT;
		}
		while(canEatW(doska)){//smthing to eat
			if(!Eat(doska,turn)){
				printf("Incorrect move.\nIf you can eat, you must eat!\nTry another move!\n");
				goto MOV;
			}
			Eat(doska,turn);
			checkFerzWhite(doska);
			doskaOut1(doska);
			tmpColl = nextColl;
			tmpLn = nextLn;
			if(canEat1(doska,tmpColl,tmpLn)){
				printf("\nEnter your eating move, please:\n");
MOV1:				scanf("%s",turn);
				if(!strcmp(turn,"End")){
					printf("Immediate finish of the game!\n");
					sops.sem_op = 10;
					semop(semid,&sops,1);
					goto FIN;
				}
				initColl = turn[0]-'A'; initLn = 8-turn[1]+'0';
				nextColl = turn[3]-'A'; nextLn = 8-turn[4]+'0';
				if(initColl == nextColl || initLn == nextLn || initColl < 0 || initLn < 0 || nextColl > 7 || nextLn > 7 || nextColl < 0 || nextLn < 0 || initColl > 7 || initLn > 7 || doska[initColl + initLn * 8] == '_' || doska[nextColl + nextLn * 8] == '_' || doska[initColl + initLn * 8] == '-' || (doska[initColl + initLn * 8] == 'W' && ((((nextLn >= initLn) && abs(nextLn-initLn)==1)) || abs(nextLn-initLn) > 2)) || doska[nextColl + nextLn * 8] != '-' || doska[initColl + initLn * 8] == 'B' || doska[initColl + initLn * 8] == 'K' || tmpColl!=initColl || tmpLn!= initLn || TwoInRaw(doska,turn)){
					printf("Incorrect move, try again!\n");
					if(tmpColl!=initColl || tmpLn!= initLn){
						printf("You've chosen the route, so you should stick to it.\n");
					}
					goto MOV1;
				}
			}
			else{
				break;
			}		
			
		}
NEXT:		printf("\n__________________________\nDoska for player1:\n");
		checkFerzWhite(doska);
		doskaOut1(doska);
		strcpy(shmaddr, doska);
		if(!bothOnField(doska)){// wins after he eats
			printf("__________________________\nPlayer1 won!\n");
			sops.sem_op = 3;
			semop(semid,&sops,1);
			goto FIN;
		}
// transfer info to another process
		sops.sem_op = 3;
		semop(semid,&sops,1);
		sops.sem_op = 0;
		printf("Waiting for player2...\n");
		semop(semid,&sops,1);//player1 is blocked here, while 0
		strcpy(doska,shmaddr);
		if(doska[0] == '?'){
			doska[0] = '_';
			printf("Player2 finished the game.\n");
			goto FIN;
		}
	}
//__________________________________________________________________End.

	printf("__________________________\nPlayer2 won!\n");
FIN:	doskaOut1(doska);
	
	shmdt(shmaddr);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID,(int)0);
	return 0;
}
/* FIELD
_|B|_|B|_|B|_|B| 7
B|_|B|_|B|_|B|_| 15
_|B|_|B|_|B|_|B| 23
-|_|-|_|-|_|-|_| 31

_|-|_|-|_|-|_|-| 39
W|_|W|_|W|_|W|_| 47
_|W|_|W|_|W|_|W| 55
W|_|W|_|W|_|W|_| 63


Some inits of doska to make the check easier:
1)for(int i = 0; i<64; i++){// init, player2 won!
	doska[i] = '-';
}
doska[64] = '\0';


2)for(int i = 0; i<64; i++){// init
	doska[i] = '-';
}
doska[1] = 'W';
doska[10] = 'B';
doska[64] = '\0';


3)for(int i = 0; i<64; i++){// init
	doska[i] = '-';
}
doska[11] = 'B';
doska[13] = 'B';
doska[25] = 'B';
doska[32] = 'B';
doska[22] = 'W';
doska[64] = '\0';


4)for(int i = 0; i<64; i++){// init
	doska[i] = '-';
}
doska[8] = 'W';
doska[26] = 'W';
doska[28] = 'B';
doska[35] = 'W';
doska[7] = 'K';
doska[56] = 'P';
doska[64] = '\0';


5)for(int i = 0; i<64; i++){// init
	doska[i] = '-';
}
doska[12] = 'B';
doska[14] = 'B';
doska[46] = 'B';
doska[30] = 'B';
doska[53] = 'W';
doska[64] = '\0';

*/

int abs(int x){
	if(x>=0)
		return x;
	else
		return -x;
}


void doskaOut1(char doska[65]){
	for(int i = 0; i<64; i++){
		if(i%8 == 0)
			printf("\n%d  |",8 - i/8);
		printf("%c|",doska[i]);
	}
	printf("\n\n    A B C D E F G H\n");
	return;
}


int checkFerzWhite(char doska[65]){
	for(int i = 0; i<8; i++){
		if(doska[i] == 'W'){// in one turn not more, then one Ferz appears
			doska[i] = 'P';
			return 1;
		}
	}
	return 0;
}


int bothOnField(char doska[65]){
	int cW = 0, cB = 0;
	for(int i = 1; i<63; i++){
		if(doska[i]=='B' || doska[i]=='K')
			cB = 1;
		else if (doska[i]=='W' || doska[i]=='P')
			cW = 1;
		if(cW && cB)
			return 1;
	}
	return 0;
}


int canEatW(char doska[65]){
	int k = 0;
	for(int i = 0; i<8;i++){
		for(int j = 0; j<8;j++){
		
			if(doska[i*8+j] != 'W' && doska[i*8+j] != 'P')
				continue;
		
			if(doska[i*8+j] == 'W'){
				if(i == 0 || i == 1){
					if(j == 7 || j == 6){
						if((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-'){
							return 1;
						}
					}
					else if (j>1 && j<6){
						if(((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-')){
							return 1;
						}
					}
					else{
						if((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-'){
							return 1;
						}
					}
				}
				
			
				
				else if(i == 7 || i == 6){
					if(j == 0 || j == 1){
						if((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-'){
							return 1;
						}
					}
					else if (j>1 && j<6){
						if(((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-')){
							return 1;
						}
					}
					else{
						if((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-'){
							return 1;
						}
					}
				}
		
				else{
					if(((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-') || ((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-')){
						return 1;
					}
					
				}
			}
			
			
			else if(doska[i*8+j] == 'P'){
				if(i == 0 || i == 1){
					if(j == 7 || j == 6){
						while(j-k-1>=0 && i+k+1<=7){
							if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else if (j>1 && j<6){
						while(j-k-1>=0 && i+k+1<=7){
							if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
						while(j+k+1<=7 && i+k+1<=7){
							if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') &&  doska[(i+k+1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else{
						while(j+k+1<=7 && i+k+1<=7){
							if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
				}
				
				
				
				else if(i == 7 || i == 6){
					if(j == 0 || j == 1){
						while(j+k+1<=7 && i-k-1>=0){
							if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else if (j>1 && j<6){
						while(j-k-1>=0 && i-k-1>=0){
							if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
						while(j+k+1<=7 && i-k-1>=0){
							if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else{
						while(j-k-1>=0 && i-k-1>=0){
							if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
				}
				else{
					while(j-k-1>=0 && i-k-1>=0){
						if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j+k+1<=7 && i-k-1>=0){
						if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j-k-1>=0 && i+k+1<=7){
						if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j+k+1<=7 && i+k+1<=7){
						if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
				}
			}
		}
	}
	return 0;
}

int noBBetween(char doska[65],char turn[6]){
	int j = turn[0]-'A', i = 8-turn[1]+'0', k = 1;
	int nextC = turn[3]-'A', nextL = 8-turn[4]+'0';
	if(i>nextL && j>nextC){
		while(j-k>nextC){
			if(doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K'){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i>nextL && j<nextC){
		while(j+k<nextC){
			if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j>nextC){
		while(j-k>nextC){
			if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j<nextC){
		while(j+k<nextC){
			if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K')){
				return 0;
			}
			k++;
		}
		
	}return 1;
}

int noWBetween(char doska[65],char turn[6]){
	int j = turn[0]-'A', i = 8-turn[1]+'0', k = 1;
	int nextC = turn[3]-'A', nextL = 8-turn[4]+'0';
	if(i>nextL && j>nextC){
		while(j-k>nextC){
			if(doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P'){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i>nextL && j<nextC){
		while(j+k<nextC){
			if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j>nextC){
		while(j-k>nextC){
			if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j<nextC){
		while(j+k<nextC){
			if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P')){
				return 0;
			}
			k++;
		}
		
	}return 1;
}

void mov(char doska[65],char turn[6]){
	int initColl = turn[0]-'A', initLn = 8-turn[1]+'0';
	int nextColl = turn[3]-'A', nextLn = 8-turn[4]+'0';
	doska[nextColl+nextLn*8] = doska[initColl+initLn*8];
	doska[initColl+initLn*8] = '-';
}


int Eat(char doska[65],char turn[6]){
	int initColl = turn[0]-'A', initLn = 8-turn[1]+'0', i = initLn, j = initColl;
	int nextColl = turn[3]-'A', nextLn = 8-turn[4]+'0';
	int k = 0, ctrl = 0;
	if(doska[initColl + initLn*8] == 'W'){
		if(abs(initLn - nextLn) != 2 || abs(initColl - nextColl) != 2)
			return 0;
		if(doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8]!='B' && doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8]!='K'){
			return 0;
		}
		doska[nextColl + nextLn*8] = 'W';
		doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8] = '-';
		doska[initColl + initLn*8] = '-';
	}
	else if(doska[initColl + initLn*8] == 'P'){
		if(abs(initLn - nextLn) != abs(initColl - nextColl))
			return 0;
		if(i>nextLn && j>nextColl){
			while(j-k-1>=nextColl){
				if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
					doska[(i-k)*8+j-k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		else if(i>nextLn && j<nextColl){
			while(j+k+1<=nextColl){
				if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
					doska[(i-k)*8+j+k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		else if(i<nextLn && j>nextColl){
			while(j-k-1>=nextColl){
				if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
					doska[(i+k)*8+j-k]='-';
					ctrl = 1;
					printf("%d",ctrl);//////////////////////////
				}
				k++;
			}
		}
		else if(i<nextLn && j<nextColl){
			while(j+k+1<=nextColl){
				if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
					doska[(i+k)*8+j+k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		if(ctrl == 0){
			return 0;
		}
		mov(doska,turn);
		
	}
	return 1;
}



int canEat1(char doska[65],int coll, int ln){
	int i = ln, j = coll, k = 1;
	if(doska[i*8+j] == 'W'){
		if(i == 0 || i == 1){
			if(j == 7 || j == 6){
				if((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-'){
					return 1;
				}
			}
			else if (j>1 && j<6){
				if(((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-')){
					return 1;
				}
			}
			else{
				if((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-'){
					return 1;
				}
			}
		}
				
				
		
		else if(i == 7 || i == 6){
			if(j == 0 || j == 1){
				if((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-'){
					return 1;
				}
			}
			else if (j>1 && j<6){
				if(((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-')){
					return 1;
				}
			}
			else{
				if((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-'){
					return 1;
				}
			}
		}
		else{
			if(((doska[(i-1)*8+j-1]=='B' || doska[(i-1)*8+j-1]=='K') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='B' || doska[(i-1)*8+j+1]=='K') && doska[(i-2)*8+j+2]=='-') || ((doska[(i+1)*8+j-1]=='B' || doska[(i+1)*8+j-1]=='K') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='B' || doska[(i+1)*8+j+1]=='K') && doska[(i+2)*8+j+2]=='-')){
				return 1;
			}
		}
	}
	else if(doska[i*8+j] == 'P'){
		if(i == 0 || i == 1){
			if(j == 7 || j == 6){
				while(j-k-1>=0 && i+k+1<=7){
					if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
			}
			else if (j>1 && j<6){
				while(j-k-1>=0 && i+k+1<=7){
					if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
				while(j+k+1<=7 && i+k+1<=7){
					if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
						return 1;
					}
					else if(doska[(i+k)*8+j+k]!='-' && doska[(i+k+1)*8+j+k+1]!='-')
						break;
					k++;
				}
				k = 1;
			}
			else{
				while(j+k+1<=7 && i+k+1<=7){
					if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
						return 1;
					}
					else if(doska[(i+k)*8+j+k]!='-' && doska[(i+k+1)*8+j+k+1]!='-')
						break;
					k++;
				}
				k = 1;
			}
		}
		
		
		
		else if(i == 7 || i == 6){
			if(j == 0 || j == 1){
				while(j+k+1<=7 && i-k-1>=0){
					if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
						return 1;
					}
					else if(doska[(i-k)*8+j+k]!='-' && doska[(i-k-1)*8+j+k+1]!='-')
						break;
					k++;
				}
				k = 1;
			}
			else if (j>1 && j<6){
				while(j-k-1>=0 && i-k-1>=0){
					if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
				while(j+k+1<=7 && i-k-1>=0){
					if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
						return 1;
					}
					else if(doska[(i-k)*8+j+k]!='-' && doska[(i-k-1)*8+j+k+1]!='-')
						break;
					k++;
				}
				k = 1;
			}
			else{
				while(j-k-1>=0 && i-k-1>=0){
					if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
			}
		}
		else{
			while(j-k-1>=0 && i-k-1>=0){
				if((doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K') && doska[(i-k-1)*8+j-k-1]=='-'){
					return 1;
				}
				else if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-')
					break;
				k++;
			}
			k = 1;
			while(j+k+1<=7 && i-k-1>=0){
				if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K') && doska[(i-k-1)*8+j+k+1]=='-'){
					return 1;
				}
				else if(doska[(i-k)*8+j+k]!='-' && doska[(i-k-1)*8+j+k+1]!='-')
					break;
				k++;
			}
			k = 1;
			while(j-k-1>=0 && i+k+1<=7){
				if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K') && doska[(i+k+1)*8+j-k-1]=='-'){
					return 1;
				}
				else if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-')
					break;
				k++;
			}
			k = 1;
			while(j+k+1<=7 && i+k+1<=7){
				if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K') && doska[(i+k+1)*8+j+k+1]=='-'){
					return 1;
				}
				else if(doska[(i+k)*8+j+k]!='-' && doska[(i+k+1)*8+j+k+1]!='-')
					break;
				k++;
			}
			k = 1;
		}	
	}
	return 0;
}

int endFindWhite(char doska[65]){
	for(int i = 0; i<8;i++)
		for(int j = 0; j<8;i++)
			if(doska[i*8+j] == 'W' || doska[i*8+j] == 'P')
				return 1;
	return 0;
}


int TwoInRaw(char doska[65],char turn[6]){
	int j = turn[0]-'A', i = 8-turn[1]+'0', k = 1;
	int nextC = turn[3]-'A', nextL = 8-turn[4]+'0';
	if(abs(j-nextC) == 1)
		return 0;
	if(i>nextL && j>nextC){
		while(j-k>nextC){
			if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-'){
				return 1;
			}
			k++;
		}
	}
	else if(i>nextL && j<nextC){
		while(j+k<nextC){
			if(doska[(i-k)*8+j+k]!='-' && doska[(i-k-1)*8+j+k+1]!='-'){
				return 1;
			}
			k++;
		}
	}
	else if(i<nextL && j>nextC){
		while(j-k>nextC){
			if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-'){
				return 1;
			}
			k++;
		}
	}
	else if(i<nextL && j<nextC){
		while(j+k<nextC){
			if(doska[(i+k)*8+j+k]!='-' && doska[(i+k+1)*8+j+k+1]!='-'){
				return 1;
			}
			k++;
		}
	}
	return 0;
}




