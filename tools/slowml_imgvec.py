#!/usr/bin/python3

#import tkinter
from PIL import Image
#from PIL import ImageTk, ImageDraw
import sys
import struct
import random

if len(sys.argv)<3:
  print('Syntax: '+sys.argv[0]+' <image file>  <output vector> [--vectorwidth|-vw width] [--vectorheight|-vh height] [--greyscale|-gs | --rgb|-rgb | --red|-r | --blue|-b | --green|-g] [--random-crop|-rc]')
  quit()

img=Image.open(sys.argv[1])

arg=3

vectorwidth=img.width
vectorheight=img.height
vectormode='gs'
pixelvals=1

while arg<len(sys.argv):
  if sys.argv[arg]=='--vectorwidth' or sys.argv[arg]=='-vw':
    arg=arg+1
    vectorwidth=int(sys.argv[arg])
  elif sys.argv[arg]=='--vectorheight' or sys.argv[arg]=='-vh':
    arg=arg+1
    vectorheight=int(sys.argv[arg])
  elif sys.argv[arg]=='--greyscale' or sys.argv[arg]=='-gs':
    vectormode='gs'
    pixelvals=1
  elif sys.argv[arg]=='--rgb' or sys.argv[arg]=='-rgb':
    vectormode='rgb'
    pixelvals=3
  elif sys.argv[arg]=='--red' or sys.argv[arg]=='-r':
    vectormode='r'
    pixelvals=1
  elif sys.argv[arg]=='--green' or sys.argv[arg]=='-g':
    vectormode='g'
    pixelvals=1
  elif sys.argv[arg]=='--blue' or sys.argv[arg]=='-b':
    vectormode='b'
    pixelvals=1
  elif sys.argv[arg]=='--random-crop' or sys.argv[arg]=='-rc':
    crop_left=random.randint(0,img.width-vectorwidth)
    crop_top=random.randrange(0,img.height-vectorheight)
    img=img.crop((crop_left,crop_top,crop_left+vectorwidth,crop_top+vectorheight))
  else:
    print('Unknown option: '+sys.argv[arg])
    quit()

  arg=arg+1

img=img.resize((vectorwidth,vectorheight))

# Opret vindue
#window = tkinter.Tk(className="Christians Spil")
#tk_visning = ImageTk.PhotoImage(img)
#windowContent=tkinter.Label(window,image=tk_visning)
#windowContent.pack()
#tkinter.mainloop()

vecfile = open(sys.argv[2],'wb')
vecfile.write(struct.pack('Q',vectorwidth*vectorheight*pixelvals+1))
vecfile.write(struct.pack('d',1.0))
for y in range(vectorheight):
  for x in range(vectorwidth):
    p=img.getpixel((x,y))
    if vectormode=='gs':
      vecfile.write(struct.pack('d',float(p[0])/(3.0*255.0)+float(p[1])/(3.0*255.0)+float(p[2])/(3.0*255.0)))
    elif vectormode=='rgb':
      vecfile.write(struct.pack('d',float(p[0])/255.0))
      vecfile.write(struct.pack('d',float(p[1])/255.0))
      vecfile.write(struct.pack('d',float(p[2])/255.0))
    elif vectormode=='r':
      vecfile.write(struct.pack('d',float(p[0])/255.0))
    elif vectormode=='g':
      vecfile.write(struct.pack('d',float(p[1])/255.0))
    elif vectormode=='b':
      vecfile.write(struct.pack('d',float(p[2])/255.0))
    else:
      print('Unknown vectormode: '+vectormode)
      quit()

vecfile.close()
print('Done')

