// -----------------------------------------------------------------------
/*
	uart_test.c

					Sep/20/2020
*/
// -----------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/select.h>
#include <jpeglib.h>
#include <stdint.h>
#define SERIAL_PORT2 "/dev/ttyS7"
#define SERIAL_PORT "/dev/ttyS5"
typedef struct {
	uint8_t *data;   // raw data
	uint32_t width;
	uint32_t height;
	uint32_t ch;     // color channels
} JpegData;

unsigned char w[3];

// allocate memory for raw data

void alloc_jpeg(JpegData *jpegData)

{
		int i;
    jpegData->data = NULL;

   jpegData->data = (uint8_t*) malloc(sizeof(uint8_t)  *

                                       jpegData->width  *

                                       jpegData->height *

                                       jpegData->ch);

	  //jpegData->data=(JSAMPARRAY)malloc(sizeof(JSAMPROW) * jpegData->height*3*jpegData->width*sizeof(JSAMPLE));
/*		for(i=0;i<jpegData->height;i++){
			jpegData->data[i]=(JSAMPROW) malloc(sizeof(JSAMPLE) * 3 * jpegData->width);
		}*/
}



// free memory for raw data

void free_jpeg(JpegData *jpegData)

{

    if (jpegData->data != NULL) {

        free(jpegData->data);

        jpegData->data = NULL;

    }

}



// read JPEG image

// 1. create JPEG decompression object

// 2. specify source data

// 3. read JPEG header

// 4. set parameters

// 5. start decompression

// 6. scan lines

// 7. finish decompression

bool read_jpeg(JpegData *jpegData,

              const char *srcfile,

              struct jpeg_error_mgr *jerr)

{

    // 1.

    struct jpeg_decompress_struct cinfo;

    jpeg_create_decompress(&cinfo);

    cinfo.err = jpeg_std_error(jerr);



    FILE *fp = fopen(srcfile, "rb");

    if (fp == NULL) {

        printf("Error: failed to open %s\n", srcfile);

        return false;

    }

    // 2.

    jpeg_stdio_src(&cinfo, fp);



    // 3.

    jpeg_read_header(&cinfo, TRUE);



    // 4. omitted

    // 5.

    jpeg_start_decompress(&cinfo);



    jpegData->width  = cinfo.image_width;

    jpegData->height = cinfo.image_height;

    jpegData->ch     = cinfo.num_components;



    alloc_jpeg(jpegData);



    // 6. read line by line

    uint8_t *row = jpegData->data;

    const uint32_t stride = jpegData->width * jpegData->ch;

    for (int y = 0; y < jpegData->height; y++) {

        jpeg_read_scanlines(&cinfo, &row, 1);

        row += stride;

    }
    // 7.
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return true;
}



// write JPEG image

// 1. create JPEG compression object

// 2. specify destination data

// 3. set parameters

// 4. start compression

// 5. scan lines

// 6. finish compression

bool write_jpeg(const JpegData *jpegData,

                const char *dstfile,

                struct jpeg_error_mgr *jerr)

{

    // 1.

    struct jpeg_compress_struct cinfo;

    jpeg_create_compress(&cinfo);

    cinfo.err = jpeg_std_error(jerr);



    FILE *fp = fopen(dstfile, "wb");

    if (fp == NULL) {

        printf("Error: failed to open %s\n", dstfile);

        return false;

    }

    // 2.

    jpeg_stdio_dest(&cinfo, fp);



    // 3.

    cinfo.image_width      = jpegData->width;

    cinfo.image_height     = jpegData->height;

    cinfo.input_components = jpegData->ch;

    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
		//jpeg_set_quality(&cinfo, 50, TRUE);




    // 4.

    jpeg_start_compress(&cinfo, TRUE);



    // 5.

    uint8_t *row = jpegData->data;

    const uint32_t stride = jpegData->width * jpegData->ch;


    for (int y = 0; y < jpegData->height; y++) {

        jpeg_write_scanlines(&cinfo, &row, 1);

        row += stride;

    }



