#BE SURE TO CHANGE THE FILE NAME BEFORE RUNNING THE PROGRAM
#After choosing the pixel, the window will close automatically; or you can choose to close it
# There are 2 outputs:
#1 with chosen pixel on original image and other will show image with only given pixels range
import cv2
import time
import numpy as np
import matplotlib
matplotlib.use("TkAgg")
from matplotlib import pyplot as plt
from Tkinter import *
import Tkinter
import numpy as np
from tkFileDialog import askopenfilename
from PIL import Image, ImageTk, ImageDraw

if __name__ == "__main__":
    root = Tk()
    #setting up a tkinter canvas with scrollbars
    frame = Frame(root,bd=2, relief=SUNKEN)
    frame.grid_rowconfigure(0, weight=1)
    frame.grid_columnconfigure(0, weight=1)
    xscroll = Scrollbar(frame, orient=HORIZONTAL)
    xscroll.grid(row=1, column=0, sticky=E+W)
    yscroll = Scrollbar(frame)
    yscroll.grid(row=0, column=1, sticky=N+S)
    canvas = Canvas(frame,  cursor = "cross",width=700, height=500, bd=0,
     xscrollcommand=xscroll.set, yscrollcommand=yscroll.set)
    canvas.grid(row=0, column=0, sticky=N+S+E+W)
    xscroll.config(command=canvas.xview)
    yscroll.config(command=canvas.yview)
    frame.pack(fill=BOTH,expand=1)

    #image loading
    #File = askopenfilename(parent=root, initialdir="",title='Choose an image.')
    #img = Image.open(File)
    img1 = cv2.imread('5.jpg')
    img = Image.open('5.jpg')
    img2 = Image.open('5.jpg')
    #img2 = cv2.imread('5.jpg')
    canvas.image = ImageTk.PhotoImage(img)
    canvas.create_image(0,0,image=canvas.image,anchor="nw")
    #canvas.config(scrollregion=canvas.bbox(ALL))
    #a=()
    counter = 0
    t = []
    a = (0,0)
    #c  = 0
    #function to be called when mouse is clicked
    def printcoords(event):
        global counter,a
        #outputting x and y coords to console
        a =  (event.x,event.y)
        rgb_im = img.convert('RGB')
        r, g, b = rgb_im.getpixel((event.x, event.y))
        t.append(r)
        t.append(g)
        t.append(b)
        counter +=1
        if counter >= 1:
            canvas.config(cursor='none')
            canvas.unbind("<Button 1>")

        #print counter
        print "(x,y) = ",a
        print "Chosen pixel has RGB as ",r,g,b

    (w,h) = img.size
    canvas.bind("<Button 1>",printcoords)
    #let the window close after 5s
    root.after(5000, lambda: root.destroy())

    #canvas.create_line(a[0], a[1], 200, 200)
    root.mainloop()

    pixels = img.load()
    delta = 7
    t2 = [t[0] +delta,t[1]+delta,t[2]+delta]
    t1 = [t[0] -delta,t[1]-delta,t[2]-delta]

    #iterate through pixels
    for i in range(w):
        for j in range(h):
            #rgb_im = img.convert('RGB')
            d,e,f = img.getpixel((i,j))
            #a = (t1[0],)
            if d not in range(t1[0],t2[0]) and e not in range(t1[1],t2[1]) and f not in range(t1[2],t2[2]):
                pixels[i,j] = (0,0,0)
    print "Choose from this RGB",t1,"to this RGB",t2

    #draw cross to see what chosen pixel
    draw = ImageDraw.Draw(img2)
    draw.line((a[0]-delta, a[1],a[0]+delta,a[1]),width=3, fill=225)
    draw.line((a[0],a[1]-delta,a[0], a[1]+delta), width=3,fill=225)
    del draw

    #color segmentation
    color = ('b','g','r')
    for i,col in enumerate(color):
        histr = cv2.calcHist([img1],[i],None,[256],[0,256])
        plt.plot(histr,color = col)
        plt.xlim([0,256])

    #plot rgb columns
    plt.title('Histogram for chosen picture')
    plt.axvline(x=t[0],linewidth=2, color='r')
    plt.axvline(x=t[1],linewidth=2, color='g')
    plt.axvline(x=t[2],linewidth=2, color='b')
    plt.show()

    #outputs
    img2.show()
    img.show()