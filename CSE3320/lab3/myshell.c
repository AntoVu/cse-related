// ADD NAME
// ADD COURSE NUMBER
// ADD LAB NUMBER
// NOT DOING NOW SINCE THIS IS PUBLIC ON GITHUB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h> // Process IDs and file sizes
#include <sys/stat.h> // File metadata and stat()
#include <unistd.h> // POSIX (portable operating system interface) calls
#include <dirent.h> // Directory handling
#include <limits.h> // Prevent buffer overflow
#include <sys/wait.h> // Used for parent process to wait for child

#define MAX_ENTRIES 1024
#define MAX_NAME 2048
#define PAGE_SIZE 5

// File structure
// name - filename
// size - size of file from stat
// mtime - last modified time from stat
// mode - permission bits from stat (check if we should be allowed to edit file)
typedef struct {
    char name[MAX_NAME];
    off_t size;
    time_t mtime;
    mode_t mode;
} FileEntry;

// Keep files and directories in different arrays so we can display them
// Expand the scope of the whole code so they're always accessible
static FileEntry files[MAX_ENTRIES];
static FileEntry dirs[MAX_ENTRIES];
static int file_count = 0;
static int dir_count = 0;
static int file_offset = 0;
static int dir_offset = 0;

// Load the current working directory and put its contents into correct array
// Get the size, time, and mode by calling stat()
// Entires get sorted into files array or dirs array depending on its type
void load_directory(void) {
    DIR *d;
    struct dirent *de;
    struct stat st;
    // Reset the states
    file_count = 0;
    dir_count = 0;
    file_offset = 0;
    dir_offset = 0;
    // Open directory
    d = opendir(".");
    if (d == NULL) {
        perror("opendir");
        return;
    }
    // Loop through entries
    while ((de = readdir(d)) != NULL) {
        // Skip "."
        if (strcmp(de->d_name, ".") == 0)
            continue;
        // Get metadata
        if (stat(de->d_name, &st) == -1) {
            perror("stat");
            continue;
        }
        // If it's a directory
        if (S_ISDIR(st.st_mode)) {
            if (dir_count < MAX_ENTRIES) {
                strncpy(dirs[dir_count].name, de->d_name, MAX_NAME - 1);
                dirs[dir_count].name[MAX_NAME - 1] = '\0';
                dirs[dir_count].size = st.st_size;
                dirs[dir_count].mtime = st.st_mtime;
                dirs[dir_count].mode = st.st_mode;
                dir_count++;
            }
        }
        // If not its a file
        else if (S_ISREG(st.st_mode)) {
            if (file_count < MAX_ENTRIES) {
                strncpy(files[file_count].name, de->d_name, MAX_NAME - 1);
                files[file_count].name[MAX_NAME - 1] = '\0';
                files[file_count].size = st.st_size;
                files[file_count].mtime = st.st_mtime;
                files[file_count].mode = st.st_mode;
                file_count++;
            }
        }
    }
    closedir(d);
}

// Alphabetical sorting
int cmp_by_name(const void *a, const void *b) {
    return strcmp(((FileEntry *)a)->name, ((FileEntry *)b)->name);
}

