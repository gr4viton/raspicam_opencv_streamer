#!/bin/bash
mjpg_streamer -i "input_file.so -f ../stream/ -n out.mjpg" -o "output_http.so -w /usr/local/www -p 8080"


