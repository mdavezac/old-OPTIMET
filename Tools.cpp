#include "Tools.h"

#include "gsl/gsl_sf_legendre.h"
#include "gsl/gsl_sf_gamma.h"
#include <cmath>
#include <assert.h>

using std::sin;
using std::cos;
using std::acos;
using std::atan2;
using std::sqrt;
using std::pow;
using std::abs;

Tools::Tools()
{
	//
}

Tools::~Tools()
{
	//
}

double Tools::findDistance(Spherical<double> point1, Spherical<double> point2)
{
	return Tools::findDistance(toCartesian(point1), toCartesian(point2));
}

double Tools::findDistance(Cartesian<double> point1, Cartesian<double> point2)
{
	return sqrt(pow((point2.x - point1.x), 2.0) +
			pow((point2.y - point1.y), 2.0) +
			pow((point2.z - point1.z), 2.0));
}

Cartesian<double> Tools::toCartesian(Spherical<double> point)
{
	return Cartesian<double>(point.rrr * sin(point.the) * cos(point.phi),
			point.rrr * sin(point.the) * sin(point.phi),
			point.rrr * cos(point.the));
}

Spherical<double> Tools::toSpherical(Cartesian<double> point)
{
	double r_l = sqrt(point.x * point.x + point.y * point.y + point.z * point.z);
	if(r_l > 0.0)
	{
		return Spherical<double>(r_l,
				acos(point.z / r_l),
				atan2(point.y, point.x));
	}
	return Spherical<double>(0.0, 0.0, 0.0);
}

bool Tools::equalDoubles(double A, double B)
{
	// Hard coded to 2
	int maxUlps = 2;

	// Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    int aInt = *(int*)&A;
    // Make aInt lexicographically ordered as a twos-complement int
    if (aInt < 0)
    	aInt = 0x80000000 - aInt;
    // Make bInt lexicographically ordered as a twos-complement int
    int bInt = *(int*)&B;
    if (bInt < 0)
    	bInt = 0x80000000 - bInt;
    int intDiff = abs(aInt - bInt);
    if (intDiff <= maxUlps)
    	return true;
    return false;
}

long Tools::toCompound(long n, long m)
{
	return n*(n+1)-m-1;
}

long Tools::toFirstIt(long p)
{
	return (long) sqrt(p + 1.0);
}

long Tools::toSecondIt(long p)
{
	long n = (long) sqrt(p + 1.0);
	return -(p+1)+n*(n+1);
}

long Tools::iteratorMax(long n)
{
	return n*n+2*n;
}

void Tools::makeUnitMatrix(long size_, complex<double>** I_)
{
	long i,j;
	for(i=0; i<size_; i++)
		for(j=0; j<size_; j++)
		{
			I_[i][j] = (i == j) ? complex<double>(1.0, 0.0) : complex<double> (0.0, 0.0);
		}
}

void Tools::pushToMatrix(complex<double>** T_small_, long rows_, long columns_,
		complex<double>** T_large_, long row_index_, long column_index_)
{
	long i,j;
	for(i=0; i<rows_; i++)
		for(j=0; j<columns_; j++)
			T_large_[i + row_index_][j + column_index_] = T_small_[i][j];
}

double Tools::getLegendre(double argument_, int orderN_, int orderM_)
{
	if(orderM_ < 0)
	{
		return pow(-1.0, abs(orderM_)) *
			(gsl_sf_fact(orderN_-abs(orderM_))) /
			(gsl_sf_fact(orderN_+abs(orderM_))) *
			gsl_sf_legendre_Plm(orderN_, abs(orderM_), argument_);
	}

	return gsl_sf_legendre_Plm(orderN_, orderM_, argument_);
}

complex<double> **Tools::Get_2D_c_double(int i_x, int i_y)
{
	int i;
	complex<double> **d_B;
	d_B=new complex<double>*[i_x];
	for(i=0;i<i_x;i++) {
		d_B[i]=new complex<double>[i_y];
	}
	return d_B;
}

complex<double> ****Tools::Get_4D_c_double(int t,  int x, int y, int z)
{
        int i,j,k;
        complex<double> ****b;
        b=new complex<double>***[t];
        for(i=0;i<t;i++) {
            b[i]=new complex<double>**[x];
        }
        for(i=0;i<t;i++) {           // rows
            for(j=0;j<x;j++) {      // then coulmns
                b[i][j]=new complex<double>*[y];
        }   }
        for(i=0;i<t;i++) {
            for(j=0;j<x;j++) {
                for(k=0;k<y;k++) {
                    b[i][j][k]=new complex<double>[z];
        }   }   }
        return b;
 }

SphericalP<double> Tools::toSphericalP(Spherical<double> point)
{
	return SphericalP<double>(point.rrr * sin(point.the) * cos(point.phi),
			point.rrr * sin(point.the) * sin(point.phi),
			point.rrr * cos(point.the));
}

SphericalP<complex<double> > Tools::toSphericalP(Spherical<complex<double> > point)
{
	return SphericalP<complex<double> >(point.rrr * sin(point.the) * cos(point.phi),
			point.rrr * sin(point.the) * sin(point.phi),
			point.rrr * cos(point.the));
}

SphericalP<complex<double> > Tools::toProjection(
		Spherical<double> point, SphericalP<complex<double> > vector)
{
	return SphericalP<complex<double> >( sin(point.the) * cos(point.phi) * vector.rrr + cos(point.the) * cos(point.phi) * vector.the - sin(point.phi) * vector.phi,
			sin(point.the) * sin(point.phi) * vector.rrr + cos(point.the) * sin(point.phi) * vector.the + cos(point.phi) * vector.phi,
			cos(point.the) * vector.rrr - sin(point.the) * vector.the);
}

SphericalP<complex<double> > Tools::fromProjection(Spherical<double> point, SphericalP<complex<double> > vector)
{
	return SphericalP<complex<double> >(
			sin(point.the) * cos(point.phi) * vector.rrr + sin(point.the) * sin(point.phi) * vector.the + cos(point.the) * vector.phi,
			cos(point.the) * cos(point.phi) * vector.rrr + cos(point.the) * sin(point.phi) * vector.the - sin(point.the) * vector.phi,
			cos(point.phi) * vector.the - sin(point.phi) * vector.rrr);
}

Spherical<double> Tools::toPoint(Spherical<double> R, Spherical<double> P)
{
	Cartesian<double> R_cart = toCartesian(R);
	Cartesian<double> P_cart = toCartesian(P);

	return toSpherical(R_cart-P_cart);
}

void Tools::Pol2Cart(double rho_, double theta_, double &x_, double &y_)
{
	x_ = rho_ * cos(theta_);
	y_ = rho_ * sin(theta_);
}
