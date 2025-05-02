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

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {

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
	 * *header is the derefereced pointer meanign the value pointed to
	 * &header is the address at which the pointer value is stored at
	 *
	 * calloc returns a pointer, not an integer, so checking if it has returned an integer value
	 * such as -1 *normally representing a bad value in functions returnings ints)
	 * is an error here and could cause seg fault errors, so we need to check against null
	 */

	header->version = 0x1; // since we are using a pointer, we need to use -> notation
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;

	return STATUS_SUCCESS;

}


