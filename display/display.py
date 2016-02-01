import pyglet
from pyglet.window import key
from pyglet.window import mouse
from pyglet.gl import *
import colorsys, sys

model_height = 60
model_width = 60

# Window parameters
width  = 800
height = (width*model_height)/model_width
scale  = width/float(model_width) # This is the scaling factor needed so drawing points from 0->model width(100) fills the window
window = pyglet.window.Window(width, height)
glClearColor(1, 1, 1, 1) # White background

# Capture keyboard events
@window.event
def on_key_press(symbol, modifiers):
    if symbol == pyglet.window.key.P:
        print ":-)"

# Capture mouse events
@window.event
def on_mouse_press(x, y, button, modifiers):
    if button == mouse.LEFT:
        print ":-D"

population = []

import sqlite3 as lite

maxMetabolism = 80
with lite.connect('population.db') as con:
    cur = con.cursor()
    cur.execute("SELECT MAX(Metabolism) FROM Organisms;")
    maxMetabolism = cur.fetchone()[0]

# Drawing
@window.event
def on_draw():
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()

    glPointSize(scale) # So the points are the right size after scaling

    glPushMatrix()
    glScalef(scale,scale,scale) # Scale coordinates, so we can draw points according to their model coordinates and fill the window

    glColor3f(0, 0, 0) # Draw black
    glBegin(GL_POINTS) # Tells opengl to start expecting points
    # Model-specific drawing: loop over grid, drawing cells if they're in state 1
    for p in population:
        h = p[8]/255.0 #species
        s = 1.0
        v = min(1.0, 0.05+p[4]/float(maxMetabolism))
        c = colorsys.hsv_to_rgb(h,s,v)
        glColor3f(c[0],c[1],c[2])
        glVertex2f(p[2],p[3])
    glEnd()

    glPopMatrix()
    
nextInt = 1
time = float(nextInt-1)
maxInt = 0

with lite.connect('population.db') as con:
    cur = con.cursor()
    cur.execute("SELECT MAX(EndGeneration) FROM Organisms;")
    maxInt = cur.fetchone()[0]

# Periodically called
def update(dt):
    global time, nextInt, population

    time += dt
    if time>nextInt*0.00001:
        nextInt += 30
        if nextInt%500==0:
            print nextInt
        if nextInt>maxInt:
            sys.exit()

        #population = [(i,0,i%model_width,i/model_width,50,50,0,0,i+nextInt) for i in range(model_width*model_height)]
        with lite.connect('population.db') as con:
            cur = con.cursor()
            cur.execute("SELECT ID,ParentID,X,Y,Metabolism,Fitness,StartGeneration,EndGeneration,Species FROM Organisms WHERE StartGeneration<" + str(nextInt) + " AND EndGeneration>" + str(nextInt) + ";")
            population = cur.fetchall()
            #print nextInt, len(population)
            #if len(population) > 1:
            #    for p in population:
            #        print p
            #    print ""

pyglet.clock.schedule(update)
pyglet.app.run()


