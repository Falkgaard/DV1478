# Diary, week 14

My activity in the course was a bit delayed since I had to await the results from an LP2 course in order to be accepted into the course over at LADOK.

This week I compiled my various ideas:

   1. World generation using tiling Voronoi diagrams, problem domain knowledge, value noise, and potentially the simulation of things such as tectonics, hydraulic erosion, precipation and what not.
   2. Training a name generator to generate interesting domains by utilizing context data as well as weighted primitive look-up.
   3. Researching how the diminishing effects of perspective can be used to offload computationally intensive tasks related to static elements via baking.

After contacting Prashant and pondering the issue further, the third alternative was chosen since it's the one I had the most extensive notes regarding, and one that I think has a fair bit of progress in practical applications as long as a few kinks can be worked out and certain caveats are taken into consideration.

After settling on this topic, I began to study Vulkan in preparation since I'll likely have to implement a basic engine as well as two front-ends to it (one offline batched baking renderer and one runtime client where the baked data can be toggled on and off in order to run tests comparing the two wit heach other).
