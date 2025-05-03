# employee_db

This project is a simple employee database application written in C. It allows you to create, open, and manage a database of employee records.

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
    git clone <repository_url>
    ```
2.  Navigate to the project directory:

    ```bash
    cd employee_db
    ```
3.  Build the project using Make:

    ```bash
    make
    ```

### Running the Application

The application is run from the command line with the following options:

-   `-n`: Creates a new database file.
-   `-f <database file>`: Specifies the path to the database file. This option is required.

#### Examples

-   Create a new database file:

    ```bash
    ./employee_db -n -f mydatabase.db
    ```

-   Open an existing database file:

    ```bash
    ./employee_db -f mydatabase.db
    ```

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

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues to suggest improvements or report bugs.
