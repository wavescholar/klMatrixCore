 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#include<math.h>
#include <iostream>
#include <fstream>
using namespace std;
//namespace KL_NN{
double random(long *idum);
void rand_init(void);
double rand2(void);
void rand_cleanup(void);

class layer{

public:
	int units;
	enum {LINEAR,LOGISTIC,TANH,THRESHOLD} node_type;
	double bias;
	
public:
	layer();
	~layer();
	void operator=(layer &source);

};

class network:public layer{

public:
	enum {BKP,MD,LMS,RUN} training_type;
	int layers;
	int input_dim;
	double training_param;
	int num_vectors;
	int passes;
	char input_file_buf[256];
	char output_file_buf[256];
	char weights_file_buf[256];
	double lower_range,upper_range; //interval to choose weights from when initializing network
	//converting above three objects to vector layer-p for compatability with bk-prop fn's
	layer *layer_p;

public:
	network();//create layers 
	~network();//delete layers
	void operator=(network &source);

};


class rn_unit_train{
public:	
    double *weights;
    double linear_output; 
    double activation;
	rn_unit_train();
	~rn_unit_train();
};//end class rn_unit_train    

class rnntwk : public network{
/* for reference these are the public members of base class network
public:
	int layers;
	int input_dim;
	double training_param;
	int num_vectors;
	int passes;
	layer *layer_p;

 //and these are the members of class layer
	int units;
	enum {LINEAR,LOGISTIC,TANH} node_type;
	double bias;
*/
private:
    
	rn_unit_train **rn_unit_p; //matrix of pointers to units
	double *data_p;  // input vector
	int count,count_l,count_u,count_w;// counters for vectors, layers, units, weights
	int vector_number; //counter fo the number of vectors in data set
	int output_dim;
	int eof_flag;
	//these are here until another input routine is written
    FILE *fp,*fp1,*fp2;  //fp is data input file
	char buf[256];

public:
	rnntwk(network *ntwk_input);
	~rnntwk();
	void run_network(); //calls propagate,get_data,file_open,write_output

private:
	void   rn_propagate();
	double   rn_get_weight();
	int   rn_get_data();
	void   rn_file_open();
	void   rn_write_output();
	
}; //end class rnntwk declaration




class md_unit_train{  
public:
    int trained;                
    double *weights;
    double *weights_temp;    
    double linear_output;
    double threshold_output;
    int rank;
	md_unit_train();
	~md_unit_train();};    // rank member is not used at this point 


class md:public network{
friend int  compare(const void *arg1,const void *arg2);
public:
	md(network *ntwk_input);
	~md();

private:
md_unit_train **md_unit_p;
md_unit_train *z,*x;  // optimization variables to be used in loops where layer and unit are fixed and we're stepping through weights 
                      // z is used in min_disturbance(), x in propagate()  
FILE *fp, *fp1;       
char buf[256];
long int save_position;
double *data_p,*desired_out;  // input & output vectors 
int pass_num;  // counter for the number of training passes for the training data 
int vector_selection;  // indicates the random choice of training vector selected from file, used in min_disturbance() and md_get_data() 
int count,count_l,count_u,count_w;  // counters for stepping through training vectors, layers, units, weights 
int adaption_layer; // indicates the layer nodes are being adapted on 
int count_lr,count_ur;  // used by rank and compare functions 
double error,error_new,temp;  // used in md_update_weights() and min_disturbance() 
double *global_error; // an array of ptrs to error sums, indexed over training passes 
int output_dim;
int **rank_mat; //units ranked by rank() fn accross layers, then rank_mat is used in min_dist() to select units for updating
int update_unit,num_inputs;  // used in min_disturbance(), md_update_weight() 

public:
void min_disturbance();

private:
void md_unit_p_init();
void md_propagate(double *data_v);
void rank();
void fseek_to_vector(int selection);
double md_error_comp(double *desired_out);
void md_update_weight(int update_unit,int num_inputs,double *data_v);
void md_output_network();
void md_get_data();
void md_file_open();
};//end class md declaration




class bp_unit_train{
public:	
    double *weights;
    double linear_output; 
    double activation;
    double derivative;
    double delta;

	bp_unit_train();
	~bp_unit_train();
};//end class bp_unit_train    

