/* Project 2. Branch Predictor
1. Bimodal
2. Gshare 
3. Hybrid
Implementation : with and without Branch target buffer
ECE 563: Microarchitecture
Kashyap Ravichandran

*/

// Taken -  1
// Not Taken - 0

#include<iostream>
#include<math.h>
#include<fstream>
#include<cstring>
#include<stdlib.h>
#include<iomanip>
#include<stdio.h>

using namespace std;	

int write_back=0;
class cache
{
	int BLOCKSIZE;
	int offset;
	int tag_from_address;
	int index_from_address;
	int L1_SIZE;
	int L1_ASSOC;
	int L2_SIZE;
	int L2_ASSOC;
	int L2_DATA_BLOCK;
	int L2_ADDR_TAGS;
	int **tag; 
	int **lru;
	int **valid_flag;
	int index;
	int index_bits;
	int block_bits;
	cache *nextlevel;
	int **dirty_flag;// to identify if a dirty block is evicted

	
public: 
// initializing the attributes of class cache
	cache(int bs,int l1_size, int l1_assoc)
	{
		BLOCKSIZE=bs;
		L1_SIZE=l1_size;
		L1_ASSOC=l1_assoc;
		index=L1_SIZE/(BLOCKSIZE*L1_ASSOC);
		
		tag = new int* [index];
		for(int i=0;i<index;i++)
			tag[i]=new int[L1_ASSOC];
		
		for(int i=0;i<index;i++)
		for(int j=0;j<L1_ASSOC;j++)
			tag[i][j]=0;
		
		valid_flag = new int* [index];
		for(int i=0;i<index;i++)
			valid_flag[i]=new int[L1_ASSOC];
		
		lru = new int* [index];
		for(int i=0;i<index;i++)
			lru[i]=new int[L1_ASSOC];	
		
		dirty_flag = new int* [index];
		for(int i=0;i<index;i++)
			dirty_flag[i]=new int[L1_ASSOC];
		
		index_bits=log2(index);
		block_bits=log2(BLOCKSIZE);
		
		for(int i =0;i<index;i++)
		for(int j=0;j<L1_ASSOC;j++)
		dirty_flag[i][j]=0;
		
		for(int i =0;i<index;i++)
		for(int j=0;j<L1_ASSOC;j++)
		valid_flag[i][j]=0;	
		
		for(int i =0;i<index;i++)
		for(int j=0;j<L1_ASSOC;j++)
		lru[i][j]=L1_ASSOC;
		
		//cout<<"index "<<index;
		//cout<<"\n index bits"<<index_bits;
		//cout<<"\noffset bits"<<block_bits;
	
	}
	
	bool readFromAddress(int address)
	{
		// the function is gonna return a 1 if there is a hit and a zero when there is a miss 
		// implement dirty bit 
		int ra_tag,ra_index;
		split(address,ra_tag,ra_index);
		for(int i=0;i<L1_ASSOC;i++)
		{
			if(tag[ra_index][i]==ra_tag&&valid_flag[ra_index][i]) // hit
			{
				//update LRU
				update_lru(ra_index,i);
				return true;
			}
		}
		
		for(int i=0;i<L1_ASSOC;i++)
		{
			if(!valid_flag[ra_index][i])
			{
				
				tag[ra_index][i]=ra_tag; // invalid miss
				//update LRU Counter
				lru[ra_index][i]=0;
				for(int j=0;j<L1_ASSOC;j++)
				{
					if(j!=i&&valid_flag[ra_index][j])
						lru[ra_index][j]++;
					
				}
				//add one to every lru?
				dirty_flag[ra_index][i]=0;
				valid_flag[ra_index][i]=1;
				return false;
				
			}
					
		}			
		int lru_hor_index=0;
		for(int i =0;i<L1_ASSOC;i++)
		{
			// Find Least recently used memeory
			if(lru[ra_index][i]==(L1_ASSOC-1)&&valid_flag[ra_index][i])
			{
				lru_hor_index=i;
				break;
			}
		}
		
		// Replace and update LRU evicted address must be sent to the next stage
		tag[ra_index][lru_hor_index]=ra_tag;
		if(dirty_flag[ra_index][lru_hor_index])
		write_back++;
		//updateLRU
		//update_lru(ra_index,lru_hor_index);
		lru[ra_index][lru_hor_index]=0;
		for(int j=0;j<L1_ASSOC;j++)
		{
			if(j!=lru_hor_index&&valid_flag[ra_index][j])
				lru[ra_index][j]++;
					
		}
		dirty_flag[ra_index][lru_hor_index]=0;
		valid_flag[ra_index][lru_hor_index]=1;	
		return false;
		
	}
		
