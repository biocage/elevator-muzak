What is this?

A co-worker wanted to give a going-away gag gift to a well-liked manager. We had used some video game flight joysticks for prototyping, and I had a few floating around. Late one night, I hacked together this small program, which I then put on an embedded ARM Linux PC and connected to a disassembled speaker and the joystick. The joystick would get powered by an AC adapter, the Linux PC would read button presses and play humorous sound-bites related to the project.

Requirements:

This uses SDL (1.2) and mplayer (called via execlp)


