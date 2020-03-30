QUT
CAB202
semester 2, 2018
Assignment1
By Matthew Stevenson

If you are a QUT student, do not use any of this code for your assingment.
You will be caught by the plagiarism system. This code is for evaluation by
potential employers.

Instructions:

1. Run the makefile inside the ZDK folder to build the neccessary libraries.
2. Run the makefile in the main folder to build the program for linux
3. Open the output file from the command line

Controls:

'w' = Jump
'a' = move left
'd' = move right
't' = toggle treasure movement

Rules:

The blocks bulit from '=' are safe blocks. Whenever the player lands on a block in a new
row, a point is earned

The blocks built from 'x' are hazards. Whenever the player touches on of these blocks,
the player loses a life and respawns

If the player leaves the gameplay area, the player loses a life and respawns

If the player makes contact with the treasure entity at the bottom of the gameplay area,
the player respawns and gains two lives. The treasure entity is then removed form the game