import subprocess
import time
import os

# adb + png2jpg 命令
WIDTH = 1080
HEIGHT = 2376
QUALITY = 60
DOWNSAMPLE = 1

CMD = f'adb exec-out "screencap | /data/local/tmp/png2jpg r {WIDTH} {HEIGHT} {QUALITY} {DOWNSAMPLE}"'

# 保存临时文件夹
TMP_DIR = "tmp_frames"
os.makedirs(TMP_DIR, exist_ok=True)

NUM_FRAMES = 20  # 测试多少帧

start_time = time.time()

for i in range(NUM_FRAMES):
    tmp_file = os.path.join(TMP_DIR, f"{i}.jpg")
    with open(tmp_file, "wb") as f:
        # 执行 adb 命令并写入文件
        proc = subprocess.Popen(CMD, shell=True, stdout=subprocess.PIPE)
        while True:
            chunk = proc.stdout.read(8192)
            if not chunk:
                break
            f.write(chunk)
        proc.wait()

end_time = time.time()
elapsed = end_time - start_time

fps = NUM_FRAMES / elapsed
print(f"Captured {NUM_FRAMES} frames in {elapsed:.2f} seconds, approx FPS = {fps:.2f}")
