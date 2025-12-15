Using the Ray Tracing in One Weekend, i made a simple cpu ray tracer.
The code structure and style used in the RT In a weekend is not really how i like to do things. It was usefull for sphere intersection maths.
I used my vec template which makes all the maths far easier.
the python builder allows for multi threaded file compilation. Then single threaded linking
i dont like how make is such a black box and i like the ability to run whatever custom structure i want. Its neat that it only compiles changed cpp files and looks for changed in hpp files etc.
Added AABBs (Cube Intersection).
Added fast AABB intersection code for the Bounding Volume Heirachy to change an O(n) intersection check for each scene object to O(log n)
Made it multi threaded. Beware this can be configured to be what is essentially a power virus since it will scale to use all available system threads.
