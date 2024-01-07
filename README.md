# The Last One Standing

In this Game you control a tank that is deployed in the middle of invadors. You have to fight off a series of enemy tanks and kill their boss by shooting your weapon.


How to run :
Using vcpkg: install vcpkg ( https://vcpkg.io/en/getting-started.html ).
If you are on windows, make sure to also add the path to your vcpkg installation folder to the "Path" environment variable. 
Then on the CMakePresets file, change the path to your own path where you installed vcpkg as such : "your_path/vcpkg/scripts/buildsystems/vcpkg.cmake". 
Then on the terminal, run cmake --preset=default, to install all the dependencies.
To build the project, run cmake --build build --config Release
Run the "VR_project.exe" executable that was built in the build/Release directory 

Dependencies: 
The libraries used in this project are:
glfw, bullet, assimp, glm, stb

Play:
You move forward with the Z key and backwards with S. Use your mouse to look around and move sideways. 
You can press Space bar to jump and left click on your mouse to shoot a bullet.

Files:
The source code, shaders objects and textures are in the project folder. 
The textures for every object are in the object folder aswell usually alongside them except for the cubemap which is in the textures folder.
