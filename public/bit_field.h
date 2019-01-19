#ifndef BIT_FIELD_H
#define BIT_FIELD_H

/* ---------------------------- Bits / Flags ------------------------------------ */

/* To manipulate a bit at a given (bit) position */
#define _BitM(bit) (1 << (bit))
#define _notB(bit)  0

/*   To manipulate bits with a bit mask.  */
#define SETB(reg,mask) {(reg) |= (mask);}
#define CLRB(reg,mask) {(reg) &= ~(mask);}

#define BSET(reg,mask) ((reg) & (mask))
#define BCLR(reg,mask) (!((reg) & (mask)))

#define ALL_SET(v,m) (((v)&(m))==(m))
#define ANY_SET(v,m) (((v)&(m))!=0)
#define NOT_SET(v,m) (((v)&(m))==0)

#endif // BIT_FIELD_H
