#ifndef CPP_POINT__
#define CPP_POINT__

namespace robo_utils {

/**
 * Represents a 2D point of integers
 */
struct point {
public:
	/**
	 * the first coordinate of the point
	 */
	int x;
	/**
	 * the second coordinate of the point
	 */
	int y;
public:
	/**
	 * Setup a new point
	 *
	 * @param[in] y the y of the point
	 * @param[in] x the x of the point
	 */
	point(int y, int x);
	/**
	 * Copy constructor of ::point
	 *
	 * @param[in] p the point to copy
	 */
	point(point& p);
	/**
	 * Deallocate the point
	 */
	~point();
public:
	/**
	 * Update the old point by summing eaach point coordinate
	 *
	 * @param[in] q the point whose coordiantes we need to add to the current one
	 */
	point& operator +=(const point& q);
	/**
	 * Update the old point by substracting eaach point coordinate
	 *
	 * @param[in] q the point whose coordiantes we need to subtract to the current one
	 */
	point& operator -=(const point& q);
	/**
	 * Update the old point by summing to each coordinate the given value
	 *
	 * @param[in] v the amount to add to each coordinate of the point
	 */
	point& operator +=(int v);
	/**
	 * Update the old point by summing to each coordinate the given value
	 *
	 * @param[in] v the amount to add to each coordinate of the point
	 */
	point& operator -=(int v);

	/**
	 * @return a copy of the given point
	 */
	point operator +() const;
	/**
	 * @return a copy containing the negative of the given point
	 */
	point operator -() const;

	friend point operator +(point p, const point& q);
	friend point operator -(point p, const point& q);
	friend point operator +(point p, int v);
	friend point operator -(point p, int v);
};

/**
 * Sum 2 points together
 *
 * @param[in] p the first point
 * @param[in] q the second point
 * @return the sum of the 2 points
 */
point operator +(point p, const point& q);
/**
 * Substract 2 points together
 *
 * @param[in] p the first point
 * @param[in] q the second point
 * @return <tt> p - q </tt>
 */
point operator -(point p, const point& q);
/**
 * incrase every coordinate of a point with a given amount
 *
 * @param[in] p the point involved
 * @param[in] v the amount to add to each coordinate of \c p
 * @return a copy of p agumented with \c v
 */
point operator +(point p, int v);
/**
 * decrease every coordinate of a point with a given amount
 *
 * @param[in] p the point involved
 * @param[in] v the amount to subtract to each coordinate of \c p
 * @return a copy of p agumented with \c v
 */
point operator -(point p, int v);
/**
 * Check if 2 points are equal
 *
 * \note
 * 2 points are equal iif the share the same coordinates
 *
 * @param[in] p the first point
 * @param[in] other the second point
 * @return \c true if they are equal, \c false otherwise;
 */
bool operator ==(const point& p, const point& other);
/**
 * Check if 2 points are not equal
 *
 * \note
 * 2 points are equal iif the share the same coordinates
 *
 * @param[in] p the first point
 * @param[in] other the second point
 * @return \c true if they are not equal, \c false otherwise;
 */
bool operator !=(const point& p, const point& other);

}

#endif
