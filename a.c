#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#define MAX_LEN 30

typedef struct stringBuffer{
    char str[100];
    struct stringBuffer *next;
}strbuf;

void setTerminalMode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);   // 現在の端末設定を取得
    new_termios.c_lflag &= ~(ICANON|ECHO);   // 非カノニカルモードにする/入力内容を表示しない
    new_termios.c_cc[VMIN] = 1;              // 最小文字数を1に設定
    new_termios.c_cc[VTIME] = 1;             // タイムアウトなし
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);  // 設定を即時反映
}

void resetTerminalMode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);
    new_termios.c_lflag |= ICANON;           // カノニカルモードに戻す
    new_termios.c_lflag |= ECHO;             // 入力内容を表示
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

char getKey(void) {
    char buf;
    if (read(STDIN_FILENO, &buf, 1) == 1) {
        return buf;
    }
    return 0;  // エラーまたは入力なし
}

int main(void){
    FILE *fp;
    char fileName[FILENAME_MAX];
    char str[10];
    char utility[MAX_LEN];
    char c;
    //ファイルオープン部
    printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動
    printf("ファイルの名前を入力してください :\t");
    scanf("%s",fileName);
    fp=fopen(fileName,"r");
    if(fp==NULL){
        printf("そのファイルは存在しません\n");
        return(0);
    }
    printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動

    // //ファイル表示部+strに文字を代入していく
    // while(fgets(str, sizeof(str), fp)) {
    //     printf("%s", str);
    // }

    //キー入力部
    //printf("Press a key (q to quit)\n");

    printf("\e[7m");    //文字の背景、色を反転
    printf("%s\n",fileName);
    printf("\e[0m");
    fseek(fp, 0, SEEK_SET);  // ファイルポインタを最初に戻す
    //ファイル表示部+strに文字を代入していく
    while(fgets(str, sizeof(str), fp)) {
        printf("%s", str);
    }

    printf("\e[7m");    //文字の背景、色を反転
    printf("Press a key (q to quit)\n");

    setTerminalMode();  // 非カノニカルモードに設定
    while (1) {
        // printf("\e[7m");    //文字の背景、色を反転
        // printf("%s\n",fileName);
        // printf("\e[0m");
        // fseek(fp, 0, SEEK_SET);  // ファイルポインタを最初に戻す
        // //ファイル表示部+strに文字を代入していく
        // while(fgets(str, sizeof(str), fp)) {
        //     printf("%s", str);
        // }

        c = getKey();  // キー入力を取得
        sprintf(utility,"You pressed: %c", c);
        printf("\e[H\e[J");  // 画面をクリアせず、上書き更新
        printf("\e[7m");    //文字の背景、色を反転
        // printf("Press a key (q to quit)\n");
        printf("%s",utility);
        printf("\e[0m");

        switch (c)
        {
            case 'q':   //qを入力して終了
                resetTerminalMode();  // 端末設定を元に戻す
                fclose(fp);
                printf("\n\e[49m\e[39m"); //画面の状態復帰
                return(0);
            default:
                printf("%s",utility);
                //printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動    
                break;
        }
    }
    resetTerminalMode();  // 端末設定を元に戻す
    fclose(fp);
    printf("\e[49m\e[39m"); //画面の状態復帰
    return(0);
}
