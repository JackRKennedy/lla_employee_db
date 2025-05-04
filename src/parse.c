#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/parse.h"
#include "../include/common.h"

struct employee_t * remove_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *employeename) {
	/* break this down
	 * Take in employee name from user
	 * check name is valid string not containing numbers for example
	 * iterate through file to find matching name
	 * remove matching employee file
	 * decrement count
	 * realloc employee buffer
	 * return status success
	 */

	// first check user string doesn't contain any numbers
	if (employeename == NULL || strlen(employeename) == 0){
		fprintf(stderr, "Error: Employee name cannot be blank\n");
		return NULL;
	}

	int found_index = -1; // non valid definition by default

	// iterate through employee names to compare
	int i =0;
	for (; i < dbhdr->count; i++) {
		if (strcmp(employees[i].name, employeename) == 0) {
			found_index = i;
			break;
		}
	}

	if (found_index == -1) {
		printf("Employee was not found, please check your spelling, or they may already have been deleted\n");
		return NULL;
	}

	// calculate the number of elements to move
	size_t num_employees_to_move = dbhdr->count - found_index - 1;

	//calculate numer of bytes to move (for memmove())
	size_t num_bytes_to_move = num_employees_to_move * sizeof(struct employee_t);

	if (num_employees_to_move > 0) { // shift all employees after the found one to the left, if the found employee isn't the last index
		memmove(&employees[found_index], &employees[found_index +1], num_bytes_to_move);
	}

	dbhdr->count--; // decrement the count variable
	// this will also handle removing the final employee in the file in the case this is the one being searched for

	struct employee_t *temp = NULL; //initialise temp pointer

	// reallocate memory to the correct size
	if (dbhdr->count ==0) {
		//if element you're removing is the last remaining employee, free the memory in full
		free(employees);
		employees = NULL;
		printf("Removed last employee %s. Database is now empty.\n", employeename);
		return employees;
	} else { // shrink the allocated amount of memory
		temp = realloc(employees, dbhdr->count * sizeof(struct employee_t));

		// if realloc didn't work for any reason, return an error
		if (temp == NULL) {
			perror("Realloc failed to shrnk\n");
			printf("Although realloc failed, employee: %s was still logically removed.\n", employeename);
			return employees;
		} else {
			employees = temp; // realloc has worked, and we now can assign the pointer to the new memory location
			printf("Employee %s has been sucessfully removed\n", employeename);
			return employees; // return new potential pointer

			// from what I gather, this should be safe enough to do, as the temp pointer is declared locally so any issues iwth it should be scope safe in terms of the main function
		}
	}

	printf("The employee %s has been removed.\n", employeename);

	return STATUS_SUCCESS;
}


void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	int i = 0;
	for (; i<dbhdr->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tName: %d\n", employees[i].hours);
	}
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	//strtok extracts data from the user string based on delimiters and breaks them up into tokens

	char *name = strtok(addstring, ","); // first time using it, you pass the string and the delimiter

	char *address = strtok(NULL, ","); // in later times you pass NULL and the delimiter for it to march forward to next instance

	char *hours = strtok(NULL, ","); // can use these to add to employees struct

	printf("%s %s %s\n", name, address, hours); // print seperated strings

	strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));

	strncpy(employees[dbhdr->count-1].address, address, sizeof(employees[dbhdr->count-1].address));

	employees[dbhdr->count-1].hours = atoi(hours); // atoi = ascii to integer as the input is in string form we need to convert it

	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int count = dbhdr->count;

	// create a buffer of space in memory to read off disk

	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == -1) {
		printf("Malloc failed\n");
		return STATUS_ERROR;

	}

	read(fd, employees, count*sizeof(struct employee_t)); // populate all data into our array

	int i = 0;

	for (; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
	}

	unsigned short realcount = dbhdr->count;

	unsigned int calculated_filesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount);

	dbhdr->version = ntohs(dbhdr->version);
	dbhdr->count = ntohs(dbhdr->count);
	dbhdr->magic = ntohl(dbhdr->magic);
	dbhdr->filesize = ntohl(calculated_filesize);

	// have to use lseek -> moves 'cursor' in the open file to a certain position, we will move to front

	lseek(fd, 0, SEEK_SET);

	// Write the updated header (now in network byte order)
	if (write(fd, dbhdr, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("Failed to write database header");
		// Handle error appropriately - maybe revert header conversions?
		return;
	}

	int i = 0;

	for (int i = 0; i < realcount; i++) {
		// Temporarily convert hours to network byte order for writing
		unsigned int network_hours = htonl(employees[i].hours);
		// Write the employee record, but use the original hours field size for the write amount
		if (write(fd, &employees[i], sizeof(struct employee_t)) != sizeof(struct employee_t)) {
			perror("Failed to write employee record");
			// Need to decide how to handle partial writes
			// Revert hours conversion before returning?
			employees[i].hours = ntohl(network_hours); // Revert if write failed
			return;
		}
		// IMPORTANT: Revert the conversion in memory after successful write,
		// so the data in 'employees' array remains in host byte order.
		employees[i].hours = ntohl(network_hours);
	}

	dbhdr->version = ntohs(dbhdr->version);
	dbhdr->count = ntohs(dbhdr->count);
	dbhdr->magic = ntohl(dbhdr->magic);
	dbhdr->filesize = ntohl(dbhdr->filesize);

	if (ftruncate(fd, calculated_filesize) == -1) {
		perror("ftruncate failed");
	}

	return; // Success


}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Received bad File Descriptor from the user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // create space on the heap/stack
	if (header == NULL) {
		printf("Malloc failed to create a Database Header\n");
		return STATUS_ERROR;
	}

	// now we unpack db header -> when dealing with binary, we need to unpack values into 'host endieness' meaning the host architecture will interpret the binary number as the correct conversion on all systems
	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read"); // tell user something went wrong
		free(header); // free header from memory
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version); // packing values to correct endieness
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->version != 1) {
		printf("Improper header version\n");
		free(header);
		return -1;
	}

	if (header->magic != HEADER_MAGIC) {
		printf("Improper header magic\n");
		free(header);
		return -1;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted Database\n");
		free(header);
		return -1;
	}

	*headerOut = header;

}

int create_db_header(int fd, struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // this is so we can pass around and use in functions, so it can be edited rather than needing to be copied

	// since we are using a memory function like calloc, we need to check it first before assigning memory otherwise we weill have a seg fault
	if (header == NULL) {
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	/* the above if conditions replaced the original (if *header == -1) which was causing errors
	 * Basically header is the pointer returned by the calloc function
	 * *header is the dereferenced pointer meaning the value pointed to
	 * &header is the address at which the pointer value is stored at
	 *
	 * calloc returns a pointer, not an integer, so checking if it has returned an integer value
	 * such as -1 *normally representing a bad value in functions returning ints)
	 * is an error here and could cause seg fault errors, so we need to check against null
	 */

	header->version = 0x1; // since we are using a pointer, we need to use -> notation
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;

	return STATUS_SUCCESS;

}


