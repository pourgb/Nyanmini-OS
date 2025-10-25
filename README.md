# Nyanmini-OS
tried making an OS with Gemini(but cute)

to build and run this OS, execute ./run.sh in terminal(assuming you have all the necessary tools installed)
on windows you might wanna install WSL to run it
note: use /format_hdc in the OS's shell to format the 1GB virtual drive to make sure it's all good, then use /fatinit to initialize the system. /fatinit will have to be used at the start of every session because it's not in the main kernel code(well it technically is but disabled for now)

oct 25 update 1:
+ added some new features to basic interpreter, 
+ fixed some weird movements of text editor, now cursor should work fine

todo:
+ expand on BASIC clone features: POKE to change the pixel at x, y character/background color/foreground color. maybe implement BASIC function declaration?
+ look into compiling basic into executables or a an executable type, may need to implement a x86 compiler(might lose sanity doing this so it could never come tbh)
+ are there any bugs? i dont know, and i really dont want to know
+ add actual scrolling to shell, currently only clears the screen and scrolling does not work
