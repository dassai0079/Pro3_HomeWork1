#include <stdio.h>
#include <unistd.h>  // sleep()を使うため
#include <sys/ioctl.h>
#include <stdlib.h>



void getWindowSize(int *rows, int *cols){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)==-1) {
        perror("error: Failed to get window size");
        exit(1);
    }
    *rows=ws.ws_row;
    *cols=ws.ws_col;
}

// 画面のクリアとカーソルの移動
void clearScreen() {
    int row,col;
    getWindowSize(&row,&col);
    // 画面全体をクリア
    //printf("\e[2J");
    system("clear");
    // カーソルを画面の最上部に移動
    printf("\e[H");
    usleep(100000);  // 0.5秒間隔で表示
    printf("\e[%dA",row);
    // 画面全体をクリア
    //printf("\e[2J");
    // カーソルを画面の最上部に移動
    printf("\e[H");
    usleep(100000);  // 0.5秒間隔で表示
    // 画面全体をクリア
    //printf("\e[2J");
    // カーソルを画面の最上部に移動
    printf("\e[H");
    usleep(100000);  // 0.5秒間隔で表示
}

void printMultipleLines() {
    // いくつかの行を表示
    for (int i = 0; i < 30; i++) {
        printf("Line %d\n", i+1);
        fflush(stdout);  // バッファを強制的にフラッシュして即時表示
        usleep(100000);  // 0.5秒間隔で表示
    }
}

int main() {
    // 複数行を表示
    printMultipleLines();
    
    // 少し待つ（表示内容がターミナルウィンドウを超えるタイミングで）
    sleep(2);  // 2秒間待つ
    
    // 画面を消去
    clearScreen();

    return 0;
}