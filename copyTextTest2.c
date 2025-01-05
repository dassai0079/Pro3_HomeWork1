#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stringBuffer {
    char str[100];  // 各行を格納
    struct stringBuffer *prev;  // 前のノード
    struct stringBuffer *next;  // 次のノード
} strbuf;

void newNextStr(strbuf *node) {
    node->next = (strbuf *)malloc(sizeof(strbuf));
    node->next->prev = node;
    node->next->next = NULL;
}
void readFileToList(FILE *fp, strbuf *heading) {
    while (fgets(heading->str, sizeof(heading->str), fp)) {
        printf("%s", heading->str);  // 行を表示（デバッグ用）
        newNextStr(heading);  // リストの次のノードを作成
        heading = heading->next;  // 次のノードに移動
    }
}
strbuf *findInsertionPosition(strbuf *heading, int targetLine, int targetPos) {
    int lineCount = 1;
    while (heading != NULL && lineCount < targetLine) {
        heading = heading->next;
        lineCount++;
    }
    if (heading == NULL) return NULL;  // 対象行が存在しない場合
    return heading;  // 位置が見つかった場合、対象のノードを返す
}
void insertStringAtPosition(strbuf *heading, char *strToInsert, int targetLine, int targetPos) {
    strbuf *insertAtNode = findInsertionPosition(heading, targetLine, targetPos);
    if (insertAtNode == NULL) return;

    // 挿入する位置の前後にノードを作成
    strbuf *newNode = (strbuf *)malloc(sizeof(strbuf));
    strcpy(newNode->str, strToInsert);  // 挿入する文字列を格納
    newNode->prev = insertAtNode->prev;
    newNode->next = insertAtNode;
    
    if (insertAtNode->prev != NULL) {
        insertAtNode->prev->next = newNode;
    }
    insertAtNode->prev = newNode;
}
void writeListToFile(FILE *outFile, strbuf *heading) {
    while (heading != NULL) {
        fputs(heading->str, outFile);
        heading = heading->next;
    }
}

int main() {
    FILE *fpIn, *fpOut;
    strbuf *heading = (strbuf *)malloc(sizeof(strbuf));  // 最初のノード

    // コピー元ファイルを開く
    fpIn = fopen("source.txt", "r");
    if (fpIn == NULL) {
        perror("Error opening source file");
        return 1;
    }

    // コピー先ファイルを開く
    fpOut = fopen("destination.txt", "r+");
    if (fpOut == NULL) {
        perror("Error opening destination file");
        fclose(fpIn);
        return 1;
    }

    // コピー元ファイルの内容をリストに読み込む
    readFileToList(fpIn, heading);

    // 挿入する文字列（コピー元ファイルの一部）を指定
    // ここでは、source.txt の 5 行目から 7 行目を destination.txt の 10 行目に挿入すると仮定
    char strToInsert[300];  // 最大3行分の文字列を格納
    strbuf *insertSource = heading;
    int lineCount = 1;
    while (insertSource != NULL && lineCount <= 7) {
        if (lineCount >= 5) {
            strcat(strToInsert, insertSource->str);
        }
        insertSource = insertSource->next;
        lineCount++;
    }

    // destination.txt の 10 行目に挿入
    insertStringAtPosition(heading, strToInsert, 10, 1);

    // 結果を新しいファイルに書き込む
    fseek(fpOut, 0, SEEK_SET);  // 先頭から書き込みを開始
    writeListToFile(fpOut, heading);

    // ファイルを閉じる
    fclose(fpIn);
    fclose(fpOut);

    return 0;
}
