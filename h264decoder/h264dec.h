#pragma once
#include "tlib.h"
#include "libavcodec/avcodec.h"


class h264dec
{
public:
    virtual bool InitH264Decoder(int width,int height);
    virtual bool H264Decode(unsigned char * inbuf, const int & inlen,
                            unsigned char * outbuf,int & outlen);
    virtual void StopH264Decoder();
    virtual void ReleaseConnection();

public:
    h264dec(void);
    virtual ~h264dec(void);

private:
    Tlib *lib;

    bool LoadlibFun();
    void (*avcodec_init)(void);
    void (*avcodec_register_all)(void);
    AVCodecContext* (*avcodec_alloc_context3)(const AVCodec *codec);
    AVFrame *(*av_frame_alloc)(void);
    AVCodec *(*avcodec_find_decoder)(enum AVCodecID id);
    AVPacket *(*av_packet_alloc)(void);
    void (*av_packet_free)(AVPacket **pkt);
    void (*av_init_packet)(AVPacket *pkt);
    int  (*avcodec_open2)(AVCodecContext *avctx,
                          const AVCodec *codec, AVDictionary **options);

    int (*avcodec_decode_video2)(AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, const AVPacket *avpkt);




    int  (*avcodec_close)(AVCodecContext *avctx);
    void (*av_free)(void *ptr);

private:
    AVCodec         *pdec;
    AVCodecContext  *pdecContext;
    AVFrame         *pdecFrame;
    AVPacket        *pkt;
    int             m_width;
    int             m_height;
};

