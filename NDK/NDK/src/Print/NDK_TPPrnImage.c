#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>

#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"
#include "NDK.h"
#include "NDK_debug.h"


extern void color_rgb(color_t c, int *r, int *g, int *b);

#define RGB565(R, G, B) ((unsigned short)(((B&0xF8)>>3) | (((unsigned short)G&0xFC)<<3) | (((unsigned short)R&0xF8)<<8)))
typedef unsigned short* ADDR16;
//typedef unsigned int color_t;/**<RGB色彩数值,0(黑色) - 0xFFFF(白色)*/

//灰度转换
static char pattern[8][8] = {
    {0,32,8,40,2,34,10,42},
    {48,16,56,24,50,18,58,26},
    {12,44,4,36,14,46,6,38},
    {60,28,52,20,62,30,54,22},
    {3,35,11,43,1,33,9,41},
    {51,19,59,27,49,17,57,25},
    {15,47,7,39,13,45,5,37},
    {63,31,55,23,61,29,53,21 }
};


static image_t* image_convert_rgb565(image_t *pimage)
{

    unsigned char * imagebits = pimage->image_buf;
    ADDR16 addr, paddr;
    int h, w, tmp;
    unsigned char * palpha, * pa  = NULL;

    if ((pimage->bytes_per_pixel != 3 && pimage->bytes_per_pixel != 4) || !imagebits) {
        return pimage;
    }

    paddr = addr =  (ADDR16)malloc(pimage->width * pimage->height * 2);
    h = pimage->height, w = pimage->width;
    if (pimage->bytes_per_pixel == 3) {
        while (h-- > 0) {
            tmp = w;
            while (tmp-- > 0) {
                *addr++ = RGB565(imagebits[0], imagebits[1], imagebits[2]);
                imagebits += 3;
            }
        }
    } else {
        pa = palpha = malloc(pimage->width * pimage->height);

        while (h-- > 0) {
            tmp = w;
            while (tmp-- > 0) {
                *addr++ = RGB565(imagebits[0], imagebits[1], imagebits[2]);
                *palpha++ = imagebits[3];
                imagebits += 4;
            }
        }
    }
    free(pimage->image_buf);
    pimage->bytes_per_pixel = 2;
    pimage->image_buf = paddr;

    pimage->image_alpha = NULL;

    return pimage;
}

//抖动算法实现彩色转黑白
image_t * ndk_image_decolor(image_t * img)
{
    int i,j,k = 0;
    color_t color;
    int r,g,b;
    char *gray;

    gray = malloc(img->height*img->width*sizeof(char));
    img = image_convert_rgb565(img);//将img转为2字节对应一个像素
    for(i=0; i<img->height; i++) {
        for(j=0; j<img->width; j++) {
            color = (*(unsigned char *)(img->image_buf+(i*img->width+j)*2))|(*(unsigned char *)(img->image_buf+(i*img->width+j)*2+1)<<4);
            color_rgb(color, &r,&g,&b);
            gray[k++] = 0.30*r+0.59*g+0.11*b -30;//-30为调整值，可修改
//          fprintf(stderr,"%d ",gray[k-1]);
        }
    }
    k = 0;
    for(i=0; i<img->height; i++) {
        for(j=0; j<img->width; j++) {
            if(gray[k++]>pattern[i&7][j&7]) {
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2) = 0xff;
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2+1) = 0xff;
            } else {
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2) = 0;
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2+1) = 0;
            }
        }
    }
    return img;
}

#if 0
image_t * image_decolor(image_t * img)
{
    int i,j,k = 0;
    char data = 0;
    color_t color;
    int r,g,b;

    img = image_convert_rgb565(img);//将img转为2字节对应一个像素
    for(i=0; i<img->height; i++) {
        for(j=0; j<img->width; j++) {
            color = (*(unsigned char *)(img->image_buf+(i*img->width+j)*2))|(*(unsigned char *)(img->image_buf+(i*img->width+j)*2+1)<<4);
            color_rgb(color, &r,&g,&b);

            if(((r+g+b)/3) > 80) { //根据阀值来设定黑白，此处暂定为80
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2) = 0xff;
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2+1) = 0xff;
            } else {
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2) = 0;
                *(unsigned char *)(img->image_buf+(i*img->width+j)*2+1) = 0;
            }
        }
    }
    return img;
}
#endif

