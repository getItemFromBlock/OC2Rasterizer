# Open Computers 2 Rasterizer

This project is a rasterizer that renders 3D textured models onto the projector screen of Open Computers 2.
Note that the project also compiles as a native windows app via the provided .sln file.
While it is not strictly required to make the program run, it is the part of the program that makes the required binary files that contains both the model and textures used by the program.

<img src="https://github.com/getItemFromBlock/OC2Rasterizer/blob/main/Assets/Readme/preview-earth.png" height="400" />

## Building
### Open Computers 2 (Sedna)
The project provides a makefile. You will need to get a cross compiler for RISC-V in order to compile the code using Linux based system to the target RISC-V architecture.
The one I use is here: https://musl.cc/. Look for the file named "riscv64-linux-musl-cross.tgz".  
You can use WSL if you are on windows.
Note that the makefile assume that you decompressed the compiler toolchain directly in your home folder, you can change the path by editing the first line of the makefile.

To build the project, just run ```make``` and if you want to rebuild or clean, run ```make clean```

### Windows
Use the visual studio solution file (.sln), but keep in mind that it will only produce the windows demo of the rasterizer.  

## Usage

After building the rasterizer file, upload it to the computer via the import.lua script or alternatively via [fimexu](https://github.com/Bs0Dd/Coverett) for improved importing speed.
The file will need execution permission to be run, you can give it by running ```chmod +x rasterizer```.

The program will then need a binary model file in order to display it.
The binary files are created by the windows version of the project, in the function RenderThread::Init().
You can find preassembled binary files in the ```Assets/Output``` folder.
Note that the binary files contain both the model data and the texture used by it.

After you have imported a model file, you can then run the command ```./rasterizer model.bin``` to display it.
By default the model will be displayed for 15 seconds, but you can add a custom time at the end of the command.

## Remarks

The framebuffer of the projector is 640 by 480, with a color space of 16 bit (r5g6b5).
The rasterizer renders an image at a resolution of 320 by 240 and then upsamples it to fill the whole screen.
Writing data to the output buffer is done by writing to the file ```/dev/fb0```

If you want to disable the cursor blinking on the projector, you can run ```echo -e '\033[?17;0;0c' > /dev/tty1```
to make in invisible, but note that this will not disable the session on it so you will still see characters being typed on the keyboard.

In the Makefile, you can see that then program is linked only to the C runtime libraries.
This is because Sedna does not have the C++ runtime library (the file is over 10 MB, whereas the hard drive is only 8 MB).
This also means that the code is closer to C with classes than actual C++.
If you compile C code directly, you don't need to worry about the libraries, as the C runtime is already on Sedna.

For the optimisation flags, Os gives a much smaller file size but runs a little slower.
You can switch to it instead of O3 if size if a constraint.

In terms of stability, the program runs perfectly fine when using a version of sedna compiled from source,
but if you are using the public version you will probably encounter random crashes and weird display bugs.
This is caused by a bug that causes floating points to behave strangely in some edge cases.
The only fix as of now is to compile Sedna for yourself

## Credits

Thanks to all people in the [OpenComputers2](https://github.com/fnuecke/oc2) and [Sedna](https://github.com/fnuecke/sedna) repositories,
as most of this knowledge is from them.
Most of the models and textures are made by me, but not all of them.

The earth texture is adapted from [this model](https://sketchfab.com/3d-models/ps1-style-low-poly-earth-4182142d89c143bcaad5910fc30b27e3)
which uses textures from [this website](https://www.solarsystemscope.com/textures/).

The cube/patterned texture is from [this website](https://freepbr.com/product/patterned-bw-vinyl/).

Various block textures are from Minecraft.

The infuser block model and texture is from the [Valkyriens Skies addon Clockwork](https://www.curseforge.com/minecraft/mc-mods/create-clockwork).

And finally, thanks to Sangar for making this amazing mod :heart:.
