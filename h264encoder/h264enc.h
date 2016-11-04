#pragma once
#include "tlib.h"
#include "libavcodec/avcodec.h"


class h264enc 
{
public:
    virtual bool InitH264Encoder(int width,int height);
    virtual bool H264Encode(unsigned char * inbuf, const int & inlen,
                            unsigned char * outbuf,int & outlen);
    virtual void StopH264Encoder();
    virtual void ReleaseConnection();

public:
    h264enc(void);
    virtual ~h264enc(void);

private:
    Tlib *lib;

    bool LoadlibFun();
    void (*avcodec_init)(void);
    void (*avcodec_register_all)(void);
    AVCodecContext* (*avcodec_alloc_context3)(const AVCodec *codec);
    AVFrame *(*av_frame_alloc)(void);
    AVCodec *(*avcodec_find_encoder)(enum AVCodecID id);
    AVPacket *(*av_packet_alloc)(void);
    void (*av_packet_free)(AVPacket **pkt);
    int (*av_new_packet)(AVPacket *pkt, int size);
    int  (*avcodec_open2)(AVCodecContext *avctx,
        const AVCodec *codec, AVDictionary **options);
    int (*avcodec_encode_video2)(AVCodecContext *avctx, 
            AVPacket *avpkt, const AVFrame *frame,
            int *got_packet_ptr);
    int  (*avcodec_close)(AVCodecContext *avctx);
    void (*av_free)(void *ptr);
    int (*avpicture_fill)(AVPicture *picture, const uint8_t *ptr,
            enum AVPixelFormat pix_fmt, int width, int height);


private:
    AVCodec         *penc;
    AVCodecContext  *pencContext;
    AVFrame         *pencFrame;
    AVPacket        *pkt;
    int             m_width;
    int             m_height;
};

