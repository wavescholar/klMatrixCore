#include <vector>

struct DPoint
{
    double x;
    double y;
};

typedef std::vector<DPoint> Sdata;
typedef std::vector<double> Tdata;

// 
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
    Sdata *sampleSpline( const Tdata &times ) const;
    Sdata *sampleFirstDeriv( const Tdata &times ) const;
    Sdata *sampleSecondDeriv( const Tdata &times) const;
    Tdata *sampleCurvature( const Tdata &times ) const;

// TODO change to use our standard Matrix Library.
// whenever it is decided on.  I think it may require a change
// in compiler, this is simple enough I'll do it by hand
// and change once we've officially changed.
private:
    // these should all be inline.
    // eventually
// will be much faster when we integrate the matrix library
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
