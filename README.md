# Branch Prediction 

This repository has codes pertaining to project 2 of NCSU's ECE 563 course on micro architecture. The project simulates the working of a bimodal, g-share and a hybrid branch predictor. There are three different types of predictors that is implemented namely:

- Bimodal
- G-Share 
- Hybrid predictor

## Bimodal

The predictor uses a 4 level smith counter. We increament the counter when the branch is taken and decrement it when the branch is not taken. The counter saturates at 0 and at 3. We use the program counter to index the branch prediction table. We ignore the last two bits and then use the next 'm' bits of the PC to index a location in the table. A complete description of the predictor is given in the project spec sheet. 

## G-Share 

This kind of a predictor table uses the PC value with the 
