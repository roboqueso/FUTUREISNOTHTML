Leap SDK Examples -- Gestures Demo
================================================================================

GesturesDemo illustrates how to use the new Gestures API in 
the context of a Cinder powered particle simulation.

Key Leap source files:
* GesturesDemo.cpp   -- Defines the gesture handlers.
* ParticleField.cpp  -- Defines the application particle field used to visualize 
                        the gestures.

--------------------------------------------------------------------------------
Compiling the source code:

We created this sample using the Cinder cross-platform C++ library for it's
easy-to-use windowing & advanced graphics utility functions. Due to Cinder's
large size however we have not included it with this package. Precompiled
packages for Windows and Mac OS X can be downloaded from http://libcinder.org/

The GestureDemo project files are set up to search "${LIBRARIES_PATH}/cinder_0.8.4" 
by default.

Once you have downloaded cinder, place it wherever you like, remove any _vc2010
or _mac suffix to name the folder cinder_0.8.4, and set the LIBRARIES_PATH
variable to point at cinder's parent directory.

To set environment variables on Windows, go to Control Panel -> System ->
Advanced system settings -> Environment Variables...

To set project variables in Xcode, Build Settings -> Add Build Setting -> Add
User-Defined Setting

To open the example in Visual Studio or Xcode:

1. Navigate to the project's Builds folder.
2. Find the project file for your IDE:
   On Mac, we provide Xcode project files.
   On Windows, we provide Visual Studio 2010 project files.
 
To compile the example programs, the LeapSDK folder must be in the same 
directory as the main Examples folder. The project is set up to
look for the header files and libraries in this location. If you move the 
project or the SDK, you must update the include file and header references also.
     
To run the pre-build executable:

1. Launch the Leap application.
2. Launch the GestureVisualizer program found in the example folder.

--------------------------------------------------------------------------------
Graphics cards:

The Leap Motion C++ sample code makes use of the Cinder cross-platform
application framework. The 3D drawing is done using OpenGL in conjunction with
Cinder support for OpenGL drawing.

Integrated graphics card OpenGL drivers have been known to perform slowly or
improperly.

If you are building or running on a laptop with integrated graphics as well
as a discrete graphics card you may wish to change your graphics settings to
always use the discrete graphics card.

--------------------------------------------------------------------------------
Copyright © 2012-2013 Leap Motion, Inc. All rights reserved.

Leap Motion proprietary and confidential. Not for distribution. Use subject to
the terms of the Leap Motion SDK Agreement available at 
https://developer.leapmotion.com/sdk_agreement, or another agreement between 
Leap Motion and you, your company or other organization.
