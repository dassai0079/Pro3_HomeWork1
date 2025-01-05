#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100

// UTF-8エンコードでn番目の文字を取得する関数
char* get_char_pointer_by_index(const char* str, int index) {
    int current_char_index = 0;  // 現在の文字のインデックス
    const char* ptr = str;       // 現在位置のポインタ

    while (*ptr != '\0') {
        // 1文字分のバイト数を取得
        unsigned char c = (unsigned char) *ptr;
        int char_len = 1;

        // UTF-8の文字のバイト数を調べる
        if (c >= 0x80 && c <= 0xBF) {
            char_len = 1;
        } else if (c >= 0xC0 && c <= 0xDF) {
            char_len = 2;
        } else if (c >= 0xE0 && c <= 0xEF) {
            char_len = 3;
        } else if (c >= 0xF0 && c <= 0xF7) {
            char_len = 4;
        }

        // 現在の文字が目的のインデックスならそのポインタを返す
        if (current_char_index == index) {
            return (char*) ptr;
        }

        // 次の文字に進む
        ptr += char_len;
        current_char_index++;
    }

    // 指定されたインデックスが範囲外の場合はNULLを返す
    return NULL;
}

int main() {
    char str[MAX_SIZE] = "foooふーフーfooo";
    int index = 4;  // 例: 5番目の文字（'ふ'）を指定

    char* char_ptr = get_char_pointer_by_index(str, index);

    if (char_ptr != NULL) {
        printf("Index %d: Character '%s' is at position %ld in the array.\n", index, char_ptr, char_ptr - str);
    } else {
        printf("Index %d is out of range.\n", index);
    }
    printf("%s\n",&str[4]);

    return 0;
}
