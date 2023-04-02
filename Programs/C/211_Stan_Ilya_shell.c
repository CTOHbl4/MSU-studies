#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/wait.h>
#include <fcntl.h>
#define MAXFNAME 256 // in Linux limitation on names of files

/*
KShell() -> Shellif() -> Komanda() -> Konveyer() -> ExeCute()
  ^			    |
  |----------------------KShell()  {, if (). Refer to () as to the new Komanda-Shell, united into one Komanda. The () should be as one Komanda and return only one final status.}
*/
void printArr(unsigned int *arr, int c);
void free2(char **arr);
int splitWithSpaces(char *line);
int creatArgvAll(char *input, char **argvAll);
int KShell(char **argvAll, unsigned int *Start);
int Shellif(char **argvAll, unsigned int *Start);
int Komanda(char **argvAll, unsigned int *NextOp);
int Konveyer(char**argvAll,unsigned int *Start);
int exeCute(char **argvAll,unsigned int *Start, int flag);
int checkTheInput(char *line);


int main(){
	char *input;
	char **argvAll;
	int check;
	unsigned int Start = 0;
	printf("File redirection before the command.\n");
	while(1){
		input = malloc(256);
		argvAll = malloc(33*sizeof(char*));
START:		printf("\n>>>Enter the Shell-command $ ");
		check = splitWithSpaces(input);
		if(check == -1){ free(input); free(argvAll);return 0;}//goto START;
		if(checkTheInput(input)){
			printf("Incorrect placement of skobkas.\nTry again.\n");
			goto START;
		}
		creatArgvAll(input,argvAll);
		free(input);
		KShell(argvAll,&Start);
		Start = 0;
		free2(argvAll);
	}
}









void printArr(unsigned int *arr, int c){
	for (int i = 0; i<c;i++){
		printf("%u, ", arr[i]);
	}
	printf("\n");
}

void free2(char **arr){
	int i = 0;
	while(arr[i]!=NULL)
		free(arr[i++]);
	free(arr);
}











//_____________________work with input


//_________<operator> <space> <next word>

int splitWithSpaces(char *line){// returns -1, if empty input.
	int i = 0;
	char c = getchar();
	while(c!='\n'){
		while(c == ' ')c = getchar();
		if(c == '\n') break;
		if(i && i%250 == 0){
			line = (char *)realloc(line,i+256);
		}
		if(c == '|'){
			if((c = getchar()) == '|'){
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = c;
				line[i++] = c;
				line[i++] = ' ';
				c = getchar();
				continue;
			}
			else{
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = '|';
				line[i++] = ' ';
				continue;
			}
		}
		else if(c == '&'){
			if((c = getchar()) == '&'){
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = c;
				line[i++] = c;
				line[i++] = ' ';
				c = getchar();
				continue;
			}
			else{
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = '&';
				line[i++] = ' ';
				continue;
			}
		}
		else if(c == '>'){
			if((c = getchar()) == '>'){
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = c;
				line[i++] = c;
				line[i++] = ' ';
				c = getchar();
				continue;
			}
			else{
				if(i && line[i-1] != ' ')line[i++] = ' ';
				line[i++] = '>';
				line[i++] = ' ';
				continue;
			}
		}
		if(c == '<' || c == '(' || c == ')' || c == ';'){
			if(i && line[i-1] != ' ')line[i++] = ' ';
			line[i++] = c;
			line[i++] = ' ';
			c = getchar();
			continue;
		}
		else{
			if(i && line[i-1] != ' ')line[i++] = ' ';
			while(c!=' ' && c!='|' && c!='\n' && c!='(' && c!=')' && c!='&' && c!='>' && c!='<' && c !=';'){
				line[i++] = c;
				c = getchar();
			
			}
			line[i++] = ' ';
			continue;
		}
	
	}
	if(i && line[i-1] == ' ')line[i-1] = '\0';
	if(!i) return -1;
	return 0;
}


//_________________each word from input into an array of words

int creatArgvAll(char *input, char **argvAll){
	unsigned int i = 0, k = 0, len = strlen(input),wrdLn;
	char wrd[MAXFNAME];
	while(i<len){
		sscanf(input+i,"%s",wrd);
		wrdLn = strlen(wrd);
		i+=wrdLn+1;//plus space
		argvAll[k] = malloc(wrdLn+1);
		strcpy(argvAll[k++],wrd);
		if(k%32 == 0){
			argvAll = realloc(argvAll,sizeof(char*)*(k+33));
		}
	}
	argvAll[k] = NULL;
	return k;
}
















//___________________________________________________Shell

void h(int x){}

