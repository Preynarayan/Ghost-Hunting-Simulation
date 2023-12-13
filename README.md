# Final Assignment
Submission by Riley Robertson S#101259829 and Prayanshu Narayan S#101144277
## Description
A simulation of a haunted house, with a ghost and 4 or more hunters.
The ghost and hunters will wander around the house, while the ghost is leaving evidence the
hunters are searching for evidence. If the ghost spends too much time in the same room as a hunter, the hunter is scared away!
If nothing happens for a while, the ghost and hunters will get bored.
All hunters and the ghost run on their own threads. When they are all finished, the hunters will attempt to identify the ghost
using the evidence they collected. If they were bored or scared off, they might not have enough!
## Files
- defs.h        header file containing constant definitions, typedefs and forward declarations
- evidence.c    evidence related functionality, including list operations
- ghost.c       the function for the ghost thread to run, and related functions
- house.c       some base code, involved in creating the house with all data
- hunters.c     the function for the hunter threads to run, and other actions which hunters take
- logger.c      mostly base code, officially logs events of the game to assist TA marking
- main.c        main control flow, initializes, runs, and concludes the simulation
- room.c        involved in management of room structs, and list operations with them
- utils.c       contains RNG (surprise mechanisms) and some string converter functions
- makefile      tool allowing simply compliation and linking into an executable
- README.md     contains instructions and information related to the project
- commits.png   image used in README.md
## Compilation
To compile and link the entire project, just run `make`
## Usage
Compile the program and run it with `./main`, or use `make temp` to use it instantly. The makefile will delete everything when you are done with it.

While running, enter four names, one for each hunter in the simulation. For rapid testing,
You may want to create a file with four names to pipe into the program. The simulation will run fairly quickly. 
When it is done, the results will be presented in the console.
The program will also log actions to the console, if you would like to inspect carefully what has happened.

Note: this program uses ANSI escape sequences to format the terminal in color. The colors and formats appear slightly differently on different systems, but have been tested on the course VM and also work in our vscode terminals. Using other systems may format the output in a way that is difficult
or impossible to read.
