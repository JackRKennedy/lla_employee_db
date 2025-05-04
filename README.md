# employee_db

A simple employee database application written in C. It allows you to create, open, and manage employee records.

## Badges

[![C](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

## Features

-   Create a new employee database file.
-   Open an existing employee database file.
-   Validate the database header to ensure file integrity.
-   Add new employee records.
-   List existing employee records.

## Getting Started

### Prerequisites

-   A C compiler (e.g., GCC)
-   Make

### Building the Project

1.  Clone the repository:

    ```bash
    git clone https://github.com/JackRKennedy/lla_employee_db.git
    ```

2.  Navigate to the project directory:

    ```bash
    cd lla_employee_db
    ```

3.  Build the project using Make:

    ```bash
    make
    ```

### Running the Application

The application is run from the command line.

**Usage:**

**Options:**

-   `-n`: Creates a new database file.
-   `-f <database file>`: Specifies the path to the database file.
-   `-a "<name>,<address>,<hours>"`: Adds a new employee to the database. The employee details should be provided as a comma-separated string. Ensure there are no spaces within the string.
-   `-l`: Lists all employees in the database.

#### Examples

-   Create a new database file:

    ```bash
    ./employee_db -n -f mydatabase.db
    ```

-   Open an existing database file:

    ```bash
    ./employee_db -f mydatabase.db
    ```

-   Add a new employee to the database:

    ```bash
    ./employee_db -f mydatabase.db -a "JohnDoe,123FakeSt.,120"
    ```

-   List all employees in the database:

    ```bash
    ./employee_db -f mydatabase.db -l
    ```

**Important Notes:**

-   The `-f` option is *always* required. All commands must be run in conjunction with a target database file.
-   When using the `-a` option, ensure that the employee details are provided as a single comma-separated string without any spaces within the string itself. For example, use "JohnDoe,123FakeSt.,120", and *not* "John Doe, 123 Fake St., 120".

## File Structure

The project structure is as follows:

-   `include/`: Contains header files.
    -   `parse.h`: Defines the data structures and function prototypes for database operations.
    -   `common.h`: Defines common constants and structures.
-   `src/`: Contains source files.
    -   `main.c`: Contains the main function and command-line argument parsing.
    -   `file.c`: Contains file operation functions (e.g., creating and opening database files).
    -   `parse.c`: Contains functions for parsing and manipulating the database file, including header validation and data handling.
-   `Makefile`: Defines the build process.

## Data Structures

-   `struct dbheader_t`: Represents the database header, containing metadata about the database file.
    -   `magic`: Magic value to identify the file as a valid employee database.
    -   `version`: Version of the database file format.
    -   `count`: Number of employee records in the database.
    -   `filesize`: Size of the database file in bytes.
-   `struct employee_t`: Represents an employee record.
    -   `name`: Employee's name.
    -   `address`: Employee's address.
    -   `hours`: Employee's hours.

## Future Features

-   **Remove Employee:** Implement functionality to remove an employee from the database by name.
-   **Update Employee Details:** Implement functionality to update an employee's details (address, hours) by name.

## Contributing

Contributions are welcome! Feel free to fork the repository and contribute how you wish.

## Acknowledgments

This project was built based on tutorials and lessons from [Low Level Academy](https://lowlevelacademy.com/).