int KShell(char **argvAll, unsigned int *Start){// Start, if we found the new '(', Shell needs to wait, until all the commands(without &) are executed to get the next Komanda-Shell
	unsigned int CurInd = *Start;
	signal(SIGUSR1,h);
	int status = 0, k = 0,f_null = open("/dev/null",O_RDWR,0777);
	pid_t pid;
	while(argvAll[CurInd] != NULL){
		k = 0;
		while(argvAll[CurInd] != NULL && !((!strcmp(argvAll[CurInd],";") || !strcmp(argvAll[CurInd],"&")) && (k == 0)) && !(!strcmp(argvAll[CurInd],")") && k==-1)){
			
			if(!strcmp(argvAll[CurInd],"(")){
				k++;
			}
			else if(!strcmp(argvAll[CurInd],")")){
				k--;
				if(k==-1){
					break;
				}
			}	
			CurInd++;
		}
		
		if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")){
			CurInd++;
			if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"&")){
				if(!(pid = fork())){
					signal(SIGINT,SIG_IGN);
					dup2(f_null,0);
					//dup2(f_null,1);/////////////////////////////////////////////////
					if(!(pid = fork())){
						kill(getppid(),SIGUSR1);
						status = Shellif(argvAll,Start);
						free2(argvAll);
						exit(0);
					}
					else{
						pause();
						kill(getppid(),SIGUSR1);
						free2(argvAll);
						exit(0);
					}
				}
				pause();
				*Start = CurInd;
				close(f_null);
				return status;
			}
			
			else{
				status = Shellif(argvAll,Start);
				(*Start) = CurInd;
				close(f_null);
				return status;
			}
		}
			
			
		else if(argvAll[CurInd] == NULL){// no ';' or '&' in the line
			status = Shellif(argvAll,Start);
			(*Start)++;
		}
		else if(!strcmp(argvAll[CurInd],";")){//sequential
			
			status = Shellif(argvAll,Start);
			*Start = ++CurInd;// next word after the trigger (;, &)	
		}
		else if(!strcmp(argvAll[CurInd],"&")){ 
			//fonoviy rejim
			printf("\n+++fon\n\n");
			if(!(pid = fork())){
				
				signal(SIGINT,SIG_IGN);
				dup2(f_null,0);
				//dup2(f_null,1);//////////////////////////
				if(!(pid = fork())){
					kill(getppid(),SIGUSR1);
					status = Shellif(argvAll,Start);
					free2(argvAll);
					exit(0);
				}
				else{
					pause();
					kill(getppid(),SIGUSR1);
					free2(argvAll);
					exit(0);
				}
			}
			pause();
			*Start = ++CurInd;
			if(argvAll[CurInd]!=NULL && !strcmp(argvAll[CurInd],")")){*Start+=1; break;}
		}
	}
	close(f_null);
	return status;
}

int Shellif(char **argvAll, unsigned int *Start){// Start points at the >,<,>> or a non-operator word, then on ;, & or NULL
	int status, k = 0; // status == 0 => executed normally.
	unsigned int CurInd = *Start;
	while(argvAll[CurInd] != NULL && strcmp(argvAll[CurInd],"&") && strcmp(argvAll[CurInd],";")){ // otherwise, new Shellif or new Komanda-Shell
		status = Komanda(argvAll,&CurInd);
		if(argvAll[CurInd] == NULL)break;
		
		if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")){
			*Start = CurInd;
			break;
		
		}
		//if(status == 139)break;
		if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"&&") && status){// skip all the && until ||
			// return the index for the next Shellif
			while(argvAll[CurInd] != NULL && strcmp(argvAll[CurInd],"&") && strcmp(argvAll[CurInd],";") && strcmp(argvAll[CurInd],"||") && !(!strcmp(argvAll[CurInd],")")&&(k == -1))){
				if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")){
					k--;
					if(k == -1)
						break;
				}
				else if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"("))
					k++;
				
				CurInd++;
			}
			if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"||")){
				k = 0;
				CurInd++;
				continue;
			}
			k = 0;
			if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")) CurInd++;
			*(Start) = CurInd;
			return status;
		}
		
		
		else if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"||") && !status){// skip all the || until &&
			while(argvAll[CurInd] != NULL && !((!strcmp(argvAll[CurInd],"&") || !strcmp(argvAll[CurInd],";") || !strcmp(argvAll[CurInd],"&&")) && (k==0)) && !(!strcmp(argvAll[CurInd],")")&&(k == -1))){
				if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")){
					k--;
					if(k == -1)
						break;
				}
				else if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"("))
					k++;
				
				CurInd++;
			}
			if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],"&&")){
				k = 0;
				CurInd++;
				continue;
			}
			k = 0;
			if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")")) CurInd++;
			*(Start) = CurInd;
			return status;
		}
		if(argvAll[CurInd]!= NULL && (!strcmp(argvAll[CurInd],"&&") || !strcmp(argvAll[CurInd],"||")))CurInd++;
	}
	return status;
}

