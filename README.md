A project for Dr. Camille Hayhurst at WVU in 2021, for their Operating Systems Structure course.

For this project, we used a virtual machine running Ubuntu 20.04 and wrote the mpx operating system in C

A brief overview of features implemented:

Running programs (with optional priority)
Reading/setting the system date and time
Listing directory
Show running processes
Get help
Change the command prompt
Aliasing commands
To accomplish this, we implemented system requests including requesting input and sending output through COM ports to line printers and dumb terminals. Our process dispatcher uses and round-robin algorithm with configurable process priorities.

User manual: https://github.com/VVonderboi/MPXOS/blob/05003362c47322290fe205e8aa9a638cfaa90589/User%20Manual.pdf
