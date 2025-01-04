#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#define MAX_LEN 30

typedef struct stringBuffer{
    char str[100];
    struct stringBuffer *prev;
    struct stringBuffer *next;
}strbuf;

void setTerminalMode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO,&new_termios);   // 現在の端末設定を取得
    new_termios.c_lflag&=~(ICANON|ECHO);   // 非カノニカルモードにする/入力内容を表示しない
    new_termios.c_cc[VMIN]=1;              // 最小文字数を1に設定
    new_termios.c_cc[VTIME]=0;             // タイムアウトなし
    tcsetattr(STDIN_FILENO,TCSANOW,&new_termios);  // 設定を即時反映
}

void resetTerminalMode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO,&new_termios);
    new_termios.c_lflag|=ICANON;           //カノニカルモードに戻す
    new_termios.c_lflag|=ECHO;             //入力内容を表示
    tcsetattr(STDIN_FILENO,TCSANOW,&new_termios);
}

char getKey(void) {
    char buf;
    if(read(STDIN_FILENO,&buf,1)==1) {
        return buf;
    }
    return 0;  // エラーまたは入力なし
}

void getWindowSize(int *rows, int *cols){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)==-1) {
        perror("error: Failed to get window size");
        exit(1);
    }
    *rows=ws.ws_row;
    *cols=ws.ws_col;
}

void clearLine(int line){
    printf("\e[%d;1H",line);    //受け取った行の1列目へカーソル移動
    printf("\e[K");             //カーソル位置から行末までを消去する
}

void moveCursor(int row, int column){
    printf("\e[%d;%dH",row,column);
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
    char fileName[MAX_LEN];
    char newFile;
    int row;
    int column;
    getWindowSize(&row,&column);
    resetTerminalMode();
    printf("\e[7mWhere to copy? : ");   //ユーザーにファイル名を入力させる
    scanf("%s",fileName);
    while (getchar()!='\n');  //改行まで読み飛ばす
    clearLine(row);
    clearLine(row-1);
    fflush(stdout);
    fp=fopen(fileName,"r");
    if(fp==NULL){
        printf("\e[7m");
        printf("そのファイルは存在しません。新規に作成しますか？ (Y,y/N,n): ");
        scanf("%c",&newFile);
        printf("\e[0m");
        clearLine(row-1);
        moveCursor(row-1,1);
        printf("\e[7mq: quit c: CopyMode\nYou typed : \e[0m");
        fflush(stdout);
    
        if(newFile=='N'||newFile=='n'){         //ビューモードに戻る
            printf("コピーモードを終了します:");
            fp=NULL;
        }else if(newFile=='Y'||newFile=='y'){   //新規ファイルを作成する
            fp=fopen(fileName,"w+");
        }
    }else{
        fp=fopen(fileName,"r+");    //ファイルが存在する時は更新モードで開く
    }
    setTerminalMode();  //カノニカルモードを解除
    return fp;
}

int main(void){
    FILE *fp;
    FILE *copyFp;
    int row;    //行
    int column; //列
    char fileName[FILENAME_MAX];
    char c;
    strbuf* head = malloc(sizeof(strbuf));
    head->prev=NULL;
    head->next=NULL;
    strbuf* heading=head;

    //ファイルオープン部
    printf("\e[2J"); //ターミナルをクリア
    moveCursor(1,1);        //カーソルを1行1列目に移動
    printf("ファイルの名前を入力してください :\t");
    scanf("%s",fileName);
    fp=fopen(fileName,"r");
    if(fp==NULL){
        printf("そのファイルは存在しません\n");
        return(0);
    }

    //出力準備
    printf("\e[2J"); //ターミナルをクリア
    moveCursor(1,1);        //カーソルを1行1列目に移動

    //ファイル名表示
    printf("\e[7m");    //文字の背景、色を反転
    printf("%s\n",fileName);
    printf("\e[0m");    //文字の背景、色を標準に戻す

    //ファイルの内容を出力+構造体(リスト構造)にそれを保持させる
    while(fgets(heading->str, sizeof(heading->str), fp)) {
        printf("%s", heading->str);
        newNextStr(heading);
        heading=heading->next;
    }

    // //debug
    // heading=head;
    // int i=0;
    // int a=0;
    // char buf[3];
    // while(heading->next!=NULL){
    //     for(i=0; i<sizeof(heading->str); i+=2){
    //         for(a=0; a<2; a++){
    //             buf[a]=heading->str[i+a];
    //         }
    //         printf("%s",buf);
    //     }
    //     heading=heading->next;
    // }

    //補助文を表示
    printf("\e[7m");    //文字の背景、色を反転
    getWindowSize(&row,&column);    //ウィンドウサイズを取得
    moveCursor(row,1);
    printf("\e[7mq: quit c: CopyMode\nYou typed : %c \e[0m",c);    //文字の背景、色を反転して入力を表示
    printf("\e[0m");    //文字の背景、色を標準に戻す
    fflush(stdout);

    setTerminalMode();  //非カノニカルモードに設定

    while(1){   //メインループ
        c=getKey();  // キー入力を取得
        getWindowSize(&row,&column);    //ウィンドウサイズを取得
        clearLine(row);               //1つ前のステップで入力を表示した部分をクリア
        clearLine(row-1);
        printf("\e[7mq: quit c: CopyMode\nYou typed : %c \e[0m",c);    //文字の背景、色を反転して入力を表示
        fflush(stdout);
        switch(c){
        case 'c':
            clearLine(row);               //1つ前のステップで入力を表示した部分をクリア
            clearLine(row-1);
            copyFp=getFileToCopy(); //コピー先ファイルを取得+ファイルポインタを取得
            break;
        case 'q':
            resetTerminalMode();    //端末設定を元に戻す
            fclose(fp);
            printf("\e[2J");        //ターミナルをクリア
            moveCursor(1,1);        //カーソルを1行1列目に移動
            printf("\e[0m");        //画面の状態復帰
            return(0);
            break;
        default:
            break;
        }
    }
}
