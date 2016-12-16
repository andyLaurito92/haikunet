This module opens a BackDoor into Scilab workspace.

It works by creating a new thread that listens on requests on a TCP port.
Each string send to that thread is executed in the Scilab workspace and a result is sent back to the caller when requested.

For testing load the module (when the module is loaded you should see something like:
	
	BackDoor: listening to commands on TCP port 27020

and then try opening a telnet connection 
	
	telnet 120.0.0.1 27020

and start writing Scilab commands as you would on the command window.

The intention of this module is to use it with PowerDEVS (https://sourceforge.net/projects/powerdevs)  in order to process simulation results
in the Scilab enviroment.

