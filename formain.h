#ifndef __FORMAIN_H__
#define __FORMAIN_H__
#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include<utility>
using namespace std;

class Vershin {
public:
	explicit Vershin(int n = 0, int a = 0, int b = 0, float c = 0) : n(n), a(a), b(b), c(c) {}
	~Vershin() {}
	Vershin(Vershin const & otherVersh) : n(otherVersh.n), a(otherVersh.a), b(otherVersh.b), c(otherVersh.c) {}
	Vershin & operator= (Vershin const & otherVersh);
	int getn() const { return  n; }
	int getx() const { return  a; }
	int gety() const { return  b; }
	float getz() const { return  c; }
	//void swapdip() { swap(a, b); } //поменять местами если a>b
private:
	int n;
	int a;
	int b;
	float c;
};
class Normal {
public:
	explicit Normal(float n = 0, float a = 0, float b = 0, float c = 0) : n(n), a(a), b(b), c(c) {}
	~Normal() {}
	Normal(Normal const & otherNormal) : n(otherNormal.n), a(otherNormal.a), b(otherNormal.b), c(otherNormal.c) {}
	Normal & operator= (Normal const & otherNormal);
	float getn() const { return  n; }
	float getx() const { return  a; }
	float gety() const { return  b; }
	float getz() const { return  c; }
private:
	float n;
	float a;
	float b;
	float c;
};
class triangle {
public:
	explicit triangle( int k1 = 0, int k4 = 0, int k7 = 0) : a(k1), b(k4), c(k7) {}
	~triangle() {}
	triangle(triangle const & othertri) :  a(othertri.a), b(othertri.b), c(othertri.c) {}
	triangle & operator= (triangle const & othertri);
	int geta() const { return  a; }
	int getb() const { return  b; }
	int getc() const { return  c; }
	
private:
	int a;
	int b;
	int c;
};

class triangle2 {
public:
	explicit triangle2(float k1 = 0, float k4 = 0, float k7 = 0) : a(k1), b(k4), c(k7) {}
	~triangle2() {}
	triangle2(triangle2 const & othertri) : a(othertri.a), b(othertri.b), c(othertri.c) {}
	triangle2 & operator= (triangle2 const & othertri);
	float geta() const { return  a; }
	float getb() const { return  b; }
	float getc() const { return  c; }
private:
	float a;
	float b;
	float c;
};

class matrix4na4 {
public:
	explicit matrix4na4(float k1 = 0, float k2 = 0, int k3 = 0, float k4 = 0, float k5 = 0, float  k6 = 0, float k7 = 0, int k8 = 0, float k9 = 0, float k10 = 0, float k11 = 0,
		int k12 = 0, int k13 = 0, int k14 = 0, int k15 = 0, int k16 = 0) : a(k1), b(k2), c(k3), p(k4), d(k5), e(k6), f(k7), q(k8), i(k9), j(k10), k(k11), r(k12), l(k13), m(k14), n(k15), s(k16) {}
	~matrix4na4() {}
	matrix4na4(matrix4na4 const & othermatrix) : a(othermatrix.a), b(othermatrix.b), c(othermatrix.c), p(othermatrix.p), d(othermatrix.d), e(othermatrix.e), f(othermatrix.f), q(othermatrix.q), i(othermatrix.i), j(othermatrix.j), k(othermatrix.k), r(othermatrix.r), l(othermatrix.l), m(othermatrix.m), n(othermatrix.n), s(othermatrix.s) {}
	matrix4na4 & operator= (matrix4na4 const & othermatrix);

