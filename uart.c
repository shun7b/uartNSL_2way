#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyS5"
#define SERIAL_PORT2 "/dev/ttyS7"
int main(int argc, char *argv[])
{
    unsigned char msg[] = "serial port open...\n";
    unsigned char buf[255],buf2[255];             // バッファ
    int fd,fd2;                             // ファイルディスクリプタ
    struct termios tio,tio2;                 // シリアル通信設定
    int baudRate = B9600;
    int i;
    int len,len2;
    int ret;
    int size;

    fd = open(SERIAL_PORT, O_RDWR); 
    // デバイスをオープンする
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }
    fd2 = open(SERIAL_PORT2, O_RDWR);     // デバイスをオープンする
    if (fd2 < 0) {
        printf("open error\n");
        return -1;
    }
    tio.c_cflag += CREAD;               // 受信有効
    tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;                 // データビット:8bit
    tio.c_cflag += 0;                   // ストップビット:1bit
    tio.c_cflag += 0;                   // パリティ:None

    tio2.c_cflag += CREAD;               // 受信有効
    tio2.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio2.c_cflag += CS8;                 // データビット:8bit
    tio2.c_cflag += 0;                   // ストップビット:1bit
    tio2.c_cflag += 0;

    cfsetispeed( &tio, baudRate );
    cfsetospeed( &tio, baudRate );
    cfsetispeed( &tio2, baudRate );
    cfsetospeed( &tio2, baudRate );
    cfmakeraw(&tio);                    // RAWモード
    cfmakeraw(&tio2);
    tcsetattr( fd, TCSANOW, &tio );     // デバイスに設定を行う
    tcsetattr( fd2, TCSANOW, &tio2 ); 
    ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする
    ioctl(fd2, TCSETS, &tio2);
    // 送受信処理ループ
    while(1) {
        len = read(fd, buf, sizeof(buf));
        len2 = read(fd2, buf2, sizeof(buf2));
        if (0 < len) {
            for(i = 0; i < len; i++) {
                printf("%02X", buf[i]);
            }
            printf("\n");
        }
       
        if (0 < len2) {
            for(i = 0; i < len2; i++) {
                printf("%02X", buf2[i]);
            }
            printf("\n");
        }
        // エコーバック
       // write(fd, buf, len);
    }

    close(fd);
    close(fd2);                              // デバイスのクローズ
    return 0;
}
