# 1. Teapot 45 degree rotation around Z axis-   

<img src=./1_c_rot45.png width=60%>  


## NOTE: We changed the background colors
# 2.  Compare different frames  
### T1 = Rotate_Z(45) 
### T2 = Translate_Y

T1_model x T2_world  
<img src=./2a.jpeg width=60%>  
  
T1_world x T2_model  
<img src=./2b.jpeg width=60%> 

# 3. Face normals, vertex normals and bounding box 
  
<img src=./normals.png width=60%> 


# 4. Compare the orthographic projection vs. the perspective projection  
<img src=./4a.jpeg width=60%>  
<img src=./4b.jpeg width=60%> 

# 5. Perspective projection - Difference between changing the zoom and moving closer or farther from the model  
<img src=./5a.jpeg width=60%> 
<img src=./5b.jpeg width=60%> 

# 6. Compare camera rotations
- Rotation of the camera in the camera frame around the y (up) axis:   
<img src=./6a.jpeg width=60%> 
- Rotation of the camera in the world frame around the y (up) axis:   
<img src=./6b.jpeg width=60%> 
  
# 7. Camera position is at (300, 300, 300)  
<img src=./7.jpeg width=60%>   

# 8. Multiple models and cameras on screen  
<img src=./8a.jpeg width=60%>   
<img src=./8b.jpeg width=60%> 
  
# 9. Resizing the window  
<img src=./9a.jpeg width=60%>  
<img src=./9b.jpeg width=60%>  
  
# 10. Showing as much of your GUI as possible
<img src=./10.jpeg width=60%>  

As we can see in the screenshot, there is a window for each model\camera transformation (local/world).  
Also, we have *Camera control panel* and *Model control panel*.  
Each panel is use for changing active model/camera and to set other model/camera unique features.  

# 11. More GUI and fetures  
We added camera control through keyboard clicks.  
**W** - move camera closer to model (change Z value in CameraEye)  
**S** - move camera further from the model (change Z value in CameraEye)  
**A** - move camera left (change X value in CameraEye)  
**D** - move camera right (change X value in CameraEye)    
**Q** - move camera up (change Y value in CameraEye)  
**E** - move camera down (change Y value in CameraEye)    

**Mouse wheel** - Zoom in/out 
  
**Mouse right click and move pointer** - changing CameraAt using mouse.