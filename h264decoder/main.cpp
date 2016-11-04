#include <stdio.h>
#include "h264dec.h"

#define INBUF_SIZE 100 * 1024;


static int FindStartCode(unsigned char *Buf, int zeros_in_startcode)
{
    int info;
    int i;

    info = 1;
    for (i=0; i < zeros_in_startcode; i++) {
        if(Buf[i] != 0)
            info = 0;
    }

    if(Buf[i] != 1)
        info = 0;
    return info;
}

static bool Check_StartCode(unsigned char *Buf, int pos)
{
    int info3 = 0;

    info3 = FindStartCode(&Buf[pos-4], 3);
    return info3 == 1;
}

static int getNextNal(FILE* inpf, unsigned char* Buf)
{
    int pos = 0;
    int StartCodeFound = 0;
    int info2 = 0;
    int info3 = 0;

    int nCount = 0;
    while(!feof(inpf) && ++nCount <= 4) {
        Buf[pos++]=fgetc(inpf);
    }

    if(!Check_StartCode(Buf, pos)) {
        return 0;
    }


    while(!feof(inpf) && (Buf[pos++]=fgetc(inpf))==0);

    while (!StartCodeFound) {
        if (feof (inpf)) {
            return pos-1;
        }
        Buf[pos++] = fgetc (inpf);

        StartCodeFound = Check_StartCode(Buf, pos);
    }

    fseek (inpf, -4, SEEK_CUR);

    return pos - 4;
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("Usage: %s [input_fliename] [width] [height]" \
               "[output_filename]\n", argv[0]);
        return -1;
    }

    unsigned short usWidth = atoi(argv[2]);
    unsigned short usHeight = atoi(argv[3]);

    h264dec *pdec = new h264dec;

    if(!pdec->InitH264Decoder(usWidth, usHeight)) {
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
        int nCount = getNextNal(ifp, p_In_Frame);

        if (nCount == 0) {
            continue;
        }

        unsigned char *ptr = p_In_Frame;
        int n_Outlen = 0;

        printf("data @0x%x, size = 0x%x\n",
               (unsigned int)ptr, nCount);
        printf(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
               *ptr, *(ptr+1), *(ptr+2), *(ptr+3),
               *(ptr+4), *(ptr+5), *(ptr+6), *(ptr+8));

        pdec->H264Decode(ptr, nCount, p_Out_Frame, n_Outlen);

        if(n_Outlen > 0) {
            fwrite(p_Out_Frame, 1, n_Outlen, ofp);
        }
    }

    delete []p_In_Frame;
    delete []p_Out_Frame;
    pdec->StopH264Decoder();
    pdec->ReleaseConnection();
    fclose(ifp);
    fclose(ofp);

    return 0;
}
