# Branch Prediction 

This repository has codes pertaining to project 2 of NCSU's ECE 563 course on micro architecture. The project simulates the working of a bimodal, g-share and a hybrid branch predictor. There are three different types of predictors that is implemented namely:

- Bimodal
- G-Share 
- Hybrid 

## Bimodal

The predictor uses a 4 level smith counter. We increament the counter when the branch is taken and decrement it when the branch is not taken. The counter saturates at 0 and at 3. If the value is 1 or 0 the branch is predicted to not being taken and if its 2 or 3 it is predicted taken. We use the program counter to index the branch prediction table. We ignore the last two bits and then use the next 'm' bits of the PC to index a location in the table. A complete description of the predictor is given in the project spec document. 

## G-Share 

This kind of a predictor table uses the PC value hashed with a branch history register to index the prediction table. The updation of the value in the table is similar to the one explained in the previous section. To update the branch history register we shift the content to the left by 1  and or the actual branch result to the Least significant bit. 

## Hybrid 

The predictor combines both the bimodal and the G-Share predictor. With a 2 bit choser counter, the simulator decides which predictor's prediction it needs to chose. If the counter's value is more than 2 it choses gshare and when it is less than 2 it chooses bimodal. The counters in bimodal and gshare are updated based on the prediction of the chooser table and the actual result of the branch.

## Usage

There is a makefile attached to the repository. On a linux machine a simple make command would create the executable that can be accessed from the terminal. 

                            - Bimodal:    ./sim.out bimodal <M> <BTB Size> <BTB Associativity> <trace file>
                                       where M represents the number of PC bits used to index the bimodal table. 
                            - Gshare :    ./sim.out gsahre <M> <N> <BTB Size> <BTB Associativity> <trace file> 
                                       where M and N represent the number of PC bits and the global branch history register bits used to index the gshare table respectively.
                            - Hybrid :    ./sim.out hybrid <K> <M1> <N> <M2> <BTB Size> <BTB Associativity> <tracefile>
                                        where M1 and N are the number of PC bits and the global branch history register used to index the gshare table respectively, M2 represents the number of PC bits used to index the bimodal table and K represents the number of PC bits required to index the chooser table. 

On a windows machine, using any of the C++ IDE the file could be complied and executed. However, it is important to pass arguments to your program in the format specified above. The same can be done using command prompt as well. 

More details for this project can be seen in the project spec document and the C++ file. 

Few example trace files are also uploaded to this repo.
