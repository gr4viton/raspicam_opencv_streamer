#!/bin/bash
#sudo mjpg_streamer -i "/usr/local/lib/input_file.so -f ../script/ -n out.mjpg" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080"
mjpg_streamer -i "input_file.so -f ../script/ -n out.mjpg" -o "output_http.so -w /usr/local/www -p 8080"


