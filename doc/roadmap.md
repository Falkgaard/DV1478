# Roadmap

1. Set-up github repository
2. Study Vulkan
3. Setup barebones project

4. Partition & populate world with static geometry (terrain, buildings, trees?)
5. Get it to bake the starting cell
6. Get it to recombine that baked data as a skymap (kind of; not a cubemap, at least).

7. Get it to update the skymap with the correct data based off of the viewport position's current cell
8. Get it to interpolate between the data of the closest 4 cells based off of proximity.
9. Find a way to deal with changes along the height axis (potential avenue: some fisheye-like projection transform?)

Other stuff:

A. Make the skybox toggleable
B. Experiment with the distance radius that allows for the best results.
C. Experiment with integrating the baked data with a deferred shader.
D. Experiment with combining the static fragment data with distant dynamic geometry. (E.g. have a dragon circle a distant baked static tower at runtime)
