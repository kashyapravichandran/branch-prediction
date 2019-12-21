# Branch Prediction 

This repository has codes pertaining to project 2 of NCSU's ECE 563 course on micro architecture. The project simulates the working of a bimodal, g-share and a hybrid branch predictor. There are three different types of predictors that is implemented namely:

- Bimodal
- G-Share 
- Hybrid 

## Bimodal

The predictor uses a 4 level smith counter. We increament the counter when the branch is taken and decrement it when the branch is not taken. The counter saturates at 0 and at 3. If the value is 1 or 0 the branch is predicted to not being taken and if its 2 or 3 it is predicted taken. We use the program counter to index the branch prediction table. We ignore the last two bits and then use the next 'm' bits of the PC to index a location in the table. A complete description of the predictor is given in the project spec sheet. 

## G-Share 

This kind of a predictor table uses the PC value hashed with a branch history register to index the prediction table. The updation of the value in the table is similar to the one explained in the previous section. To update the branch history register we shift the content to the left by 1  and or the actual branch result to the Least significant bit. 

## Hybrid 

The predictor combines both the bimodal and the G-Share predictor 
