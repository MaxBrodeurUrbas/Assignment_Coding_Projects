/*
Student Name: Max Brodeur-Urbas
Student ID: 260806240
Date: October 10th 2017
*/

#define MAXRECORDS 100
#define MAXNAMELENGTH 15
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct StudentRecord
{
	char FirstNames[MAXNAMELENGTH];
	char LastNames[MAXNAMELENGTH];
	int IDNums;
	int Marks;
};

void Swap(int val1, int val2, struct StudentRecord *SRecords[]) //void because the function returns nothing. Swaps values of variables by swapping this value location
{
	struct StudentRecord *temp; //defining temp as a StudentRecord structure

	temp = SRecords[val1]; //temp variable necessary as an intermediate step so no values are erased
	SRecords[val1]=SRecords[val2];
	SRecords[val2]=temp;
}

int partitioning (struct StudentRecord *SRecords[], int left, int right) //left and right correspond to the first and last index values
{
	char *Pivot = SRecords[left]->LastNames; 	//setting the pointer pivot to the first last name in the list
	int a = left;								//making simpler clone variables that will be edited during iterations of the while loop
	int b = right;
	while(1) 									//while true
	{
		while(strcmp(SRecords[a]->LastNames, Pivot) < 0) { 	//if the string value of the pivot is greater than that of last name at index a, then add 1 to the leftmost index to move to the next in of the list.
			a = a+1;
		}
		while(strcmp(SRecords[b]->LastNames, Pivot) > 0){		 //if the string value of the pivot is less than that of the last name at index b, subtract 1 the rightmost index to move to the next in the list.
			b = b-1;
		}
		if (a>=b) 			//when a and b meet the partitioning function returns a which is the index of the midpoint.
			return a;
		Swap(a, b, SRecords);	 //if the pivot is neither greater than the value at index a and not less than value at index b then use swap function to swap values at a and b then loops may continue until a and b eventually meet.
	}
}

void quicksort(struct StudentRecord *SRecords[], int left, int right){

	if (left < right) {
		int p = partitioning(SRecords, left, right); // p is the index of the midpoint

			quicksort(SRecords,left, p); //Recursively calling quicksort to sort the left half

			quicksort(SRecords,p+1, right); //recursively calling quicksort to the right half
	}
	}

int BinarySearch(char *argv, struct StudentRecord *SRecords[], int k){ //argv is the last name of the student you are searching for through the array of pointers to the array of structures.
	unsigned int L= 0, R = (k-1), Mid; // L is the beginning R is the end
	while(L < R) {

		Mid = (L+R)/2; //defining Mid as the middle of the array of pointers

		if (strcmp(SRecords[Mid]->LastNames, argv) == 0) // if the name your looking for equals the mid name, return the index number Mid
			return Mid;

		if (strcmp(SRecords[Mid]->LastNames, argv) < 0) // if the name your looking for is greater than the mid name, your looking for name in right half.
					L = Mid+1;

		if (strcmp(SRecords[Mid]->LastNames, argv) > 0) // if the name your looking for is less than the mid name you want to look through left half.
					R = Mid;
	}
	return -1;
}

int main(int argc, char * argv[]) {

	struct StudentRecord SRecords[MAXRECORDS];
    	int numrecords, nummarks, recordnum;

	FILE * NamesIDs;
	if((NamesIDs = fopen(argv[1], "r")) == NULL){
		printf("Can't read from file %s\n", argv[1]);
		exit(1);
	}

	numrecords=0;
    	while (fscanf(NamesIDs,"%s%s%d",&(SRecords[numrecords].FirstNames[0]),
		      				&(SRecords[numrecords].LastNames[0]),
		      				&(SRecords[numrecords].IDNums)) != EOF) {
	  numrecords++;
 	}

	fclose(NamesIDs);

	FILE * marks;
	if((marks = fopen(argv[2], "r")) == NULL){
		printf("Can't read from file %s\n", argv[2]);
		exit(1);
	}
	nummarks=0;
	while(fscanf(marks,"%d",&(SRecords[nummarks].Marks)) != EOF) {
	    nummarks++;
	}

	fclose(marks);

	struct StudentRecord *pSRecords[MAXRECORDS];

	int i;
	for (i=0; i< numrecords; i++) {
		pSRecords [i] = &SRecords[i];
	}

	quicksort(pSRecords,0,numrecords-1); // calling the quicksort function to sort the info

	int result =BinarySearch(argv[3], pSRecords, 50); // calling binary search to search using inputed name

	if (result != -1) // binary defined returns -1 when it fails so whenever it doesnt = -1 that means the name was found
			printf("The following record was found: \nName: %s %s\nStudent ID: %d\nStudent Grade: %d\n",pSRecords[result]->FirstNames, pSRecords[result]->LastNames, pSRecords[result]->IDNums, pSRecords[result]->Marks); //the final print is the students name was found.
	else
			printf("No record was found for student with last name %s\n", argv[3]);

	return EXIT_SUCCESS;
}