	void split(int address, int &tag_address,int &index_address )
	{
		//int temp3=address;
		int temp2=0;
		temp2=pow(2,block_bits)-1;
		offset=address&temp2;
		int temp= address>>(block_bits);
		int temp1=1;
		temp1=pow(2,index_bits)-1;
		index_address=temp&temp1;
		tag_address=address>>(block_bits+index_bits);
		
	}

	void update_lru(int index,int lru_index)
	{
		// Memeber function to update the LRU counter
		int temp=lru[index][lru_index];
		lru[index][lru_index]=0;
		for(int i=0;i<L1_ASSOC;i++)
		{
			if(lru[index][i]<temp&&valid_flag[index][i]&&lru_index!=i)
				lru[index][i]++;
		}
				
	}
	
	void output()
	{
		
		cout<<"FINAL BTB CONTENTS\n";
		//int check_variable=0;
		for(int i=0;i<index;i++)
		{
			for(int j=0;j<L1_ASSOC-1;j++)
			for(int k=0;k<L1_ASSOC-1-j;k++)
				if(lru[i][k]>lru[i][k+1])
				{
					int t= tag[i][k];
					tag[i][k]=tag[i][k+1];
					tag[i][k+1]=t;
					t=dirty_flag[i][k];
					dirty_flag[i][k]=dirty_flag[i][k+1];
					dirty_flag[i][k+1]=t;
					t=lru[i][k];
					lru[i][k]=lru[i][k+1];
					lru[i][k+1]=t;
					
					
				}			
		}
		
		for(int i =0;i<index;i++)
		{
			cout<<" set "<<i<<":";
			for(int j=0;j<L1_ASSOC;j++)
			{
				if(tag[i][j])
					cout<<"  "<<std::hex<<tag[i][j]<<std::dec<<" ";
				else
					cout<<"    ";
			}
			cout<<"\n";
		}
	

	}
	

};

class bimodal
{
	int *branchpredict;
	int size;
	int assoc;
	int num_bits;
	
	// metrics;
		
	void split(int address, int &ra_index)
	{
		address=address>>2;
		ra_index=address&(size-1);
		
	}
	
	void calculate()
	{
		rate=float(mispredictions)/float(num_branches);	
	}	

	public:
	
	int num_branches;
	int mispredictions;
	float rate;
		
	bimodal(int s, int a, int num)
	{
	
		num_bits=num;
		size=pow(2,num);
		branchpredict = new int [size];
		
		for(int i=0;i<size;i++)
			branchpredict[i]=2;
			
		num_branches=0;
		mispredictions=0;
				
		
	}
		
	void prediction(int address,int actual)
	{
		int ra_index;
		split(address,ra_index);
		num_branches++;		
		int predict;
		if(branchpredict[ra_index]>=2)
			predict=1;
		else if(branchpredict[ra_index]<2)
			predict=0;
		
		if(actual)
		{
			branchpredict[ra_index]++;
			if(branchpredict[ra_index]>=3)
				branchpredict[ra_index]=3;
		}
		else if (!actual)
		{
			branchpredict[ra_index]--;
			if(branchpredict[ra_index]<=0)
				branchpredict[ra_index]=0;
		}
		
		if((actual&&!predict)||(!actual&&predict))
			mispredictions++;
		
	}
	
