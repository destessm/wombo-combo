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
My textures don’t work. Not sure why they don’t get sent to the shader 
correctly, but they don’t. I got the normals working fine (I think) and the 
lighting too (although I used a point light; would have probably been better 
with just a distance light). The issue was entirely in getting the textures 
to the shader. I even have all the math in place to make the “procedural 
texture-splatting” to work (theoretically), but I could not test it. 
