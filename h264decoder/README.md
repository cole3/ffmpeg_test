###decoder h264 es to YUV420
    from a h264 es file to a yuv file, using libavcodec.so
####use ffplay to play yuv file
    ffplay -f rawvideo -pixel_format yuv420p -video_size 1920x1200 test.yuv