	void output()
	{
		calculate();
		cout<<"OUTPUT\n";
		cout<<" number of predictions: "<<num_branches;
		cout<<"\n number of mispredictions: "<<mispredictions;
		cout<<std::setprecision(2)<<std::fixed;
		cout<<"\n misprediction rate: "<<rate*100<<"%\n";
		cout<<"FINAL BIMODAL CONTENTS\n";
		for(int i=0;i<size;i++)
			cout<<i<<" "<<branchpredict[i]<<"\n";
	}
	void output1()
	{
		cout<<"FINAL BIMODAL CONTENTS\n";
		for(int i=0;i<size;i++)
			cout<<i<<" "<<branchpredict[i]<<"\n";
	}
	
};

class gshare
{
	int *branchpredict;
	//int BHR;
	int num_bits;
	int num_BHR;
	unsigned int BHR;
	int size;
	
	//metrics 
	
	
	
	void split(int address, int &ra_index)
	{
		address=address>>2;
		int temp=pow(2,num_bits)-1;
		ra_index=address&temp;
		
	}
	
	void calculate()
	{
		rate=float(misprediction)/float(num_branches);	
	}
	
	void update_BHR(int branch)
	{
		BHR=BHR>>1;
		//BHR=BHR&~(1<<(num_BHR));
		if(branch)
			BHR |= (1<<(num_BHR-1));
		else
			BHR &= ~(1<<(num_BHR-1));
		//BHR=BHR&~(1<<(num_BHR));
		
	}
	public:
	int num_branches;
	int misprediction;
	float rate;
	
	gshare(int m, int n,int s, int assoc)
	{
		num_bits=m;
		num_BHR=n;
		//size=0;
		size=pow(2,num_bits);
				
		branchpredict= new int[size];
		
		
		for(int i=0;i<size;i++)
			branchpredict[i]=2;
		
		BHR=0;
		
		num_branches=0;
		misprediction=0;		
	}
	
	void predict(int address, int actual)
	{
		num_branches++;
		unsigned int temp=BHR<<(num_bits-num_BHR);
		int ra_index=0;
		split(address,ra_index);
		ra_index=ra_index^temp;
		int predict;
		if(branchpredict[ra_index]>=2)
			predict=1;
		else if(branchpredict[ra_index]<2)
			predict=0;
		
		if(actual)
		{
			branchpredict[ra_index]++;
			if(branchpredict[ra_index]>=3)
				branchpredict[ra_index]=3;
		}
		else if (!actual)
		{
			branchpredict[ra_index]--;
			if(branchpredict[ra_index]<=0)
				branchpredict[ra_index]=0;
		}
		
		if((actual&&!predict)||(!actual&&predict))
			misprediction++;
		
		update_BHR(actual);
		
					
	}
		void output()
	{
		calculate();
		cout<<"OUTPUT\n";
		cout<<" number of predictions: "<<num_branches;
		cout<<"\n number of mispredictions: "<<misprediction;
		cout<<std::setprecision(2)<<std::fixed;
		cout<<"\n misprediction rate: "<<rate*100<<"%\n";
		cout<<"FINAL GSHARE CONTENTS\n";
		for(int i=0;i<size;i++)
			cout<<i<<" "<<branchpredict[i]<<"\n";
	}
	void output1()
	{
		cout<<"FINAL GSHARE CONTENTS\n";
		for(int i=0;i<size;i++)
			cout<<i<<" "<<branchpredict[i]<<"\n";
	}
};

class hybrid
{
	int *bimodal;
	int *gshare;
	int *chose;
	
	int m1;
	int n1;
	int m2;
	int k;
	int BHR;
	
	int size1;
	int size2;
	int size3;
	
	
	
	void split1(int address, int &ra_index)
	{
		address=address>>2;
		ra_index=address&(size1-1);
	}
	