	float geta() const { return  a; }
	float getb() const { return  b; }
	float getc() const { return  c; }
	int getp() const { return  p; }
	float getd() const { return  d; }
	float gete() const { return  e; }
	float getf() const { return  f; }
	int getq() const { return  q; }
	float geti() const { return i; }
	float getj() const { return  j; }
	float getk() const { return  k; }
	float getr() const { return  r; }
	int getl() const { return  l; }
	int getm() const { return  m; }
	int getn() const { return  n; }
	int gets() const { return  s; }
	
private:
	float a;
	float b;
	float c;
	int p;
	float d;
	float e;
	float f;
	int q;
	int l;
	int m;
	int n;
	int s;
	float i;
	float j;
	float k;
	int r;
};

class matrix4na4no2 {
public:
	explicit matrix4na4no2(float k1 = 0, float k2 = 0, float k3 = 0, float k4 = 0, float k5 = 0, float  k6 = 0, float k7 = 0, float k8 = 0, float k9 = 0, float k10 = 0, float k11 = 0,
		float k12 = 0, float k13 = 0, float k14 = 0, float k15 = 0, float k16 = 0) : a(k1), b(k2), c(k3), p(k4), d(k5), e(k6), f(k7), q(k8), i(k9), j(k10), k(k11), r(k12), l(k13), m(k14), n(k15), s(k16) {}
	~matrix4na4no2() {}
	matrix4na4no2(matrix4na4no2 const & othermatrix) : a(othermatrix.a), b(othermatrix.b), c(othermatrix.c), p(othermatrix.p), d(othermatrix.d), e(othermatrix.e), f(othermatrix.f), q(othermatrix.q), i(othermatrix.i), j(othermatrix.j), k(othermatrix.k), r(othermatrix.r), l(othermatrix.l), m(othermatrix.m), n(othermatrix.n), s(othermatrix.s) {}
	matrix4na4no2 & operator= (matrix4na4no2 const & othermatrix);

	float geta() const { return  a; }
	float getb() const { return  b; }
	float getc() const { return  c; }
	float getp() const { return  p; }
	float getd() const { return  d; }
	float gete() const { return  e; }
	float getf() const { return  f; }
	float getq() const { return  q; }
	float geti() const { return i; }
	float getj() const { return  j; }
	float getk() const { return  k; }
	float getr() const { return  r; }
	float getl() const { return  l; }
	float getm() const { return  m; }
	float getn() const { return  n; }
	float gets() const { return  s; }
	
private:
	float a;
	float b;
	float c;
	float p;
	float d;
	float e;
	float f;
	float q;
	float l;
	float m;
	float n;
	float s;
	float i;
	float j;
	float k;
	float r;
};

Vershin proizved(Vershin const & tri, matrix4na4 const & matrix);

Normal proizved2(Normal const & tri, matrix4na4no2 const & matrix);

triangle2 VecProizved(triangle2 const & tri, triangle2 const & tri2);

triangle2 Norma(triangle2 const & tri);

class matrix3na3 {
public:
	explicit matrix3na3(float k1 = 0, float k2 = 0, float k3 = 0, float k4 = 0, float k5 = 0, float k6 = 0, float k7 = 0, float k8 = 0, float k9 = 0) : a(k1), b(k2), p(k3), d(k4), e(k5), q(k6), l(k7), m(k8), s(k9) {}
	~matrix3na3() {}
	matrix3na3(matrix3na3 const & othermatrix) : a(othermatrix.a), b(othermatrix.b), p(othermatrix.p), d(othermatrix.d), e(othermatrix.e), q(othermatrix.q), l(othermatrix.l), m(othermatrix.m), s(othermatrix.s) {}
	matrix3na3 & operator= (matrix3na3 const & othermatrix);

	float geta() const { return  a; }
	float getb() const { return  b; }
	float getp() const { return  p; }
	float getd() const { return  d; }
	float gete() const { return  e; }
	float getq() const { return  q; }
	float getl() const { return  l; }
	float getm() const { return  m; }
	float gets() const { return  s; }
	
private:
	float a;
	float b;
	float p;
	float d;
	float e;
	float q;
	float l;
	float m;
	float s;
};

triangle2 proizved3(matrix3na3 const & matrix, triangle2 const & tri);

#endif
