#ifndef CPP_POINT__
#define CPP_POINT__

namespace robo_utils {

	struct point {
	  public:
		int x;
		int y;
	  public:
		point(int y, int x);
		point(point& p);
		~point();
	  public:
		point& operator +=(const point& q);
		point& operator -=(const point& q);
		point& operator +=(int v);
		point& operator -=(int v);

		point operator +() const;
		point operator -() const;

		friend point operator +(point p, const point& q);
		friend point operator -(point p, const point& q);
		friend point operator +(point p, int v);
		friend point operator -(point p, int v);
	};

	point operator +(point p, const point& q);
	point operator -(point p, const point& q);
	point operator +(point p, int v);
	point operator -(point p, int v);
	bool operator ==(const point& p, const point& other);
	bool operator !=(const point& p, const point& other);

}

#endif