int Komanda(char **argvAll, unsigned int *NextOp){// In the beginning NextOp indexes the word or >,<,>>. Then it points at NULL, || or && or ; or &
	unsigned int CurInd = *NextOp;
	int BI = dup(0), BO = dup(1),fdIn, fdOut, status;// not equal to 0 or 1
	if(!strcmp(argvAll[CurInd],"(")){
		(*NextOp)++;
		CurInd++;
		status = KShell(argvAll,&CurInd);
		if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")"))CurInd++;
		*NextOp = CurInd;
		dup2(BI,0);
		dup2(BO,1);
		return status;
	}
	
	if(!strcmp("<",argvAll[CurInd])){ // input change
		fdIn = open(argvAll[++CurInd],O_RDONLY, 0777);
		dup2(fdIn,0);
		close(fdIn);
		CurInd++;
	}
	if(!strcmp(">",argvAll[CurInd])){ // output change
		fdOut = open(argvAll[++CurInd],O_CREAT | O_TRUNC | O_WRONLY , 0777);
		dup2(fdOut,1);
		close(fdOut);
		CurInd++;
	}
	if(!strcmp(">>",argvAll[CurInd])){
		fdOut = open(argvAll[++CurInd],O_CREAT | O_APPEND | O_WRONLY , 0777);
		dup2(fdOut,1);
		close(fdOut);
		CurInd++;
	}
	
	if(!strcmp("<",argvAll[CurInd])){ // input change
		fdIn = open(argvAll[++CurInd],O_RDONLY, 0777);
		dup2(fdIn,0);
		close(fdIn);
		CurInd++;
	}
	
	if(!strcmp("(",argvAll[CurInd])){
		(*NextOp)++;
		CurInd++;
		status = KShell(argvAll,&CurInd);
		if(argvAll[CurInd] != NULL && !strcmp(argvAll[CurInd],")"))CurInd++;
		*NextOp = CurInd;
		dup2(BI,0);
		dup2(BO,1);
		return status;
	}
	
// then, Konveyer()
	*NextOp = CurInd;
	status = Konveyer(argvAll,NextOp);
	dup2(BI,0);
	dup2(BO,1);
	return status;
}

int Konveyer(char**argvAll,unsigned int *Start){ // Start points at the simple command. Then it points at the operator after the Konveyer
//returns the status of executed komanda
	int pd_Konv[2], status = 0, flag = 0, KonvBI, palki[100], tmp = 0;
	unsigned int i = *Start,j = 0,kolvoPalok;
	pid_t pid;
	while(argvAll[i]!=NULL && strcmp(argvAll[i],";") && strcmp(argvAll[i],"&") && strcmp(argvAll[i],"&&") && strcmp(argvAll[i],"||") && strcmp(argvAll[i],")"))
		if(!strcmp(argvAll[i],"|")) palki[j++] = i++;
		else i++;
	palki[j] = -1;
	
	i = *Start;
	kolvoPalok = j;
	j = 0;
	for(;j<kolvoPalok;j++){
		pipe(pd_Konv);
		if(!(pid = fork())){
			if(flag){
				dup2(KonvBI,0);
				close(KonvBI);
			}
			dup2(pd_Konv[1],1);
			close(pd_Konv[1]);
			status = exeCute(argvAll,Start,0);
			close(pd_Konv[0]);
			free2(argvAll);
			exit(0);
		}
		else{// father coordinates the exeCution of pr[i] and remembers the prev pipe[0]
			close(pd_Konv[1]);
			if(flag) close(KonvBI);
			*Start = palki[j]+1;
			if(flag){
				close(KonvBI);
			}
			KonvBI = pd_Konv[0];
			flag = 1;
		}
	} // exits before the last command is executed
	if(flag){
		dup2(KonvBI,0);
		close(KonvBI);
	}
	status = exeCute(argvAll,Start,5);
	while((tmp = wait(&status)) && tmp!=-1);
	if((status != 0)/* && (status != 256)*/)printf("><><Error accured: %d\n",status);
	return status;
}


//______creates a new process to use execvp()

int exeCute(char **argvAll,unsigned int *Start,int flag){
	int i = *Start, j = 0, status = 0;char **vector = malloc(10*sizeof(char*));
	pid_t pid1;
	while(argvAll[i]!=NULL && strcmp(argvAll[i],";") && strcmp(argvAll[i],"&") && strcmp(argvAll[i],"&&") && strcmp(argvAll[i],"||") && strcmp(argvAll[i],"|") && strcmp(argvAll[i],")")){
		vector[j++] = argvAll[i++];
		if(!(j%10))
			vector = realloc(vector,(j+10)*sizeof(char*));
	}
	vector[j] = NULL;
	
	if(!(pid1 = fork())){
		status = execvp(vector[0],vector);
		free2(argvAll);
		free(vector);
		exit(status);//exec didn't change the code
	}
	else{
		if(flag)wait(&status);
		*Start = i;
		free(vector);
		return status;
	}
}













int checkTheInput(char *line){// ideally, all the checks should be there.
	// check the balance
	int balance = 0;
	for(int i = 0; i<strlen(line);i++){
		if(line[i]=='(')
			balance++;
		else if(line[i]==')')
			balance--;
		if(balance<0)
			return -1;
	}
	if(balance)
		return -1;
	return 0;
}

