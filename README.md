# Spectral
Emulator and compiler for 6502

# Welcome
Welcome to Spectral! Spectral is a project I've been working on which includes a 6502 emulator written in modern C++ (SpectralEmu), and a Lua -> 6502 assembly compiler (Unnamed).

# How do I get started?
Compile SpectralEmu with Visual Studio 2022, if you don't want to use VS22 you can retarget the project, I will not be explaining that here.
Don't forget to compile SpectralEmu in **Release** unless you are debugging it, as it will run a LOT faster in Release mode.
Run SpectralEmu and let it do it's magic with my first test script written in it. It should tell you it has made a dump after emulating the instructions pre-coded for you.
Open this dump file in a hex editor to view it's secrets, this is how the program is stored, and runs in memory.

# Where can I learn about the 6502?
There are TONS of resources about the 6502 instruction set in the src/note.txt file. It was enough for me to make this emulator so it should be enough to teach you how to code the 6502.

# Where can I compile my 6502 code?
As of now I have not finished the lua -> 6502 compilation project so for now you may resort to using an online website such as https://www.masswerk.at/6502/assembler.html for compiling your code.
