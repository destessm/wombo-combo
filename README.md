# wombo-combo
My CS539 Project


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
