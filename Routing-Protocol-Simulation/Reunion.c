#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

void remove_repeatation(char *str);
char * getDistance(char from[50], char too[50], int setting);
char * findPath(char from[50], char too[50], int j, int no_vertices);
void getTable (char from[50], int setting);
void printTableInfo(char name[50], char home[50]);

char topology[100][7][100]; 
char table[10][10][50]; 
int size = 0;
int distances[100];
int traveled = 0;
int next_traveled = 0;
char result[50];
char smallpath[50];
int no_vertices;

// change method to 2 to minimize distance, 3 to minimize time, 4 to minimize gold, 5 to minimize trolls, and 6 to minimize hops
int method = 6;

int main( int argc, char ** argv) {
	
  // Open the file
  FILE * fPointer;
  fPointer = fopen(argv[1], "r");
  
  // Read each line of the file
  char lines[100][50];
  int i = 0;
  while(!feof(fPointer)){  
    if (fgets(lines[i], 100, fPointer) == NULL) break;
	size++;
	i++;
  }
  
  // Get each element of the file
  for( int i = 0; i < size; i++) {
	  char string[1024];
	  strcpy(string, lines[i]);
	  string[1023] = '\0';
	  strcpy(topology[i][0], strtok(string, " "));
	  topology[i][0][50] = '\0';
	  strcpy(topology[i][1], strtok(NULL, " "));
	  topology[i][1][50] = '\0';
	  strcpy(topology[i][2], strtok(NULL, " "));
	  topology[i][2][50] = '\0';
	  strcpy(topology[i][3], strtok(NULL, " "));
	  topology[i][3][50] = '\0';
	  strcpy(topology[i][4], strtok(NULL, " "));
          topology[i][4][50] = '\0';
	  strcpy(topology[i][5], strtok(NULL, " "));
	  topology[i][5][50] = '\0';
	  strcpy(topology[i][6], "1");
	  topology[i][6][50] = '\0';
	  //printf("%s ", topology[i][0]);
          //printf("%s ", topology[i][1]);
	  //printf("%s ", topology[i][2]);
          //printf("%s ", topology[i][3]);
          //printf("%s ", topology[i][4]);
	  //printf("%s\n", topology[i][5]);
  }
  
  // get all vertexes in the topology file
  char vertexes[50];
  int j = 0;
  for (int i = 0; i < size * 2 - 1; i++) {
	  vertexes[i] = topology[j][0][0];
	  i++;
	  vertexes[i] = topology[j][1][0];
	  vertexes[i+1] = '\0';
	  printf("vertexes is %s\n ", vertexes);
	  j++;	
  }
  
  // remove duplicate vertexes
  remove_repeatation(vertexes);
  // get number of vertices
  no_vertices = strlen(vertexes);
  printf("vertexes is %s\n ", vertexes);
  
  // initialize table
  
  for (int i = 1; i <= no_vertices; i++){
	  table[0][i][0] = vertexes[i - 1];
	  table[0][i][1] = '\0';
	printf("%s\n", table[0][i]);
  }

  
  printf("Dwarf   Home   Hops   Dist   Time   Gold   Trolls   Path\n");
  getTable("R", method);
  findPath("R", "C", 8, no_vertices);
  printTableInfo("Balin", "R");
  getTable("V", method);
  findPath("V", "C", 8, no_vertices);
  printTableInfo("Dori", "V");
  getTable("W", method);
  findPath("W", "C", 8, no_vertices);
  printTableInfo("Dwalin", "V");
  getTable("M", method);
  findPath("M", "C", 8, no_vertices);
  printTableInfo("Gloin", "V");
  getTable("E", method);
  findPath("E", "C", 8, no_vertices);
  printTableInfo("Nori", "V");
  getTable("T", method);
  findPath("T", "C", 8, no_vertices);
  printTableInfo("Oin", "V");
  getTable("S", method);
  findPath("S", "C", 8, no_vertices);
  printTableInfo("Ori", "V");

  fclose(fPointer);

  return 1;
}

