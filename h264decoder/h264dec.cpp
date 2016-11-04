#include <stdio.h>
#include "h264dec.h"


#if 0
#define ERROR() std::cout << __FUNCTION__ << ":" << \
        __LINE__  << std::endl
#else
#define ERROR() printf("ERROR: %s:%d\n", __FUNCTION__, __LINE__)
#endif

#define LOAD_LIB_FUNC(lib, f)   do { \
    f = (typeof(f))((lib)->loadFunction(#f)); \
    if (!f) { ERROR(); } \
} while (0)


h264dec::h264dec(void)
    :lib(NULL)
    ,pdec(NULL)
    ,pdecContext(NULL)
    ,pdecFrame(NULL)
{
}

h264dec::~h264dec(void)
{
}

bool h264dec::LoadlibFun()
{
    lib = new Tlib("/usr/local/lib/libavcodec.so");

    LOAD_LIB_FUNC(lib, avcodec_register_all);
    LOAD_LIB_FUNC(lib, avcodec_alloc_context3);
    LOAD_LIB_FUNC(lib, av_frame_alloc);
    LOAD_LIB_FUNC(lib, avcodec_find_decoder);
    LOAD_LIB_FUNC(lib, avcodec_open2);
    LOAD_LIB_FUNC(lib, av_packet_alloc);
    LOAD_LIB_FUNC(lib, av_packet_free);
    LOAD_LIB_FUNC(lib, av_init_packet);
    LOAD_LIB_FUNC(lib, avcodec_decode_video2);
    LOAD_LIB_FUNC(lib, avcodec_close);
    LOAD_LIB_FUNC(lib, av_free);

    if (!lib->ok) {
        return false;
    }

    avcodec_register_all();
    return true;
}

bool h264dec::InitH264Decoder(int width,int height)
{
    if (!LoadlibFun()) {
        ERROR();
        return false;
    }

    m_width = width;
    m_height = height;
    pdec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (pdec == NULL ) {
        ERROR();
        return false;
    }

    pdecContext = avcodec_alloc_context3(pdec);
    pdecFrame = av_frame_alloc();

    //pdecContext->width  = width;
    //pdecContext->height = height;
    //pdecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (pdec->capabilities & CODEC_CAP_TRUNCATED)
        pdecContext->flags |= CODEC_FLAG_TRUNCATED;

    if (avcodec_open2(pdecContext, pdec, NULL) < 0) {
        ERROR();
        return false;
    }

    pkt = av_packet_alloc();
    av_init_packet(pkt);

    return true;
}

bool h264dec::H264Decode(unsigned char *inbuf, const int &inlen,
                         unsigned char *outbuf, int &outlen)
{
    int got_frame = 0;
    unsigned char *showYUV[3];
    int showW[3], showH[3];
    int ret = -1;

    pkt->data = inbuf;
    pkt->size = inlen;

    ret = avcodec_decode_video2(pdecContext, pdecFrame, &got_frame,
                                pkt);

    if (ret < 0) {
        ERROR();
        return false;
    }

    if (got_frame) {
        showYUV[0] = outbuf;
        showYUV[1] = showYUV[0] + m_width * m_height;
        showYUV[2] = showYUV[1] + m_width * m_height/4;
        showW[0] = m_width;
        showW[1] = m_width>>1;
        showW[2] = m_width>>1;
        showH[0] = m_height;
        showH[1] = m_height>>1;
        showH[2] = m_height>>1;

        for (int i=0; i<3; i++) {
            memcpy(showYUV[i], pdecFrame->data[i],
                   showW[i] * showH[i]);
        }

        outlen = m_width * m_height * 3 / 2;
    } else {
        outlen = 0;
    }

    return true;
}

void h264dec::StopH264Decoder()
{
    if (pkt) {
        av_packet_free(&pkt);
        pkt = NULL;
    }
    if (pdecContext != NULL) {
        avcodec_close(pdecContext);
        av_free( pdecContext  );
        pdecContext = NULL;
        if(pdecFrame) {
            av_free(pdecFrame);
            pdecFrame = NULL;
        }
    }
    if (lib) {
        delete lib;
        lib=0;
    }
}

void h264dec::ReleaseConnection()
{
    delete this;
}