// Smaler to larger sorting
int cmp_by_size(const void *a, const void *b) {
    off_t diff = ((FileEntry *)a)->size - ((FileEntry *)b)->size;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

// Older to newer sorting
int cmp_by_date(const void *a, const void *b) {
    time_t diff = ((FileEntry *)a)->mtime - ((FileEntry *)b)->mtime;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

// Let user choose how to sort files and directories
void sort_entries(void) {
    char choice[8];
    // Read user input
    printf("\nSort by: (N)ame  (S)ize  (D)ate: ");
    if (fgets(choice, sizeof(choice), stdin) == NULL)
        return;
    // Allow for both uppercase and lowercase (take notes lab2)
    char c = toupper(choice[0]);
    // Sort by name using qsort
    if (c == 'N') {
        qsort(files, file_count, sizeof(FileEntry), cmp_by_name);
        qsort(dirs, dir_count, sizeof(FileEntry), cmp_by_name);
        printf("Sorted by name.\n");
    }
    // Sort by size using qsort
    else if (c == 'S') {
        qsort(files, file_count, sizeof(FileEntry), cmp_by_size);
        qsort(dirs, dir_count, sizeof(FileEntry), cmp_by_name);
        printf("Sorted by size.\n");
    }
    // Sort by date using qsort
    else if (c == 'D') {
        qsort(files, file_count, sizeof(FileEntry), cmp_by_date);
        qsort(dirs, dir_count, sizeof(FileEntry), cmp_by_date);
        printf("Sorted by date.\n");
    }
    // Complain
    else {
        printf("Unknown sort option. Keeping current order.\n");
    }
    // Reset pagination (set view list back to 0-4 instead of keeping it on something like 5-9 after sorting)
    file_offset = 0;
    dir_offset = 0;
}

// Displays the menu
// Show the current directory
// Show current date and time
// Show list of files
// Show list of directories
// Show command options
void display_menu(void) {
    char cwd[PATH_MAX];
    time_t t;
    int i, end;
    printf("\n================================================\n");
    // Current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("  Current Working Dir: %s\n", cwd);
    else perror("getcwd");
    // Time
    t = time(NULL);
    // Used ChatGPT to show how to convert from UTC to CST
    setenv("TZ", "America/Chicago", 1);
    tzset();
    struct tm *local = localtime(&t);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%B %d %Y, %I:%M %p %Z", local);
    printf("  Current Time: %s\n", timebuf);
    // Show files
    printf("\n  Files:\n");
    if (file_count == 0) {
        printf("    (no files)\n");
    }
    else {
        end = file_offset + PAGE_SIZE;
        if (end > file_count) end = file_count;
        for (i = file_offset; i < end; i++)
            printf("    %3d.  %s\n", i, files[i].name);
        // If too many files on display, show prompts to tell user you can change view
        if (file_offset > 0)
            printf("    [P] Previous files...\n");
        if (end < file_count)
            printf("    [N] More files...\n");
    }
    // Show directories
    printf("\n  Directories:\n");
    if (dir_count == 0) {
        printf("    (no subdirectories)\n");
    }
    else {
        end = dir_offset + PAGE_SIZE;
        if (end > dir_count) end = dir_count;
        for (i = dir_offset; i < end; i++)
            printf("    %3d.  %s\n", i, dirs[i].name);
        if (dir_offset > 0)
            printf("    [P] Previous dirs...\n");
        if (end < dir_count)
            printf("    [N] More dirs...\n");
    }
    // Available commands
    printf("\n------------------------------------------------\n");
    printf("  Operations:\n");
    printf("    E  Edit a file\n");
    printf("    R  Run a program\n");
    printf("    C  Change directory\n");
    printf("    S  Sort directory listing\n");
    printf("    M  Move into a directory (by number)\n");
    printf("    N  Next page\n");
    printf("    P  Previous page\n");
    printf("    Q  Quit\n");
    printf("------------------------------------------------\n");
    printf("  Command: ");
}

// Runs the program by forking child process to prevent using system()
void run_program(char *cmd_line) {
    // Remove newline character
    cmd_line[strcspn(cmd_line, "\n")] = '\0';
    // Complain
    if (strlen(cmd_line) == 0) {
        printf("No command entered.\n");
        return;
    }
    // argv[0] is program name then arguments then null
    char *argv[MAX_ENTRIES + 1];
    int argc = 0;
    char buf[MAX_NAME * 2];
    strncpy(buf, cmd_line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    // Tokenize into argv
    char *token = strtok(buf, " \t");
    while (token != NULL && argc < MAX_ENTRIES) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL; 
    // More complaining
    if (argc == 0) {
        printf("No command entered.\n");
        return;
    }
    // Fork process
    pid_t pid = fork();
    // If fork failed start complaining
    if (pid == -1) {
        perror("fork");
    }
    else if (pid == 0) {
        // Child process
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }
    else {
        // Parent wait
        wait(NULL);
    }
}

// Opens nano text editor
// Used Claude to show how to open files in nano while running code
void cmd_edit(void) {
    char input[MAX_NAME];
    char cmd[MAX_NAME + 8];
    // Display
    printf("  Edit file name (or number): ");
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    input[strcspn(input, "\n")] = '\0';
    // More complaining
    if (strlen(input) == 0) {
        printf("  No file specified.\n");
        return;
    }
    // Change number to filename
    char *filename = input;
    char resolved[MAX_NAME];
    int num = -1;
    // If input is an int
    int is_num = 1;
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isdigit((unsigned char)input[i])) { is_num = 0; break; }
    }
    if (is_num) num = atoi(input);
    if (num >= 0 && num < file_count) {
        strncpy(resolved, files[num].name, MAX_NAME - 1);
        resolved[MAX_NAME - 1] = '\0';
        filename = resolved;
    }
    // Run nano
    snprintf(cmd, sizeof(cmd), "nano %s", filename);
    run_program(cmd);
}

 // Asks for a program name and arguments if applicable and runs it
 // Also accepts numbers corresponding to the list
void cmd_run(void) {
    char input[MAX_NAME * 2];
    // Gets program name or number
    printf("  Run (program [args], or file number): ");
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    input[strcspn(input, "\n")] = '\0';
    // More complaining
    if (strlen(input) == 0) {
        printf("  No program specified.\n");
        return;
    }
    // Extract first token
    char first_token[MAX_NAME];
    strncpy(first_token, input, MAX_NAME - 1);
    first_token[MAX_NAME - 1] = '\0';
    strtok(first_token, " \t");

    int is_num = 1;
    for (int i = 0; first_token[i] != '\0'; i++) {
        if (!isdigit((unsigned char)first_token[i])) { is_num = 0; break; }
    }
    // Check if number
    char final_cmd[MAX_NAME * 2];
    if (is_num) {
        int num = atoi(first_token);
        if (num < 0 || num >= file_count) {
            printf("  Invalid file number.\n");
            return;
        }
        // Validate index and check executable (if you have access)
        if (access(files[num].name, X_OK) != 0) {
            printf("  '%s' is not executable.\n", files[num].name);
            return;
        }
        // Build
        snprintf(final_cmd, sizeof(final_cmd), "./%s", files[num].name);
    }
    else {
        // Use raw command
        strncpy(final_cmd, input, sizeof(final_cmd) - 1);
        final_cmd[sizeof(final_cmd) - 1] = '\0';
    }

    run_program(final_cmd);
}

// Changes directory via path or directory number
void cmd_change_dir(void) {
    char input[MAX_NAME];
    // Gets path or number
    printf("  Change to (path or dir number): ");
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    input[strcspn(input, "\n")] = '\0';
    // Complain
    if (strlen(input) == 0) {
        printf("  No directory specified.\n");
        return;
    }
    // Resolve directory if needed
    char *target = input;
    char resolved[MAX_NAME];
    // Check if input is num
    int is_num = 1;
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isdigit((unsigned char)input[i])) { is_num = 0; break; }
    }
    // Find what directory it correlates to
    if (is_num) {
        int num = atoi(input);
        if (num < 0 || num >= dir_count) {
            printf("  Invalid directory number.\n");
            return;
        }
        strncpy(resolved, dirs[num].name, MAX_NAME - 1);
        resolved[MAX_NAME - 1] = '\0';
        target = resolved;
    }
    // Detect if change failed (path doesn't exist)
    if (chdir(target) == -1) {
        perror("chdir");
    }
    else {
        // Reload
        load_directory();
    }
}