void printTableInfo(char name[50], char home[50]) {
	int time = 0;
	int dist = 0;
	int gold = 0; 
	int trolls = 0;
	char buffer[50];
	traveled = 0;

	for (int i = 0; i < strlen(smallpath); i++) {
		if (i != strlen(smallpath) - 1) {
			char character[50];
			character[0] = smallpath[i];
			character[1] = '\0';

			char character2[50];
			character2[0] = smallpath[i+1];
			character2[1] = '\0';
			strcpy(buffer, getDistance(character, character2, 2)); // distance 
			dist = dist + atoi(strtok(buffer, " "));
			strcpy(buffer, getDistance(character, character2, 3)); // time
			time = time + atoi(strtok(buffer, " "));
			strcpy(buffer, getDistance(character, character2, 4)); // coins
			gold = gold + atoi(strtok(buffer, " "));
			strcpy(buffer, getDistance(character, character2, 5)); // distance
			trolls = trolls + atoi(strtok(buffer, " "));
		}
	}
	printf("%s   %s   %d   %d   %d   %d   %d   %s\n", name, home, strlen(smallpath) - 1, dist, time, gold, trolls, smallpath);

}
// pseudo function that goes through the table and finds the path, puts it in a string
char * findPath(char from[50], char too[50], int j, int no_vertices) {
	// initialize j to bottom row
	strcpy(smallpath, "");
	for (int i = 1; i <= no_vertices; i++){
		if (strstr(table[0][i], too) != NULL) {
			// get the char that preceded this char at the jth column
			char preceded[50];
			strtok(table[j][i], " ");
			strcpy(preceded, strtok(NULL, " "));
			if (strstr(from, preceded)){
				strcat(smallpath, from);
				strcat(smallpath, too);
				
				//return from;
			}
			else {
				//strcpy(smallpath, findPath(from, preceded, j - 1, no_vertices));
				//strcat(smallpath, too);
				findPath(from, preceded, j - 1, no_vertices);
				strcat(smallpath, too);
				//return smallpath;
			}
		}
	}
	return "test";
}

void getTable (char from[50], int setting) {
  char too[50];
  strcpy(too, "W");

  // setting from 
  char next[50];
  strcpy(next, from);
  //printf("the next is %s\n", from);
  
  for (int i = 1; i <= no_vertices; i++){
	  distances[i] = 999999;
  }
  char dont_check[50];
  char test[50];
  //printf("%s\n", test);
  //printf("number vert is %d\n", no_vertices);
  // row procedure
  for (int j = 1; j <= no_vertices; j++){ 
	  // column procedure
	  int smallest = 999999;
	  dont_check[j-1] = next[0];
	  dont_check[j] = '\0';
	  //printf("dont_check is %s\n", dont_check);

	  strcpy(table[j][0], next); // set first column of row j to the next character checked
	  //printf("column 1 of table is %s\n", table[j][0]);
	  for (int i = 1; i <= no_vertices; i++){ // find distance between this character and every other
		  if (strstr(dont_check, table[0][i]) == NULL) { // only do the following on the characters that we have not gone to yet
			  char distance[50];
			  strcpy(distance, getDistance(table[j][0], table[0][i], setting)); // get distance
			  char save[50];
			  strcpy(save, distance);
			  int i_distance = atoi(strtok(distance, " "));
			  //printf("char 1 is %s, char 2 is %s, distance is %s, int distance is %d\n", table[j][0], table[0][i], save, i_distance);
			  if (i_distance < distances[i] || j == 1) {
				//printf("got here\n");
				strcpy(table[j][i], save); // put distance in column i row j
				distances[i] = i_distance; // update distance
			  }
			  else {
				  strcpy(table[j][i], table[j-1][i]);
			  }
			  
			  // if the current distance is the smallest found so far and from is not the same as too, save it
			  if (distances[i] < smallest && strstr(table[j][0], table[0][i]) == NULL) {
				  smallest = distances[i];
				  strcpy(next, table[0][i]);
				//printf("got here, new next is %s\n", next);
			  }

		  }
		else {
			strcpy(table[j][i], table[j-1][i]);
		}
	        //printf("smallest was %d\n", smallest);
		//printf("for row %d column %d, it is %s \n", j, i, table[j][i]);
		//printf("%s ", table[j][i]);
	  }
	  //printf("\n");
	  traveled = smallest;  
  }
}

// to find all the data required for the tables find each connection and increment, ie. ABCDEFG find distance of AB, add distance of BC to AB, then CD
char * getDistance(char from[50], char too[50], int setting) {
	int j = 0;
	int i;
	for (i = 0; i < size; i++){
	        
		if (strstr(topology[i][0], from) != NULL && strstr(topology[i][1], too) != NULL) {		
			int dis = atoi(topology[i][setting]);
			dis = dis + traveled;
			sprintf(result, "%d", dis);
			strcat(result, " ");
			strcat(result, from);
			return result;
		}

		if (strstr(topology[i][0], too) != NULL && strstr(topology[i][1], from) != NULL) {
			int dis = atoi(topology[i][setting]);
			dis = dis + traveled;
			sprintf(result, "%d", dis);
			strcat(result, " ");
			strcat(result, from);
			return result;
		}
	}
	
	if (i == size) {
		strcpy(result, "999999");
		strcat(result, " ");
		strcat(result, from);
		return result;
	}

	return "test 1";
}

// source for this function :: https://stackoverflow.com/questions/1532819/algorithm-efficient-way-to-remove-duplicate-integers-from-an-array
void remove_repeatation(char str[]) {
    char flag[256] = {0}; 
    for(int i = 0; str[i] != '\0';) {
        if(0 == flag[str[i]]) 
        {
            flag[str[i]] = 1; 
            i++; 
        }
        else
        {
            for(int j=i; str[j] != '\0'; j++)
                str[j] = str[j+1];
        }
    }
}
