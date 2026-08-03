#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	if(argc == 2) {
		FILE *fd = fopen(argv[1], "r");
		if(!fd) {
			fprintf(stderr, "This file doesn't exist\n");
			return 0;
		}
		char buf[4096];
		int n;
		while((n = fread(buf, sizeof(char), 4095, fd)) > 0) {
			buf[n] = '\0';
			char *tok = strtok(buf, "\n");
			while(tok != NULL) {
				int pipefd[2];
				pipe(pipefd);
				printf("\n\n\ndownloading... %s\n\n\n", tok);
				if(fork() == 0) {
					close(pipefd[0]);
					dup2(pipefd[1], STDOUT_FILENO);
					dup2(pipefd[1], STDERR_FILENO);
					close(pipefd[1]);

					execlp("yt-dl", "yt-dl", "-t", "mp3", "--progress", "--cookies-from-browser", "brave", "--remote-components", "ejs:github", tok, NULL);
					perror("execlp failed!\n");
					_exit(1);
				} else {
					char buf[4096];

					close(pipefd[1]); // close write don't need it
					int r;

					while((r = read(pipefd[0], buf, 4095)) > 0) {
						buf[r] = '\0';
						printf("%s", buf);
					}

					close(pipefd[0]);
					wait(NULL);
					tok = strtok(NULL, "\n");
				}
			}
		}
		fclose(fd);
	}
	return 0;
}
