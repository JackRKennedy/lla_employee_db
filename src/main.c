#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

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
	printf("\t -a - [required] comma seperated values of employee name, address, hours\n");
	printf("\t -l - lists all employees\n");
	printf("\t -r - [required] name of employee to remove\n");


	// no need to return anything with a void function type in this version of c
}

int main(const int argc, char *argv[]) {
	bool newfile = false;
	bool list = false;
	char *filepath = NULL;
	char *addstring = NULL; // for adding employees
	char *employeename = NULL; // for removing employees by name
	int c; // command line flag variable
	int dbfd = -1; // not valid file descriptor by default
	struct dbheader_t *dbhdr = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:lr:")) != -1){ // "nf:a:lr:" are the primary commands and flags we could use, need to be listed here
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
			case 'a':
				addstring = optarg; // new flag for adding employees to the database via comma seperated values "John Doe,123 Fake St.,120"
				break;
			case 'l':
				list = true;
				break;
			case 'r':
				employeename = optarg; // take in employee name
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
			return -1;
		}
		if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
		printf("Failed to create database header\n");
		return -1;
		}
	} else { // i.e. the file exists and does not need to be created
			dbfd = open_db_file(filepath);
			if (dbfd == STATUS_ERROR) { // if we are still getting error, explain we cannot open a file and close the program
				printf("Unable to open database file - filepath: %s\n", filepath);
				return -1;
			}

			if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
				printf("Failed to validate database header\n");
				return -1;
			}
		}

	if(read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
		printf("Failed to read employees.\n");
		return 0;
	}; // passing address of a pointer to function as we want to overwrite it

	if (addstring) { // i.e. if the -a flag has been used and a string has been provided
		// now that we are adding an employee, we need to increase the amount of memory we need
		// can do this by updating the count and then reallocating memory

		dbhdr->count++;
		employees = realloc(employees, dbhdr->count * sizeof(struct employee_t));
		// realloc uses the original pointer (employees) and reallocates the second parameters size

		add_employee(dbhdr, employees, addstring);
		list_employees(dbhdr, employees);
	}

	if (employeename) { // if -r flag is used
		if (strlen(employeename) == 0) { // check the string isn't empty
			printf("Bad input from user\n"); // print error
			return STATUS_ERROR;
		}

		struct employee_t *updated_employees = remove_employee(dbhdr, employees, employeename); // assign updated employees pointer to remove_employees fucntion

		if (updated_employees == NULL && dbhdr->count > 0) {
			fprintf(stderr, "Error: unable to remove employee\n");
			return STATUS_ERROR;
		} else {
			employees = updated_employees; // if the employee was removed, update the employees pointer to the new one
			list_employees(dbhdr, employees); // print the updated list of employees
		}
	}
	// after breaking out of the while loop
	newfile ? printf("New file created - %s\n", filepath) : printf("No new file created\n");   // prints the newly created file name

if (list) {
	list_employees(dbhdr, employees);
}
if (filepath != NULL) {
	printf("Filepath %s\n", filepath);
}
else {
	printf("Filepath is a required argument\n");
	print_usage(argv); // print_usage explains to the user how the main program should be run, including flags and arguments etc.
}
	output_file(dbfd, dbhdr, employees);

	return 0;
}
