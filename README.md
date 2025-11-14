# png2jpg

### command
```
adb shell screencap -p /sdcard/screenshot.png
adb push png2jpg /data/local/tmp/
adb shell chmod +x /data/local/tmp/png2jpg

# mode i
adb shell "screencap -p |/data/local/tmp/png2jpg i /sdcard/screenshot.png /sdcard/test.jpg  85 2"
# mode p
adb shell "screencap -p |/data/local/tmp/png2jpg p /sdcard/test.jpg  85 2"
# mode r
# windows need git bash
adb exec-out "screencap  |/data/local/tmp/png2jpg r 85 2">1.jpg

```

Related to  https://github.com/openstf/minicap
