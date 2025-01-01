#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void set_terminal_mode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);  // 現在の端末設定を取得
    new_termios.c_lflag &= ~(ICANON|ECHO);   // 非カノニカルモードにする/入力内容を表示しない
    //printf("%d\n",new_termios.c_cc[VMIN]);
    //printf("%d\n",new_termios.c_cc[VTIME]);
    new_termios.c_cc[VMIN] = 1;              // 最小文字数を1に設定
    new_termios.c_cc[VTIME] = 1;             // タイムアウトなし
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);  // 設定を即時反映
}

void reset_terminal_mode(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);
    new_termios.c_lflag |= ICANON;           // カノニカルモードに戻す
    new_termios.c_lflag |= ECHO;             // 入力内容を表示
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

char get_key(void) {
    char buf;
    if (read(STDIN_FILENO, &buf, 1) == 1) {
        return buf;
    }
    return 0;  // エラーまたは入力なし
}

int main(void) {
    printf("%d\n",STDIN_FILENO);
    printf("Press a key (q to quit)\n");

    set_terminal_mode();  // 非カノニカルモードに設定

    while (1) {
        char c = get_key();  // キー入力を取得
        if (c == 'q') {
            break;  // 'q'が入力されたら終了
        }
        printf("You pressed: %c\n", c);
    }

    reset_terminal_mode();  // 端末設定を元に戻す
    return 0;
}
