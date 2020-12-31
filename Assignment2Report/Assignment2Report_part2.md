# 1. Light sources -   
* ### Light point:
<img src=./point_of_light.png width=60%> 
* ### Parallel Light:
<img src=./parallel_light.png width=60%>  

# 2.  Specify a material for each model -  
Each material has different parameters (ambient, diffuse and specular).  
By choosing different parameters, we can mimic each real world material characteristics.
* ### Metal:
<img src=./metal.png width=60%> 
* ### Rubbur:
<img src=./rubber.png width=60%>  
* ### Plastic:
<img src=./plastic.png width=60%>  

# 3. Shading -   
* ### Flat:
<img src=./flat.png width=60%> 
* ### Gouraud:
<img src=./gouraud.png width=60%>   
* ### Phong:
<img src=./phong.png width=60%>  

# 4. Demonstrated on each section individually:  
In section 1 - Point vs. Parallel light source.  
In section 2 - Different materials.  
In section 3 - Different shading modules.  

# 5. Post-processing effects -  
* ### Bloom:  
We set the threshold to be the mean of the frame brighness.  
Every pixel that it's brightness value above the threshold, starting to "glow".  
As you can see, **before** applying Bloom effect  

<img src=./bloom_before.jpeg width=60%>  
 and **after** applying Bloom effect   
 
<img src=./bloom_after.jpeg width=60%>   

* ### Fog:  
We created a Linear-Fog using the fourmola from the lecture slides.  
You can see the far bunny is "fogier" then the closer one.  
<img src=./fog.jpeg width=60%>  