//image_t格式转为打印格式
print_buf* ndk_image2printbuf(image_t * img)
{

    print_buf * outbuf;
    char mask = 7;
    int i,j,k = 0;
    char data = 0;

    outbuf = calloc(1,sizeof(print_buf));
    outbuf->width = 8*((img->width+7)/8);
    outbuf->height = img->height;

    outbuf->image_buf = malloc(sizeof(char)*(outbuf->width*outbuf->height/8));
    img = image_convert_rgb565(img);

    for(i=0; i<img->height; i++) {
        for(j=0; j<img->width; j++) {
            //if((*((unsigned char *)(img->image_buf+(i*img->width+j)*2))!=0x5d)&&(*((unsigned char *)(img->image_buf+(i*img->width+j)*2+1))!=0xe7))
            if((*((unsigned char *)(img->image_buf+(i*img->width+j)*2))==0)&&(*((unsigned char *)(img->image_buf+(i*img->width+j)*2+1))==0)) {
                data |= 1<<mask;
            }
            mask--;
//          fprintf(stderr,"%d- ",mask);
            if(mask==0xff) {
                mask = 7;
                *(unsigned char *)(outbuf->image_buf+k) = data;
//              fprintf(stderr,"-%x ",data);
                k++;
                data =0;
            }
        }
        if(mask!=7) {
            mask = 7;
            *(unsigned char *)(outbuf->image_buf+k) = data;
//          fprintf(stderr,"~%x ",data);
            k++;
            data =0;
        }
    }
//  fprintf(stderr,"\n________%d__________\n",k);
//
//  for(i=0;i<k;i++)
//  {
//      fprintf(stderr,"%x ",*(unsigned char *)(outbuf->image_buf+i));
//  }
//  fprintf(stderr,"\n__________________\n");
    return outbuf;


}

//bmp转打印格式
#if 0
print_buf* bmp2printbuf(char * file)
{
    print_buf * outbuf;
    FILE *fp;
    int i,ret;
    int bytes, row_bytes;
    char head[62];

    if((file==NULL)||((access(file,F_OK)<0))) {
        return NULL;
    }
    if((fp = fopen(file, "r"))<0) {
        return NULL;
    }

    if(fread(head,1,62,fp)!=62) {
        goto end1;
    }

    outbuf = calloc(1,sizeof(print_buf));
    outbuf->height = (head[25]<<24)|(head[24]<<16)|(head[23]<<8)|head[22];
    bytes = (head[37]<<24)|(head[36]<<16)|(head[35]<<8)|head[34];
    if((outbuf->height==0)||(bytes==0)) {
        goto end2;
    }
    row_bytes = bytes/outbuf->height;
    outbuf->width = row_bytes*8;

    if(row_bytes==0) {
        goto end2;
    }

    outbuf->image_buf = calloc(1,bytes);

    for (i = 0; i < outbuf->height; i++) { /*传进来的image_buf是从上向下，转成BMP要求的从下向上*/
        ret = fread(outbuf->image_buf + (outbuf->height - 1 - i) * row_bytes, 1,row_bytes,fp);
//      fprintf(stderr,"%d-%d-%d ",i,ret,outbuf->height);
        if(ret!=row_bytes) {
            goto end3;
        }
    }
    for (i = 0; i < bytes; i++) {
        *(char *)(outbuf->image_buf + i) = ~*(char *)(outbuf->image_buf + i);
    }

    fclose(fp);
    return outbuf;
end3:
    free(outbuf->image_buf);
end2:
    free(outbuf);
end1:
    fclose(fp);
end:
    return NULL;
}
#endif

/* End of this file */