    // 6.

    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);

    fclose(fp);



    return true;

}
// -----------------------------------------------------------------------
int main(int argc,char *argv[])
{
	int fd,fd2;
	int it;
	uint8_t buf[960],buf2[960];
	struct termios tio,tio2;
	uint8_t input[460800];
	uint8_t r_ary[307200],g_ary[307200],b_ary[307200];
	printf( "*** start ***\n");
	fprintf(stderr, "*** Sep/30/2020 ***\n");

	for (it = 0; it < sizeof(buf); it++)
		{
		buf[it] = 0;
    buf2[it] = 0;
		}

	int rv; 
	struct timeval timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 5000;

	fd_set set,set2;

	if ((fd = open(SERIAL_PORT, O_RDWR)) < 0)
		{
		fprintf(stderr, "open error\n");
		exit(1);
		}
    if ((fd2 = open(SERIAL_PORT2, O_RDWR)) < 0)
		{
		fprintf(stderr, "open error\n");
		exit(1);
		}
	FD_ZERO(&set);
	FD_SET(fd, &set);
	FD_ZERO(&set2);
	FD_SET(fd2, &set2);
	fprintf(stderr, "*** check *** aaa ***\n");
  bzero(&tio, sizeof(tio2));
	bzero(&tio, sizeof(tio));

	/* 115200bps, フロー制御有り, ８ビット，DTR/DSR無効，受信可能 */
	tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	tio.c_iflag =IGNPAR|IGNBRK;
	tio.c_cc[VMIN] = 1;	/* 入力データをバッファしない */
	tcsetattr(fd, TCSANOW, &tio); /* アトリビュートのセット */
  tio2.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	tio2.c_iflag =IGNPAR|IGNBRK;
	tio2.c_cc[VMIN] = 1;	/* 入力データをバッファしない */
	tcsetattr(fd, TCSANOW, &tio); /* アトリビュートのセット */
	int sum=0;
	uint32_t i,j;

	w[0]='M';
	w[1]=0x0d;
	w[2]=0x0a;
	write(fd, w, 3);

	for( sum=0;sum<2;it++){
		
		int len = read(fd, buf, sizeof(buf));
    int len2 = read(fd2, buf2, sizeof(buf));
		//printf("%dread = %d:len%d",sum, it,len);
		sum=len+sum;
		for (i = 0; i<len; i++)
		{
				printf("%4x,%4x",buf[i],buf2[i]);
				input[j]=buf[i];
				j++;
		}
		//printf("\n");					
	}
  close(fd);
  close(fd2);
  return 0;
  for( j=0;j<480;j++){
    for( i=0;i<320;i++){
              r_ary[j*640 + i*2 ] =   (input[j*960 + i*3] & 0xf0)>>4;
                g_ary[j*640 + i*2 ] =    input[j*960 + i*3] & 0xf;  
              b_ary[j*640 + i*2 ] =   (input[j*960 + i*3 +1] & 0xf0)>>4;  
              r_ary[j*640 + i*2 +1] =  input[j*960 + i*3 +1] & 0xf;
              g_ary[j*640 + i*2 +1] = (input[j*960 + i*3 +2] & 0xf0)>>4;  
              b_ary[j*640 + i*2 +1] =  input[j*960 + i*3+2] & 0xf;  
        }
  }

	printf("*** end %d***\n",sum);
	
    JpegData jpeg_out_data;

		struct jpeg_compress_struct outcinfo;

    struct jpeg_error_mgr jerr,outjerr;



    // src/dst file

    char *src = "./lena.jpg";

    char *dst = "./out.jpg";
		uint32_t x,y,four_x,four_y;
		uint32_t k;
		uint32_t point_set_x[3],point_set_y[3];
		uint32_t four_width,four_height;
		uint32_t triangle_width,triangle_height;
		uint32_t triangle_x,triangle_y,line_width;
		uint32_t two_triangle_width,two_triangle_height;
		uint32_t two_triangle_top_x,two_triangle_top_y,two_triangle_right,two_triangle_left;
		uint32_t four_triangle_width,four_triangle_height;
		uint32_t four_triangle_top_x,four_triangle_top_y,four_triangle_middle_y,four_triangle_bottom_x,four_triangle_bottom_y,four_triangle_right,four_triangle_left;
		uint32_t slope_up_begin_x,slope_up_begin_y,slope_up_end_x,slope_up_end_y,slope_up_width,draw_slope_up_plot;
		uint32_t slope_down_begin_x,slope_down_begin_y,slope_down_end_x,slope_down_end_y,slope_down_width,draw_slope_down_plot;
		uint32_t angle_curve_x,angle_curve_y,angle_curve_r;
/*    if (!read_jpeg(&jpegData, src, &jerr)){

        free_jpeg(&jpegData);

        return -1;

    }*/

    //printf("Read:  %s\n", src);




    // reverse all bits

   // int size = jpegData.width * jpegData.height * jpegData.ch;
//    int diff[2]={0,0};
 //   int sub;
		

		jpeg_out_data.height=480;
		jpeg_out_data.width=640;
		jpeg_out_data.ch=3;

		alloc_jpeg(&jpeg_out_data);

//		printf("h=%d,w=%d",jpegData.height,jpegData.width);
		for(i=0;i<jpeg_out_data.height;i++){
			for(j=0;j<jpeg_out_data.width;j++){
					//jpeg_out_data.data[(i*jpegData.width+j)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
				//	printf("\n%d\n",jpegData.data[i*jpegData.width+j*jpegData.ch+k]);
//				jpeg_out_data.data[((jpeg_out_data.height-j)*jpeg_out_data.width+i)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
//				jpeg_out_data.data[((jpeg_out_data.height-j)*jpeg_out_data.width+i)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
									
					jpeg_out_data.data[(uint32_t)((i*jpeg_out_data.width+j)*jpeg_out_data.ch+0)]=r_ary[(uint32_t)((i*jpeg_out_data.width+j))]*16;
					jpeg_out_data.data[(uint32_t)((i*jpeg_out_data.width+j)*jpeg_out_data.ch+1)]=g_ary[(uint32_t)((i*jpeg_out_data.width+j))]*16;
					jpeg_out_data.data[(uint32_t)((i*jpeg_out_data.width+j)*jpeg_out_data.ch+2)]=b_ary[(uint32_t)((i*jpeg_out_data.width+j))]*16;
			}
    }
		
    if (!write_jpeg(&jpeg_out_data, dst, &outjerr)){
  //      free_jpeg(&jpegData);
        free_jpeg(&jpeg_out_data);
        return -1;
    }
    printf("Write: %s\n", dst);
//    free_jpeg(&jpegData);
    free_jpeg(&jpeg_out_data);
    return 0;

}