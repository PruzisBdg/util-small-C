/* -------------------------------------------------------------------------
|
|                             Utility Library - macros
|
|  These macros are included in 'util.h'; but have their own file.
|                    "Because we are so many"
|
----------------------------------------------------------------------------- */

#ifndef MAP_MACROS_H
#define MAP_MACROS_H

// CAT() force expansion of 'x','y' before they are pasted. Double XCAT() forces that expansion even
// if 'x', 'y' are them selves macros. (This is an old K&R trick)
#define CAT(x,y)  x##y
#define XCAT(x,y)  CAT(x,y)

/* --------------------------------- Counts Args --------------------------------------------- */

// Counts zero or more parameters, but each must resolve to a C type.
// If the parms are tokens use _NumArgs() below.
#define _NumParms(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

// Counts 1-32 tokens the argument list. Unlike _NumParms() the parameters can be any token except ',' or '#"
// (which are for the preprocessor). An empty list still counts as 1.
#define _NumArgs(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,\
                 _33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,\
                 _65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90, N,...) N
// It's that cheesy reverse-trick...
// ...A reverse-list of numbers (below) get shoved rightwards past 'N' (above) by an arg-list which we insert in front (of the numbers).
// e.g 11 args pushes the '11' under 'N'. That '11' is then returned by PP_ARG_N() (above) as the arg-count.
#define PP_RSEQ_N() \
   90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65, \
   64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,\
   32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

/* -------------------------------------- Maps ----------------------------------------------- */

// e.g _Map(op, a,b,c,d)  -> op4(a,b,c,d) -> op(a), op(b), op(c) op(d)
#define _Map(_f, ...) XCAT(_func, _NumArgs(__VA_ARGS__))(_f, __VA_ARGS__)

#define _func1( _f, a) _f(a)
#define _func2( _f, a,b) _f(a), _f(b)
#define _func3( _f, a,b,c) _f(a), _f(b), _f(c)
#define _func4( _f, a,b,c,d) _f(a), _f(b), _f(c), _f(d)
#define _func5( _f, a,b,c,d,e) _f(a), _f(b), _f(c), _f(d), _f(e)
#define _func6( _f, a,b,c,d,e,f) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f)
#define _func7( _f, a,b,c,d,e,f,g) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g)
#define _func8( _f, a,b,c,d,e,f,g,h) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h)
#define _func9( _f, a,b,c,d,e,f,g,h,i) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i)
#define _func10(_f, a,b,c,d,e,f,g,h,i,j) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j)
#define _func11(_f, a,b,c,d,e,f,g,h,i,j,k) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k)
#define _func12(_f, a,b,c,d,e,f,g,h,i,j,k,l) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l)
#define _func13(_f, a,b,c,d,e,f,g,h,i,j,k,l,m) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m)
#define _func14(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n)
#define _func15(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o)
#define _func16(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p)
#define _func17(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q)
#define _func18(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r)
#define _func19(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s)
#define _func20(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t)
#define _func21(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u)
#define _func22(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u), _f(v)
#define _func23(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u), _f(v), _f(w)
#define _func24(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u), _f(v), _f(w), _f(x)
#define _func25(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u), _f(v), _f(w), _f(x), _f(y)
#define _func26(_f, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) _f(a), _f(b), _f(c), _f(d), _f(e), _f(f), _f(g), _f(h), _f(i), _f(j), _f(k), _f(l), _f(m), _f(n), _f(o), _f(p), _f(q), _f(r), _f(s), _f(t), _f(u), _f(v), _f(w), _f(x), _f(y), _f(z)

// 2 args per function.
#define _MapPairs(_f, ...) XCAT(_func2_, _NumArgs(__VA_ARGS__))(_f, __VA_ARGS__)

#define _func2_2( _f, a0,a1)    _f(a0,a1)
#define _func2_4( _f, a0,a1,b0,b1)    _f(a0,a1), _f(b0,b1)
#define _func2_6( _f, a0,a1,b0,b1,c0,c1)    _f(a0,a1), _f(b0,b1), _f(c0,c1)
#define _func2_8( _f, a0,a1,b0,b1,c0,c1,d0,d1)    _f(a0,a1), _f(b0,b1), _f(c0,c1), _f(d0,d1)
#define _func2_10(_f, a0,a1,b0,b1,c0,c1,d0,d1,e0,e1)    _f(a0,a1), _f(b0,b1), _f(c0,c1), _f(d0,d1), f(e0,e1)
#define _func2_12(_f, a0,a1,b0,b1,c0,c1,d0,d1,e0,e1,f0,f1)    _f(a0,a1), _f(b0,b1), _f(c0,c1), _f(d0,d1), f(e0,e1), f(f0,f1)

