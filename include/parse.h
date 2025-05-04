#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct dbheader_t {
	unsigned int magic; // magic value tells parser if this is a valid file to deal with
	unsigned short version; // version of file spec
	unsigned short count; // number of employees
	unsigned int filesize; // metadata, size of the file in kb, used for corruption avoidance
};

struct employee_t {
	char name[256];
	char address[256]; // 256 is a good round number for buffers
	unsigned int hours;
};

/* all functions will be the same scaffolding, returning some good or bad value or a file descriptor*/
int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut);
void output_file(int fd, struct dbheader_t *, struct employee_t *employees);
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring);

// TODO: implement the following functions
struct employee_t * remove_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *employeename);
void update_employee_hours(struct dbheader_t *dbhdr, struct employee_t *employee, char *employeename, char *newhours);

#endif
