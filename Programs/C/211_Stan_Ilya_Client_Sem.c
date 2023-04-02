#include<stdio.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<string.h>
//Client Shashki: B, W, P, K
void doskaOut2(char doska[65]);
int checkFerzBlack(char doska[65]);// 1 = black Ferz
int bothOnField(char doska[65]);
int canEatB(char doska[65]);
int noWBetween(char doska[65],char turn[6]);
int noBBetween(char doska[65],char turn[6]);
void mov(char doska[65],char turn[6]);
int Eat(char doska[65],char turn[6]);
int canEat1(char doska[65],int coll, int ln);
void invert(char doska[65]);
int TwoInRaw(char doska[65],char turn[6]);
int abs(int x){
	if(x<0)
		return -x;
	else
		return x;
}

// client's game logic is the same as server's
int main(){
	printf("Welcome to a game of shashki!\nThe basics:\n1) W and B are usual shashki,\nand P and K (Princess and King) are damki.\n2) The turn should be written, like: A1-B2.\n3) All the pointers on the doska should point at the '-' symbols.\nSo, to eat the shashka, you need to point the next cell\nafter the shashka.\n4) To finish the game, the player should enter \"End\", as a turn.\n5) This program does not cover all the endgames.\nSo, player2 and player1 should negotiate in that case\nand use the End command.\n__________________________\n");
	printf("Welcome, player2!\n");
	key_t key;
	int semid,shmid;
	struct sembuf sops;
	char *shmaddr;
	char doska[65];
	key = ftok("a.txt",'S');
	
	semid = semget(key,1,0666|IPC_CREAT);
	shmid = shmget(key,65,0666 | IPC_CREAT);
	shmaddr = shmat(shmid,NULL,0);
	sops.sem_num = 0;
	sops.sem_flg = 0;
	char turn[6];
	int initColl,initLn,nextLn,nextColl,tmpColl,tmpLn;
	while(1){
		sops.sem_op = -2;
		printf("Waiting for player1...\n");
		semop(semid,&sops,1);// player2 is blocked here, while <0
		if(semctl(semid,0,GETVAL,0) == 8){//-2+10, player1 entered "End"
			printf("Server ended the session.\n");
			return 0;
		}
		printf("__________________________\n\nPlayer2\n\n");
		strcpy(doska,shmaddr);
		
		invert(doska);
		doskaOut2(doska);
		if(!bothOnField(doska)){//sem == 1
			printf("__________________________\nPlayer1 won!\n");
			doskaOut2(doska);
			sops.sem_op = -1;
			semop(semid,&sops,1);
			
		}
		printf("\nEnter your move, please:\n");
MOV:		scanf("%s",turn);
		if(!strcmp(turn,"End")){// player 2 entered End
			printf("Immediate finish of the game!\n");
			sops.sem_op = -1;
			doskaOut2(doska);
			doska[0] = '?';
			strcpy(shmaddr,doska);
			semop(semid,&sops,1);
			return 0;
		}
		
		initColl = 7-turn[0]+'A'; initLn = turn[1]-'0'-1;
		nextColl = 7-turn[3]+'A'; nextLn = turn[4]-'0'-1;
		
		//check and make the move
		if(initColl == nextColl || initLn == nextLn || initColl < 0 || initLn < 0 || nextColl > 7 || nextLn > 7 || nextColl < 0 || nextLn < 0 || initColl > 7 || initLn > 7 || doska[initColl + initLn * 8] == '_' || doska[nextColl + nextLn * 8] == '_' || doska[initColl + initLn * 8] == '-' || (doska[initColl + initLn * 8] == 'B' && (((nextLn >= initLn) && abs(nextLn-initLn)==1) || abs(nextLn-initLn) > 2)) || doska[nextColl + nextLn * 8] != '-' || doska[initColl + initLn * 8] == 'W' || doska[initColl + initLn * 8] == 'P' || TwoInRaw(doska,turn)){
			printf("Incorrect move, try again1!\n");
			goto MOV;
		}
		if(!canEatB(doska)){
			if(doska[initColl + initLn * 8] == 'K'){
				if((abs(nextColl - initColl) != abs(nextLn - initLn)) && !noWBetween(doska,turn)){
					printf("Incorrect move, try again2!\n");
					goto MOV;
				}
				else{
					mov(doska,turn);
				}
			}
			else if(doska[initColl + initLn * 8] == 'B'){
				if(abs(nextColl - initColl) != abs(nextLn - initLn) || abs(nextLn - initLn) != 1){
					printf("Incorrect move, try again3!\n");
					goto MOV;
				}
				else{
					mov(doska,turn);
				}
			}
			goto NEXT;
		}
		while(canEatB(doska)){
			if(!Eat(doska,turn)){
				printf("Incorrect move.\nIf you can eat, you must eat!\nTry another move!\n");
				goto MOV;
			}
			Eat(doska,turn);
			checkFerzBlack(doska);
			doskaOut2(doska);
			tmpColl = nextColl;
			tmpLn = nextLn;	
			if(canEat1(doska,tmpColl,tmpLn)){
				printf("\nEnter your eating move, please:\n");
MOV1:				scanf("%s",turn);
				initColl = 7-turn[0]+'A'; initLn = turn[1]-'0'-1;
				nextColl = 7-turn[3]+'A'; nextLn = turn[4]-'0'-1;
				if(initColl == nextColl || initLn == nextLn || initColl < 0 || initLn < 0 || nextColl > 7 || nextLn > 7 || nextColl < 0 || nextLn < 0 || initColl > 7 || initLn > 7 || doska[initColl + initLn * 8] == '_' || doska[nextColl + nextLn * 8] == '_' || doska[initColl + initLn * 8] == '-' || (doska[initColl + initLn * 8] == 'B' && (((nextLn >= initLn) && abs(nextLn-initLn)==1) || abs(nextLn-initLn) > 2)) || doska[nextColl + nextLn * 8] != '-' || doska[initColl + initLn * 8] == 'W' || doska[initColl + initLn * 8] == 'P' || tmpColl!=initColl || tmpLn!= initLn || TwoInRaw(doska,turn)){
					printf("Incorrect move, try again4!\n");
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
		
		
NEXT:		printf("\n__________________________\nDoska for player2:\n");	
		checkFerzBlack(doska);
		if(!bothOnField(doska)){// sem == 1
			printf("__________________________\nPlayer2 won!\n");
			doskaOut2(doska);
			invert(doska);
			strcpy(shmaddr,doska);
			sops.sem_op = -1;
			semop(semid,&sops,1);
			break;
		}
		doskaOut2(doska);
		invert(doska);
		strcpy(shmaddr,doska);

		sops.sem_op = -1;
		semop(semid,&sops,1);
	}
	
	return 0;
}

void invert(char doska[65]){
	char tmp;
	for(int i = 0; i<32; i++){
		tmp = doska[i];
		doska[i] = doska[63-i];
		doska[63-i] = tmp;
	}
}

void doskaOut2(char doska[65]){
	for(int i = 0; i<64; i++){
		if(i%8 == 0)
			printf("\n%d  |",1+i/8);
		printf("%c|",doska[i]);
	}
	printf("\n\n    H G F E D C B A\n");
}


int checkFerzBlack(char doska[65]){
	for(int i = 0; i<8; i++){
		if(doska[i] == 'B'){
			doska[i] = 'K';
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

int canEatB(char doska[65]){
	int k = 0;
	for(int i = 0; i<8;i++){
		for(int j = 0; j<8;j++){
			if(doska[i*8+j] == 'B'){
				if(i == 0 || i == 1){
					if(j == 7 || j == 6){
						if((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-'){
							return 1;
						}
					}
					else if (j>1 && j<6){
						if(((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-')){
							return 1;
						}
					}
					else{
						if((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-'){
							return 1;
						}
					}
				}
				
				
				
				else if(i == 7 || i == 6){
					if(j == 0 || j == 1){
						if((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-'){
							return 1;
						}
					}
					else if (j>1 && j<6){
						if(((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-')){
							return 1;
						}
					}
					else{
						if((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-'){
							return 1;
						}
					}
				}
				else{
					if(((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-') || ((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-')){
							return 1;
						}
				}
			}
			
			
			
			else if(doska[i*8+j] == 'K'){
				if(i == 0 || i == 1){
					if(j == 7 || j == 6){
						while(j-k-1>=0 && i+k+1<=7){
							if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else if (j>1 && j<6){
						while(j-k-1>=0 && i+k+1<=7){
							if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
						while(j+k+1<=7 && i+k+1<=7){
							if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else{
						while(j+k+1<=7 && i+k+1<=7){
							if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
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
							if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else if (j>1 && j<6){
						while(j-k-1>=0 && i-k-1>=0){
							if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
						while(j+k+1<=7 && i-k-1>=0){
							if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
					else{
						while(j-k-1>=0 && i-k-1>=0){
							if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
								return 1;
							}
							k++;
						}
						k = 0;
					}
				}
				else{
					while(j-k-1>=0 && i-k-1>=0){
						if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j+k+1<=7 && i-k-1>=0){
						if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j-k-1>=0 && i+k+1<=7){
						if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
							return 1;
						}
						k++;
					}
					k = 0;
					while(j+k+1<=7 && i+k+1<=7){
						if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
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
	int j = 7-turn[0]+'A', i = turn[1]-'0'-1;
	int nextC = 7-turn[3]+'A', nextL = turn[4]-'0'-1;
	int k = 1;
	if(i>nextL && j>nextC){
		while(j-k>nextC && i-k>nextL){
			if(doska[(i-k)*8+j-k]=='B' || doska[(i-k)*8+j-k]=='K'){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i>nextL && j<nextC){
		while(j+k<nextC && i-k>nextL){
			if((doska[(i-k)*8+j+k]=='B' || doska[(i-k)*8+j+k]=='K')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j>nextC){
		while(j-k>nextC && i+k<nextL){
			if((doska[(i+k)*8+j-k]=='B' || doska[(i+k)*8+j-k]=='K')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j<nextC){
		while(j+k<nextC && i+k<nextL){
			if((doska[(i+k)*8+j+k]=='B' || doska[(i+k)*8+j+k]=='K')){
				return 0;
			}
			k++;
		}
		
	}return 1;
}

int noWBetween(char doska[65],char turn[6]){
	int j = 7-turn[0]+'A', i = turn[1]-'0'-1;
	int nextC = 7-turn[3]+'A', nextL = turn[4]-'0'-1;
	int k = 1;
	if(i>nextL && j>nextC){
		while(j-k>nextC && i-k>nextL){
			if(doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P'){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i>nextL && j<nextC){
		while(j+k<nextC && i-k>nextL){
			if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j>nextC){
		while(j-k>nextC && i+k<nextL){
			if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P')){
				return 0;
			}
			k++;
		}
		return 1;
	}
	else if(i<nextL && j<nextC){
		while(j+k<nextC && i+k<nextL){
			if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P')){
				return 0;
			}
			k++;
		}
		
	}return 1;
}

void mov(char doska[65],char turn[6]){
	int initColl = 7-turn[0]+'A', initLn = turn[1]-'0'-1;
	int nextColl = 7-turn[3]+'A', nextLn = turn[4]-'0'-1;
	doska[nextColl+nextLn*8] = doska[initColl+initLn*8];
	doska[initColl+initLn*8] = '-';
}


int Eat(char doska[65],char turn[6]){
	int initColl = 7-turn[0]+'A', initLn = turn[1]-'0'-1;
	int nextColl = 7-turn[3]+'A', nextLn = turn[4]-'0'-1;
	int k = 1, ctrl = 0;
	int i = initLn, j = initColl;

	if(doska[initColl + initLn*8] == 'B'){
		if(abs(initLn - nextLn) != 2 || abs(initColl - nextColl) != 2)
			return 0;
		if(doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8]!='W' && doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8]!='P'){
			return 0;
		}
		doska[nextColl + nextLn*8] = 'B';
		doska[(nextColl+initColl)/2+(initLn+nextLn)/2*8] = '-';
		doska[initColl + initLn*8] = '-';
	}
	
	
	else if(doska[initColl + initLn*8] == 'K'){
		if(abs(initLn - nextLn) != abs(initColl - nextColl))
			return 0;
		
		if(i>nextLn && j>nextColl){
			while(j-k-1>=nextColl){
				if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
					doska[(i-k)*8+j-k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		else if(i>nextLn && j<nextColl){
			while(j+k+1<=nextColl){
				if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
					doska[(i-k)*8+j+k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		else if(i<nextLn && j>nextColl){
			while(j-k-1>=nextColl){
				if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
					doska[(i+k)*8+j-k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		else if(i<nextLn && j<nextColl){
			while(j+k+1<=nextColl){
				if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
					doska[(i+k)*8+j+k]='-';
					ctrl = 1;
				}
				k++;
			}
		}
		if(!ctrl){
			return 0;
		}
		mov(doska,turn);
		
	}
	return 1;
}


int canEat1(char doska[65],int coll,int ln){
	int i = ln, j = coll, k = 1;
	if(doska[i*8+j] == 'B'){
		if(i == 0 || i == 1){
			if(j == 7 || j == 6){
				if((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-'){
					return 1;
				}
			}
			else if (j>1 && j<6){
				if(((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-')){
					return 1;
				}
			}
			else{
				if((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-'){
					return 1;
				}
			}
		}
				
				
		
		else if(i == 7 || i == 6){
			if(j == 0 || j == 1){
				if((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-'){
					return 1;
				}
			}
			else if (j>1 && j<6){
				if(((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-')){
					return 1;
				}
			}
			else{
				if((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-'){
					return 1;
				}
			}
		}
		else{
			if(((doska[(i-1)*8+j-1]=='W' || doska[(i-1)*8+j-1]=='P') && doska[(i-2)*8+j-2]=='-') || ((doska[(i-1)*8+j+1]=='W' || doska[(i-1)*8+j+1]=='P') && doska[(i-2)*8+j+2]=='-') || ((doska[(i+1)*8+j-1]=='W' || doska[(i+1)*8+j-1]=='P') && doska[(i+2)*8+j-2]=='-') || ((doska[(i+1)*8+j+1]=='W' || doska[(i+1)*8+j+1]=='P') && doska[(i+2)*8+j+2]=='-')){
				return 1;
			}
		}
	}
	else if(doska[i*8+j] == 'K'){
		if(i == 0 || i == 1){
			if(j == 7 || j == 6){
				while(j-k-1>=0 && i+k+1<=7){
					if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
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
					if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
				while(j+k+1<=7 && i+k+1<=7){
					if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
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
					if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
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
					if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
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
					if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
						return 1;
					}
					else if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-')
						break;
					k++;
				}
				k = 1;
				while(j+k+1<=7 && i-k-1>=0){
					if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
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
					if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
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
				if((doska[(i-k)*8+j-k]=='W' || doska[(i-k)*8+j-k]=='P') && doska[(i-k-1)*8+j-k-1]=='-'){
					return 1;
				}
				else if(doska[(i-k)*8+j-k]!='-' && doska[(i-k-1)*8+j-k-1]!='-')
						break;
				k++;
			}
			k = 1;
			while(j+k+1<=7 && i-k-1>=0){
				if((doska[(i-k)*8+j+k]=='W' || doska[(i-k)*8+j+k]=='P') && doska[(i-k-1)*8+j+k+1]=='-'){
					return 1;
				}
				else if(doska[(i-k)*8+j+k]!='-' && doska[(i-k-1)*8+j+k+1]!='-')
						break;
				k++;
			}
			k = 1;
			while(j-k-1>=0 && i+k+1<=7){
				if((doska[(i+k)*8+j-k]=='W' || doska[(i+k)*8+j-k]=='P') && doska[(i+k+1)*8+j-k-1]=='-'){
					return 1;
				}
				else if(doska[(i+k)*8+j-k]!='-' && doska[(i+k+1)*8+j-k-1]!='-')
						break;
				k++;
			}
			
			k = 1;
			while(j+k+1<=7 && i+k+1<=7){
				if((doska[(i+k)*8+j+k]=='W' || doska[(i+k)*8+j+k]=='P') && doska[(i+k+1)*8+j+k+1]=='-'){
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


int TwoInRaw(char doska[65],char turn[6]){
	int j = 7-turn[0]+'A', i = turn[1]-'0'-1, k = 1;
	int nextC = 7-turn[3]+'A', nextL = turn[4]-'0'-1;
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