class bkp : public network{
/* for reference these are the public members of base class network
public:
	int layers;
	int input_dim;
	double training_param;
	int num_vectors;
	int passes;
	layer *layer_p;

 //and these are the members of class layer
	int units;
	enum {LINEAR,LOGISTIC,TANH} node_type;
	double bias;
*/
private:
    
	bp_unit_train **bp_unit_p; //matrix of pointers to units
	bp_unit_train *x;  // optimization variable to be used where layer and unit are fixed 

	double *data_p,*desired_out;  // input & output vectors
	int count,count_l,count_u,count_w;// counters for training vectors, layers, units, weights
	int vector_number; //counter fo the number of vectors in data set
	double error;   
	double **global_error; // an array of ptrs to error sums, indexed over training passes 
	int output_dim;
	//these are here until another input routine is written
    FILE *fp,*fp1,*fp3;  //fp is data input file
	char buf[256];

public:
	bkp(network *ntwk_input);
	~bkp();
	void operator=(bkp &source);
	void back_prop(); //calls propagate,error-comp,update_wight,get_data,file_open

private:
	void   bp_propagate(double *data_v);
	double bp_error_comp(double *desired_out);
	void   bp_update_weight();
	void   bp_get_data();
	void   bp_file_open();
	void   bp_output_network();
	
}; //end class bkp declaration


static long int save_position;

//replace with passes and count from passes to 0
//int pass_num=0;  

// indicates the random choice of training vector selected from file
//make member of get data
//int vector_number;





void win_train();

#define POP_SIZE 60
#define P_c 0.2
#define P_m 0.01
#define PRECISION 25
#define a 0
#define b 3
#define Pi 3.14159
#define GENERATIONS 20

class klGA {
/* A genetic algorithm is used to find a maximum of a function           *
 * on a closed inteval.  Report is printed to file for every generation. *
 * Parqameters may be modified,  the function to optimize may be         *
 * modified by looking in the function evaluate_pop().                   */

/* P_c = probability of crossover, P_m = probability of mutation, PRECISION    *
 *  = the number of bits required to break [a,b] into the required # of pieces */
   

/* Declarations for NR based random number generator */
//#define IM1 2147483563
//#define IM2 2147483399
//#define AM (1.0/IM1)
//#define IMM1 (IM1-1)
//#define IA1 40014
//#define IA2 40692
//#define IQ1 53668
//#define IQ2 52774
//#define IR1 12211
//#define IR2 3791
//#define NTAB 32
//#define NDIV (1+IMM1/NTAB)
//#define EPS 1.2e-7
//#define RNMX (1.0-EPS)
                                         
double ran2(long *idum);
static long *seed;

public:
	klGA()
{


}

/* Functions used in the GA  */
void initialize_pop(struct individual *p_population);
void print_report(struct individual *p_population);
double evaluate(struct individual *p_population);
double location(struct individual *p_population);
void rank(struct individual *p_population);
void selection(struct individual *p_population);
void breed(struct individual *p_population);
void mutate(struct individual *p_population);
void swap_populations(struct individual *p_population);
void evaluate_pop(struct individual *p_population);
void clear_pop(struct individual *p_population);

/* data structure of individuals */
struct individual {
    int chromasome[PRECISION];
    double x_value;
    double eval;
    int breeder;
    int mutated[PRECISION];
    int generation;
    int rank;};
struct individual population[POP_SIZE];
struct individual new_population[POP_SIZE];
struct super_individual {
    int chromasome[PRECISION];
    double x_value;
    double eval;
    int rank;};
struct super_individual best_of[GENERATIONS];

