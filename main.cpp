#include "tgaimage.h"
#include "formain.h"
#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <cstdlib> 
#include <utility>

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const TGAColor color = red;
const int sizeim = 5000; // size of image 5000*5000
#define PI 3.14159265 


float dot2(triangle2 t1, triangle2 t2)		 //the scalar product of float vectors
{
	return ((t1.geta())*(t2.geta()) + (t1.getb())*(t2.getb()) + (t1.getc())*(t2.getc()));
}
matrix4na4no2 obratnaya(matrix4na4no2 M)	 //inverse matrix
{
	int i, j, k;
	float A[4][4];
	{
		A[0][0] = M.geta();
		A[0][1] = M.getb();
		A[0][2] = M.getc();
		A[0][3] = M.getp();

		A[1][0] = M.getd();
		A[1][1] = M.gete();
		A[1][2] = M.getf();
		A[1][3] = M.getq();

		A[2][0] = M.geti();
		A[2][1] = M.getj();
		A[2][2] = M.getk();
		A[2][3] = M.getr();

		A[3][0] = M.getl();
		A[3][1] = M.getm();
		A[3][2] = M.getn();
		A[3][3] = M.gets();
	}

	float E[4][4];
	// filling of the identity matrix
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			if (i == j) E[i][j] = 1;
			else E[i][j] = 0;
		}
	}

	
	for (k = 0; k<4; k++) {
		for (j = k + 1; j<4; j++) {
			A[k][j] = A[k][j] / A[k][k]; 
		}
		for (j = 0; j<4; j++) {
			E[k][j] = E[k][j] / A[k][k];  
		}
		A[k][k] = A[k][k] / A[k][k];
												
		if (k>0) {
			for (i = 0; i<k; i++) {   
				for (j = 0; j<4; j++) {
					E[i][j] = E[i][j] - E[k][j] * A[i][k];
				}
				for (j = 4 - 1; j >= k; j--) {
					A[i][j] = A[i][j] - A[k][j] * A[i][k];
				}
			}
		}
		for (i = k + 1; i<4; i++) {    
			for (j = 0; j<4; j++) {
				E[i][j] = E[i][j] - E[k][j] * A[i][k];
			}
			for (j = 4 - 1; j >= k; j--) {
				A[i][j] = A[i][j] - A[k][j] * A[i][k];
			}
		}
	}

	M = matrix4na4no2(E[0][0], E[0][1], E[0][2], E[0][3],
					  E[1][0], E[1][1], E[1][2], E[1][3],
					  E[2][0], E[2][1], E[2][2], E[2][3],
					  E[3][0], E[3][1], E[3][2], E[3][3]);

	return M;
}

