# openGLPlotLive
A simple openGL 2D plotter for quickly updated data.

# What can it do
Plot simple data in realtime.
Current data formats:
* Vector of pt2D - custom (x,y) point structure
* Vector of consecutive x,y values. e.g. {x1,y1,x2,y2,...,xn,yn}
* Vector of 2-vector x,y pairs. e.g. {vector<float> {x1,y1}, vector<float> {x2,y2}, ... vector<float {xn,yn}}

<img src="https://github.com/tbattz/openGLPlotLive/raw/master/Other/plotGif.gif" width="600" height="600">

# Simple Plots
## Window Setup
To create a window for plotting, you will need the following
```c++
// Window Size
int windowWidth  = 800;
int windowHeight = 800;

// Init GLFW
std::shared_ptr<GLPL::IWindow> window = std::shared_ptr<GLPL::IWindow>(new GLPL::Window(windowWidth, windowHeight));
std::shared_ptr<GLPL::Window> window2 = std::dynamic_pointer_cast<GLPL::Window>(window);
```

## Create Data
You will need some data to plot, currently it needs to adhere to one of three formats. Plans for the future is to enable more formats to be added.
For example, creating data in the vector<pt2D> format, with all the data to be plotted from the first frame.
```c++
// Graph 2
vector<pt2D> data2;
for(int i=-1000; i<1000; i++) {
	pt2D pt2;
	float x = i/1000.0;
	pt2.x = x;
	pt2.y = -x-1;
	data2.push_back(pt2);
}
```
You can also initialise data, and create it as the plot is being draw. For example for data of the format vector<vector<float>>.
```c++
// Graph 5 - Vector of Vectors
vector<vector<float>> data5;
```

## Create Plot
You will need to create a plot to add the lines to. Creating a plot that starts at x=0.0, y=0.25 (proportion of the window), and has width and height of 0.75 the respective lengths,
```c++
// Create Plot
GLPL::Plot myplot(0.0, 0.25, 0.75, 0.75, window);
// Create Lines
Line2Dpts line2(&data2);
Line2DVecVec line5(&data5);
// Add lines to axes
myplot.axes.addLine(&line2);
myplot.axes.addLine(&line5);
```

## The Drawing Loop
To draw in real time you will require a drawing loop.
The following draws the plot, axes and lines, updates the plot data for plot 5 and prepares the plot for the next draw call.
```c++
while(!glfwWindowShouldClose(window->getWindow())) {
	// Pre-loop draw
	window2->preLoopDraw(true);

	// Update Graph 5
	i -= 10;
	for(int j=0; j < 10; j++) {
		vector<float> tempVec = {i/1000.0, 0.5*i/1000.0};
		data5.push_back(tempVec);
		i += 1;
	}
	line5.updateInternalData();

	// Draw Plot
	myplot.Draw();

	// Post-loop draw
	window2->postLoopDraw();
}
```
## After the draw loop, handle closing the window
```c++
// Close Window
glfwTerminate();
```

## Shader Directory
When including in another project, you'll need to copy the Shader directory to the root of your project. Provided the openGLPlotLive directory is in the top level of your project, this can be done in cmake with the following,
```cmake
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../openGLPlotLive/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders)
```

# Dependencies
##  Automated Install
A few scripts have been created to automate the installation process. In the event these fail, you may have to compile the dependencies manually. Support for Windows systems have been removed as of 1/2/2017. If may still be possible to complie on Windows, check the Windows-old-store branch for a bit of help for installation of some of the required packages.

### Linux (Ubuntu)
On Ubuntu these dependencies can be installed by running
```
sudo chmod +x installDependencies.sh
./installDependenciesUbuntu.txt
```
## Manual Install
### Linux (Ubuntu)
* Building Dependencies
	* cmake
	```
	sudo apt-get install cmake
	```
* GLFW (multi-platform library for OpenGL)
	* Download from http://www.glfw.org/download.html and extract.
	* Install dependencies
	```
	sudo apt-get install xorg-dev libglu1-mesa-dev
	```
	* Navigate to the glfw root directory and use cmake
	```
	cmake -G "Unix Makefiles"
	make -j4
	sudo make install
	```

* GLM (The OpenGL Mathematics Library)
	* Download a release from Github and extract, https://github.com/g-truc/glm/tags
	* Copy the glm folder inside the glm release into the include directory
	```
	cd <glm_root_dir>
	sudo cp -r glm /usr/local/include/
	```

* FreeType - Font Rendering Library
	* Download and extract from https://www.freetype.org/index.html
	* Naigate to the root directory and run
	* Copy include files to /usr/local/include
	```
	sudo cp -r include/* /usr/local/include 
	```
	Please DO NOT run ./configure on a system running the Unity window manager, as this will most likely break Unity.


# Compiling
A CMakeLists.txt file is included for compiling with Cmake. This should work for systems running Ubuntu. Navigate to the build directory and remove any old CMakeFiles
```
cd build
rm -r *
```
Run cmake to generate a makefile.
```
cmake ../                           # Linux
cmake -G "MinGW Makefiles" ../      # Windows
```
Compile the program.
```
make -j4
```

## Debugging
To enable debugging, when running cmake, enable the debugging flag.
```
cmake -DCMAKE_BUILD_TYPE=Debug
```

# Runnning an Example
An example binary is created, examplePlot. To run this
```
cd build/apps
./examplePlot
```

# Compiling an Eclipse Project
To generate an Eclipse project, from the root directory,
```
cd build
rm -r CMakeFiles/
cmake -G "Eclipse CDT4 - Unix Makefiles" ../src
```
Then import the project into Eclipse using File >> Import >> General >> Existing Projects into Workspace. Click next and set the root directory to <workspace>/openGLMap/build. Click Finish. The project can now be built with Eclipse using the 'all' Build Target. The source files will be inside the "[Source Directory]" and are linked to their actual counterpats.