	void split2(int address, int &ra_index)
	{
		address=address>>2;
		ra_index=address&(size2-1);
		int temp=BHR<<(m1-n1);	
		ra_index=ra_index^temp;
	}
	void split3(int address, int &ra_index)
	{
		address=address>>2;
		ra_index=address&(size3-1);	
	}
	void update_BHR(int actual)	
	{
		BHR=BHR>>1;
		if(actual)
			BHR|=(1<<(n1-1));
		else
			BHR&=~(1<<(n1-1));
			//cout<<"\n BHR "<<BHR;
			//cout<<(1<<(n1-1));
			//cout<<n1;
	}
public:
	int num_branches;
	int misprediction;
	float rate;
	
	hybrid(int k1, int mm1, int nn1, int mm2)
	{
		k=k1;
		size1=pow(2,k);
		
		m1=mm1;
		n1 =nn1;
		size2=pow(2,m1);
		
		m2=mm2;
		size3=pow(2,m2);
		
		num_branches=0;
		misprediction=0;
		rate=0;
		
		bimodal = new int[size3];
		for(int i=0;i<size3;i++)
			bimodal[i]=2;
		
		gshare = new int[size2];
		for(int i=0;i<size2;i++)
			gshare[i]=2;

		chose = new int[size1];
		for(int i=0;i<size1;i++)
			chose[i]=1;
		
		BHR=0;
	}	
	
	void predict(int address, int actual)
	{
		int ra_index1,ra_index2,ra_index3;
		
		num_branches++;
		
		split1(address,ra_index1);
		split2(address,ra_index2);
		split3(address,ra_index3);
		
		//cout<<"\n bimodal "<<ra_index3<<" Gshare "<<ra_index2<<" Choser "<<ra_index1;		
		int predict1, predict2;
	
		if(gshare[ra_index2]>=2)
			predict1=1;
		else if(gshare[ra_index2]<2)
			predict1=0;
		
		if(bimodal[ra_index3]>=2)
			predict2=1;
		else if(bimodal[ra_index3]<2)
			predict2=0;
		//cout<<" Actual "<<actual<<" bimodal "<<predict2<<" Gshare "<<predict1;	
		update_BHR(actual);
		//cout<<" BHR "<<BHR;
		if(chose[ra_index1]>=2)
		{
			if(predict1!=actual)
				misprediction++;
			if(actual)
			{	
				gshare[ra_index2]++;
				if(gshare[ra_index2]>=3)
					gshare[ra_index2]=3;
			}
			else
			{
				gshare[ra_index2]--;
				if(gshare[ra_index2]<=0)
					gshare[ra_index2]=0;
			}
		}
		else if(chose[ra_index1]<2)
		{
			if(predict2!=actual)
				misprediction++;
			if(actual)
			{
				bimodal[ra_index3]++;
				if(bimodal[ra_index3]>=3)
					bimodal[ra_index3]=3;
			}
			else
			{
				bimodal[ra_index3]--;
				if(bimodal[ra_index3]<=0)
					bimodal[ra_index3]=0;
			}
		}
		if((actual&&predict1&&!predict2)||(!actual&&!predict1&&predict2))
		{
			// increment the choser value
			chose[ra_index1]++;
			if(chose[ra_index1]>=3)
				chose[ra_index1]=3;
		}
		else if ((actual&&!predict1&&predict2)||(!actual&&predict1&&!predict2))
		{
			// decrement the choser value
			chose[ra_index1]--;
			if(chose[ra_index1]<=0)
				chose[ra_index1]=0; 	
		}			
	}
	void output()
	{
		rate=float(misprediction)/num_branches;
		cout<<"OUTPUT\n";
		cout<<" number of predictions: "<<num_branches;
		cout<<"\n number of mispredictions: "<<misprediction;
		cout<<std::setprecision(2)<<std::fixed;
		cout<<"\n misprediction rate: "<<rate*100<<"%\n";
		cout<<"FINAL CHOOSER CONTENTS\n";
		for(int i=0;i<size1;i++)
			cout<<i<<" "<<chose[i]<<"\n";	
		cout<<"FINAL GSHARE CONTENTS\n";
		for(int i=0;i<size2;i++)
			cout<<i<<" "<<gshare[i]<<"\n";
		cout<<"FINAL BIMODAL CONTENTS\n";
		for(int i=0;i<size3;i++)
			cout<<i<<" "<<bimodal[i]<<"\n";
	}
	void output1()
	{
		cout<<"FINAL CHOOSER CONTENTS\n";
		for(int i=0;i<size1;i++)
			cout<<i<<" "<<chose[i]<<"\n";	
		cout<<"FINAL GSHARE CONTENTS\n";
		for(int i=0;i<size2;i++)
			cout<<i<<" "<<gshare[i]<<"\n";
		cout<<"FINAL BIMODAL CONTENTS\n";
		for(int i=0;i<size3;i++)
			cout<<i<<" "<<bimodal[i]<<"\n";
	}	
};


