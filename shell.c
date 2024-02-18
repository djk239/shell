/*
Dale King
Operating Systems Spring 2024
Simple Shell
Make command : gcc -std=c11 -pedantic shell.c -o shell
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>



//prototype functions
void cd(char *path);
void pwd();
void exit_shell();
void makedir(char *path);
void removedir(char *path);
void ls(char *path);
void rm(char *path);
void help();
void mv(char *path, char *dest);
void copyfile(char *path, char *dest);
void makefile(char *path);

//Array to hold commands for help to print
const char* commands[] =
{
"cd <directory> - change directory",
"pwd - print working directory",
"exit - exit the shell",
"help - display help",
"mkdir <directory> - create a directory",
"rmdir <directory> - remove a directory",
"ls <directory> - list files in a directory",
"cp <source> <destination> - copy a file",
"mv <source> <destination> - move a file",
"rm <file> - remove a file",
"touch <source> - creates a file"
};

int main() {
    char input[1024];

    while (1) {
        printf("myshell> ");
        fgets(input, 1024, stdin);

        input[strcspn(input, "\n")] = 0;

        //Variables used to hold the command line arguments
        //Currently supported 1 command and 2 arguments
        //Ex : <command> <argument1> <argument2>
        char *command = strtok(input, " ");
        char *arg1 = strtok(NULL, " ");
        char *arg2 = strtok(NULL, " ");

        //Check for enter clicked
        if(strlen(input) <= 0){
            continue;
        }

        //Handles function calling based on command entered by user
        if (strcmp(command, "cd") == 0) {
            cd(arg1);
        }
        else if (strcmp(command, "pwd") == 0) {
            pwd();
        }
        else if (strcmp(command, "exit") == 0) {
            exit_shell();
        }
        else if (strcmp(command, "mkdir") == 0) {
            makedir(arg1);
        }
        else if (strcmp(command, "rmdir") == 0) {
            removedir(arg1);
        }
        else if (strcmp(command, "ls") == 0) {
            ls(arg1);
        }
        else if (strcmp(command, "rm") == 0) {
            rm(arg1);
        }
        else if (strcmp(command, "help") == 0) {
            help();
        }
        else if (strcmp(command, "mv") == 0) {
            mv(arg1, arg2);
        }
        else if (strcmp(command, "cp") == 0) {
            copyfile(arg1, arg2);
        }
        else if (strcmp(command, "touch") == 0) {
            makefile(arg1);
        }
        //Extend commands above if necessary
        
        //handles unknown command
        else {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}

//Change Directory Function
//cd <arg1>
void cd(char *path) {
    //check for argument
    if (path == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    } else {
        //Change directory and check invalid.
        if (chdir(path) != 0) {
            //print error if invalid directory
            perror("cd");
        }
    }
}

//Print Working Directory Function
//pwd
void pwd() {
    char cwd[1024];

    //Prints current working directory if exist
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
    }
    else {
        //print error if no current directory
        perror("pwd");
    }
}

//Exit function
//exit
void exit_shell(){
    exit(1);
}

//Make directory function
//mkdir <arg1>
void makedir(char *path){
    //check for argument
    if (path == NULL) {
        fprintf(stderr, "mkdir: missing argument\n");
    } else {
        char current_path[1024];
        //get our current path and check for valid
        if (getcwd(current_path, sizeof(current_path)) == NULL) {
            perror("getcwd");
            return;
        }
        char full_path[2048];
        //combine current path with our path name (ensures directory is created in our current working directory)
        snprintf(full_path, sizeof(full_path), "%s/%s", current_path, path);

        //make directory
        if (mkdir(full_path) != 0) {
            perror("mkdir");
        }
        else{
            //if valid let user know directory created
            printf("created directory\n");
        }
    }
}

//Remove directory function
//rmdir <arg1>
void removedir(char *path){
 if (path == NULL) {
        fprintf(stderr, "rmdir: missing argument\n");
    } else {
        char current_path[1024];
        if (getcwd(current_path, sizeof(current_path)) == NULL) {
            perror("getcwd");
            return;
        }
        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "%s/%s", current_path, path);

        //remove directory
        if (rmdir(full_path) != 0) {
            perror("rmdir");
        }
        else{
            printf("removed directory\n");
        }
    }
}

//List files function
//ls <arg1>
void ls(char *path) {
    DIR *dir;
    struct dirent *entry;

    //check for path provided in argument, if no path then current directory
    if (path == NULL) {
        path = ".";
    }

    //open directory
    dir = opendir(path);

    //check for valid
    if (dir == NULL) {
        perror("ls");
        return;
    }

    //print directory entries
    while ((entry = readdir(dir)) != NULL) {
        printf("%s  ", entry->d_name);
    }
    printf("\n");

    //close directory
    closedir(dir);
}

//Remove file function
//rm <arg1>
void rm(char *path){
    //checks for path
    if (path == NULL) {
        fprintf(stderr, "rm: missing argument\n");
    } else {
        char current_path[1024];
        if (getcwd(current_path, sizeof(current_path)) == NULL) {
            perror("getcwd");
            return;
        }
        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "%s/%s", current_path, path);

        //remove file
        if (remove(full_path) != 0) {
            perror("rm");
        }
        else{
            //if file removed let user know
            printf("remove complete\n");
        }
    }
}

//help function
//help
void help()
{
    printf("Commands:\n");
    //goes over commands array and prints each entry
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        printf("%s\n", commands[i]);
    }
    printf("\n");
}

//Move function
//mv <arg1> <arg2>
void mv(char *path, char *dest)
{
    //checks for paths
    if (path == NULL || dest == NULL)
    {
        fprintf(stderr, "mv: missing argument\n");
    }
    else {
        // Check if the source file exists
        if (access(path, F_OK) != 0)
        {
            perror("mv: source file does not exist");
            return;
        }
        //moves file and checks if invalid
        if(rename(path, dest) != 0){
            perror("mv");
        }
        else{
            //if valid let user know
            printf("move complete\n");
        }
    }

}

//Copy file function
//cp <arg1> <arg2>
void copyfile(char *path, char *dest)
{
    FILE *sourceFile, *destFile;
    //check for paths
    if (path == NULL || dest == NULL)
    {
        fprintf(stderr, "cp: missing argument\n");
    }

    //open files (one for copying into)
    sourceFile = fopen(path, "rb");
    destFile = fopen(dest, "wb");

    //checks for valid source
    if(sourceFile == NULL)
    {
        fprintf(stderr, "cp: error opening file\n");
    }
    else {
        char ch;
        //prints entire contents of source into dest
        while((ch = fgetc(sourceFile)) != EOF)
        {
            fputc(ch, destFile);
        }
        //close files and let user know completion
        fclose(sourceFile);
        fclose(destFile);
        printf("copy complete\n");
    }
}

//Make file "touch" function
void makefile(char *path)
{
    FILE *file;
    //check for path
    if(path == NULL){
        fprintf(stderr, "touch: missing argument\n");
    }
    else
    {
    //create and close file, let user know completion
    file = fopen(path, "a");
    fclose(file);
    printf("file created\n");
    }
}
