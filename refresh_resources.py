"""Script forces to track changes for html pages (rebuild WebView that uses html data)"""
import os

file_to_refresh = ".pio/build/nodemcuv2/src/Resources.cpp.o"
if os.path.exists(file_to_refresh):
    os.remove(file_to_refresh)
    print("File removed before building:", file_to_refresh)