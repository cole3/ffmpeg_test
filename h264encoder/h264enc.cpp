#include <stdio.h>
#include "h264enc.h"


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


h264enc::h264enc(void)
    :lib(NULL)
    ,penc(NULL)
    ,pencContext(NULL)
    ,pencFrame(NULL)
{
}

h264enc::~h264enc(void)
{
}

bool h264enc::LoadlibFun()
{
    lib = new Tlib("/usr/local/lib/libavcodec.so");

    LOAD_LIB_FUNC(lib, avcodec_register_all);
    LOAD_LIB_FUNC(lib, avcodec_alloc_context3);
    LOAD_LIB_FUNC(lib, av_frame_alloc);
    LOAD_LIB_FUNC(lib, avcodec_find_encoder);
    LOAD_LIB_FUNC(lib, avcodec_open2);
    LOAD_LIB_FUNC(lib, av_packet_alloc);
    LOAD_LIB_FUNC(lib, av_packet_free);
    LOAD_LIB_FUNC(lib, av_new_packet);
    LOAD_LIB_FUNC(lib, avcodec_encode_video2);
    LOAD_LIB_FUNC(lib, avcodec_close);
    LOAD_LIB_FUNC(lib, av_free);
    LOAD_LIB_FUNC(lib, avpicture_fill);

    if (!lib->ok) {
        return false;
    }

    avcodec_register_all();
    return true;
}

bool h264enc::InitH264Encoder(int width,int height)
{
    if (!LoadlibFun()) {
        ERROR();
        return false;
    }

    m_width = width;
    m_height = height;

    pencContext = avcodec_alloc_context3(penc);
    if (!pencContext) {
        ERROR();
        return false;
    }

    pencContext->codec_id = AV_CODEC_ID_H264;
    pencContext->codec_type = AVMEDIA_TYPE_VIDEO;
    pencContext->pix_fmt = AV_PIX_FMT_YUV420P;
    pencContext->width = width;
    pencContext->height = height;
    pencContext->bit_rate = 400000;
    pencContext->gop_size = 10;
    pencContext->time_base.num = 1;
    pencContext->time_base.den = 25;
    pencContext->qmin = 20;
    pencContext->qmax = 51;
    pencContext->max_b_frames = 1;

    penc = avcodec_find_encoder(pencContext->codec_id);
    if (penc == NULL) {
        ERROR();
        return false;
    }

    if (avcodec_open2(pencContext, penc, NULL) < 0) {
        ERROR();
        return false;
    }

    pencFrame = av_frame_alloc();

    pkt = av_packet_alloc();
    av_new_packet(pkt, width*height*3/2);

    return true;
}

bool h264enc::H264Encode(unsigned char *inbuf, const int &inlen,
                         unsigned char *outbuf, int &outlen)
{
    int got_frame = 0;
    int ret = -1;

    pencFrame->format = AV_PIX_FMT_YUV420P;
    pencFrame->width = m_width;
    pencFrame->height = m_height;

    avpicture_fill((AVPicture *)pencFrame, inbuf, 
            pencContext->pix_fmt, m_width, m_height);

    pencFrame->data[0] = inbuf;
    pencFrame->data[1] = pencFrame->data[0] + m_width * m_height;
    pencFrame->data[2] = pencFrame->data[1] + m_width * m_height / 4;

    ret = avcodec_encode_video2(pencContext, pkt, pencFrame,
            &got_frame);

    if (ret < 0) {
        ERROR();
        return false;
    }

    if (got_frame) {
        memcpy(outbuf, pkt->data, pkt->size);
        outlen = pkt->size;
    } else {
        outlen = 0;
    }

    return true;
}

void h264enc::StopH264Encoder()
{
    if (pkt) {
        av_packet_free(&pkt);
        pkt = NULL;
    }
    if (pencContext != NULL) {
        avcodec_close(pencContext);
        av_free( pencContext  );
        pencContext = NULL;
        if(pencFrame) {
            av_free(pencFrame);
            pencFrame = NULL;
        }
    }
    if (lib) {
        delete lib;
        lib=0;
    }
}

void h264enc::ReleaseConnection()
{
    delete this;
}