// 'p0' of the macro is passed to every call as it's 1st arg
#define _Map1stRem(_f, p0, ...) XCAT(_funcp, _NumArgs(__VA_ARGS__))(_f, p0, __VA_ARGS__)
#define _funcp1( _f, p0, a)  _f(p0,a)
#define _funcp2( _f, p0, a,b)  _f(p0,a), _f(p0,b)
#define _funcp3( _f, p0, a,b,c)  _f(p0,a), _f(p0,b), _f(p0,c)
#define _funcp4( _f, p0, a,b,c,d)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d)
#define _funcp5( _f, p0, a,b,c,d,e)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e)
#define _funcp6( _f, p0, a,b,c,d,e,f)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e), _f(p0,f)
#define _funcp7( _f, p0, a,b,c,d,e,f,g)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e), _f(p0,f), _f(p0,g)
#define _funcp8( _f, p0, a,b,c,d,e,f,g,h)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e), _f(p0,f), _f(p0,g), _f(p0,h)
#define _funcp9( _f, p0, a,b,c,d,e,f,g,h,i)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e), _f(p0,f), _f(p0,g), _f(p0,h), _f(p0,i)
#define _funcp10(_f, p0, a,b,c,d,e,f,g,h,i,j)  _f(p0,a), _f(p0,b), _f(p0,c), _f(p0,d), _f(p0,e), _f(p0,f), _f(p0,g), _f(p0,h), _f(p0,i), _f(p0,j)

// 'p0' of the macro is passed to every call as it's first arg. Then the arguments in pairs
#define _Map1stRem2(_f, p0, ...) XCAT(_funcp2a, _NumArgs(__VA_ARGS__))(_f, p0, __VA_ARGS__)
#define _funcp2a2( _f, p0, a,b)  _f(p0,a,b)
#define _funcp2a4( _f, p0, a,b,c,d)  _f(p0,a,b), _f(p0,c,d)
#define _funcp2a6( _f, p0, a,b,c,d,e,f)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f)
#define _funcp2a8( _f, p0, a,b,c,d,e,f,g,h)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h)
#define _funcp2a10(_f, p0, a,b,c,d,e,f,g,h,i,j)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j)
#define _funcp2a12(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l)
#define _funcp2a14(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n)
#define _funcp2a16(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p)
#define _funcp2a18(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r)
#define _funcp2a20(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t)
#define _funcp2a22(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v)
#define _funcp2a24(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v),_f(p0,w,x)
#define _funcp2a26(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v),_f(p0,w,x),_f(p0,y,z)
#define _funcp2a28(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,bb)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v),_f(p0,w,x),_f(p0,y,z),_f(p0,aa,bb)
#define _funcp2a30(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,bb,cc,dd)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v),_f(p0,w,x),_f(p0,y,z),_f(p0,aa,bb),_f(p0,cc,dd)
#define _funcp2a32(_f, p0, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,bb,cc,dd,ee,ff)  _f(p0,a,b), _f(p0,c,d), _f(p0,e,f), _f(p0,g,h), _f(p0,i,j), _f(p0,k,l),_f(p0,m,n),_f(p0,o,p),_f(p0,q,r),_f(p0,s,t),_f(p0,u,v),_f(p0,w,x),_f(p0,y,z),_f(p0,aa,bb),_f(p0,cc,dd),_f(p0,ee,ff)


// e.g _Infix(+, a,b,c,d)  -> _infix4(+, a,b,c,d) -> a+b+c+d
#define _Infix(_op, ...) XCAT(_infix, _NumArgs(__VA_ARGS__))(_op, __VA_ARGS__)

#define _infix2( op, a,b) a op b
#define _infix3( op, a,b,c) a op b op c
#define _infix4( op, a,b,c,d) a op b op c op d
#define _infix5( op, a,b,c,d,e) a op b op c op d op e
#define _infix6( op, a,b,c,d,e,f) a op b op c op d op e op f
#define _infix7( op, a,b,c,d,e,f,g) a op b op c op d op e op f op g
#define _infix8( op, a,b,c,d,e,f,g,h) a op b op c op d op e op f op g op h
#define _infix9( op, a,b,c,d,e,f,g,h,i) a op b op c op d op e op f op g op h op i
#define _infix10(op, a,b,c,d,e,f,g,h,i,j) a op b op c op d op e op f op g op h op i op j
#define _infix11(op, a,b,c,d,e,f,g,h,i,j,k) a op b op c op d op e op f op g op h op i op j op k
#define _infix12(op, a,b,c,d,e,f,g,h,i,j,k,l) a op b op c op d op e op f op g op h op i op j op k op l
#define _infix13(op, a,b,c,d,e,f,g,h,i,j,k,l,m) a op b op c op d op e op f op g op h op i op j op k op l op m
#define _infix14(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n) a op b op c op d op e op f op g op h op i op j op k op l op m op n
#define _infix15(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o
#define _infix16(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p
#define _infix17(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q
#define _infix18(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r
#define _infix19(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s
#define _infix20(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t
#define _infix21(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u
#define _infix22(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v
#define _infix23(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w
#define _infix24(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x
#define _infix25(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y
#define _infix26(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y op z

