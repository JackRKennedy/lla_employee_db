#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/file.h"
#include "../include/common.h"


int create_db_file(char *filename) {

	int fd = open(filename, O_RDONLY); // this should always file as the create function shouldn't be called if a file exists,
	// however, if the FD is a valid fd, then we should close the file and inform the user the file already exists
	if (fd != -1) {
		close(fd);
		printf("The file %s already exists.\n", filename);
		return STATUS_ERROR;
	}

	// now if the file fails to open in readonly mode, do the following (create a file)
	fd = open(filename, O_RDWR | O_CREAT, 0644); //
	/*
	 * O_CREAT -> if the pathname does not exist, create it as a regular file
	 * O_RDWR -> opens the file as read/write
	 * 0644 -> gives user and group read/write permission but everyone else readonly
	 */
		if (fd == -1) {
			perror(("open"));
			return STATUS_ERROR;
		}

	return fd;
	}

int open_db_file(char *filename) {
	int fd = open(filename, O_RDWR, 0644); // same functionality as the second part of create db file function, only no creation
	/*
	 * O_RDWR -> opens the file as read/write
	 * 0644 -> gives user and group read/write permission, but everyone else readonly
	 */
	if (fd == -1) {
		perror(("open"));
		return STATUS_ERROR;
	}

	return fd;
}


