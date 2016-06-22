# Relighting
 An implementation for environment mapping by physical-based rendering.  
 Base on [Physically Based Rendering, Second Edition](http://pbrt.org/) and [https://github.com/McNopper/OpenGL](https://github.com/McNopper/OpenGL).

## Procedure
- Vertex shader
  - For each vertex
    - Calculate its view space position and normal vector.
    - Calculate its tangent space according to its normal.
- Fragment shader
  - For each vertex
    - Sample 64 times from sky box texture according to tangent space.
    - For each sample
      - Use eye-to-object direction vector, normal vector and object-to-texture vector to calculate BRDF value.
      - Add the texture color to output color.
    - Normalize the output color.

## Result
![result1](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/1.jpg?raw=true)
![result2](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/2.jpg?raw=true)
![result3](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/3.jpg?raw=true)
![result4](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/4.jpg?raw=true)
![result5](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/5.jpg?raw=true)
![result6](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/6.jpg?raw=true)
![result7](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/7.jpg?raw=true)
![result8](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/8.jpg?raw=true)
![result9](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/9.jpg?raw=true)
![result10](https://github.com/AmazingZhen/Relighting/blob/master/Relighting/res/10.jpg?raw=true)

## Demo program
- In the folder "demo"
- How to control?
  - press WASD to move the model, up and down to scale the model, left and right to switch the model.
  - press BN to swtich the scene.
  - press 1, 2, 3, 4 to switch reflect mode.
  - press ZX to change the roughness of the material, CV to change the reflect reflectivity(only valid in mode 4)

## Problems and improvements
- There may exist some redundant computation in transform.
- Some transform may be incorrect in some specific situations.
  - It seems that the normal vector will become wrong when scaling the model.
- The rendering process is somehow fake, without worrying about visibility and rendering shadows.