#define _Prefix(_op, ...) XCAT(_prefix, _NumArgs(__VA_ARGS__))(_op, __VA_ARGS__)

#define _prefix1( op, a) op a
#define _prefix2( op, a,b) op a op b
#define _prefix3( op, a,b,c) op a op b op c
#define _prefix4( op, a,b,c,d) op a op b op c op d
#define _prefix5( op, a,b,c,d,e) op a op b op c op d op e
#define _prefix6( op, a,b,c,d,e,f) op a op b op c op d op e op f
#define _prefix7( op, a,b,c,d,e,f,g) op a op b op c op d op e op f op g
#define _prefix8( op, a,b,c,d,e,f,g,h) op a op b op c op d op e op f op g op h
#define _prefix9( op, a,b,c,d,e,f,g,h,i) op a op b op c op d op e op f op g op h op i
#define _prefix10(op, a,b,c,d,e,f,g,h,i,j) op a op b op c op d op e op f op g op h op i op j
#define _prefix11(op, a,b,c,d,e,f,g,h,i,j,k) op a op b op c op d op e op f op g op h op i op j op k
#define _prefix12(op, a,b,c,d,e,f,g,h,i,j,k,l) op a op b op c op d op e op f op g op h op i op j op k op l
#define _prefix13(op, a,b,c,d,e,f,g,h,i,j,k,l,m) op a op b op c op d op e op f op g op h op i op j op k op l op m
#define _prefix14(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n) op a op b op c op d op e op f op g op h op i op j op k op l op m op n
#define _prefix15(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o
#define _prefix16(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p
#define _prefix17(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q
#define _prefix18(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r
#define _prefix19(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s
#define _prefix20(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t
#define _prefix21(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u
#define _prefix22(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v
#define _prefix23(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w
#define _prefix24(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x
#define _prefix25(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y
#define _prefix26(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) op a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y op z

#define _Postfix(_op, ...) XCAT(_postfix, _NumArgs(__VA_ARGS__))(_op, __VA_ARGS__)

#define _postfix1( op, a) a op
#define _postfix2( op, a,b) a op b op
#define _postfix3( op, a,b,c) a op b op c op
#define _postfix4( op, a,b,c,d) a op b op c op d op
#define _postfix5( op, a,b,c,d,e) a op b op c op d op e op
#define _postfix6( op, a,b,c,d,e,f) a op b op c op d op e op f op
#define _postfix7( op, a,b,c,d,e,f,g) a op b op c op d op e op f op g op
#define _postfix8( op, a,b,c,d,e,f,g,h) a op b op c op d op e op f op g op h op
#define _postfix9( op, a,b,c,d,e,f,g,h,i) a op b op c op d op e op f op g op h op i op
#define _postfix10(op, a,b,c,d,e,f,g,h,i,j) a op b op c op d op e op f op g op h op i op j op
#define _postfix11(op, a,b,c,d,e,f,g,h,i,j,k) a op b op c op d op e op f op g op h op i op j op k op
#define _postfix12(op, a,b,c,d,e,f,g,h,i,j,k,l) a op b op c op d op e op f op g op h op i op j op k op l op
#define _postfix13(op, a,b,c,d,e,f,g,h,i,j,k,l,m) a op b op c op d op e op f op g op h op i op j op k op l op m op
#define _postfix14(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n) a op b op c op d op e op f op g op h op i op j op k op l op m op n op
#define _postfix15(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op
#define _postfix16(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op
#define _postfix17(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op
#define _postfix18(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op
#define _postfix19(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op
#define _postfix20(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op
#define _postfix21(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op
#define _postfix22(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op
#define _postfix23(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op
#define _postfix24(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op
#define _postfix25(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y op
#define _postfix26(op, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) a op b op c op d op e op f op g op h op i op j op k op l op m op n op o op p op q op r op s op t op u op v op w op x op y op z op

#endif // MAP_MACROS_H

// ===================================================  EOF ===========================================================

