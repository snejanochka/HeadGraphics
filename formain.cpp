#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "formain.h"
#include <math.h> 

using namespace std;

Vershin &  Vershin:: operator= (Vershin const & otherVersh)
{
	if (this == &otherVersh) return *this;
	n = otherVersh.n;
	a = otherVersh.a;
	b = otherVersh.b;
	c = otherVersh.c;
	return *this;
}
Normal &   Normal:: operator= (Normal const & otherNormal)
{
	if (this == &otherNormal) return *this;
	n = otherNormal.n;
	a = otherNormal.a;
	b = otherNormal.b;
	c = otherNormal.c;
	return *this;
}
triangle &  triangle:: operator= (triangle const & othertri)
{
	if (this == &othertri) return *this;
	a = othertri.a;
	b = othertri.b;
	c = othertri.c;
	return *this;
}
triangle2 &  triangle2:: operator= (triangle2 const & othertri)
{
	if (this == &othertri) return *this;
	a = othertri.a;
	b = othertri.b;
	c = othertri.c;
	return *this;
}
matrix4na4no2 &  matrix4na4no2:: operator= (matrix4na4no2 const & othertri)
{
	if (this == &othertri) return *this;
	a = othertri.a;
	b = othertri.b;
	c = othertri.c;
	p = othertri.p;
	d = othertri.d;
	e = othertri.e;
	f = othertri.f;
	q = othertri.q;
	i = othertri.i;
	j = othertri.j;
	k = othertri.k;
	l = othertri.l;
	m = othertri.m;
	n = othertri.n;
	s = othertri.s;


	return *this;
}

matrix4na4 &  matrix4na4:: operator= (matrix4na4 const & othertri)
{
	if (this == &othertri) return *this;
	a = othertri.a;
	b = othertri.b;
	c = othertri.c;
	p = othertri.p;
	d = othertri.d;
	e = othertri.e;
	f = othertri.f;
	q = othertri.q;
	i = othertri.i;
	j = othertri.j;
	k = othertri.k;
	l = othertri.l;
	m = othertri.m;
	n = othertri.n;
	s = othertri.s;


	return *this;
}
matrix3na3 &  matrix3na3:: operator= (matrix3na3 const & othertri)
{
	if (this == &othertri) return *this;
	a = othertri.a;
	b = othertri.b;
	p = othertri.p;
	d = othertri.d;
	e = othertri.e;
	q = othertri.q;
	l = othertri.l;
	m = othertri.m;
	s = othertri.s;


	return *this;
}

triangle2 proizved3(matrix3na3 const & matrix, triangle2 const & tri) {
	float xn;
	float yn;
	float wn;
	xn = (tri.geta()*matrix.geta()) + (tri.getb()*matrix.getb()) + (tri.getc()*matrix.getp());
	yn = (tri.geta()*matrix.getd()) + (tri.getb()*matrix.gete()) + (tri.getc()*matrix.getq());
	wn = (tri.geta()*matrix.getl()) + (tri.getb()*matrix.getm()) + (tri.getc()*matrix.gets());
	return triangle2(xn, yn, wn);

}
triangle2 Norma(triangle2 const & tri) {
	float xn;
	float yn;
	float zn;
	float dlinna = (1 / sqrt(tri.geta()*tri.geta() + tri.getb()*tri.getb() + tri.getc()*tri.getc()));
	xn = (tri.geta() * dlinna);
	yn = (tri.getb() * dlinna);
	zn = (tri.getc() * dlinna);
	return triangle2(xn, yn, zn);

}

triangle2 VecProizved(triangle2 const & tri, triangle2 const & tri2) {
	float xn;
	float yn;
	float zn;

	xn = (tri.getb()*tri2.getc()) - (tri.getc()*tri2.getb());
	yn = -(tri.geta()*tri2.getc()) + (tri.getc()*tri2.geta());
	zn = (tri.geta()*tri2.getb()) - (tri.getb()*tri2.geta());
	return triangle2(xn, yn, zn);

}
Normal proizved2(Normal const & tri, matrix4na4no2 const & matrix) {
	float xn;
	float yn;
	float wn;
	float zn;
	xn = (tri.getn()*matrix.geta()) + (tri.getx()*matrix.getd()) + (tri.gety()*matrix.geti()) + (tri.getz()*matrix.getl());
	yn = (tri.getn()*matrix.getb()) + (tri.getx()*matrix.gete()) + (tri.gety()*matrix.getj()) + (tri.getz()*matrix.getm());
	wn = (tri.getn()*matrix.getc()) + (tri.getx()*matrix.getf()) + (tri.gety()*matrix.getk()) + (tri.getz()*matrix.getn());
	zn = (tri.getn()*matrix.getp()) + (tri.getx()*matrix.getq()) + (tri.gety()*matrix.getr()) + (tri.getz()*matrix.gets());
	return Normal(xn, yn, wn, zn);

}
Vershin proizved(Vershin const & tri, matrix4na4 const & matrix) {
	int xn;
	int yn;
	int wn;
	float zn;
	xn = (tri.getn()*matrix.geta()) + (tri.getx()*matrix.getd()) + (tri.gety()*matrix.geti()) + (tri.getz()*matrix.getl());
	yn = (tri.getn()*matrix.getb()) + (tri.getx()*matrix.gete()) + (tri.gety()*matrix.getj()) + (tri.getz()*matrix.getm());
	wn = (tri.getn()*matrix.getc()) + (tri.getx()*matrix.getf()) + (tri.gety()*matrix.getk()) + (tri.getz()*matrix.getn());
	zn = (tri.getn()*matrix.getp()) + (tri.getx()*matrix.getq()) + (tri.gety()*matrix.getr()) + (tri.getz()*matrix.gets());
	return Vershin(xn, yn, wn, zn);

}