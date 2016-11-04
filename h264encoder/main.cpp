#include <stdio.h>
#include "h264enc.h"



static int getYUVFrame(FILE *inpf, unsigned char *Buf,
        unsigned short w, unsigned short h)
{
    int size = fread(Buf, 1, w*h*3/2, inpf);

    if (size < w*h*3/2) {
        return -1;
    }
    
    return size;
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("Usage: %s [input_fliename] [width] [height]"
                " [output_filename]\n", argv[0]);
        return -1;
    }

    unsigned short usWidth = atoi(argv[2]);
    unsigned short usHeight = atoi(argv[3]);

    h264enc *penc = new h264enc;

    if(!penc->InitH264Encoder(usWidth, usHeight)) {
        return false;
    }

    unsigned char *p_In_Frame =
        new unsigned char[usWidth * usHeight * 3/2];
    unsigned char *p_Out_Frame =
        new unsigned char[usWidth * usHeight * 3/2];
    FILE* ifp = fopen(argv[1],"rb");
    FILE* ofp = fopen(argv[4],"wb");

    bool b_continue = true;
    int nReadUnit = usWidth * usHeight * 3/2;

    while (!feof(ifp)) {
        int nCount = getYUVFrame(ifp, p_In_Frame, usWidth, usHeight);

        if (nCount <= 0) {
            continue;
        }

        unsigned char *ptr = p_In_Frame;
        int n_Outlen = 0;

        printf("data @0x%x, size = 0x%x\n",
               (unsigned int)ptr, nCount);
        printf(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
               *ptr, *(ptr+1), *(ptr+2), *(ptr+3),
               *(ptr+4), *(ptr+5), *(ptr+6), *(ptr+8));

        penc->H264Encode(ptr, nCount, p_Out_Frame, n_Outlen);

        if(n_Outlen > 0) {
            fwrite(p_Out_Frame, 1, n_Outlen, ofp);
        }
    }

    delete []p_In_Frame;
    delete []p_Out_Frame;
    penc->StopH264Encoder();
    penc->ReleaseConnection();
    fclose(ifp);
    fclose(ofp);

    return 0;
}
