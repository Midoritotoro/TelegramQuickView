#include "VideoThumbnail.h"
#include <QtCore>
#include <QImage>

void getVideoThumbnail(const std::string& filePath) {
    AVFormatContext* fmt_ctx_ = nullptr;

    // Open video file
    int errCode = avformat_open_input(&fmt_ctx_, filePath.c_str(), nullptr, nullptr);
    if (errCode != 0) {
        
        return;
    }
    
    // read the audio and video flow information
    errCode = avformat_find_stream_info(fmt_ctx_, nullptr);
    if (errCode != 0) {
       
        avformat_close_input(&fmt_ctx_);
        return;
    }
  
    // Print Output Video Related Information
    av_dump_format(fmt_ctx_, 0, filePath.c_str(), 0);


    AVPacket* pkt = av_packet_alloc();
    AVFrame* temp_frame = av_frame_alloc();
    SwsContext* sws_ctx = nullptr;
    int ret = 0;
   
    bool preview_done = false;

    int videoStream = 0;
    for (int i = 0; i<int(fmt_ctx_->nb_streams) && !preview_done; i++) {
        // only process video information
        if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {

                videoStream = i;
            const AVCodec* codec = avcodec_find_decoder(fmt_ctx_->streams[i]->codecpar->codec_id);
            AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
 
                avcodec_parameters_to_context(codec_ctx, fmt_ctx_->streams[i]->codecpar);
            // Open the decoder
            avcodec_open2(codec_ctx, codec, nullptr);
      
            // Read frame data
            while (av_read_frame(fmt_ctx_, pkt) >= 0) {
                if (pkt->stream_index == videoStream) {

                    av_frame_unref(temp_frame);
    
                        while ((ret = avcodec_receive_frame(codec_ctx, temp_frame)) == AVERROR(EAGAIN)) {
                            ret = avcodec_send_packet(codec_ctx, pkt);
                     
                            if (ret < 0) {
                   
                                break;
                            }
                        }

                    if (ret < 0 && ret != AVERROR_EOF) {
             
                        continue;
                    }

                    // Isometric zoom
                    int dstH = 240;
                    int dstW = qRound(dstH * (float(temp_frame->width) / float(temp_frame->height)));
                    // Eliminate possible alarms
                    dstH = (dstH >> 4) << 4;
                    dstW = (dstW >> 4) << 4;

                    qDebug() << "777" << dstW << dstH;

                    sws_ctx = sws_getContext(
                        temp_frame->width,
                        temp_frame->height,
                        static_cast<AVPixelFormat>(temp_frame->format),
                        dstW,
                        dstH,
                        static_cast<AVPixelFormat>(AV_PIX_FMT_RGBA),
                        SWS_FAST_BILINEAR,
                        nullptr,
                        nullptr,
                        nullptr
                    );
                    int linesize[AV_NUM_DATA_POINTERS];
                    linesize[0] = dstW * 4;
                    qDebug() << "888";
                    // Generate a picture
                    QImage preview = QImage(dstW, dstH, QImage::Format_RGBA8888);
                    uint8_t* data = preview.bits();
                    sws_scale(sws_ctx,
                        temp_frame->data,
                        temp_frame->linesize,
                        0,
                        temp_frame->height,
                        &data,
                        linesize);
                    sws_freeContext(sws_ctx);

                    qDebug() << "9999";
                    avcodec_close(codec_ctx);
                    avcodec_free_context(&codec_ctx);
                    preview_done = true;
                    preview.save("D:/preview.jpg");
                    break;
                }
            }
        }
    }

    qDebug() << "10";
    av_frame_free(&temp_frame);
    av_packet_free(&pkt);
    avformat_close_input(&fmt_ctx_);
   
}
