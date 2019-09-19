# README #

### Introduction ###
This program read contiguous voxel grid and output a ply file ( and 3mf file).
 


---simplified BSD license---   
Lib3MF (c)


### How do I get set up? ###

#### Summary of set up

`mkdir build`

`cd build`

`cmake -DUSE_3MF=ON ..` or `cmake ..`

`make`



Pass `--help` to see help on the executable file:

<code>  
---Reading Parameters---  

--type   	0: ply, 1: 3mf, >=2: both (default: 0)  
  
--pI     	(REQUIRED) The path of input data. (default: )
  
--pO     	The path of output file. (default: output)
  
--sN     	The number of input data. (default: 1)
  
--sW     	The width of input data. (default: 80)
  
--sH     	The height of input data. (default: 48)
  
--sD     	The depth of input data. (default: 80)
  
--s      	The scale of output file. Default is a unit volume. (default: 1)
  
--sb     	The space between each cube. (default: 0.8)
  
--t      	The number of thread to run. (0: auto detect, >0: the number you want.) (default: 0)
  
--bff    	Flip the surface direction (default: 0)
  
--bbb    	In enable, add 8 vetices on the each cornor of the volume. (default: 0)
  
--verbal 	verbal (default: 0)
  
</code>

#### Dependencies

C++11  

(Optional) lib3mf
