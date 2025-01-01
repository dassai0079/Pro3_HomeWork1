#include <stdio.h>
#include <unistd.h>
#include <termios.h>

typedef struct string String;
struct string{
    char str[100];
    String *next;
};

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
    
    //ファイルオープン部
    printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動
    //printf("\e[7m");    //文字の背景、色を反転
    printf("ファイルの名前を入力してください :\t");
    scanf("%s",fileName);
    fp=fopen(fileName,"r");
    if(fp==NULL){
        //char isMakeFile;
        printf("そのファイルは存在しません\n");
        return(0);
        // printf("新たに作成しますか？ （Y,y/N,n）");
        // scanf("%c",isMakeFile);
        // if(isMakeFile=="y" || isMakeFile=="y"){
        //     fp=fopen(fileName,"w");
        // }
    }

    //ファイル表示部+strに文字を代入していく
    while(fgets(str, sizeof(str), fp)) {
        printf("%s", str);
    }

    //キー入力部
    printf("Press a key (q to quit)\n");
    setTerminalMode();  // 非カノニカルモードに設定
    while (1) {
        char c = getKey();  // キー入力を取得
        switch (c)
        {
            case 'q':   //qを入力して終了
                break;
            default:
                break;
        }
        //printf("You pressed: %c\n", c);
    }
    resetTerminalMode();  // 端末設定を元に戻す

    fclose(fp);
    // printf("\e[37m\e[47m");
    // printf("hello world\n");
    printf("\e[49m\e[39m"); //画面の状態復帰
    return(0);
}

