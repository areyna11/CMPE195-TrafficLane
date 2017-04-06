#1. The image will be display for choosing pixel
#2. After clicking on pixel, the user won't be allowed to choose another one. The histogram of image will be shown as well as the sliders of RGB for users to adjust.
#3. On the root window, a button 'Generate Image Seg' will be shown after choosing pixel. 
#4. Clicking on 'Generate Image Seg' button will show the final image output based on RGB range from sliders. 


import cv2
import time
import numpy as np
import matplotlib
matplotlib.use("TkAgg")
from matplotlib import pyplot as plt
from matplotlib.widgets import Slider
from Tkinter import *
from pylab import *
import Tkinter
import numpy as np
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
    canvas = Canvas(frame,  cursor = "cross",width=700, height=400, bd=0,
     xscrollcommand=xscroll.set, yscrollcommand=yscroll.set)
    canvas.grid(row=0, column=0, sticky=N+S+E+W)
    xscroll.config(command=canvas.xview)
    yscroll.config(command=canvas.yview)
    frame.pack(fill=BOTH,expand=1)

    #B1 = Tkinter.Button(root, text = "Generate Image Segmentation")

    #image loading
    #File = askopenfilename(parent=root, initialdir="",title='Choose an image.')
    #img = Image.open(File)
    img1 = cv2.imread('5.jpg')
    img = Image.open('5.jpg')
    #img2 = Image.open('5.jpg')
    #img2 = cv2.imread('5.jpg')
    canvas.image = ImageTk.PhotoImage(img)
    canvas.create_image(0,0,image=canvas.image,anchor="nw")

    #value of scale
    red = DoubleVar()
    green =DoubleVar()
    blue = DoubleVar()

    #window display RGB 
    window = Tkinter.Toplevel()
    window.geometry("500x300")
    #R slider
    redSlider = Scale( window,fg="red",troughcolor="red", from_=0, 
        to=255,length= 500,orient=HORIZONTAL, tickinterval=51, variable = red)
    redSlider.pack(anchor=CENTER)

    #G Slider
    greenSlider = Scale( window,fg="green",troughcolor="green", from_=0, 
        to=255,length= 500,orient=HORIZONTAL,tickinterval=51,variable = green )
    greenSlider.pack(anchor=CENTER)

    #B Slider
    blueSlider = Scale( window,fg="blue",troughcolor="blue", from_=0, 
        to=255,length= 500,orient=HORIZONTAL,tickinterval=51,variable = blue )
    blueSlider.pack(anchor=CENTER)

    #Tkinter.Label(window).pack()


    counter = 0
    #set default delta
    delta = 4
    t = []
    a = (0,0)
    #get width & height
    (w,h) = img.size
    #c  = 0
    #function to be called when mouse is clicked
    def printcoords(event):
        global counter,a,delta,canvas, B1

        #outputting x and y coords to console
        a =  (event.x,event.y)

        #create a cross symbol at chosen pixel
        canvas.create_line((a[0]-delta, a[1],a[0]+delta,a[1]),width=3, fill="blue")
        canvas.create_line((a[0],a[1]-delta,a[0], a[1]+delta), width=3,fill="blue")
        rgb_im = img.convert('RGB')
        r, g, b = rgb_im.getpixel((event.x, event.y))
        t.append(r)
        t.append(g)
        t.append(b)
        Tkinter.Label(window).pack()

        temp = "Default Delta is "+str(delta)+'\n'+"From chosen pixel, R range = (" +str(r-delta) +',' + str(r+delta) + ')' +'\n' +"From chosen pixel, G range = (" +str(g-delta) +',' + str(g+delta) + ')' +'\n' +"From chosen pixel, B range = (" +str(b-delta) +',' + str(b+delta) + ')'
        Tkinter.Label(window, text=temp).pack()
        redSlider.set(r)
        greenSlider.set(g)
        blueSlider.set(b)
        #t2 = [t[0] +delta,t[1]+delta,t[2]+delta]
        #t1 = [t[0] -delta,t[1]-delta,t[2]-delta]

        #iterate through pixels
        sam = Image.open('5.jpg')
        #callback function for button
        def seg():
            #set range
            img = sam.copy()
            pixels = img.load()
            t2 = [int(red.get()) +delta,int(green.get())+delta,int(blue.get())+delta]
            t1 = [int(red.get()) -delta,int(green.get())-delta,int(blue.get())-delta]

            #iterate through pixels
            for i in range(w):
                for j in range(h):
                    d,e,f = img.getpixel((i,j))
                    if d not in range(t1[0],t2[0]) and e not in range(t1[1],t2[1]) and f not in range(t1[2],t2[2]):
                        pixels[i,j] = (0,0,0) 
            #display output image and the range of RGB
            a = Tkinter.Toplevel(root)
            canvas = Canvas(a,width=600,height =400)
            canvas.pack()
            canvas.im = ImageTk.PhotoImage(img)
            canvas.create_image(0,0,image=canvas.im,anchor="nw")
            temp2 = "Current R range = (" +str(int(red.get())-delta) +',' + str(int(red.get())+delta) + ')' +'\n' +"Current G range = (" +str(int(green.get())-delta) +',' + str(int(green.get())+delta) + ')' +'\n' +"Current B range = (" +str(int(blue.get())-delta) +',' + str(int(blue.get())+delta) + ')'
            Tkinter.Label(a, text=temp2).pack()
            #img = sam.copy()
        #Click button to generate color
        B1 = Tkinter.Button(root, text = "Generate Image Segmentation",command= seg)
        B1.pack()


        #set cursor invisible after choosing pixel
        counter +=1
        if counter >= 1:
            canvas.config(cursor='none')
            canvas.unbind("<Button 1>")

        #histogram of image
        color = ('b','g','r')
        plt.figure(1)
        plt.title('Histogram for chosen picture')
        for i,col in enumerate(color):
            histr = cv2.calcHist([img1],[i],None,[256],[0,256])
            plt.plot(histr,color = col)
            plt.xlim([0,256])
        #plot 3 RGB lines to show distribution
        plt.axvline(x=r,linewidth=3, color='r',)
        plt.axvline(x=g,linewidth=3, color='g')
        plt.axvline(x=b,linewidth=3, color='b',label ='temp')
        #labels for each RGB Line
        plt.annotate('B', xy=(b-2, 2000), xytext=(b+10, 2000),
            arrowprops=dict(facecolor='blue', arrowstyle="->"),
            )
        plt.annotate('R', xy=(r-2, 3000), xytext=(r+10, 3000),
            arrowprops=dict(facecolor='red', arrowstyle="->"),
            )
        plt.annotate('G', xy=(g-2, 4000), xytext=(g+10, 4000),
            arrowprops=dict( arrowstyle="->",facecolor='green'),
            )
        plt.show()

        #print a
        #print counter
        #print "(x,y) = ",a
        #print "Chosen pixel has RGB as ",r,g,b
        #color segmentation
        
    canvas.bind("<Button 1>",printcoords)
    root.mainloop()