// Simplified version of change directory (only accepts number)
void cmd_move_to_dir(void) {
    char input[16];
    // Get number
    printf("  Move to directory number: ");
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    input[strcspn(input, "\n")] = '\0';
    // Ensure its a number
    int is_num = 1;
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isdigit((unsigned char)input[i])) { is_num = 0; break; }
    }
    if (!is_num) {
        printf("  Please enter a valid number.\n");
        return;
    }
    // Complain
    int num = atoi(input);
    if (num < 0 || num >= dir_count) {
        printf("  Invalid directory number.\n");
        return;
    }
    // If not found then error
    if (chdir(dirs[num].name) == -1) {
        perror("chdir");
    }
    else {
        load_directory();
    }
}

// Advance to next file and directory page view
void cmd_next(void) {
    int changed = 0;
    // Move forward a page
    if (file_offset + PAGE_SIZE < file_count) {
        file_offset += PAGE_SIZE;
        changed = 1;
    }
    if (dir_offset + PAGE_SIZE < dir_count) {
        dir_offset += PAGE_SIZE;
        changed = 1;
    }
    if (!changed)
        printf("  Already at the last page.\n");
}

// Go back to previous file and directory page view
void cmd_prev(void) {
    int changed = 0;
    // Go back a page
    if (file_offset - PAGE_SIZE >= 0) {
        file_offset -= PAGE_SIZE;
        changed = 1;
    }
    // Handle edge case
    else if (file_offset > 0) {
        file_offset = 0;
        changed = 1;
    }
    if (dir_offset - PAGE_SIZE >= 0) {
        dir_offset -= PAGE_SIZE;
        changed = 1;
    }
    else if (dir_offset > 0) {
        dir_offset = 0;
        changed = 1;
    }
    if (!changed)
        printf("  Already at the first page.\n");
}

// Start the shell and accepts optional command line args for a start file path
int main(int argc, char *argv[]) {
    // If file path provided
    if (argc > 1) {
        if (chdir(argv[1]) == -1) {
            perror("chdir (startup)");
        }
    }
    // Load initial directory
    load_directory();
    char input[8];
    // Infinite loop until user quits
    while (1) {
        display_menu();
        // Read user input (only accepts first character)
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // EOF same as quit
            break;
        }
        // Accept both upper and lowercase (take notes lab2)
        char cmd = toupper((unsigned char)input[0]);
        // See which command user chose
        switch (cmd) {
            // Edit file
            case 'E':
                cmd_edit();
                load_directory();
                break;
            // Run file
            case 'R':
                cmd_run();
                break;
            // Change directory
            case 'C':
                cmd_change_dir();
                break;
            // Sort entry
            case 'S':
                sort_entries();
                break;
            // Move to number directory
            case 'M':
                cmd_move_to_dir();
                break;
            // Next on view
            case 'N':
                cmd_next();
                break;
            // Previous view
            case 'P':
                cmd_prev();
                break;
            // Quit
            case 'Q':
                printf("\nQuitting.\n");
                exit(0);
            // If empty just do nothing
            case '\n':
            case '\r':
                break;
            // If user is incapable of following directions
            default:
                printf("  Unknown command '%c'. Please try again.\n", cmd);
                break;
        }
    }

    return 0;
}

// References:
// modified version of myshell.c provided by professor
// GNU C manual: https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html
// linux man pages: https://man7.org/linux/man-pages/man2/syscalls.2.html
// cppreference: https://en.cppreference.com/w/c
// sysstat: https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html
// ctime: https://www.geeksforgeeks.org/c/ctime-function-in-c-c/
// wait: https://www.geeksforgeeks.org/c/wait-system-call-c/