#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#define MAX_LEN 30

typedef struct stringBuffer{
    char str[100];
    struct stringBuffer *prev;
    struct stringBuffer *next;
}strbuf;

void setTerminalMode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);   // 現在の端末設定を取得
    new_termios.c_lflag &= ~(ICANON|ECHO);   // 非カノニカルモードにする/入力内容を表示しない
    new_termios.c_cc[VMIN] = 1;              // 最小文字数を1に設定
    new_termios.c_cc[VTIME] = 0;             // タイムアウトなし
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

void newNextStr(strbuf* from){ //受け取ったstrbufのnext要素を埋める
    strbuf* to = malloc(sizeof(strbuf));
    if(from->next=NULL){
        to->next=NULL;
    }else if(from->next!=NULL){
        to->next=from->next;
        to->next->prev=to;
    }
    from->next=to;
    to->prev=from;
}

FILE *getFileToCopy(){
    FILE *fp;
    char ch;
    int index = 0;
    char fileName[MAX_LEN];
    resetTerminalMode();
    // ユーザーにファイル名を入力させる
    printf("\e[7mWhere to copy? : ");
    scanf("%s",fileName);
    setTerminalMode();
    fp=fopen(fileName,"r");
    if(fp==NULL){
        printf("そのファイルは存在しません\n");
        return NULL;
    }
    printf("open!!\n");
    return fp;
}

int main(void){
    FILE *fp;
    FILE *copyFp;
    char fileName[FILENAME_MAX];
    char utility[MAX_LEN];
    char c;
    int a;
    strbuf* head = malloc(sizeof(strbuf));
    head->prev=NULL;
    head->next=NULL;
    strbuf* heading=head;

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
    printf("\e[7m");    //文字の背景、色を反転
    printf("%s\n",fileName);
    printf("\e[0m");
    //fseek(fp, 0, SEEK_SET);  // ファイルポインタを最初に戻す
    //ファイル表示部+strに文字を代入していく
    while(fgets(heading->str, sizeof(heading->str), fp)) {
        printf("%s", heading->str);
        newNextStr(heading);
    }
    printf("\e[7m");    //文字の背景、色を反転
    printf("Press a key (q to quit)\n");
    printf("\e[0m");

    setTerminalMode();  // 非カノニカルモードに設定

    while(1){   //メインループ
        // fflush(stdout);
        // printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動
        // heading=head;
        // while(heading->next==NULL){
        //     printf("%s",heading->str);
        //     heading=heading->next;
        // }
        c = getKey();  // キー入力を取得
        fflush(stdout);
        printf("\e[7m%c\e[0m",c);    //文字の背景、色を反転
        switch(c){
        case 'c':
            copyFp=getFileToCopy();
            break;
        case 'q':
            resetTerminalMode();  // 端末設定を元に戻す
            fclose(fp);
            printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動
            printf("\n\e[0m"); //画面の状態復帰
            return(0);
            break;
        default:
            break;
        }
    }
}
