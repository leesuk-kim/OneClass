/**
\brief incomplete gamma and beta function and distribution
\details
Cited from numerical recipies ASC, 3rd edithion

\author Leesuk Kim, lktime@skku.edu
*/

#include <math.h>
#include <limits>
#include <algorithm>

double gammln(const double xx);

class Gauleg18 {
public:
	static const int ngau = 18;
	static const double y[18];
	static const double w[18];
};

class Gamma : public Gauleg18 {
private :
	static const int ASWITCH=100;
	static const double EPS;
	static const double FPMIN;
	double gln;

public:

	double gammp(const double a, const double x);

	double gammq(const double a, const double x);

	double gser(const double a, const double x);

	double gcf(const double a, const double x);

	double gammpapprox(double a, double x, int psig);
	double invgammp(double p, double a);

};

class Beta : Gauleg18 {
private:
	static const int SWITCH=3000;
	static const double EPS, FPMIN;

public:
	static double betai(const double a, const double b, const double x);

	static double betacf(const double a, const double b, const double x);

	static double betaiapprox(double a, double b, double x);

	static double invbetai(double p, double a, double b);

};


class Gammadist : Gamma {
private:
	double alph, bet, fac;
public:
	inline Gammadist(double aalph, double bbet = 1.) : alph(aalph), bet(bbet) {
		if (alph <= 0. || bet <= 0.) throw("bad alph,bet in Gammadist");
		fac = alph*log(bet)-gammln(alph);
	}

	inline double p(double x) {
		if (x <= 0.) throw("bad x in Gammadist");
		return exp(-bet*x+(alph-1.)*log(x)+fac);
	}

	inline double cdf(double x) {
		if (x < 0.) throw("bad x in Gammadist");
		return gammp(alph,bet*x);
	}

	inline double invcdf(double p) {
		if (p < 0. || p >= 1.) throw("bad p in Gammadist");
		return invgammp(p,alph)/bet;
	}
};

class Betadist : Beta {
private:
	double alph, bet, fac;

public:
	inline Betadist(double aalph, double bbet) : alph(aalph), bet(bbet) {
		if (alph <= 0. || bet <= 0.) throw("bad alph,bet in Betadist");
		fac = gammln(alph+bet)-gammln(alph)-gammln(bet);
	}

	inline double p(double x) {
		if (x <= 0. || x >= 1.) throw("bad x in Betadist");
		return exp((alph-1.)*log(x)+(bet-1.)*log(1.-x)+fac);
	}

	inline double cdf(double x) {
		if (x < 0. || x > 1.) throw("bad x in Betadist");
		return betai(alph,bet,x);
	}

	inline double invcdf(double p) {
		if (p < 0. || p > 1.) throw("bad p in Betadist");
		return invbetai(p,alph,bet);
	}
};