int main(int argc, char *argv[])
{
	if(!argc)
	{
		cout<<" Invalid call, change it!";
		return 0;
	}
	if(strcmp(argv[1],"bimodal")==0)
	{
		int arg[3]={0};
		int temp;
		//cout<<argv[1];
		for(int i=2;i<=4;i++)
		{
			for(int j=0;argv[i][j];j++)
			
			{
				arg[i-2]*=10;
				temp=argv[i][j]-48;
				arg[i-2]+=temp;
			}
		}
		//cout<<arg[0]<<arg[1]<<arg[2];
		ifstream fin;
		fin.open(argv[5]);	
		//cout<<"Hello";
		if(fin.is_open())
		{
			int address=0;
			int num_branches=0,misprediction=0, hits=0;
			char temp1[10]; char temp2[10];
			
			if(arg[1])
			{
				cache BTB(4,arg[1],arg[2]);
				bimodal bim(arg[1],arg[2],arg[0]);
				while(!fin.eof())
				{
					fin.getline(temp1,10,'\n');
					address=0;
					//cout<<temp1;
					
					
					for(int i=0;temp1[i]!=' ';i++)
						temp2[i]=temp1[i];
						
					address=strtol(temp2,NULL,16);
					if(address)
					{
						num_branches++;
						bool miss_hit=BTB.readFromAddress(address);
						if(!miss_hit)
						{
							if(temp1[7]=='t'||temp1[7]=='T')
								misprediction++;
						}
						else if(miss_hit)
						{
							hits++;
							if(temp1[7]=='t'||temp1[7]=='T')
							{
								bim.prediction(address,1);						
							}
							else if (temp1[7]=='n'||temp1[7]=='N')
							{
								bim.prediction(address,0);
							}
						}
					}
				}
				
				//================ Output ===================
				
				cout<<"COMMAND\n";
				for(int i=0;i<argc;i++)
					cout<<argv[i]<<" ";
				
				cout<<"\n";
				cout<<"OUTPUT";
				cout<<"\nsize of BTB: "<<arg[1];
				cout<<"\nnumber of branches: "<<num_branches;
				cout<<"\nnumber of predictions from branch predictor: "<<hits;
				cout<<"\nnumber of mispredictions from branch predictor: "<<bim.mispredictions;
				cout<<"\nnumber of branches miss in BTB and taken: "<<misprediction;
				cout<<"\ntotal mispredictions:	"<<bim.mispredictions+misprediction;
				float rate=(bim.mispredictions+misprediction)/float(num_branches)*100;
				cout<<std::setprecision(2)<<std::fixed;
				cout<<"\nmisprediction rate: "<<rate<<"%\n";
				BTB.output();
				cout<<"\n";
				bim.output1();
			}
			else
			{
					//cout<<"Hello";
					int address=0;
					char temp1[10]; char temp2[10];
					bimodal bim(arg[1],arg[2],arg[0]);
					while(!fin.eof())
					{
						fin.getline(temp1,10,'\n');
						address=0;
						//cout<<temp1;
						
						for(int i=0;temp1[i]!=' ';i++)
							temp2[i]=temp1[i];
							
						address=strtol(temp2,NULL,16);
						if(address)	
						{
							if(temp1[7]=='t'||temp1[7]=='T')
							{
								bim.prediction(address,1);						
							}
							else if (temp1[7]=='n'||temp1[7]=='N')
							{
								bim.prediction(address,0);
							}
						}
					}
					
					//================ Output ===================
					
					cout<<"COMMAND\n";
					for(int i=0;i<argc;i++)
						cout<<argv[i]<<" ";
					
					cout<<"\n";
					bim.output();
			}
			
		}
	}
	else if (strcmp(argv[1],"gshare")==0)
	{
		int arg[4]={0};
		int temp;
		//cout<<argv[1];
		for(int i=2;i<=5;i++)
		{
			for(int j=0;argv[i][j];j++)
			
			{
				arg[i-2]*=10;
				temp=argv[i][j]-48;
				arg[i-2]+=temp;
			}
		}
		//cout<<arg[0]<<arg[1]<<arg[2];
		ifstream fin;
		fin.open(argv[6]);	
		//cout<<"Hello";
		if(fin.is_open())
		{
			int address=0;
			int num_branches=0,misprediction=0, hits=0;
			char temp1[10]; char temp2[10];
			
			if(arg[2])
			{
				cache BTB(4,arg[2],arg[3]);
				gshare gs(arg[0],arg[1],0,0);
				while(!fin.eof())
				{
					fin.getline(temp1,10,'\n');
					address=0;
					//cout<<temp1;
					
					for(int i=0;temp1[i]!=' ';i++)
						temp2[i]=temp1[i];
						
					address=strtol(temp2,NULL,16);
					if(address)
					{
						num_branches++;
						bool miss_hit=BTB.readFromAddress(address);
						if(!miss_hit)
						{
							if(temp1[7]=='t'||temp1[7]=='T')
								misprediction++;
						}
						else if(miss_hit)
						{
							hits++;
							if(temp1[7]=='t'||temp1[7]=='T')
							{
								gs.predict(address,1);						
							}
							else if (temp1[7]=='n'||temp1[7]=='N')
							{
								gs.predict(address,0);
							}
						}
					}
				}
				
				//================ Output ===================
				
				cout<<"COMMAND\n";
				for(int i=0;i<argc;i++)
					cout<<argv[i]<<" ";
				
				cout<<"\nOUTPUT";
				cout<<"\nsize of BTB: "<<arg[2];
				cout<<"\nnumber of branches: "<<num_branches;
				cout<<"\nnumber of predictions from branch predictor: "<<hits;
				cout<<"\nnumber of mispredictions from branch predictor: "<<gs.misprediction;
				cout<<"\nnumber of branches miss in BTB and taken: "<<misprediction;
				cout<<"\ntotal mispredictions:	"<<gs.misprediction+misprediction;
				float rate=(gs.misprediction+misprediction)/float(num_branches)*100;
				cout<<std::setprecision(2)<<std::fixed;
				cout<<"\nmisprediction rate: "<<rate<<"%\n";
				BTB.output();
				cout<<"\n";
				gs.output1();
			}
			else
			{
					//cout<<"Hello";
					int address=0;
					char temp1[10]; char temp2[10];
					gshare gs(arg[0],arg[1],arg[0],0);
					while(!fin.eof())
					{
						fin.getline(temp1,9);
						address=0;
						//cout<<temp1;
						
						for(int i=0;temp1[i]!=' ';i++)
							temp2[i]=temp1[i];
							
						address=strtol(temp2,NULL,16);
						if(address)
						{
								
							if(temp1[7]=='t'||temp1[7]=='T')
							{
								gs.predict(address,1);						
							}
							else if (temp1[7]=='n'||temp1[7]=='N')
							{
								gs.predict(address,0);
							}
						}
					}
					
					//================ Output ===================
					
					cout<<"COMMAND\n";
					for(int i=0;i<argc;i++)
						cout<<argv[i]<<" ";
					
					cout<<"\n";
					gs.output();
			}
			
		}
	}
	else if(strcmp(argv[1],"hybrid")==0)
	{
		int arg[6]={0};
		int temp;int address=0;int num_branches=0,misprediction=0,hits=0;
		char temp1[10]; char temp2[10];
		for(int i=2;i<=7;i++)
		{
			for(int j=0;argv[i][j];j++)
			{
				arg[i-2]*=10;
				temp=argv[i][j]-48;
				arg[i-2]+=temp;
			}
		}
		ifstream fin;
		fin.open(argv[8]);	
		if(!arg[4])
		{
			//cout<<"Hello";
			if(fin.is_open())
			{
				
				//int j=1;
				
				hybrid hyb(arg[0],arg[1],arg[2],arg[3]);
				while(!fin.eof())
				{
					fin.getline(temp1,10,'\n');
					address=0;
					//cout<<"\n"<<j<<". PC: "<<temp1;
					//j++;
					for(int i=0;temp1[i]!=' ';i++)
						temp2[i]=temp1[i];
					//cout<<"\n";
					//cout<<temp2;	
					address=strtol(temp2,NULL,16);
					if(address)	
					{
						if(temp1[7]=='t'||temp1[7]=='T')
						{
							hyb.predict(address,1);						
						}
						else if (temp1[7]=='n'||temp1[7]=='N')
						{
							//cout<<"1";
							hyb.predict(address,0);
						}
					}		//if(j==100)
					//break;
				}
				
				//================ Output ===================
				
				cout<<"COMMAND\n";
				for(int i=0;i<argc;i++)
					cout<<argv[i]<<" ";
				
				cout<<"\n";
				hyb.output();
			}
		}
		else 
		{
				cache BTB(4,arg[4],arg[5]);
				hybrid hyb(arg[0],arg[1],arg[2],arg[3]);
				while(!fin.eof())
				{
					fin.getline(temp1,10,'\n');
					address=0;
					//cout<<temp1;
					
					for(int i=0;temp1[i]!=' ';i++)
						temp2[i]=temp1[i];
						
					address=strtol(temp2,NULL,16);
					if(address)
					{
						num_branches++;
						bool miss_hit=BTB.readFromAddress(address);
						if(!miss_hit)
						{
							if(temp1[7]=='t'||temp1[7]=='T')
								misprediction++;
						}
						else if(miss_hit)
						{
							hits++;
							if(temp1[7]=='t'||temp1[7]=='T')
							{
								hyb.predict(address,1);						
							}
							else if (temp1[7]=='n'||temp1[7]=='N')
							{
								hyb.predict(address,0);
							}
						}
					}
				}
				
				//================ Output ===================
				
				cout<<"COMMAND\n";
				for(int i=0;i<argc;i++)
					cout<<argv[i]<<" ";
				
				cout<<"\n";
				cout<<"OUTPUT";
				cout<<"\nsize of BTB: "<<arg[2];
				cout<<"\nnumber of branches: "<<num_branches;
				cout<<"\nnumber of predictions from branch predictor: "<<hits;
				cout<<"\nnumber of mispredictions from branch predictor: "<<hyb.misprediction;
				cout<<"\nnumber of branches miss in BTB and taken: "<<misprediction;
				cout<<"\ntotal mispredictions:	"<<hyb.misprediction+misprediction;
				float rate=(hyb.misprediction+misprediction)/float(num_branches)*100;
				cout<<std::setprecision(2)<<std::fixed;
				cout<<"\nmisprediction rate: "<<rate<<"%\n";
				BTB.output();
				cout<<"\n";
				hyb.output1();	
		}
	
	}
	
	
	
	
	
	
	
	//cout<<"Hey";
	
	
	return 0;
}

