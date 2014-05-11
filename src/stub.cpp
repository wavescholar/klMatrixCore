#include "kl_matrix.h"

void testKlBinaryOI()
{
	{
		__int64 GBWorthOfDoubles = __int64(1073741824LL/sizeof(double));
		__int64 rzG = __int64 (std::sqrt ((double)GBWorthOfDoubles)) ;

		klMatrix<double> klmd (2*rzG,2*rzG);
		klmd =1;

		klBinaryIO::WriteWinx64( klmd, "test.klmd");

		__int64 rows,cols;
		klBinaryIO::QueryWinx64("test.klmd",rows,cols);

		klMatrix<double> klmdMat(rows,cols);

		klBinaryIO::MatReadWinx64("test.klmd",klmdMat);

		klmdMat +=1;

	}

	{
		__int64 GBWorthOfDoubles = __int64(1073741824LL/sizeof(double));
		__int64 rzG = __int64 (std::sqrt ((double)GBWorthOfDoubles)) ;

		klVector<double> klvd (2*rzG);
		klvd =1;

		klBinaryIO::WriteWinx64( klvd, "test.klvd");

		__int64 rows,cols;
		klBinaryIO::QueryWinx64("test.klvd",rows,cols);

		if (rows!=0)
			throw "klBinaryIO::QueryWinx64(fileName ,rows,cols) returned non zero rows for vector"; 
		klVector<double> readklmd(cols);
		
		klBinaryIO::VecReadWinx64("test.klvd",readklmd);
	}


}