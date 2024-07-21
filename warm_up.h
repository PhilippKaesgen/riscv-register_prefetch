#ifndef WARM_UP_H
#define WARM_UP_H

#define CAT(A,B) NAME_HELPER(A,B)
#define NAME_HELPER(A,B) A ## B

/* prefetch data with intention to read only
 *
 */
#define warm_up_rd(start, num_bytes) warm_up(start, num_bytes, load)

/* prefetch data with intention to write
 *
 */
#define warm_up_wr(start, num_bytes) warm_up(start, num_bytes, store)

#define mask_lower(idx)\
  (idx < 63UL ? ((1UL<<(idx+1UL))-1UL) : ~0UL)

#define mask_upper(idx)\
  (idx < 64UL ? ~((1UL<<idx)-1UL) : 0UL)

#define mask(start,end)\
  mask_upper(start) & mask_lower(end)

#define bits(vec,start,end)\
  !!(vec & mask(start,end))

#define bit(vec,idx)\
  !!(vec & (1UL<<idx))

#define warm_up(start, num_bytes, func) \
if (num_bytes > 0) {\
  unsigned long preload = (unsigned long) start,\
                    end = (unsigned long) start + num_bytes;\
  /* handle non-8B aligned start addresses  */\
  if (bit(preload,0))\
      CAT(func,1)(preload);\
  if (bits(num_bytes, 1,64) && bit(preload,1)) \
      CAT(func,2)(preload);\
  if (bits(num_bytes, 2,64) && bit(preload, 2)) \
      CAT(func,4)(preload);\
  /* use load double as much as possible */\
  while (bits(num_bytes, 3,64) && (preload < (end & ~7UL)))\
      CAT(func,8)(preload);   \
   /* handle trailing bytes */\
  if (preload < end) {\
    unsigned long end1 = end-1UL;\
    if (bit(end1, 2))\
        CAT(func,4)(preload);\
    if (bit(end1, 1))\
        CAT(func,2)(preload);\
    if (bit(end1, 0))\
        CAT(func,1)(preload);\
  }\
}


#define load1(ADDR)\
    asm volatile("lb x0, 0(%0)"::"r"(ADDR++));

#define load2(ADDR)\
    asm volatile("lh x0, 0(%0)"::"r"(ADDR+=2));

#define load4(ADDR)\
    asm volatile("lw x0, 0(%0)"::"r"(ADDR+=4));

#define load8(ADDR)\
    asm volatile("ld x0, 0(%0)"::"r"(ADDR+=8));

#define store1(ADDR){\
        unsigned char tmp; \
        asm volatile("lb %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        asm volatile("sb %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        ADDR++;\
  }
#define store2(ADDR){\
        unsigned short tmp; \
        asm volatile("lh %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        asm volatile("sh %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        ADDR+=2;\
  }
#define store4(ADDR){\
        unsigned tmp; \
        asm volatile("lw %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        asm volatile("sw %0, 0(%1)":"=r"(tmp):"r"(ADDR));\
        ADDR+=4;\
  }
#define store8(ADDR){\
        unsigned long tmp; \
        asm volatile("ld %0, 0(%1)":"=r"(tmp):"r"(ADDR)); \
        asm volatile("sd %0, 0(%1)":"=r"(tmp):"r"(ADDR)); \
        ADDR+=8;\
  }

#endif  // WARM_UP_H
