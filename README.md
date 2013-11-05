Aurora Audibilis
======

An audio visualizer designed for dome projection.

This app was created for "Nature of Code, Cosmos Edition," a two week course at ITP / NYU. We were given access to the planetarium at the [Lower East Side Girls Club](http://www.girlsclub.org/).

This sketch is inspired by the [Aurora](http://en.wikipedia.org/wiki/Aurora_Borealis). A ribbon of light wraps around the theater and dances to the music (using FFT data). As the song progresses, constellations from the northern hemisphere slowly reveal themselves. 

###Documentation

Here are some long exposure photos taken by [Max](http://maxmadesign.com/) at the planetarium:  

![image](https://raw.github.com/wdlindmeier/Cosmos/master/dome_shots.jpg)

  
Playing on a flat screen:  

![image](https://raw.github.com/wdlindmeier/Cosmos/master/screenshot.jpg)
  

This is a screenshot of the simple shader test (found in FisheyeShaderTest). It warps content to the contours of the dome using a glsl shader. I didn't end up using this code in my final work, but it may be useful to someone else.

![image](https://raw.github.com/wdlindmeier/Cosmos/master/shader_test.jpg)

Written in Cinder 0.8.6 (dev). 

###Contents

* **FisheyeShaderTest**: A basic app the warps content for the dome. 
* **PlanetariumAudio.app**: A build of the final audio visualizer
* **PlanetariumAudio**: The source for the audio visualizer