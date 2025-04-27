#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

// the below comments are documentation for the function immediately below it, enables hover-over definition of function print_usage

/**
 *@brief Prints standard function of the employee_db main.c file to the terminal
 *
 *@return Returns 3 printed statements containing the flags required to run the employee_db executable
 **/
void print_usage(char *argv[]) {
	printf("Usage %c -n -f <database file\n", *argv[0]);
	printf("\t -n - creates new database file\n");
	printf("\t -f - [required] path to database file\n");

	// no need to return anything with a void function type in this version of c
}

int main(const int argc, char *argv[]) {
	bool newfile = false;
	char *filepath = NULL;
	int c; // command line flag variable
	int dbfd = -1; // not valid file descriptor by default

	while ((c = getopt(argc, argv, "nf:")) != -1){
		/*reads command line arguments passed with file call
		currently has flags n - boolean and f: - string ':' denoting a string expectation
		*/
		switch (c) {
			case 'n':
				newfile = true; // if no file recognised then create a new one and leave
				break;
			case 'f':
				filepath = optarg; // reads the filename from the commandline and sets the name to it
				break;
			case '?': // unknown input
				printf("Unknown option -%c", c); // shows the user the flag they entered is invalid
			default:
				return -1; // we shouldn't ever get to this case, so if we do, return -1 and kill the program
		}
	}

	if (newfile) {
		dbfd = create_db_file(filepath); // creates a new db at the filepath given by user - returns database file descriptor
		if (dbfd == STATUS_ERROR) {
			printf("Unable to create database file\n");
		}
	} else { // i.e. the file exists and does not need to be created
			dbfd = open_db_file(filepath);
			if (dbfd == STATUS_ERROR) { // if we are still getting error, explain we cannot open file and close the program
				printf("Unable to open database file %s\n", filepath);
				return -1;
			}

		}

	// after breaking out of the while loop
	printf("Newfile %d\n", newfile);    // prints the newly created file name
if (filepath != NULL) {
	printf("Filepath %d\n", *filepath);
}
else {
	printf("Filepath is a required argument\n");
	print_usage(argv); // print_usage explains to user how the main program should be run, including flags and arguments etc
}

	return 0;
}
