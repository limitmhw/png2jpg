# png2jpg

### command
```
adb shell screencap -p /sdcard/screenshot.png
adb push png2jpg /data/local/tmp/
adb shell chmod +x /data/local/tmp/png2jpg
adb shell /data/local/tmp/png2jpg /sdcard/screenshot.png /sdcard/screenshot.jpg 50 2
```
