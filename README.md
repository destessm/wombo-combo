# wombo-combo
My CS539 Project

Final Turn In:

New Camera Controls (yay!):
Mouse - turn / tilt
wasd - forward, left, backward, right
b - pause frustum (also renders frustum as red lines)
p - pause mouse (basically to get out of the window without closing)
h - return to center of the map
x - print current position (mostly for debugging)

Everything actually works! I fixed my camera so that it runs smoothly with the mouse. 
Getting frustum culling was really quite difficult because I had done it completely wrong
previously. I was not checking if the current node had children first, so I think it would
actually check "internal" nodes for culling, which was bad. I also had to optimize my 
octree so that it wouldn't take a full three minutes to load (I timed it, it was that bad). 
I also had a long-standing bug that my culling would only work behind the camera. This 
actually got fixed with an optimization that I found for my previous culling algorithm. 
This fix, though, created a bug where it would cull things that were on the edge of the 
screen. After widening my frustum a little bit (which barely helped), I figured out how to
give it some head room by changing what I was comparing the signed distance to my frustum
planes against from 0 to -15000. This seems like a lot, but it's really just far enough
to stop this culling issue. I still have a small remnant of this bug (some nodes will 
still get culled even if they're just barely on screen) but it did fix most of it. The
biggest break I had was when I realized that I was still using a 512x512 height map. This
was aparently too much work for my computer to cull, limiting my FPS to something around
ten, which was abysmal. Once I changed back to a 256x256 height map, I jumped to about 45
FPS (this was right before I found the culling optimization). This was a great learning 
experience, and now I can truly appreciate how beautifully smooth most games run, even 
though they're using thousands of times more triangles per scene than I am. 




//Old stuff below


Particle Effects Part 2!

pretty much everything works. My only problems are the same that I sent you an email about. 
The stb_image.h library doesn't work for me still, so I had to use readPPM, which didn't
have an alpha channel. The color changing also doesn't work properly and is a little bit
terrifying. With the image as the particle, it actually runs really slowly. I was able to
max out my particles at around 20,000 before using the image for this project, but now it
caps at about 10,000. Hopefully these are small errors that we'll be able to fix in class.

The camera is still the same terrible camera as below.

// old stuff below

Particle Effects!

Started working on a new camera, might get that done post-submission (if so, I'll resend the repo after)
I actually added a small functionality to the bad camera I am using: a, s and d now move the position
 of the "At" away from the center of my terrain. Works the same as moving the eye. Terrible, I know,
but I just wanted to get it to look at something besides the center. 

This time everything seems to actually be working! My only small issue is with the 
randomness of the directions for my particles; sometimes they form lines because
they keep the same x-z direction between multiple particles. We can look at it in
class maybe or I can take a quick look at it later. Otherwise this works!




//old stuff below
Collision Dection + Octree

Camera is still in the works. Directions are still below. Sphere fires with 'i'. I don't remember why.

My collision detection worked before I implemented my octree (albeit it 
actually took ~30 seconds between frames), so I have to figure out why 
implementing the octree broke it. Had to do a lot of work on my octree to 
get it more functional. Rendering from my octree doesn't work yet, but it's 
implemented. Need to figure out what's wrong with it. Didn't reaize my octree 
was mostly wrong until saturday and it needed hours of work that took up a lot 
of my time. I currently have my render-from-octree commented out because it made 
the terrain disappear entirely. 



// old stuff below
Mid-point for Collision Detection

Camera is still in the works.
My spheres seem to move correctly, although I haven't fully implemented 
collision detection. Still not rendering though. I'll have to keep working on
what ever it is that is making things not render.




//old stuff below
Hi, here are some notes about my code!

CAMERA CONTROLS:
z and Z decrease and increase the x value respectively
x and X decrease and increase the y value respectively
c and C decrease and increase the z value respectively
(it always looks at the center of the terrain)
you can sort-of zoom in and out by alternately hitting z and c (or Z and C). 
I am so sorry for not fixing this yet. I keep meaning to set aside the time for it.
I'll get to it this week!

WHAT’S WRONG:
I am still having issues rendering my flock of boids. The boids themselves act
as they should (or so it seems through print-statements), but for some reason
I haven't gotten them to render. My boundaries are fairly 'loose', in that the
flock will get partially outside of the boundaries before rubber-banding back
inside of them. Another thing I didn't get to finish was rotating the boids,
as I was waiting until I could properly see the boids to fix this. I didn't 
account for how long this would take me, and I only started part-way through 
this week. Hopefully we can fix this rendering issue in class but if not, I'll
fix it before I get working on next week's assignment. I will also do my best
to get a functional camera working. 

The good news:
I finally got my textures looking good! Woo! 
