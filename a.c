#include <stdio.h>

int main(void){
    FILE *fp;
    char fileName[FILENAME_MAX];
    //printf("\e[2J\e[1;1H"); //ターミナルをクリア=>カーソルを1行1列目に移動
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
    printf("Open File !\n");
    fclose(fp);
    // printf("\e[37m\e[47m");
    // printf("hello world\n");
    printf("\e[49m\e[39m"); //画面の状態復帰
    return(0);
}

