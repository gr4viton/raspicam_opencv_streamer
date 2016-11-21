#!/bin/bash
sudo mjpg_streamer -i "/usr/local/lib/input_file.so -f /home/pi/DEV/bprp/raspicam_cv_stream/build/ -n out.mjpg" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080"


