#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>



unsigned char Sbox[] = {99,124,119,123,242,107,111,197,48,1,103,43,254,215,171,
  118,202,130,201,125,250,89,71,240,173,212,162,175,156,164,114,192,183,253,
  147,38,54,63,247,204,52,165,229,241,113,216,49,21,4,199,35,195,24,150,5,154,
  7,18,128,226,235,39,178,117,9,131,44,26,27,110,90,160,82,59,214,179,41,227,
  47,132,83,209,0,237,32,252,177,91,106,203,190,57,74,76,88,207,208,239,170,
  251,67,77,51,133,69,249,2,127,80,60,159,168,81,163,64,143,146,157,56,245,
  188,182,218,33,16,255,243,210,205,12,19,236,95,151,68,23,196,167,126,61,
  100,93,25,115,96,129,79,220,34,42,144,136,70,238,184,20,222,94,11,219,224,
  50,58,10,73,6,36,92,194,211,172,98,145,149,228,121,231,200,55,109,141,213,
  78,169,108,86,244,234,101,122,174,8,186,120,37,46,28,166,180,198,232,221,
  116,31,75,189,139,138,112,62,181,102,72,3,246,14,97,53,87,185,134,193,29,
  158,225,248,152,17,105,217,142,148,155,30,135,233,206,85,40,223,140,161,
  137,13,191,230,66,104,65,153,45,15,176,84,187,22};

unsigned char Sbox_bak[256];

unsigned char map[4][4];

void addroundkey(unsigned char long_key[][4], int offset){
	
	int i, j;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			map[i][j] = map[i][j]^long_key[i+offset][j];
		}
	}

}

void shiftrow(){
	
	int i, j;
	unsigned char tem[4][4];
	memcpy(tem,map,4*4*sizeof(char));
	for(i=0;i<4;i++){
		for(j=1;j<4;j++){ //0 does not shift anything, start from 1 could improve performance.
			map[i][j] = tem[(i+j)%4][j];
		}
	}

}

void subbytes(){
	
	int i, j;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			map[i][j] = Sbox[map[i][j]];
		}
	}

}

unsigned char times_2(unsigned char input){
	if(input>127){
		input = input<<1;
		input = input^0x1b;
	}
	else{
		input = input<<1;
	}
	return input;
}
unsigned char times_3(unsigned char input){
	return times_2(input)^input;
}
unsigned char times_9(unsigned char input){
	return times_2(times_2(times_2(input)))^input;
}
unsigned char times_11(unsigned char input){
	return times_2(times_2(times_2(input))^input)^input;
}
unsigned char times_13(unsigned char input){
	return times_2(times_2(times_2(input)^input))^input;
}
unsigned char times_14(unsigned char input){
	return times_2(times_2(times_2(input)^input)^input);
}

void mixcolumns(){
	
	int i, j, v;
	
	unsigned char ans[4][4];
	unsigned int tem;
	//memset(ans,0,4*4*sizeof(char));
	
	for(i=0;i<4;i++){
		
		ans[i][0] = times_2(map[i][0])^times_3(map[i][1])^map[i][2]^map[i][3];
		ans[i][1] = map[i][0]^times_2(map[i][1])^times_3(map[i][2])^map[i][3];
		ans[i][2] = map[i][0]^map[i][1]^times_2(map[i][2])^times_3(map[i][3]);
		ans[i][3] = times_3(map[i][0])^map[i][1]^map[i][2]^times_2(map[i][3]);
				
	}
	
	memcpy(map,ans,4*4*sizeof(char));
	
}

void keygen(unsigned char *key, int key_len){
	
	srand(time(NULL));

	int i;
	for(i=0;i<key_len;i++){
		key[i] = rand()%256;
		
	}
	
	return;
}