int main(int argc, char** argv) {
	TGAImage image(sizeim, sizeim, TGAImage::RGB);
	TGAImage image2;
	char a;
	int n = 0, k = 0, nn = 0;
	double x, y, z, iz, w, wx, wy, wz ;
	int ix, iy;
	int x1, y1, x2, y2, x3, y3;
	float nx1, nx2, nx3, ny1, ny2, ny3, nz1, nz2, nz3;
	float z1, z2, z3;
	float z1c, z2c, z3c;
	float z1l, z2l, z3l;
	float u1, u2, u3;
	float v1, v2, v3;
	char p1, p2, p3, p4, p5, p6;
	int k1, k2, k3, k4, k5, k6, k7, k8, k9;
	int lix, liy, liz; //light pos
	int tx, ty, tz;
	int Cx, Cy, Cz;
	
	float fi;
	bool setflag;
	int campos = 0, choise2 = 0;
	float plmax=2;
	matrix4na4 matrix;
	matrix4na4no2 M, Ml, Mobr;
	matrix3na3 Mmal;
	Vershin ishod, polych;
	vector <Vershin> spisokVersh;
	vector <Normal> spisokNormal;
	vector <triangle> spisokTri;
	vector <triangle> spisokTriNorm;
	vector <triangle> spisoktexture;
	vector <pair<float, float>> coordtextyr;
	int *zbuffer = new int[sizeim*sizeim];// x + y*sizeim;
	int *zbuffer2 = new int[sizeim*sizeim];
	int newsize = sizeim / 2;
	int Ax = newsize, Ay = newsize, Az = 0;
	int choise;
	float xpers, ypers, zpers;
	for (int j = 0; j < sizeim*sizeim; j++)
	{
		zbuffer[j] = -1000000;
	}
	for (int j = 0; j < sizeim*sizeim; j++)
	{
		zbuffer2[j] = -1000000;
	}
	cout << "1-simple render, 2 - fill, 3 - Zbuffer, 4 - perspective distortion, 5 - 3D, 6 - Guro, 7 - Phong, 8 - Texture, 9 - Camera transform, 10 - Shadow ";
	cin >> choise;
	if (choise == 4)
	{
	cout << "Enter C (0,0,C) coordinate of camera (more than 5000, for clarity): ";
	cin >> campos;
	}
	if (choise == 5)
	{
		cout << "1-Parallel transfer, 2-Rotation:" << endl;
	
		cin >> choise2;
		
		
		
		if (choise2 == 1)
		{
			cout << "Enter dX, dY and dZ :" << endl;
			cin >> tx >> ty >> tz;
		}
		else
		{
			cout << "Enter the angle of rotation fi:" << endl;
			cin >> fi;
		}
	}
	if (choise == 6)
	{
		cout << "Enter the light pos (x, y, z): ";
		cin >> lix >>  liy >> liz;
	}
	if (choise == 7)
	{
		cout << "Enter the light pos (x, y, z): ";
		cin >> lix >> liy >> liz;
	}
	if (choise == 8)
	{
		cout << "Enter the light pos (x, y, z): ";
		cin >> lix >> liy >> liz;
		image2.read_tga_file("african_head_diffuse.tga");
		image2.flip_vertically();
	}
	if (choise == 9)
	{
		triangle2 xvec(1,0,0), yvec(0, 1, 0), zvec(0, 0, 1);
		
		cout << "Enter the Camera pos (x, y, z): ";
		cin >> Cx >> Cy >> Cz;
		cout << "Enter the light pos (x, y, z): ";
		cin >> lix >> liy >> liz;
		
		triangle2 CA(Cx - Ax, Cy - Ay, Cz - Az); //the direction vector of the camera
		
		triangle2 W = Norma(CA);
		triangle2 U = Norma(VecProizved(yvec, W));
		triangle2 V = Norma(VecProizved(W, U));
		triangle2 T = Norma(triangle2( -Cx, - Cy, - Cz));
		M  = matrix4na4no2(dot2(xvec, U), dot2(xvec, V), dot2(xvec, W), 0,
						   dot2(yvec, U), dot2(yvec, V), dot2(yvec, W), 0,
			               dot2(zvec, U), dot2(zvec, V), dot2(zvec, W), 0,
						   dot2(T, U),	  dot2(T, V),    dot2(T, W),  1);
		Mmal = matrix3na3(dot2(xvec, U), dot2(xvec, V), dot2(xvec, W),
						  dot2(yvec, U), dot2(yvec, V), dot2(yvec, W),
						  dot2(zvec, U), dot2(zvec, V), dot2(zvec, W));
		
	}
	if (choise == 10)
	{
		triangle2 xvec(1, 0, 0), yvec(0, 1, 0), zvec(0, 0, 1);

		cout << "Enter the Camera pos (x, y, z): ";
		cin >> Cx >> Cy >> Cz;
		cout << "Enter the light pos (x, y, z): ";
		cin >> lix >> liy >> liz;
	
		triangle2 CA(Cx - Ax, Cy - Ay, Cz - Az);
		triangle2 W = Norma(CA);
		triangle2 U = Norma(VecProizved(yvec, W));
		triangle2 V = Norma(VecProizved(W, U));
		triangle2 T = Norma(triangle2(-Cx, -Cy, -Cz));
		M = matrix4na4no2(dot2(xvec, U), dot2(xvec, V), dot2(xvec, W), 0,
			dot2(yvec, U), dot2(yvec, V), dot2(yvec, W), 0,
			dot2(zvec, U), dot2(zvec, V), dot2(zvec, W), 0,
			dot2(T, U), dot2(T, V), dot2(T, W), 1);
		Mmal = matrix3na3(dot2(xvec, U), dot2(xvec, V), dot2(xvec, W),
			dot2(yvec, U), dot2(yvec, V), dot2(yvec, W),
			dot2(zvec, U), dot2(zvec, V), dot2(zvec, W));

		triangle2 LA(lix - Ax, liy - Ay, liz - Az);
		triangle2 Wl = Norma(LA);
		triangle2 Ul = Norma(VecProizved(yvec, Wl));
		triangle2 Vl = Norma(VecProizved(Wl, Ul));
		triangle2 Tl = Norma(triangle2(-lix, -liy, -liz));
		Ml = matrix4na4no2(dot2(xvec, Ul), dot2(xvec, Vl), dot2(xvec, Wl), 0,
			dot2(yvec, Ul), dot2(yvec, Vl), dot2(yvec, Wl), 0,
			dot2(zvec, Ul), dot2(zvec, Vl), dot2(zvec, Wl), 0,
			dot2(Tl, Ul), dot2(Tl, Vl), dot2(Tl, Wl), 1);
		
	}

	ifstream in;
	in.open("african_head.obj");
	while (!in.eof()) //read from file
{
	in>>a;
	if (a == 'v') { // vertex  coordinates
		in >> a;

		if (a == '0'|| a == '-') {
			in >> x >> y >> z; 
			n++;
			if (a == '-') { x = -x; }
			ix = floor(x*newsize) + newsize;
			iy = floor(y*newsize) + newsize;
			iz = z;
			Vershin ver(n,ix,iy,iz);
			spisokVersh.push_back(ver);
		}
		else
		{		// normal coordinates
			if (a == 'n') {
				in >> a;
				if (a == '0' || a == '-') {
					in >> x >> y >> z;
					nn++;
					if (a == '-') { x = -x; }
					Normal nor(nn, x, y, z);
					spisokNormal.push_back(nor);
				}

			}
			else
			{		// texture coordinates
				if (a == 't') {
					in >> a;
					if (a == '0' ) {
						in >> x >> y;
						nn++;
						coordtextyr.push_back(make_pair(x, y));
					}
				
				}
			}
		}

	}


	if (a == 'f') {
			in >> k1;
			in >> p1;
			in >> k2;
			in >> p2;
			in >> k3;

			in >> k4;
			in >> p3;
			in >> k5;
			in >> p4;
			in >> k6;

			in >> k7;
			in >> p5;
			in >> k8;
			in >> p6;
			in >> k9;
			k++;
			triangle tri(k1, k4, k7);
			spisokTri.push_back(tri);

			triangle tri2(k3, k6, k9);
			spisokTriNorm.push_back(tri2);

			triangle text(k2, k5, k6);
			spisoktexture.push_back(text);
	}
}

	
	TGAColor random = TGAColor(255, 255, 255, 255);
	for (int i = 1; i < k; ++i)		//for each triangle get
	{
		// 1st vertex
		x1 = spisokVersh[spisokTri[i].geta()-1].getx();
		y1 = spisokVersh[spisokTri[i].geta()-1].gety();
		z1 = spisokVersh[spisokTri[i].geta()-1].getz();
		// 1st normal 
		nx1 = spisokNormal[spisokTriNorm[i].geta() - 1].getx();
		ny1 = spisokNormal[spisokTriNorm[i].geta() - 1].gety();
		nz1 = spisokNormal[spisokTriNorm[i].geta() - 1].getz();
		// texture coordinates for 1st  vertex 
		u1 = coordtextyr[spisoktexture[i].geta() - 1].first;
		v1 = coordtextyr[spisoktexture[i].geta() - 1].second;
		// 2nd vertex  
		x2 = spisokVersh[spisokTri[i].getb()-1].getx();
		y2 = spisokVersh[spisokTri[i].getb()-1].gety();
		z2 = spisokVersh[spisokTri[i].getb()-1].getz();
		// 2nd normal 
		nx2 = spisokNormal[spisokTriNorm[i].getb() - 1].getx();
		ny2 = spisokNormal[spisokTriNorm[i].getb() - 1].gety();
		nz2 = spisokNormal[spisokTriNorm[i].getb() - 1].getz();
		// texture coordinates for 2nd vertex  
		u2 = coordtextyr[spisoktexture[i].getb() - 1].first;
		v2 = coordtextyr[spisoktexture[i].getb() - 1].second;
		// 3rd vertex  
		x3 = spisokVersh[spisokTri[i].getc()-1].getx();
		y3 = spisokVersh[spisokTri[i].getc()-1].gety();
		z3 = spisokVersh[spisokTri[i].getc()-1].getz();
		// 3rd normal 
		nx3 = spisokNormal[spisokTriNorm[i].getc() - 1].getx();
		ny3 = spisokNormal[spisokTriNorm[i].getc() - 1].gety();
		nz3 = spisokNormal[spisokTriNorm[i].getc() - 1].getz();
		//texture coordinates for 3rd vertex  
		u3 = coordtextyr[spisoktexture[i].getc() - 1].first;
		v3 = coordtextyr[spisoktexture[i].getc() - 1].second;

		switch (choise)
		{
		case 1: //rendering a grid of triangles
		{
			image.BRline(x1, y1, x2, y2, white);
			image.BRline(x1, y1, x3, y3, white);
			image.BRline(x3, y3, x2, y2, white);
		}
		break;
		case 2:
		{	//fill triangles with random colors
			image.BRline(x1, y1, x2, y2, white);
			image.BRline(x1, y1, x3, y3, white);
			image.BRline(x3, y3, x2, y2, white);
			random = TGAColor(rand() % 255, rand() % 255, rand() % 255, 100 );
			image.fill(x1, y1, x2, y2, x3, y3, random);
		}
		break;
		case 3:
		{    //building a Z-buffer
			setflag = true;
			image.zbuftriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, color, zbuffer, setflag);
		}
		break;
		case 4:	//building a perspective distortion
		{
			z1 = floor(z1*newsize) + newsize;
			z2 = floor(z2*newsize) + newsize;
			z3 = floor(z3*newsize) + newsize;
			

			xpers = x1 / (1 - (z1 / campos));
			ypers = y1 / (1 - (z1 / campos));
			z1 = z1 / (1 - (z1 / campos));
			x1 = floor(xpers);
			y1 = floor(ypers);
			xpers = x2 / (1 - (z2 / campos));
			ypers = y2 / (1 - (z2 / campos));
			z2 = z2 / (1 - (z2 / campos));
			x2 = floor(xpers);
			y2 = floor(ypers);
			xpers = x3 / (1 - (z3 / campos));
			ypers = y3 / (1 - (z3 / campos));
			z3 = z3 / (1 - (z3 / campos));
			x3 = floor(xpers);
			y3 = floor(ypers);


			z1 = (z1 - newsize)/ newsize;
			z2 = (z2 - newsize) / newsize;
			z3 = (z3 - newsize) / newsize;

			image.BRline(x1, y1, x2, y2, white);
			image.BRline(x1, y1, x3, y3, white);
			image.BRline(x3, y3, x2, y2, white);
			
		}
		break;
		case 5: //3D distortion
		{

			z1 = floor(z1*newsize) + newsize;
			z2 = floor(z2*newsize) + newsize;
			z3 = floor(z3*newsize) + newsize;

			switch (campos)
			{
			case 1: // matrix for parallel transfer
			{
			 matrix = matrix4na4(1, 0, 0, 0, 
								 0, 1, 0, 0,
								 0, 0, 1, 0,
								 tx, ty, tz, 1);
			
			}
			break;

			case 2:  //matrix for rotation around the y-axis
			{
				matrix = matrix4na4(cos(fi * PI / 180), 0, sin(fi * PI / 180), 0,
									0, 1, 0, 0,
									-sin(fi * PI / 180), 0, cos(fi * PI / 180), 0,
									0, 0,  0, 1);
			}
			break;
			}
		
			ishod = Vershin(x1, y1, z1, 1);
			polych = proizved(ishod, matrix);

			wx = polych.getn();
			wy = polych.getx();
			wz = polych.gety();
			w = polych.getz(); //transition from homogeneous coordinates
			x1 = floor((wx) / (w));
			y1 = floor((wy) / (w));
			z1 = floor((wz) / (w));

			ishod = Vershin(x2, y2, z2, 1);
			polych = proizved(ishod, matrix);

			wx = polych.getn();
			wy = polych.getx();
			wz = polych.gety();
			w = polych.getz(); //transition from homogeneous coordinates
			x2 = floor((wx) / (w));
			y2 = floor((wy) / (w));
			z2 = floor((wz) / (w));

			ishod = Vershin(x3, y3, z3, 1);
			polych = proizved(ishod, matrix);

			wx = polych.getn();
			wy = polych.getx();
			wz = polych.gety();
			w = polych.getz(); //transition from homogeneous coordinates
			x3 = floor((wx) / (w));
			y3 = floor((wy) / (w));
			z3 = floor((wz) / (w));

			z1 = (z1 - newsize) / newsize;
			z2 = (z2 - newsize) / newsize;
			z3 = (z3 - newsize) / newsize;

			image.BRline(x1, y1, x2, y2, white);
			image.BRline(x1, y1, x3, y3, white);
			image.BRline(x3, y3, x2, y2, white);
		}
		break;
		case 6:
		{
			setflag = false;
			image.guro(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx1, ny1, nz1, nx2, ny2, nz2, nx3, ny3, nz3, lix, liy, liz, newsize, zbuffer, &plmax);
		}
		break;
		case 7:
		{
			setflag = false;
			image.phong(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx1, ny1, nz1, nx2, ny2, nz2, nx3, ny3, nz3, lix, liy, liz, newsize, zbuffer, &plmax);
		}
		break;
		case 8:
		{
			
			setflag = false;
			image.texture(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx1, ny1, nz1, nx2, ny2, nz2, nx3, ny3, nz3, lix, liy, liz, newsize, zbuffer, &plmax, u1, v1, u2, v2, u3, v3, image2);
		}
		break;
		case 9: //Camera transform
		{

			z1 = floor(z1*newsize);
			z2 = floor(z2*newsize);
			z3 = floor(z3*newsize);

			Normal  newcor1 = proizved2(Normal(x1, y1, z1, 1), M);
			Normal  newcor2 = proizved2(Normal(x2, y2, z2, 1), M);
			Normal  newcor3 = proizved2(Normal(x3, y3, z3, 1), M);
			

			triangle2 newcornorm1 = proizved3(Mmal, triangle2(nx1, ny1, nz1));
			triangle2 newcornorm2 = proizved3(Mmal, triangle2(nx2, ny2, nz2));
			triangle2 newcornorm3 = proizved3(Mmal, triangle2(nx3, ny3, nz3));

			z1 = ((newcor1.gety() ) / newsize);
			z2 = ((newcor2.gety() ) / newsize);
			z3 = ((newcor3.gety() ) / newsize);

			setflag = false;
			
			if (Cz > 0)
			{
				image.guro(newcor1.getn(), newcor1.getx(), z1, newcor2.getn(), newcor2.getx(), z2, newcor3.getn(), newcor3.getx(), z3, newcornorm1.geta(), newcornorm1.getb(), newcornorm1.getc(), newcornorm2.geta(), newcornorm2.getb(), newcornorm2.getc(), newcornorm3.geta(), newcornorm3.getb(), newcornorm3.getc(), lix, liy, liz, newsize, zbuffer, &plmax);
			}
			else
			{
				image.guro(newcor1.getn() + sizeim, newcor1.getx(), z1, newcor2.getn() + sizeim, newcor2.getx(), z2, newcor3.getn() + sizeim, newcor3.getx(), z3, newcornorm1.geta(), newcornorm1.getb(), newcornorm1.getc(), newcornorm2.geta(), newcornorm2.getb(), newcornorm2.getc(), newcornorm3.geta(), newcornorm3.getb(), newcornorm3.getc(), lix, liy, liz, newsize, zbuffer, &plmax);
			}
		}

		break;
		case 10: // new z-buftr
		{

			z1 = floor(z1*newsize);
			z2 = floor(z2*newsize);
			z3 = floor(z3*newsize);

			

			Normal  newcor1l = proizved2(Normal(x1, y1, z1, 1), Ml);
			Normal  newcor2l = proizved2(Normal(x2, y2, z2, 1), Ml);
			Normal  newcor3l = proizved2(Normal(x3, y3, z3, 1), Ml);

			

			z1l = ((newcor1l.gety()) / newsize);
			z2l = ((newcor2l.gety()) / newsize);
			z3l = ((newcor3l.gety()) / newsize);
			

			
			setflag = false; 
			image.zbuftriangle(newcor1l.getn(), newcor1l.getx(), z1l, newcor2l.getn(), newcor2l.getx(), z2l, newcor3l.getn(), newcor3l.getx(), z3l, color, zbuffer2, setflag);

			
		}
		break;
		}
	}
	if (choise == 10)
	{

		for (int i = 1; i < k; ++i)//head with shadow
		{
			x1 = spisokVersh[spisokTri[i].geta() - 1].getx();
			y1 = spisokVersh[spisokTri[i].geta() - 1].gety();
			z1 = spisokVersh[spisokTri[i].geta() - 1].getz();

			nx1 = spisokNormal[spisokTriNorm[i].geta() - 1].getx();
			ny1 = spisokNormal[spisokTriNorm[i].geta() - 1].gety();
			nz1 = spisokNormal[spisokTriNorm[i].geta() - 1].getz();

			u1 = coordtextyr[spisoktexture[i].geta() - 1].first;
			v1 = coordtextyr[spisoktexture[i].geta() - 1].second;

			x2 = spisokVersh[spisokTri[i].getb() - 1].getx();
			y2 = spisokVersh[spisokTri[i].getb() - 1].gety();
			z2 = spisokVersh[spisokTri[i].getb() - 1].getz();

			nx2 = spisokNormal[spisokTriNorm[i].getb() - 1].getx();
			ny2 = spisokNormal[spisokTriNorm[i].getb() - 1].gety();
			nz2 = spisokNormal[spisokTriNorm[i].getb() - 1].getz();

			u2 = coordtextyr[spisoktexture[i].getb() - 1].first;
			v2 = coordtextyr[spisoktexture[i].getb() - 1].second;

			x3 = spisokVersh[spisokTri[i].getc() - 1].getx();
			y3 = spisokVersh[spisokTri[i].getc() - 1].gety();
			z3 = spisokVersh[spisokTri[i].getc() - 1].getz();

			nx3 = spisokNormal[spisokTriNorm[i].getc() - 1].getx();
			ny3 = spisokNormal[spisokTriNorm[i].getc() - 1].gety();
			nz3 = spisokNormal[spisokTriNorm[i].getc() - 1].getz();

			u3 = coordtextyr[spisoktexture[i].getc() - 1].first;
			v3 = coordtextyr[spisoktexture[i].getc() - 1].second;

			z1 = floor(z1*newsize);
			z2 = floor(z2*newsize);
			z3 = floor(z3*newsize);

			Normal  newcor1 = proizved2(Normal(x1, y1, z1, 1), M);
			Normal  newcor2 = proizved2(Normal(x2, y2, z2, 1), M);
			Normal  newcor3 = proizved2(Normal(x3, y3, z3, 1), M);
			triangle2 newcornorm1 = proizved3(Mmal, triangle2(nx1, ny1, nz1));
			triangle2 newcornorm2 = proizved3(Mmal, triangle2(nx2, ny2, nz2));
			triangle2 newcornorm3 = proizved3(Mmal, triangle2(nx3, ny3, nz3));

			z1 = ((newcor1.gety()) / newsize);
			z2 = ((newcor2.gety()) / newsize);
			z3 = ((newcor3.gety()) / newsize);

			Mobr = obratnaya(M);

			image.superguro(
				newcor1.getn(), newcor1.getx(), z1,
				newcor2.getn(), newcor2.getx(), z2,
				newcor3.getn(), newcor3.getx(), z3,
				newcornorm1.geta(), newcornorm1.getb(), newcornorm1.getc(),
				newcornorm2.geta(), newcornorm2.getb(), newcornorm2.getc(),
				newcornorm3.geta(), newcornorm3.getb(), newcornorm3.getc(),
				lix, liy, liz,
				newsize, zbuffer, Mobr, Ml, Mmal, zbuffer2, &plmax);
		}

	}

	
	
	image.flip_vertically(); 
	image.write_tga_file("output.tga");
	
	return 0;
}

