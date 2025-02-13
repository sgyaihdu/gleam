sample usage:

Step 1: set LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=xxx/mpp/out/lib:$LD_LIBRARY_PATH
    xxx is sdk package path.

Step 2: compile sample code
    make;

Step 3: run sample
Usage : ./sample_svp_npu_main <index> [file/vi/yolo_version]
index:
         0) svp_acl_resnet50.
         1) svp_acl_resnet50_multi_thread.
         2) svp_acl_resnet50_dynamic_batch_with_mem_cached.
         3) svp_acl_lstm.
         4) svp_acl_rfcn.(FILE->VDEC->VPSS->SVP_NPU->VGS->VO_BT1120).
         5) svp_acl_event.
         6) svp_acl_aipp.
         7) svp_acl_preemption.
         8) [1, 5],yolov1 to yolov5; [7, 8],yolov7 to yolov8.(VI->VPSS->SVP_NPU->VGS->VO)
         9) [1, 5],yolov1 to yolov5; [7, 8],yolov7 to yolov8.(FILE->VDEC->VPSS->SVP_NPU->VGS->VO)
         a) sample_svp_npu_acl_e2e_hrnet.(VI->VPSS->SVP_NPU->VGS->VO)
         b) sample_svp_npu_acl_motr. <file/vi> 0, (VI->VPSS->SVP_NPU->VGS->VO); 1, (FILE->VDEC->VPSS->SVP_NPU->VGS->VO).

*************************************************************************************************************
Motr Sample:
    This sample is about Tracking on dancer based on YoloX and Motr
    * Motr process is connected by YoloX, Motr, Qim (3 om files) in series.
    * Information between frames are stored in some global variabled updated by postprocess of Motr and Qim.
    * Whole process follows 0, (VI->VPSS->SVP_NPU->VGS->VO); 1, (FILE->VDEC->VPSS->SVP_NPU->VGS->VO)

Input video:
    h264 file without B frames. Video resolution supports [114 X 114] to [3840 X 2160]

Tip:
1. Data related to Motr Download URL [https://github.com/DanceTrack/DanceTrack#dataset]
2. Use ffmpeg to concat image into video, and convert it to h264 format (without B frame).
    command follow this:
    [cd dancetrack/train/dancetrackxxxx/img1]
    [ffmpeg -r 25 -i %08d.jpg -c:v libx264 -preset slow -profile:v high -level 4.2 -pix_fmt yuv420p -g 25 -bf 0 tmp.mp4]
    [ffmpeg -i tmp.mp4 -c:v libx264 -preset slow -profile:v high -level 4.0 -x264-params ref=1:no-deblock=1:bframes=0 -c:a copy dance_video.h264]
   Notice: Copy [dance_video.h264] to [./data/image].
3. Open-sourced tool ffmpeg URL [https://github.com/BtbN/FFmpeg-Builds/releases/tag/latest]. Version [ffmpeg-n6.0-latest-linux64-gpl-6.0]
*************************************************************************************************************
