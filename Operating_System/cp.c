#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFF_SIZE = 5;

char *path_join(char *dst, const char *path, const char *extra) {
    if (dst == NULL) {
        dst = (char *) malloc(sizeof(char) * (strlen(path) + strlen(extra) + 10));
    }
    int dst_i = 0;
    for (int i = 0; i < strlen(path); ++i) {
        dst[dst_i++] = path[i];
    }
    if (dst[dst_i - 1] != '/' && dst[dst_i - 1] != '\\')
        dst[dst_i++] = '/';
    for (int i = 0; i < strlen(extra); ++i) {
        dst[dst_i++] = extra[i];
    }
    dst[dst_i] = '\0';
    return dst;
}

char *get_filename(char *absolute_path) {
    unsigned long i;
    for (i = strlen(absolute_path) - 1; i >= 0; --i) {
        if (absolute_path[i] == '/' || absolute_path[i] == '\\') {
            break;
        }
    }
    char *filename = (char *) malloc(sizeof(char) * strlen(absolute_path) - i);
    strcpy(filename, &absolute_path[i + 1]);
    return filename;
}

bool isDir(const char *dir_str, bool strict) {
    struct stat status;
    if (stat(dir_str, &status) == -1) {
        printf("read stat on \"%s\" is failed!\n", dir_str);
        if (strict)
            exit(0);
        else
            return false;
    } else {
        if (!S_ISDIR(status.st_mode)) {
            if (strict)
                printf("\"%s\" is not a directory!\n", dir_str);
            return false;
        }
    }
    return true;
}

bool isFile(const char *file_str, bool strict) {
    struct stat status;
    if (lstat(file_str, &status) == -1) {
        printf("read stat on \"%s\" is failed!\n", file_str);
        if (strict)
            exit(0);
        else
            return false;
    } else {
        if (!S_ISREG(status.st_mode)) {
            if (strict)
                printf("\"%s\" is not a file!\n", file_str);
            return false;
        }
    }
    return true;
}

bool copy_file(char *src_file_path, char *dst_file_path, bool ignore_error) {
    if (!isFile(src_file_path, true)) {
        printf("\"%s\" is not a file!\n", src_file_path);
        if (ignore_error) {
            return false;
        } else {
            exit(0);
        }
    }

    int fd_src = open(src_file_path, O_RDONLY);
    if (fd_src == -1) {
        printf("open file \"%s\" failed;\n", src_file_path);
        if (ignore_error) {
            return false;
        } else {
            exit(0);
        }
    }
    remove(dst_file_path);
    int fd_dst = open(dst_file_path, O_WRONLY | O_CREAT);
    chmod(dst_file_path, 777);
    if (fd_dst == -1) {
        printf("open(create) file \"%s\" failed;\n", dst_file_path);
        if (ignore_error) {
            return false;
        } else {
            exit(0);
        }
    }
    char *buff = malloc(sizeof(char) * (BUFF_SIZE + 10));
    size_t r_size;
    printf("copy");
    while ((r_size = read(fd_src, buff, BUFF_SIZE)) != 0) {
        printf(".");
        fflush(stdout);
        if (r_size == -1) {
            printf("read file \"%s\" failed;\n", src_file_path);
            if (ignore_error) {
                return false;
            } else {
                exit(0);
            }
        }
        if (write(fd_dst, buff, r_size) == -1) {
            printf("write file \"%s\" failed;\n", dst_file_path);
            if (ignore_error) {
                return false;
            } else {
                exit(0);
            }
        }
    }
    printf("\"%s\"--success\n", src_file_path);
    return true;
}

bool copy_dir_files(char *src_path, char *dst_path, bool ignore_error) {
    if (!isDir(src_path, true))
        exit(0);

    struct stat status;
    stat(src_path, &status);
    mkdir(dst_path, status.st_mode);

    DIR *src_dir = opendir(src_path);
    DIR *dst_dir = opendir(dst_path);
    if (src_dir == NULL || dst_dir == NULL) {
        printf("open dir failed!\n");
        if (ignore_error)
            return false;
        else
            exit(0);
    }

    struct dirent *dirp = NULL;
    char *src_file_path;
    char *dst_file_path;

    while ((dirp = readdir(src_dir)) != NULL) {
        if (dirp->d_name[0] == '.')
            continue;
        src_file_path = path_join(NULL, src_path, dirp->d_name);
        dst_file_path = path_join(NULL, dst_path, dirp->d_name);

        bool flag = false;//success flag
        if (isDir(src_file_path, false)) {
            src_file_path[strlen(src_file_path)] = '/';
            dst_file_path[strlen(dst_file_path)] = '/';
            flag = copy_dir_files(src_file_path, dst_file_path, true);
            printf("copy...dir\"%s\"", dirp->d_name);
            if (flag)
                printf("--success\n");
            else
                printf("--failed\n");
        } else if (isFile(src_file_path, false)) {
            flag = copy_file(src_file_path, dst_file_path, true);
        }


    }

    return true;

}

int main(int argc, char **argv) {
    char *src, *dst;
    if (argc == 3 && isFile(argv[1], false)) {
        src = argv[1];
        dst = argv[2];
        if (isDir(dst, false)) {
            char *filename = get_filename(src);
            char *dst_file_path = path_join(NULL, dst, filename);
            copy_file(src, dst_file_path, true);
        } else if (isFile(dst, false)) {
            copy_file(src, dst, true);
        } else {
            printf("[format]:cp -r <src_dir> <dst_dir>\n"
                   "or [format]:cp <src_file> <dst_file>\n"
                   "or [format]:cp <src_file> <dst_di>");
            exit(0);
        }
    } else if (argc == 4 && strcmp(argv[1], "-r") == 0) {
        src = argv[2];
        dst = argv[3];
        copy_dir_files(src, dst, true);
    } else {
        printf("[format]:cp -r <src_dir> <dst_dir>\n"
               "or [format]:cp <src_file> <dst_file>\n"
               "or [format]:cp <src_file> <dst_di>");
        exit(0);
    }
    return 0;
}