 char report_file[80];

 
void initialize_pop(struct individual *p_population)
{
    int count;
    int count_chrom;
    for(count=0;count<POP_SIZE;count++)
    {
        for(count_chrom=0;count_chrom<PRECISION;count_chrom++)    /* initialize the starting genetic structure */
        {
            if(ran2(seed)>0.5)
            (p_population+count)->chromasome[count_chrom]=1;
            else     
            (p_population+count)->chromasome[count_chrom]=0;
        }
        (p_population+count)->x_value=location(p_population+count);
        (p_population+count)->eval=evaluate(p_population+count);    /* evaluate starting pop */

        /* set breeder, mutation =0 and generation =0 */
        (p_population+count)->breeder=0;
        for(count_chrom=0;count_chrom<PRECISION;count_chrom++)
        {
            (p_population+count)->mutated[count_chrom]=0;
        }
        (p_population+count)->generation=0;
        (p_population+count)->rank=0;
    }
}
void evaluate_pop(struct individual *p_population)
{
    int count;
    for(count=0;count<POP_SIZE;count++)
    {
        (p_population+count)->x_value=location(p_population+count);
        (p_population+count)->eval=evaluate(p_population+count);
    }
}

double location(struct individual *p_population)
{
    int count,piece_num=0;
    double x;
    for(count=0;count<PRECISION;count++)       /* convert binary chromasome to piece_number on interval */
    {
        if((p_population)->chromasome[count]==1)
        piece_num+=pow((double)2,count);
    }
    x=(double)(a+(double)piece_num*(b-a)/(pow((double)2,PRECISION)-1));   /* convert piece_num to location on interval */
    return x;
}

double evaluate(struct individual *p_population)
{
    int count,piece_num=0;
    double x;
    for(count=0;count<PRECISION;count++)       /* convert binary chromasome to piece_number on interval */
    {
        if((p_population)->chromasome[count]==1)
        piece_num+=pow((double)2,count);
    }   
    x=(double)(a+(double)piece_num*(b-a)/(pow((double)2,PRECISION)-1));   /* convert piece_num to location on interval */
    return ((cos(Pi*x))+3);                          /*!!!!!!!!!!!!!!!!!HERE'S THE FUNCTION TO OPTIMIZE  !!!!!!!!!!!!*/
}

void rank(struct individual *p_population)
{
    int counta,countb,countc,higher,most_fit=POP_SIZE+1;
    for(counta=0;counta<POP_SIZE;counta++)
    {
        higher=0;
        for(countb=0;countb<POP_SIZE;countb++)
        {
            if((p_population+counta)->eval<(p_population+countb)->eval)
            higher++;
        }
        (p_population+counta)->rank=higher;    
    }
    for(counta=0;counta<POP_SIZE;counta++)
    {
        if((p_population+counta)->rank<=most_fit)
        most_fit=(p_population+counta)->rank;
    }
    for(counta=0;counta<POP_SIZE;counta++)
    {
        
        if((p_population+counta)->rank==most_fit)
            {
                for(countc=0;countc<PRECISION;countc++)
                {
                    best_of[(p_population+counta)->generation].chromasome[countc]=(p_population+counta)->chromasome[countc];
                }
                best_of[(p_population+counta)->generation].x_value=(p_population+counta)->x_value;
                best_of[(p_population+counta)->generation].eval=(p_population+counta)->eval;
            }
    }    
}

void selection(struct individual *p_population)
{
    double total_fitness=0,prob_selection[POP_SIZE],prob_cumulative[POP_SIZE],random;
    int count,sub_count;
    for(count=0;count<POP_SIZE;count++)
        prob_cumulative[count]=0;
    for(count=0;count<POP_SIZE;count++)    /* caculate the total population fitness*/
        total_fitness+=(p_population+count)->eval;
    for(count=0;count<POP_SIZE;count++)   /* calculate the probability of seleciton */
        prob_selection[count]=(p_population+count)->eval/total_fitness; 
    for(count=0;count<POP_SIZE;count++)   /* calculate the cumulative probability */
        for(sub_count=0;sub_count<=count;sub_count++)
        {
            prob_cumulative[count]+=prob_selection[sub_count];
        }
    /*!!! temporary print statements !!!*/
    /*printf("\nprob_selection\tprob_cumulative");
    for(count=0;count<POP_SIZE;count++)
    {
        printf("\n%lf\t%lf",prob_selection[count],prob_cumulative[count]);
    }*/
    /* select a new population based on fitness values */
    /* put new population in new_population[] temporarily then copy to population[] after selection and breeding */ 
    for(count=0;count<POP_SIZE;count++)
    {
        random=ran2(seed);
        for(sub_count=0;sub_count<POP_SIZE;sub_count++)
        {
            if(random<prob_cumulative[sub_count])
            {
                new_population[count]=population[sub_count];
                break;
            }
        }
    }
    /* decide who we're going to breed */
    for(count=0;count<POP_SIZE;count++)
    {
        random=ran2(seed);
        if(random<P_c)
        new_population[count].breeder=1;
    }
}

void breed(struct individual *p_population)
{
    int count,sub_count,i,j,buff[PRECISION];
    for(count=0;count<POP_SIZE;count++)
    {
        if((p_population+count)->breeder==1)
        /*select a spot to crosslink and find the next breeder */
        {
            for(j=0;j<POP_SIZE;j++)
            {
                
                if(ran2(seed)<=(double)j/POP_SIZE)
                {
                    break;
                }
                
            }
            for(sub_count=count+1;sub_count<POP_SIZE;sub_count++)
            {
                if((p_population+sub_count)->breeder==1)
                /* crosslinking takes place here */
                {
                    for(i=0;i<PRECISION;i++)
                    {
                        buff[i]=(p_population+count)->chromasome[i];
                    }
                    for(i=j;i<PRECISION;i++)
                    {
                        (p_population+count)->chromasome[i]=(p_population+sub_count)->chromasome[i];
                        (p_population+sub_count)->chromasome[i]=buff[i];
                    }
                    /* once we've made it this far, we want to sart over at count=sub_count+1 */                    
                    count=sub_count+1;
                    
                    break;
                }
            }
        }
    }
}

void mutate(struct individual *p_population)
{
    int count,sub_count;
    for(count=0;count<POP_SIZE;count++)
    {
        for(sub_count=0;sub_count<PRECISION;sub_count++)
        {
            if(ran2(seed)<P_m)
            {
                switch((p_population+count)->chromasome[sub_count])
                {
                    case 0:
                    {
                        (p_population+count)->chromasome[sub_count]=1;
                        (p_population+count)->mutated[sub_count]=1;
                    }
                    case 1:
                    {
                        (p_population+count)->chromasome[sub_count]=0;
                        (p_population+count)->mutated[sub_count]=1;
                    }
                }
            }
        }
    }
}                        
void swap_populations(struct individual *p_population)
{
    int count;
    for(count=0;count<POP_SIZE;count++)
    {
        population[count]=new_population[count];        
    }
}        
void print_report(struct individual *p_population)
{
    int count;
    int count_chrom;
    FILE *fp;
    if((fp=fopen(report_file,"a+"))==NULL)
    {
        fprintf(stderr,"\nError opening file!");
        exit(1);
    }
    fprintf(fp,"\n");
    fprintf(fp,"chromasome\tlocation\teval\t       breeder?    mutated bits(1=y/0=n)   generation\trank\n");
    for(count=0;count<POP_SIZE;count++)
    {
        for(count_chrom=0;count_chrom<PRECISION;count_chrom++)    
        {
            fprintf(fp,"%d",(*(p_population+count)).chromasome[count_chrom]);
        }
        fprintf(fp,"\t%lf",(*(p_population+count)).x_value);
        fprintf(fp,"\t%lf",(*(p_population+count)).eval);        
        fprintf(fp,"\t%d\t\t",(*(p_population+count)).breeder);
        for(count_chrom=0;count_chrom<PRECISION;count_chrom++)
        {
            fprintf(fp,"%d",(*(p_population+count)).mutated[count_chrom]);
        }
        fprintf(fp,"\t\t%d",(*(p_population+count)).generation);
        fprintf(fp,"\t%d\n",(*(p_population+count)).rank);
    }
    fprintf(fp,"\nBEST OF GENERATIONS\nchromasome\tx_value\t\teval\t\tgeneration\n");
    for(count=0;count<=(*(p_population)).generation;count++)
    {
        for(count_chrom=0;count_chrom<PRECISION;count_chrom++)    
        {
            fprintf(fp,"%d",best_of[count].chromasome[count_chrom]);
        }
        fprintf(fp,"\t%lf",best_of[count].x_value);
        fprintf(fp,"\t%lf",best_of[count].eval);
        fprintf(fp,"\t%d\n",count);
    }
    fclose(fp);                
}

void clear_pop(struct individual *p_population)
{
    /* clears location,eval,breeder,mutated bits,and rank - also updates generation */
    int count,sub_count;
    for(count=0;count<POP_SIZE;count++)
    {
        (p_population+count)->x_value=0;
        (p_population+count)->eval=0;
        (p_population+count)->breeder=0;
        (p_population+count)->rank=0;
        (p_population+count)->generation+=1;
        for(sub_count=0;sub_count<PRECISION;sub_count++)
        {
            (p_population+count)->mutated[sub_count]=0;
        }
    }
}
            
///* Capable of generating a random sequence with a period >10^18        *
// * Essentially removes all correlations that may exist                 *
// * when calling rand() more than 2^15 times.  It is also               *
// * good for filling up k dim volumes with o(k) points,                 *
// * since rand tends to place points grouped in hyperplanes.            *
// * This algorithm is 2x speed of rand() and should not be              *
// * used for generating large numbers of random bits -                  *
// * there are easier ways...                                            *
// * Call function with *idum a negative integer to intitialize,         *
// * thereafter, do not alter idum between successive deviates           *
// * in a sequence                                                       */    
//double ran2(long *idum)
//{
//    int j;
//    long k;
//    static long idum2=123456789;
//    static long iy=0;
//    static long iv[NTAB];
//    double temp;
//    if(*idum<=0)
//    {
//        if(-(*idum)<1)
//        *idum=1;
//        else
//        *idum=-(*idum);
//        idum2=(*idum);
//        for(j=NTAB+7;j>=0;j--)
//        {
//            k=(*idum)/IQ1;
//            *idum=IA1*(*idum-k*IQ1)-k*IR1;
//            if(*idum<0)
//            *idum+=IM1;
//            if(j<NTAB)
//            iv[j]=*idum;
//        }
//        iy=iv[0];
//    }
//    k=(*idum)/IQ1;
//    *idum=IA1*(*idum-k*IQ1)-k*IR1;
//    if(*idum<0)
//    *idum+=IM1;
//    k=idum2/IQ2;
//    idum2=IA2*(idum2-k*IQ2)-k*IR2;
//    if(*idum<2)
//    idum2+=IM2;
//    j=iy/NDIV;
//    iy=iv[j]-idum2;
//    iv[j]=*idum;
//    if(iy<1)
//    iy+=IMM1;
//    if((temp=AM*iy)>RNMX)
//    return RNMX;
//    else
//    return temp;
//}
    
void  klGeneticAlg_main()

{
    /* some variables so we can run the algorithm for 20 trials */
    int q,trial;
    double results_x[20],results_fx[20],mean_fx=0,best_fx=-1,best_x;
    printf("\nWe need a filename for a report:");
    gets(report_file);
    flushall();
    printf("\nGive me seed <0 to initialize random number generator:");
    scanf("%ld",seed);
    ran2(seed);
    printf("\nGive me new seed for remainder of calls in sequence of random deviates:");
    scanf("%ld",seed);
    for(trial=0;trial<20;trial++){
        
    initialize_pop(population);
    rank(population);
    selection(population);
    breed(new_population);
    mutate(new_population);
    if(trial==1||trial==5||trial==15||trial==19)
    print_report(new_population);
    swap_populations(population);    
    clear_pop(population);
    for(q=0;q<GENERATIONS;q++)
    {
        evaluate_pop(population);
        rank(population);
        selection(population);
        breed(new_population);
        mutate(new_population);
        if(trial==1||trial==5||trial==15||trial==19)
        print_report(new_population);
        swap_populations(population);
        clear_pop(population);
    }
    results_x[trial]=best_of[GENERATIONS-3].x_value;
    results_fx[trial]=best_of[GENERATIONS-3].eval;

    
    /* end of trials loop */   }
    for(trial=0;trial<20;trial++)
    {
        printf("\nresults_x[%d]=%lf",trial,results_x[trial]);
        printf("\tresults_fx[%d]=%lf",trial,results_fx[trial]);
        mean_fx+=results_fx[trial];
        if(results_fx[trial]>=best_fx)
        {
            best_fx=results_fx[trial];
            best_x=results_x[trial];
        }
    }
    printf("\nmean_fx=%lf",mean_fx/20);
    printf("\nbest_fx=%lf",best_fx);
    printf("\nbest_x=%lf",best_x);
        
    
    
}
};

//}
