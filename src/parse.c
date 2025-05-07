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

int update_employee_hours(struct dbheader_t *dbhdr, struct employee_t *employees, char *update_employee) {
	/*
	 * Take in string with employee name and new hours separated by commas
	 * do sanity checks on strings
	 * separate two values using strtok into two tokens
	 * use name token to search for employee
	 * access employee[found_index].hours and update
	 * no need to update memory buffer, filesize shouldn't change
	 * update employee pointer and return Status
	 */

	// sanity checks on string
	if (update_employee == NULL || strlen(update_employee) <= 0) {
		fprintf(stderr, "Error: -u flag must be accompanied by a valid, non-empty string\n");
		return STATUS_ERROR;
	}

	// declare a found index
	int found_index = -1;

	// break up user string into tokens
	char *name = strtok(update_employee, ","); // name variable from user string
	char *new_hours = strtok(NULL, ","); // new_hours variable from user string

	// iterate through employee names

	for (int i = 0; i < dbhdr->count; i++) {
		if (strcmp(employees[i].name, name) == 0) {
			found_index = i; // found_index is now the index of the employee
			break;
		} // if employee name not found, found_index remains at -1, and we can return status_error
	}

	if (found_index == -1) {
		printf("Employee was not found, please check your spelling, or they may already have been deleted\n");
		return STATUS_ERROR;
	} else {
		employees[found_index].hours = atoi(new_hours); // update the hours value
		return STATUS_SUCCESS;
	}

}

struct employee_t * remove_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *employeename) {
	/*
	 * Take in employee name from user
	 * check name is git a valid string not containing numbers, for example - still to be implemented
	 * iterate through file to find matching name
	 * remove matching employee file by shifting all employees after the found employee to the left
	 * decrement count
	 * realloc employee buffer to match new count
	 * return new employee_t pointer employees
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

    printf("DEBUG: Removing employee at index %d. Current count: %u.\n", found_index, dbhdr->count);
    size_t num_employees_to_move = dbhdr->count - found_index - 1;
    size_t num_bytes_to_move = num_employees_to_move * sizeof(struct employee_t);
    printf("DEBUG: Employees to move: %zu. Bytes to move: %zu.\n", num_employees_to_move, num_bytes_to_move);
    printf("DEBUG: sizeof(struct employee_t) is: %zu\n", sizeof(struct employee_t));
    printf("DEBUG: Destination address: %p\n", (void*)&employees[found_index]);
    if (num_employees_to_move > 0) {
        printf("DEBUG: Source address: %p\n", (void*)&employees[found_index + 1]);
    } // debugging issues with memory allocation after removal of an employee

    // Original code continues...
	if (num_employees_to_move > 0) {
		memmove(&employees[found_index], &employees[found_index + 1], num_bytes_to_move);
		printf("DEBUG: memmove completed.\n");

		// Add post-move check - Check the data now at found_index
		printf("DEBUG: Post-move data check:\n");
		printf("  New Data Slot [%d]: Hours = %d @ %p\n",
			   found_index, employees[found_index].hours, (void*)&employees[found_index]);
		// Optionally print the next one too, if applicable
		if (dbhdr->count > found_index + 2) { // Check bounds carefully
			printf("  New Data Slot [%d]: Hours = %d @ %p\n",
			  found_index + 1, employees[found_index + 1].hours, (void*)&employees[found_index + 1]);
		}

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
			printf("Employee %s has been successfully removed\n", employeename);
			return employees; // return a new potential pointer

			// from what I gather, this should be safe enough to do, as the temp pointer is declared locally so any issues iwth it should be scope safe in terms of the main function
		}
	}
}


void list_employees(const struct dbheader_t *dbhdr, struct employee_t *employees) {
	for (int i = 0; i<dbhdr->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	//strtok extracts data from the user string based on delimiters and breaks them up into tokens

	char *name = strtok(addstring, ","); // first time using it, you pass the string and the delimiter
	if (!name) {
		fprintf(stderr, "Error parsing name.\n"); return STATUS_ERROR;
	} // if the name is null, then the string is invalid and we should return an error

	char *address = strtok(NULL, ","); // in later times you pass NULL and the delimiter for it to march forward to the next instance
	if (!address) {
		fprintf(stderr, "Error parsing address.\n"); return STATUS_ERROR;
	}

	char *hours = strtok(NULL, ","); // can use these to add to employees struct
	if (!hours) {
		fprintf(stderr, "Error parsing hours.\n"); return STATUS_ERROR;
	}

	printf("%s %s %s\n", name, address, hours); // print seperated strings

	strncpy(employees[dbhdr->count -1].name, name, sizeof(employees[dbhdr->count -1].name));

	strncpy(employees[dbhdr->count -1].address, address, sizeof(employees[dbhdr->count -1].address));

	employees[dbhdr->count -1].hours = atoi(hours); // atoi = ascii to integer as the input is in string form we need to convert it

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
	if (employees == NULL) {
		printf("Calloc failed for employees\n");
		return STATUS_ERROR;

	}
	// calculate bytes to read
	size_t bytes_to_read = count * sizeof(struct employee_t);
	if (read(fd, employees, bytes_to_read) != bytes_to_read) {
		perror("Failed to read all employee data\n");
		free(employees);
		return STATUS_ERROR;
		// populate all data into our array
	}
	for (int i = 0; i < count; i++) {
		// this should work now with the conversion updated in output file
		employees[i].hours = ntohl(employees[i].hours);
	}

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	/*
	 * Changes made to the output file now handle some issues with poor memory allocation (i believe this was na issue i was having)
	 * it corrects issues I was having with network and host conversions
	 * this 'should' write the employees to the file in network byte order but keey the employees pointer in host byte order
	 * i could probably adjust my logic to handle this more elegantly but this looks like an effective solution
	 */
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
	}

	unsigned short realcount = dbhdr->count;

	unsigned int calculated_filesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount);

	struct dbheader_t header_net; // temporary header for byte conversion
	memcpy(&header_net, dbhdr, sizeof(struct dbheader_t)); // copy the original header

	header_net.version = htons(header_net.version); // convert to network byte order
	header_net.count = htons(header_net.count);
	header_net.magic = htonl(header_net.magic);
	header_net.filesize = htonl(calculated_filesize);

	// dbhdr->version = ntohs(dbhdr->version);
	// dbhdr->count = ntohs(dbhdr->count);
	// dbhdr->magic = ntohl(dbhdr->magic);
	// dbhdr->filesize = ntohl(calculated_filesize);

	// have to use lseek -> moves 'cursor' in the open file to a certain position, we will move to front

	lseek(fd, 0, SEEK_SET);

	// Write the network-order header
	if (write(fd, &header_net, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("Failed to write database header");
		return;
	}


	// --- Write Employees ---
	for (int i = 0; i < realcount; i++) {
		struct employee_t emp_net; // Create a temporary struct for writing
		memcpy(&emp_net, &employees[i], sizeof(struct employee_t)); // Copy current employee data

		// Convert hours to network byte order IN THE TEMPORARY STRUCT
		emp_net.hours = htonl(emp_net.hours);

		// Write the temporary struct (which has hours in network byte order)
		if (write(fd, &emp_net, sizeof(struct employee_t)) != sizeof(struct employee_t)) {
			perror("Failed to write employee record");
			// Consider how to handle partial writes if necessary
			return; // Stop writing on error
		}
		// No need to revert anything in the main 'employees' array, it was never changed.
	}

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
	return STATUS_SUCCESS;
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


