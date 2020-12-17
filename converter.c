#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define H 200
#define W 200
#define CMDNO 13

//-----------------------------------------------------------------------------------------------------
//Helper functions

//reinitialize the commands
// commands [0] -> [5] is for DATA commands
// commands [6] is for Colour command
// commands [7],[8] are for DATA, targetX
// commands [9],[10] are for DATA, targetY
// commands [11] is for TOOL BLOCK command
// commands [12] is for DY command
void reinitialize(unsigned char commands[CMDNO]) {
  for(int i = 0; i < 4; i++) commands[i] = 0xC0;
  commands[4] = 0xC3;
  commands[5] = 0xFF;
  commands[6] = 0x83;
  commands[7] = 0xC0;
  commands[8] = 0x84;
  commands[9] = 0xC0;
  commands[10] = 0x85;
  commands[11] = 0x82;
  commands[12] = 0x60;
}
void ChangeColour(unsigned char commands[CMDNO], unsigned char colour) {
  commands[0] = commands[0] | (colour >> 6);
  commands[1] = commands[1] | colour;
  commands[2] = commands[2] | (colour >> 2);
  commands[3] = commands[3] | (colour << 4) | (colour >> 4);
  commands[4] = commands[4] | (colour << 2);
}

void ChangeTxTy(unsigned char commands[CMDNO], int x, int y) {
  commands[7] = 
}
// Checks the header of the file
// Program only works for 200 x 200 files with maximum gray value of 255
bool FileOk(char line[50]) {
  if (strlen(line) < 12 || strlen(line) > 15) return 0;
  if (strncmp(line, "P5 200 200", 10)) return 0;
  if (line[12] == ' ') return 0;
  
  int a = strtol(line + 11, NULL, 10);
  if (a > 255 || a < 0) return 0;
  return 1;
}

// Gets the name of the output file using the input file
void GetName(char out[50], const char in[50]) {
  int len = strchr(in, '.') - in;
  strncpy(out, in, len);
  out[len] = '\0';
  strcat(out, ".sk");
}
//-----------------------------------------------------------------------------------------------------
// Read/Write functions

void read(unsigned char image[H][W], FILE *in) {
  char line[50];
  fgets(line, 50, in);
  printf("%s",line);

  if (! FileOk(line)) {
    fclose(in);
    printf("Only 200 x 200 files with a maximum gray value of 255 are supported\n");
    exit(1);
  }
  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++)
  image[i][j] = fgetc(in);


  fclose(in);
}
void write(unsigned char image[H][W], char filename[50]) {
  //unsigned char output[H][W * 10]; //buffer used to write commands in the converted file

  FILE *ofp = fopen(filename, "wb");
  if (ofp == NULL) {
		printf("Cannot write file %s", filename);
		exit(1);
	}
  unsigned char newdraw[CMDNO]; 

  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++){
      reinitialize(newdraw);
      ChangeColour(newdraw, image[i][j]);
      ChangeTxTy(newdraw, i, j);
      fwrite(newdraw, 1, CMDNO, ofp);
  }
}

// A replacement for the library assert function.
void assert(int line, int b) {
  if (b) return;
  printf("The test on line %d fails.\n", line);
  exit(1);
}

//-----------------------------------------------------------------------------------------------------
// Testing
void testfile() {
  assert(__LINE__, FileOk("P5 200 200 255") == 1);
  assert(__LINE__, FileOk("P5 201 200 255") == 0);
  assert(__LINE__, FileOk("P5 199 200 255") == 0);
  assert(__LINE__, FileOk("P5 200 201 255") == 0);
  assert(__LINE__, FileOk("P4 200 200 255") == 0);
  assert(__LINE__, FileOk("5 200 200 255") == 0);
  assert(__LINE__, FileOk("P5 200 200 256") == 0);
  assert(__LINE__, FileOk("P5 200 200 254") == 1);
  assert(__LINE__, FileOk("P5 200 200 155") == 1);
  assert(__LINE__, FileOk("P5 200 200 355") == 0);
  assert(__LINE__, FileOk("P5 200 200 255  ") == 0);
  assert(__LINE__, FileOk("P5 200 200 25") == 1);
  assert(__LINE__, FileOk("P5 200 200 ") == 0);

}
void testGetName() {
  char c[50];
  GetName(c,"bands.pgm"); assert(__LINE__, !strcmp(c, "bands.sk"));
  GetName(c,"abcdefghijklm.pgm"); assert(__LINE__, !strcmp(c, "abcdefghijklm.sk"));
  GetName(c,"b.pgm"); assert(__LINE__, !strcmp(c, "b.sk"));

}
void testChangeColour() {
  unsigned char test[CMDNO];
  reinitialize(test);
  ChangeColour(test,0);
  assert(__LINE__, (test[0] == 0xC0 && test[1] == 0xC0 && test[2] == 0xC0 && test[3] == 0xC0 && test[4] == 0xC3 && test[5] == 0xFF)); 
  reinitialize(test);
  ChangeColour(test,0xFF);
  assert(__LINE__, (test[0] == 0xC3 && test[1] == 0xFF && test[2] == 0xFF && test[3] == 0xFF && test[4] == 0xFF && test[5] == 0xFF));
  reinitialize(test);
  ChangeColour(test,0xAA);
  assert(__LINE__, (test[0] == 0xC2 && test[1] == 0xEA && test[2] == 0xEA && test[3] == 0xEA && test[4] == 0xEB && test[5] == 0xFF));
}
void test() {
  testfile();
  testGetName();
  testChangeColour();
  printf("All tests pass\n");
}

int main(int n, char *args[]){
  switch (n) {
  case 1 : test(); break;
  }
  if(n == 2){
  FILE *in = fopen(args[1], "rb");
  int len = strlen(args[1]);
  if (in == NULL || strcmp(args[1]+ len - 4, ".pgm")) {
  printf("Please use a .pgm file\n");
  exit(1);
  }
  unsigned char image[H][W];
  read(image, in);
//  for(int i = 0; i < H; i++) {
//  for(int j = 0; j < W; j++)
//  printf("%x ",image[i][j]);
//  printf("\n");
//  }
  char filename[50];
  GetName(filename, args[1]);
  write(image, filename);
  }
  return 0;
}
