# mhip_tracker : Software

The particle.io Boron is an Arduino controller and programmed similarly.  The quickest and easiest way to program your Boron is via the particle.io Web IDE.  Once you register your device and create an account on particle.io, you will be able to navigate to the online [console](https://console.particle.io) where you can access your device, write code, and flash your device.  Simply copy the content of the src/mhip_tracker.ino file into the Web IDE.  Buttons on the left margin of the IDE let you compile and flash your device.

Alternatively you can use [Visual Code](https://code.visualstudio.com/) editor with the [Particle Workbench](https://www.particle.io/workbench/) extension.  If you don't already have Visual Code installed, the Particle Workbench link will give you the opportunity to install it with the extension pre-installed.  Otherwise, if you do have Visual Code, you can install the extension via the Visual Code marketplace.

Note that this code was written and tested on deviceOS 0.9.0.

(The information below is generic particle.io development documentation.)

## Development

Particle projects are composed of 3 important elements:

### ```/src``` folder:

This is the source folder that contains the firmware files for your project. It should *not* be renamed. 
Anything that is in this folder when you compile your project will be sent to our compile service and compiled into a firmware binary for the Particle device that you have targeted.

If your application contains multiple files, they should all be included in the `src` folder. If your firmware depends on Particle libraries, those dependencies are specified in the `project.properties` file referenced below.

### ```.ino``` file:

This file is the firmware that will run as the primary application on your Particle device. It contains a `setup()` and `loop()` function, and can be written in Wiring or C/C++. For more information about using the Particle firmware API to create firmware for your Particle device, refer to the [Firmware Reference](https://docs.particle.io/reference/firmware/) section of the Particle documentation.

### ```project.properties``` file:

This is the file that specifies the name and version number of the libraries that your project depends on. Dependencies are added automatically to your `project.properties` file when you add a library to a project using the `particle library add` command in the CLI or add a library in the Desktop IDE.

## Adding additional files to your project

### Projects with multiple sources

If you would like add additional files to your application, they should be added to the `/src` folder. All files in the `/src` folder will be sent to the Particle Cloud to produce a compiled binary.

### Projects with external libraries

If your project includes a library that has not been registered in the Particle libraries system, you should create a new folder named `/lib/<libraryname>/src` under `/<project dir>` and add the `.h`, `.cpp` & `library.properties` files for your library there. Read the [Firmware Libraries guide](https://docs.particle.io/guide/tools-and-features/libraries/) for more details on how to develop libraries. Note that all contents of the `/lib` folder and subfolders will also be sent to the Cloud for compilation.

## Compiling your project

When you're ready to compile your project, make sure you have the correct Particle device target selected and run `particle compile <platform>` in the CLI or click the Compile button in the Desktop IDE. The following files in your project folder will be sent to the compile service:

- Everything in the `/src` folder, including your `.ino` application file
- The `project.properties` file for your project
- Any libraries stored under `lib/<libraryname>/src`

**Copyright 2019, MagicHome Inc., All rights reserved.**
