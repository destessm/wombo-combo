# wombo-combo
My CS539 Project

Hi, here are some notes about my code!

CAMERA CONTROLS:
z and Z decrease and increase the x value respectively
x and X decrease and increase the y value respectively
c and C decrease and increase the z value respectively
(it always looks at the center of the terrain)
you can sort-of zoom in and out by alternately hitting z and c (or Z and C). 


WHAT’S WRONG:
I am unable to render the red cubes to represent the octree visually. I 
instead am printing the sizes for each cube and whether they are leaves
or internal nodes in the order that they are processed (depth first). I
believe that my issue lies in my pointers (which I often have troubles
with), and so when I try to run a function to read through the tree, it
gets a bad-access error on a line that compares the node's first child
to NULL. I was also having issues using break-points. XCode seemed to 
be lying to me as to what was contained in my pointers (not sure why
exactly), so I eventually had to resort to std::couts that would let me
see more clearly what was going on. This didn't help in the problem 
functions, though, since they both hit errors so quickly. In addition to
this, I never got a chance to fix my camera. I will try to do this over
break next week so that the next assignment will be easier to interact
with. Also my textures still look awful. Not sure why, didn't get any
time to fix it. 
