
#include "expm.h"
#include <stdlib.h>
#include <stdio.h>
void expm(double *x, int n, double *z, precond_type precond_kind);
void expm_eigen(double *x, int n, double *z, double tol);
int klMatrixExponential()
{
	int n=0; int i=0;int j=0;
	double * Xtr;
	precond_type pcType=Ward_2;
	double* X = (double*)malloc(4 * sizeof(double));
	double* Z = (double*)malloc(4* sizeof(double));
	*(X) = 1; 
	*(X+1)=1;
	*(X+2)=1; 
	*(X+3)=1;
	
	expm(X, 2, Z,pcType);
	printf("%f  %f \n",*(Z), *(Z+1));
	printf("%f  %f\n",*(Z+2), *(Z+3));

	expm_eigen(X,2, Z, 0.0001f);
	free(X);
	free(Z);

	n=5;
	X = (double*)malloc(n*n* sizeof(double));

	Z = (double*)malloc(n*n* sizeof(double));

  *(X+0)=-0.2265e+01;  *(X+1)=0.3401e+00;  *(X+2)=0.1605e+00;  *(X+3)=0.3114e+00;  *(X+4)=0.6003e+00;
  *(X+5)=0.3401e+00;  *(X+6)=-0.2390e+01;  *(X+7)=0.8465e+00;  *(X+8)=0.3564e+00;  *(X+9)=0.8367e+00;
  *(X+10)=0.1605e+00;  *(X+11)=0.8465e+00;  *(X+12)=-0.2290e+01;  *(X+13)=0.5922e+00;  *(X+14)=0.5924e+00;
  *(X+15)=0.3114e+00;  *(X+16)=0.3564e+00;  *(X+17)=0.5922e+00;  *(X+18)=-0.1982e+01;  *(X+19)=0.3205e+00;
  *(X+20)=0.6003e+00;  *(X+21)=  0.8367e+00;  *(X+22)=  0.5924e+00;  *(X+23)=  0.3205e+00;  *(X+24)= -0.1648e+01;

	Xtr = (double*)malloc(n*n* sizeof(double));
	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
			*(Xtr +i*n+j) = 2.0 * ( *(X +j*n+i) ) ;
	}
	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
			printf("%f ",*(Xtr+i*n+j));
		printf("\n");
	}

  	expm(Xtr, n, Z,pcType);
	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
			printf("%f ",*(Z+i*n+j));
		printf("\n");
	}
	//We get
	//0.171047 0.128277 0.111044 0.102334 0.177268
	//0.128277 0.253775 0.206422 0.149529 0.258081
	//0.111044 0.206422 0.246522 0.161491 0.231250
	//0.102334 0.149529 0.161491 0.223013 0.173741
	//0.177268 0.258081 0.231250 0.173741 0.398087

	//ExpoKit
	//0.9994D-01  0.1385D+00  0.1304D+00  0.1082D+00  0.1775D+00
	//0.1385D+00  0.2125D+00  0.2014D+00  0.1626D+00  0.2648D+00
	//0.1304D+00  0.2014D+00  0.1953D+00  0.1582D+00  0.2502D+00
	//0.1082D+00  0.1626D+00  0.1582D+00  0.1388D+00  0.2020D+00
	//0.1775D+00  0.2648D+00  0.2502D+00  0.2020D+00  0.3404D+00

	expm_eigen(X,n, Z, 0.0001f);
	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
			printf("%f ",*(Z+i+j));
		printf("\n");
	}
	free(X);
	free(Z);

	n= 20;
	Xtr = (double*)malloc(n*n* sizeof(double));
	Z = (double*)malloc(n*n* sizeof(double));
	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
		{
			if(i!=j)
				*(Xtr +i*n+j) = 0.0 ;
			else
			{
				*(Xtr +i*n+j) = i;//exp((double) i/1000.0);
				//printf("%f  %f   %f \n",i,(double) i/1000,exp((double) i/1000.0));
			}
		}
	}
	expm(Xtr, n, Z,pcType);

	for( i =0;i<n;i++)
	{
		for( j=0;j<n;j++)
			printf("%f ",*(Z+i*n+j));
		printf("\n");
	}

	return 0;
}

