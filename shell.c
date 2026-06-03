// 執行 ls | grep c
// 印出當前目錄裡所有含有 "c" 的檔名

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd); // 建立 pipe

    // 子行程 A：跑 ls
    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        // 把 stdout 接到 pipe 寫端
        dup2(fd[1], 1);
        // 關掉不需要的 fd
        close(fd[0]);
        close(fd[1]);
        // 執行 ls
        char *args[] = {"ls", NULL};
        execvp("ls", args);
    }

    // 子行程 B：跑 grep
    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        // 把 stdin 接到 pipe 讀端
        dup2(fd[0], 0);
        // 關掉不需要的 fd
        close(fd[0]);
        close(fd[1]);
        // 執行 grep
        char *args[] = {"grep", "c", NULL};
        execvp("grep", args);
    }

    // 父行程：關掉 pipe，等兩個子行程結束
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);

    return 0;
}