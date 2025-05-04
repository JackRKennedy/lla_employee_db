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

	read(fd, employees, count*sizeof(struct employee_t)); // populate all date into our array

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
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;

	dbhdr->version = ntohs(dbhdr->version);
	dbhdr->count = ntohs(dbhdr->count);
	dbhdr->magic = ntohl(dbhdr->magic);
	dbhdr->filesize = ntohl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));

	// have to use lseek -> moves 'cursor' in the open file to a certain position, we will move to front

	lseek(fd, 0, SEEK_SET);

	write(fd, dbhdr, sizeof( struct dbheader_t));

	int i =0;

	for (; i < realcount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}
	return;

}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Received bad File Descriptor from the user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
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

	header->version = ntohs(header->version);
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


