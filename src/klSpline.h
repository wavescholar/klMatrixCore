 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#include <vector>

struct DPoint
{
    double x;
    double y;
};

typedef std::vector<double> Tdata;

class klLSSpline
{

public:
    // constructors
    klLSSpline();
    klLSSpline( const Boundary *controlPoints, bool isClosed );
    ~klLSSpline();
    
    // create a new spline
    int create( const Boundary *controlPoints, bool isClosed );
    
    // get the time range
    void timeRange( double &start, double &end) const;

    // sample various spline properties based on a list of times
    std::vector<DPoint> *sampleSpline( const std::vector<double> &times ) const;
    std::vector<DPoint> *sampleFirstDeriv( const std::vector<double> &times ) const;
    std::vector<DPoint> *sampleSecondDeriv( const std::vector<double> &times) const;
    std::vector<double> *sampleCurvature( const std::vector<double> &times ) const;


private:
inline void _sample( double ti, DPoint &dp ) const
{
    int sect = static_cast<int>(floor( (ti-3.0)/3.0 ));
    double *xs = _xSections[sect];
    double *ys = _ySections[sect];
    double t = ti - 3.0*sect;
    double t2 = t*t;
    double t3 = t2*t;
    
    dp.x = xs[0] + xs[1]*t + xs[2]*t2 + xs[3]*t3;
    dp.y = ys[0] + ys[1]*t + ys[2]*t2 + ys[3]*t3;

}

inline void _sampleFirstDeriv( double ti, DPoint &dp ) const
{
    int sect = static_cast<int>(floor( (ti-3.0)/3.0 ));
    double *xs = _xSections[sect];
    double *ys = _ySections[sect];
    double t = ti - 3.0*sect;
    double t2 = t*t;
    
    dp.x = xs[1] + 2.0*xs[2]*t + 3.0*xs[3]*t2;
    dp.y = ys[1] + 2.0*ys[2]*t + 3.0*ys[3]*t2;
}

inline void _sampleSecondDeriv( double ti, DPoint &dp ) const
{
    int sect = static_cast<int>(floor( (ti-3.0)/3.0 ));
    double *xs = _xSections[sect];
    double *ys = _ySections[sect];
    double t = ti - 3.0*sect;
    
    dp.x = 2.0*xs[2] + 6.0*xs[3]*t;
    dp.y = 2.0*ys[2] + 6.0*ys[3]*t;
}

inline void _sampleCurvature( double ti, double &k ) const
{
    int sect = static_cast<int>(floor( (ti-3.0)/3.0 ));
    double *xs = _xSections[sect];
    double *ys = _ySections[sect];
    double t = ti - 3.0*sect;
    double t2 = t*t;
    DPoint fd;
    DPoint sd;
    fd.x = xs[1] + 2.0*xs[2]*t + 3.0*xs[3]*t2;
    fd.y = ys[1] + 2.0*ys[2]*t + 3.0*ys[3]*t2;
    sd.x = 2.0*xs[2] + 6.0*xs[3]*t;
    sd.y = 2.0*ys[2] + 6.0*ys[3]*t;

    k = (fd.x*sd.y - fd.y*sd.x)/pow( fd.x*fd.x+fd.y*fd.y, 1.5 );
}

private:
    int _numSections;
    int _timeEnd;
    double **_ySections;
    double **_xSections;
};
    

int klLSSpline::create( const Boundary *controlPoints, bool isClosed )
{
    int blen = controlPoints->x.size();
    // choose the number of points per spline based roughly on
    // perimeter
    _numSections = static_cast<int>(ceil( blen/double(SI_offset) ));
    _timeEnd = blen+SI_offset;
    _xSections= new double*[ _numSections];
    _ySections= new double*[ _numSections];
    
    double yforLSQ[SI_numPts];
    for( int i=0; i<_numSections; ++i )
    {
        for( int j=0; j<SI_numPts; ++j )
        {
            yforLSQ[j] = controlPoints->x[ (j+i*SI_offset)%blen];
        }
        
        _xSections[i] = CubicFitLeastSquares( yforLSQ );

        for( int j=0; j<SI_numPts; ++j )
        {
            yforLSQ[j] = controlPoints->y[ (j+i*SI_offset)%blen];
        }
        _ySections[i] = CubicFitLeastSquares( yforLSQ );
    }

    return 0;
}
    

inline void klLSSpline::timeRange( double &start, double &end) const
{
    start = SI_offset;
    end = _timeEnd;
}

inline std::vector<DPoint>  *klLSSpline::sampleSpline( const std::vector<double> & times ) const
{
    int ts = times.size();
    std::vector<DPoint>  *toRetp = new std::vector<DPoint> ( ts );
    std::vector<DPoint>  &toRet = *toRetp;

    for( int i=0; i<ts; ++i )
    {
        _sample( times[i], toRet[i] );
    }
    return toRetp;
}

inline std::vector<DPoint>  *klLSSpline::sampleFirstDeriv( const std::vector<double> &times ) const
{
    int ts = times.size();
    std::vector<DPoint>  *toRetp = new std::vector<DPoint> ( ts );
    std::vector<DPoint>  &toRet = *toRetp;
    for( int i=0; i<ts; ++i )
    {
        _sampleFirstDeriv( times[i], toRet[i] );
    }
    return toRetp;
}

inline std::vector<DPoint>  *klLSSpline::sampleSecondDeriv( const Tdata &times) const
{
    int ts = times.size();
    std::vector<DPoint>  *toRetp = new std::vector<DPoint> ( ts );
    std::vector<DPoint>  &toRet = *toRetp;
    for( int i=0; i<ts; ++i )
    {
        _sampleSecondDeriv( times[i], toRet[i] );
    }
    return toRetp;
}

inline std::vector<double> *klLSSpline::sampleCurvature( const std::vector<double> &times ) const
{
    int ts = times.size();
    std::vector<double> *toRetp = new std::vector<double>( ts );
    std::vector<double> &toRet = *toRetp;
    for( int i=0; i<ts; ++i )
    {
        _sampleCurvature(times[i], toRet[i] );
    }
    return toRetp;
}
