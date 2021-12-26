#!/usr/bin/python3
import sys;
import os;

def is_eof(f):
  s = f.read(1)
  if s != b'':    # restore position
    f.seek(-1, os.SEEK_CUR)
  return s == b''

while True:
  line=sys.stdin.readline()
  if line=="":
    break
  c=int(line)
  #print("Chars: " + str(c))
  data=sys.stdin.read(c)
  #print("Data: " + data)
  sys.stdout.write(data)
  nl=sys.stdin.readline()
  #print("Newline: " + nl)

