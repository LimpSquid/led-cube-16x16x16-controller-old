#ifndef BIT_MANIPULATION_H
#define	BIT_MANIPULATION_H

#define BIT_SHIFT(x)        (1 << x)
#define BIT_SHIFT_INV(x)    (~BIT_SHIFT(x))  

inline unsigned char __attribute__((always_inline)) compute_lsb_pos(const unsigned int value);

#endif	/* BIT_MANIPULATION_H */