void key_expend(unsigned char long_key[][4], unsigned char *key, int key_len){
	
	int n = key_len/4 ;
	unsigned char Rcon = 1;
	
	memcpy(long_key,key,n*4*sizeof(char));//first part, just copy

	int i,j;
	int count = 4*(n+6+1);
	for(i=n;i<count;i++){
		if(i%n==0){
			
			long_key[i][0] = long_key[i-n][0] ^ Sbox[long_key[i-1][1]] ^ Rcon; 
			long_key[i][1] = long_key[i-n][1] ^ Sbox[long_key[i-1][2]];
			long_key[i][2] = long_key[i-n][2] ^ Sbox[long_key[i-1][3]];
			long_key[i][3] = long_key[i-n][3] ^ Sbox[long_key[i-1][0]];
			
			if(Rcon >= 0x80){
				Rcon *= 2;
				Rcon = Rcon ^ 0x1b;
			}
			else{
				Rcon *= 2;
			}

		}
		else if(n>6 && i%8==4){
			for(j=0;j<4;j++){
				long_key[i][j] = long_key[i-n][j] ^ Sbox[long_key[i-1][j]];
			}
		}
		else{
			for(j=0;j<4;j++){
				long_key[i][j] = long_key[i-n][j]^long_key[i-1][j];
			}
		}
		
	}
	
	/*for(i=0;i<count;i++){
		printf("%d ",long_key[i][0]);
		printf("%d ",long_key[i][1]);
		printf("%d ",long_key[i][2]);
		printf("%d ",long_key[i][3]);
		printf("\n");
	}*/
	
	return;
}

void print_map(){
	int i, j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			printf("%02x ",map[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void map_to_file(FILE *file_ptr,unsigned char map[][4]){
	
	int i, j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			char buf[3];
			buf[2]='\0';
			sprintf(buf,"%02X",map[i][j]);
			fwrite(buf,2*sizeof(char),1,file_ptr);//row major
		}
	}
	
}

void key_file_name_gen(char *output, char *app_code, char *user_name){
	strcpy(output, app_code);
	strcat(output,"_");
	strcat(output,user_name);
	strcat(output,"_pw_.aes");
	return;
}

void encrypt_file_name_gen(char *output, char *app_code, char *user_name){
	strcpy(output, app_code);
	strcat(output,"_");
	strcat(output,user_name);
	strcat(output,"_.key");
	return;
}

int main(int argc , char* argv[]){
	
	int i, j, v;
	int round;
	
	//init
	for(i = 0; i < 256; i++){
		Sbox_bak[Sbox[i]] = i;
	}
	
	if(argc!=4){
		printf("Usage: ./aes_encrypt_decrypt.o <app-code> <user-name> <password>\n");
		return 1;
	}
	
	round = strlen(argv[3])/16;
	if(strlen(argv[3])%16!=0){round++;}
	
////////////////////////////key genarate start///////////////////////////////	
	unsigned char key[8*4+1];
	int key_len;

	key_len = 16;
	keygen(key,key_len);
	
	FILE *key_file;
	char key_file_name[500]={'\0'};
	key_file_name_gen(key_file_name,argv[1],argv[2]);
	key_file = fopen(key_file_name,"wb");
	
	unsigned char key_buf[64+1];
	for(i=0;i<key_len;i++){
		sprintf(&key_buf[i*2],"%02X",key[i]);
	}
	
	fwrite(key_buf,sizeof(char),key_len*2,key_file);
	printf("key at ./%s\n",key_file_name);
	fclose(key_file);
////////////////////////////key genarate end///////////////////////////////	
	unsigned char long_key[60][4];
	key_expend(long_key,key,key_len);
	
	char encrypt_file_name[500]={'\0'};
	encrypt_file_name_gen(encrypt_file_name,argv[1],argv[2]);
		
	FILE *encrypt_file = fopen(encrypt_file_name,"wb");
	
	int round_count;

	for(round_count=0;;round_count++){
		int padding_size = 0;
		if(strlen(&argv[3][round_count*16])<16){
			padding_size = 16-strlen(&argv[3][round_count*16]);
		}
		
		memcpy(map,&argv[3][round_count*16],sizeof(char)*(16-padding_size));
		memset((char*)map+(16-padding_size),padding_size,padding_size*sizeof(char));
		
////////////////////////////encrypt start///////////////////////////////

		addroundkey(long_key,0);

		for(i=4;i<4*(key_len/4+6+1)-4;i+=4){
			subbytes();
			shiftrow();
			mixcolumns();
			addroundkey(long_key ,i);
			//if(i==4){print_map();}
		}

		
		subbytes();
		shiftrow();
		addroundkey(long_key, i);
////////////////////////////encrypt end/////////////////////////////////

		map_to_file(encrypt_file,map);
		
		if(padding_size>0){break;}
	}

	fclose(encrypt_file);
	printf("encrypt file at ./%s\n",encrypt_file_name);
	
	return 0;
	
}