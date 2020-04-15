#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

unsigned char key[8*4+1];

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

unsigned char mix[4][4] = {{2,3,1,1}, {1,2,3,1}, {1,1,2,3}, {3,1,1,2}};

unsigned char *input;

unsigned char map[4][4];

unsigned char long_key[60][4];

int key_len;

void addroundkey(int offset){
	
	int i, j;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			map[j][i] = map[j][i]^long_key[i+offset][j];
		}
	}

}

void shiftrow(){
	
	int i, j;
	unsigned char tem[4][4];
	memcpy(tem,map,4*4*sizeof(char));
	for(j=0;j<4;j++){
		for(i=0;i<4;i++){
			map[i][j] = tem[i][(j+i)%4];
		}
	}

}

void shiftrow_bak(){
	
	int i, j;
	unsigned char tem[4][4];
	memcpy(tem,map,4*4*sizeof(char));
	for(j=0;j<4;j++){
		for(i=0;i<4;i++){
			map[i][j] = tem[i][(j-i+4)%4];
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

void subbytes_bak(){
	
	int i, j;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			map[i][j] = Sbox_bak[map[i][j]];
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
	memset(ans,0,4*4*sizeof(char));
	
	for(i=0;i<4;i++){
		
		ans[0][i] = times_2(map[0][i])^times_3(map[1][i])^map[2][i]^map[3][i];
		ans[1][i] = map[0][i]^times_2(map[1][i])^times_3(map[2][i])^map[3][i];
		ans[2][i] = map[0][i]^map[1][i]^times_2(map[2][i])^times_3(map[3][i]);
		ans[3][i] = times_3(map[0][i])^map[1][i]^map[2][i]^times_2(map[3][i]);
				
	}
	
	memcpy(map,ans,4*4*sizeof(char));
	
}

void mixcolumns_bak(){
	
	int i, j, v;
	
	unsigned char ans[4][4];
	unsigned int tem;
	memset(ans,0,4*4*sizeof(char));
	
	for(i=0;i<4;i++){
		
		ans[0][i] = times_14(map[0][i])^times_11(map[1][i])^times_13(map[2][i])^times_9(map[3][i]);
		ans[1][i] = times_9(map[0][i])^times_14(map[1][i])^times_11(map[2][i])^times_13(map[3][i]);
		ans[2][i] = times_13(map[0][i])^times_9(map[1][i])^times_14(map[2][i])^times_11(map[3][i]);
		ans[3][i] = times_11(map[0][i])^times_13(map[1][i])^times_9(map[2][i])^times_14(map[3][i]);
				
	}
	
	memcpy(map,ans,4*4*sizeof(char));
	
}

void keygen(unsigned char *key){
	
	srand(time(NULL));

	int i;
	/*for(i=0;i<key_len;i++){
		key[i] = rand()%256;
		
	}*/
	
	for(i=0;i<10;i++){
		key[i] = '0'+i;	
	}
	for(i=0;i<10;i++){
		key[10+i] = '0'+i;	
	}
	for(i=0;i<10;i++){
		key[20+i] = '0'+i;	
	}
	for(i=0;i<2;i++){
		key[30+i] = '0'+i;	
	}
	
	return;
}

/*void mapgen(){
	
	int i,j;
	int sum = 0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			map[j][i] = sum;
			sum += 17;
		}
	}
	return;
}*/

void key_expend(){
	
	int n = key_len/4 ;
	int Rcon = 1;
	
	memcpy(long_key,key,n*4*sizeof(char));//first part, just copy

	int i,j;
	int count = 4*(n+6+1);
	for(i=n;i<count;i++){
		if(i%n==0){
			long_key[i][0] = long_key[i-n][0] ^ Sbox[long_key[i-1][1]] ^ Rcon; 
			long_key[i][1] = long_key[i-n][1] ^ Sbox[long_key[i-1][2]];
			long_key[i][2] = long_key[i-n][2] ^ Sbox[long_key[i-1][3]];
			long_key[i][3] = long_key[i-n][3] ^ Sbox[long_key[i-1][0]];

			Rcon *= 2;
			if(Rcon > 0x80){
				Rcon = Rcon ^ 0x1b;
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

void print_file(char *file_name){
	FILE *file_ptr;
	file_ptr = fopen(file_name,"rb");
	unsigned char buf[50];
	memset(buf,'\0',sizeof(char)*50);
	while(fread(buf,sizeof(char),45,file_ptr)>0){
		printf("%s",buf);
		memset(buf,'\0',sizeof(char)*50);
	}
	printf("\n");
	return;
}

void print_input(){
	
	int round;
	round = strlen(input)/16;
	if(strlen(input)%16!=0){round++;}
	
	int i, j, v;
	
	for(v=0;v<round;v++){
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				printf("%02x ",input[v*16+i+j*4]);
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n");
	
}

void map_to_file(FILE *file_ptr,unsigned char map[][4]){
	
	int i, j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			char buf[4];
			
			sprintf(buf,"%02X",map[j][i]);

			fwrite(buf,2*sizeof(char),1,file_ptr);//row major
		}
	}
	
}

void map_to_file_unpadding(FILE *file_ptr,unsigned char map[][4]){
	
	int i, j;
	int count = 16-map[3][3];
	printf("Unpadding size: %d.",count);
	for(i=0;count>0&&i<4;i++){
		for(j=0;count>0&&j<4;j++){
			fwrite(&map[j][i],sizeof(char),1,file_ptr);//row major
			count--;
		}
	}
	
}

int main(int argc , char* argv[]){
	
	int i, j, v;
	int round;
	
	//init
	for(i = 0; i < 256; i++){
		Sbox_bak[Sbox[i]] = i;
	}
	
	if(argc!=3){
		printf("Usage: ./aes_encrypt_decrypt.o <file-to-encrypt> <encrypt-mode>\n");
		return 1;
	}
	
	FILE *file_ptr;
	file_ptr = fopen(argv[1],"rb");
	
	fseek(file_ptr,0,SEEK_END);
	round = ftell(file_ptr)/16;
	if(ftell(file_ptr)%16!=0){round++;}
	fseek(file_ptr,0,SEEK_SET);

	
////////////////////////////key genarate start///////////////////////////////	
	//32-byte random key
	if(strcmp(argv[2],"32")==0){
		key_len = 32;
		keygen(key);
	}
	//24-byte random key
	else if(strcmp(argv[2],"24")==0){
		key_len = 24;
		keygen(key);
	}
	//16-byte random key
	else if(strcmp(argv[2],"16")==0){
		key_len = 16;
		keygen(key);
	}
	//invaild key
	else{
		printf("encrypt-mode could be 32, 24 or 16.\n");
		return 2;
	}
	
	FILE *key_file;
	char key_file_name[500]={'\0'};
	strcpy(key_file_name,argv[1]);
	strcat(key_file_name,".key");
	key_file = fopen(key_file_name,"wb");
	unsigned char key_buf[64+1];
	for(i=0;i<key_len;i++){
		sprintf(&key_buf[i*2],"%02X",key[i]);
	}
	
	fwrite(key_buf,sizeof(char),key_len*2,key_file);
	strcpy(key_file_name,argv[1]);
	strcat(key_file_name,".key");
	printf("key is now at ./%s\n",key_file_name);
	fclose(key_file);
////////////////////////////key genarate end///////////////////////////////		
	key_expend();

	
	FILE *encrypt_file;
	char encrypt_file_name[500]={'\0'};
	strcpy(encrypt_file_name,argv[1]);
	strcat(encrypt_file_name,".aes");
	encrypt_file = fopen(encrypt_file_name,"wb");
	
	int round_count;
	input = malloc(sizeof(char)*16);
	
	for(round_count=0;;round_count++){
		int padding_size;
		padding_size = 16-fread(input,sizeof(char),16,file_ptr);
		memset(&input[16-padding_size],padding_size,padding_size*sizeof(char));
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				map[i][j] = input[i+j*4]; //col major;
			}
		}
		
////////////////////////////encrypt start///////////////////////////////
		print_map();
		addroundkey(0);
		print_map();
		for(i=4;i<4*(key_len/4+6+1)-4;i+=4){
			subbytes();
			shiftrow();
			mixcolumns();
			addroundkey(i);
			//if(i==4){print_map();}
		}
		print_map();
		
		subbytes();
		shiftrow();
		addroundkey(i);
////////////////////////////encrypt end/////////////////////////////////

		
		map_to_file(encrypt_file,map);
		
		if(padding_size>0){break;}
	}
	
	free(input);
	
	fclose(file_ptr);
	
	fclose(encrypt_file);
	printf("encrypt_file is now at ./%s\n",encrypt_file_name);

	
	return 0;
	
}