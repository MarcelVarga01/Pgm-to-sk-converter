#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define H 200
#define W 200
//number of commands for setting colour, position
#define CNO 7
#define PNO 8

//-----------------------------------------------------------------------------------------------------
//Helper functions

// Reinitialize the Colour Commands
// commands [0] -> [5] is for DATA commands
// commands [6] is for Colour command
void ReinitColour(unsigned char commands[CNO]) {
  for(int i = 0; i < 4; i++) commands[i] = 0xC0;
  commands[4] = 0xC3;
  commands[5] = 0xFF;
  commands[6] = 0x83;
}
// Change Colour commands to make s -> data store the colour of the pixel
void ChangeColour(unsigned char commands[], unsigned char colour) {
  commands[0] = commands[0] | (colour >> 6);
  commands[1] = commands[1] | colour;
  commands[2] = commands[2] | (colour >> 2);
  commands[3] = commands[3] | (colour << 4) | (colour >> 4);
  commands[4] = commands[4] | (colour << 2);
}

// Reinitialize the Position commands
// commands [0], [1], [2] are for 2 x DATA, targetX
// commands [3], [4], [5] are for 2 x DATA, targetY
// command [6] is for TOOL LINE / NONE
// command [7] is for DY
void ReinitPos(unsigned char commands[PNO], bool draw) {
  commands[0] = 0xC0;
  commands[1] = 0xC0;
  commands[2] = 0x84;

  commands[3] = 0xC0;
  commands[4] = 0xC0;
  commands[5] = 0x85;
  if(draw)
    commands[6] = 0x81;
  else 
    commands[6] = 0x80;
  commands[7] = 0x40;
}

// Change Draw commands to set tx and ty to current x and y
void ChangePos(unsigned char commands[PNO], int x, int y) {
  commands[0] = commands[0] | (x >> 6);
  commands[1] = commands[1] | x;
  commands[3] = commands[3] | (y >> 6);
  commands[4] = commands[4] | y;
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

void read(unsigned char image[H][W], char *file) {
  FILE *in = fopen(file, "rb");
  int len = strlen(file);

  if (in == NULL || strcmp(file + len - 4, ".pgm")) {
    printf("Please use an existing .pgm file\n");
    exit(1);
    }
  char line[50];
  fgets(line, 50, in);

  if (! FileOk(line)) {
    fclose(in);
    printf("Only 200 x 200 files with a maximum gray value of 255 are supported\n");
    exit(1);
  }
  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++)
      image[j][i] = fgetc(in);


  fclose(in);
}
void write(unsigned char image[H][W], char filename[50]) {
  //unsigned char output[H][W * 10]; //buffer used to write commands in the converted file

  FILE *ofp = fopen(filename, "wb");
  if (ofp == NULL) {
		printf("Cannot write file %s", filename);
		exit(1);
	}
  unsigned char ColourCMDS[CNO]; 
  unsigned char DrawCMDS[PNO];
  unsigned char NewLineCMDS[10];

  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++){
      if (j == 0 || image[i][j-1] != image[i][j]) {
        ReinitColour(ColourCMDS);
        ChangeColour(ColourCMDS, image[i][j]);
        fwrite(ColourCMDS, 1, CNO, ofp);
        }
      if (j == 0) {
        ReinitPos(NewLineCMDS, 0);
        ChangePos(NewLineCMDS, i, 0);
        fwrite(NewLineCMDS, 1, PNO, ofp);
      }
      if (j == W - 1 || image[i][j+1] != image[i][j]) {
        ReinitPos(DrawCMDS, 1);
        ChangePos(DrawCMDS, i, j);
        fwrite(DrawCMDS, 1, PNO, ofp);
      }
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
  unsigned char test[CNO];
  ReinitColour(test);
  ChangeColour(test,0);
  assert(__LINE__, (test[0] == 0xC0 && test[1] == 0xC0 && test[2] == 0xC0 && test[3] == 0xC0 && test[4] == 0xC3 && test[5] == 0xFF)); 
  ReinitColour(test);
  ChangeColour(test,0xFF);
  assert(__LINE__, (test[0] == 0xC3 && test[1] == 0xFF && test[2] == 0xFF && test[3] == 0xFF && test[4] == 0xFF && test[5] == 0xFF));
  ReinitColour(test);
  ChangeColour(test,0xAA);
  assert(__LINE__, (test[0] == 0xC2 && test[1] == 0xEA && test[2] == 0xEA && test[3] == 0xEA && test[4] == 0xEB && test[5] == 0xFF));
}
void testChangePos() {
  unsigned char test[PNO];
  ReinitPos(test, 1);
  ChangePos(test, 0, 0);
  assert(__LINE__, (test[0] == 0xC0 && test[1] == 0xC0 && test[3] == 0xC0 && test[4] == 0xC0));
  ReinitPos(test, 1);
  ChangePos(test, 199, 199);
  assert(__LINE__, (test[0] == 0xC3 && test[1] == 0xC7 && test[2] ==0x84 && test[3] ==0xC3 && test[4] == 0xC7 && test[5] == 0x85 && test[6] == 0x81 && test[7] == 0x40));
  ReinitPos(test, 1);
  assert(__LINE__, (test[0] == 0xC0 && test[1] == 0xC0 && test[2] ==0x84 && test[3] ==0xC0 && test[4] == 0xC0 && test[5] == 0x85 && test[6] == 0x81 && test[7] == 0x40));
  ChangePos(test, 64, 89);
  assert(__LINE__, (test[0] == 0xC1 && test[1] == 0xC0 && test[2] ==0x84 && test[3] ==0xC1 && test[4] == 0xD9 && test[5] == 0x85 && test[6] == 0x81 && test[7] == 0x40));

}
void test() {
  testfile();
  testGetName();
  testChangeColour();
  testChangePos();
  printf("All tests pass\n");
}

int main(int n, char *args[]){
  switch (n) {
  case 1 : test(); break;
  }
  if(n == 2){
  unsigned char image[H][W];
  read(image, args[1]);
//  for(int i = 0; i < H; i++) {
//  for(int j = 0; j < W; j++)
//  printf("%x ",image[i][j]);
//  printf("\n");
//  }
  char filename[50];
  GetName(filename, args[1]);
  write(image, filename);
  printf("File %s has been written.\n", filename);
  }
  return 0;
}
