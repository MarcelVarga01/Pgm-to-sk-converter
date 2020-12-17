#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define H 200
#define W 200

const int COLOUR = 6, TARGETX = 7, TARGETY = 8, DY = 9;

//-----------------------------------------------------------------------------------------------------
//Helper functions
void ChangeColour(unsigned char data[10], unsigned char colour) {


}
//initialize the commands
// commands [0] -> [5] is for DATA commands
// commands [6] is for Colour command
// commands [7],[8] are for targetX,targetY
// commands [9] is for DY command
void initialize(unsigned char commands[10]) {
  for(int i = 0; i < 5; i++) commands[i] = 0xC0;
  commands[5] = 0xFF;
  commands[COLOUR] = 0x83;
  commands[TARGETX] = 
  commands[TARGETY] = 
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
    unsigned char newdraw[10]; 
    initialize(newdraw);
    for (int i = 0; i < H; i++)
      for (int j = 0; j < W; j++){
        changecolour(newdraw, image[i][j]);
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
void test() {
    testfile();
    testGetName();
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
//        for(int i = 0; i < H; i++) {
//            for(int j = 0; j < W; j++)
//                printf("%x ",image[i][j]);
//            printf("\n");
//            }
        char filename[50];
        GetName(filename, args[1]);
        write(image, filename);
    }
    return 0;
}
