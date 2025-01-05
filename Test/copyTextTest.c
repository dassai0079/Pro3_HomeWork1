#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stringBuffer {
    char str[100];
    struct stringBuffer *prev;
    struct stringBuffer *next;
} strbuf;

// 新しいノードを作成
void newNextStr(strbuf *current) {
    strbuf *newNode = (strbuf *)malloc(sizeof(strbuf));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    newNode->prev = current;
    newNode->next = NULL;
    current->next = newNode;
}

// コピー元ファイルから指定された位置の文字列を取得
void copyContentFromFile(FILE *srcFile, int startPos, int endPos, strbuf *heading) {
    fseek(srcFile, startPos, SEEK_SET);  // ファイルのstartPosまで移動

    int currentPos = startPos;
    while (currentPos <= endPos && fgets(heading->str, sizeof(heading->str), srcFile)) {
        currentPos += strlen(heading->str);
        if (currentPos > endPos) {
            // endPosに達したら、切り取り処理
            heading->str[endPos - (currentPos - strlen(heading->str))] = '\0';
            break;
        }
        newNextStr(heading);
        heading = heading->next;
    }
}


void insertIntoDestFile(FILE *destFile, strbuf *heading, int destPos) {
    fseek(destFile, destPos, SEEK_SET);  // 挿入位置に移動
    strbuf *current = heading;

    // 挿入する文字列をファイルに書き込む
    while (current != NULL) {
        fputs(current->str, destFile);
        current = current->next;
    }
}

int main() {
    FILE *srcFile = fopen("source.txt", "r");
    FILE *destFile = fopen("destination.txt", "r+");

    if (!srcFile || !destFile) {
        fprintf(stderr, "Error opening files.\n");
        return 1;
    }

    // コピー元ファイルから、挿入する範囲の文字列を取得
    strbuf *heading = (strbuf *)malloc(sizeof(strbuf));
    int startPos = 50;  // 例えば、コピー元ファイルの50文字目から
    int endPos = 150;   // 150文字目までの文字列をコピー

    copyContentFromFile(srcFile, startPos, endPos, heading);

    // コピー先ファイルに挿入
    int destPos = 200;  // 挿入位置はコピー先ファイルの200文字目
    insertIntoDestFile(destFile, heading, destPos);

    fclose(srcFile);
    fclose(destFile);

    return 0;
}
