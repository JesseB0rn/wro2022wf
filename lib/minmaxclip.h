/**
 * This define returns the smaller of the two numbers
 */
#define min2(a, b) (a < b ? a : b)

/**
 * This define returns the smallest of the three numbers
 */
#define min3(a, b, c) (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c)

/**
 * This function returns the bigger of the two numbers
 */
#define max2(a, b) (a > b ? a : b)

/**
 * This function returns the biggest of the three numbers
 */
#define max3(a, b, c) (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c)

/**
 * Returns x if it is between min and max. If outside the range,
 * it returns min or max.
 */
#define clip(a, b, c) min2(c, max2(b, a))
