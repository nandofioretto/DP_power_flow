# CpuBE
This code implements an inference-based algorithm used to compare against one which exploits Graphical Processing Units (GPUs) to speed up the resolution of exact and approximated inference-based algorithms for discrete optimization (e.g., WCSPs)
For details, please refer to the original paper:

Ferdinando Fioretto, Tiep Le, Enrico Pontelli, William Yeoh, Tran Cao Son
[Exploiting GPUs in Solving (Distributed) Constraint Optimization Problems with Dynamic Programming](http://link.springer.com/chapter/10.1007%2F978-3-319-23219-5_9), In proceeding of CP 2015.


Compiling:
------------
CpuBE has been tested on MAC-OS-X and Linux operating systems. To compile:

      mkdir build
      cd build
      cmake ..
      make

The executable will be placed in the bin/ folder

Executing:
------------
To execute CpuBE you need to specify a file format (currently [xcsp](http://arxiv.org/pdf/0902.2362v1.pdf) and [wcps](http://graphmod.ics.uci.edu/group/WCSP_file_format) formats are supported) and a solver [Bucket Elimination](http://www.sciencedirect.com/science/article/pii/S0004370299000594) or [MiniBucket Elimination](http://dl.acm.org/citation.cfm?id=1622343):

	cpuBE
	--format=[xml|wcsp] inputFile
	--agt=[cpuBE|cpuMiniBE z]
		where z is the maximal size of the mini-bucket
	Optional Parameters:
	[--root=X]      : The agent with id=X is set to be the root of the pseudoTree
	[--heur={0,1,2,3,4,5}] : The PseudoTree construction will use heuristic=X
		where 0 = ascending order based on the variables' ID
			  1 = descending order based on the variables' ID
			  2 = ascending order based on the number of neighbors of a variables
			  3 = descending order based on the number of neighbors of a variables
			  4 = ascending order based on the variables' name (lexicographic order)
			  5 = descending order based on the variables' name (lexicographic order)
	[--max[MB|GB=X]: X is the maximum amount of memory used by the GPU

Examplex:

    cpuBE --format=wcsp ../test/rep_0_x_10.wcsp --agt=cpuBE
    
	cpuBE --format=xml ../test/rep_0_x_10.xml --agt=cpuBE
	