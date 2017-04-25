import os
import sys

stdin = os.open('/dev/fd/0', os.O_RDWR)

os.write(stdin, b'\n')
sys.stdin.flush()
prev_msg = sys.stdin.buffer.

print(len(prev_msg))
sys.stdin.readline()
