#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"
#include <math.h> 
#include <vector>
#include <algorithm>
#include "formain.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const int sizeim = 5000;

TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {
}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp) {
	unsigned long nbytes = width*height*bytespp;
	data = new unsigned char[nbytes];
	memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage &img) {
	width = img.width;
	height = img.height;
	bytespp = img.bytespp;
	unsigned long nbytes = width*height*bytespp;
	data = new unsigned char[nbytes];
	memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() {
	if (data) delete [] data;
}

TGAImage & TGAImage::operator =(const TGAImage &img) {
	if (this != &img) {
		if (data) delete [] data;
		width  = img.width;
		height = img.height;
		bytespp = img.bytespp;
		unsigned long nbytes = width*height*bytespp;
		data = new unsigned char[nbytes];
		memcpy(data, img.data, nbytes);
	}
	return *this;
}

bool TGAImage::read_tga_file(const char *filename) {
	if (data) delete [] data;
	data = NULL;
	std::ifstream in;
	in.open (filename, std::ios::binary);
	if (!in.is_open()) {
		std::cerr << "can't open file " << filename << "\n";
		in.close();
		return false;
	}
	TGA_Header header;
	in.read((char *)&header, sizeof(header));
	if (!in.good()) {
		in.close();
		std::cerr << "an error occured while reading the header\n";
		return false;
	}
	width   = header.width;
	height  = header.height;
	bytespp = header.bitsperpixel>>3;
	if (width<=0 || height<=0 || (bytespp!=GRAYSCALE && bytespp!=RGB && bytespp!=RGBA)) {
		in.close();
		std::cerr << "bad bpp (or width/height) value\n";
		return false;
	}
	unsigned long nbytes = bytespp*width*height;
	data = new unsigned char[nbytes];
	if (3==header.datatypecode || 2==header.datatypecode) {
		in.read((char *)data, nbytes);
		if (!in.good()) {
			in.close();
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
	} else if (10==header.datatypecode||11==header.datatypecode) {
		if (!load_rle_data(in)) {
			in.close();
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
	} else {
		in.close();
		std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
		return false;
	}
	if (!(header.imagedescriptor & 0x20)) {
		flip_vertically();
	}
	if (header.imagedescriptor & 0x10) {
		flip_horizontally();
	}
	std::cerr << width << "x" << height << "/" << bytespp*8 << "\n";
	in.close();
	return true;
}

bool TGAImage::load_rle_data(std::ifstream &in) {
	unsigned long pixelcount = width*height;
	unsigned long currentpixel = 0;
	unsigned long currentbyte  = 0;
	TGAColor colorbuffer;
	do {
		unsigned char chunkheader = 0;
		chunkheader = in.get();
		if (!in.good()) {
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
		if (chunkheader<128) {
			chunkheader++;
			for (int i=0; i<chunkheader; i++) {
				in.read((char *)colorbuffer.raw, bytespp);
				if (!in.good()) {
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (int t=0; t<bytespp; t++)
					data[currentbyte++] = colorbuffer.raw[t];
				currentpixel++;
				if (currentpixel>pixelcount) {
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		} else {
			chunkheader -= 127;
			in.read((char *)colorbuffer.raw, bytespp);
			if (!in.good()) {
				std::cerr << "an error occured while reading the header\n";
				return false;
			}
			for (int i=0; i<chunkheader; i++) {
				for (int t=0; t<bytespp; t++)
					data[currentbyte++] = colorbuffer.raw[t];
				currentpixel++;
				if (currentpixel>pixelcount) {
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
	} while (currentpixel < pixelcount);
	return true;
}

bool TGAImage::write_tga_file(const char *filename, bool rle) {
	unsigned char developer_area_ref[4] = {0, 0, 0, 0};
	unsigned char extension_area_ref[4] = {0, 0, 0, 0};
	unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
	std::ofstream out;
	out.open (filename, std::ios::binary);
	if (!out.is_open()) {
		std::cerr << "can't open file " << filename << "\n";
		out.close();
		return false;
	}
	TGA_Header header;
	memset((void *)&header, 0, sizeof(header));
	header.bitsperpixel = bytespp<<3;
	header.width  = width;
	header.height = height;
	header.datatypecode = (bytespp==GRAYSCALE?(rle?11:3):(rle?10:2));
	header.imagedescriptor = 0x20; // top-left origin
	out.write((char *)&header, sizeof(header));
	if (!out.good()) {
		out.close();
		std::cerr << "can't dump the tga file\n";
		return false;
	}
	if (!rle) {
		out.write((char *)data, width*height*bytespp);
		if (!out.good()) {
			std::cerr << "can't unload raw data\n";
			out.close();
			return false;
		}
	} else {
		if (!unload_rle_data(out)) {
			out.close();
			std::cerr << "can't unload rle data\n";
			return false;
		}
	}
	out.write((char *)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)footer, sizeof(footer));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.close();
	return true;
}

bool TGAImage::unload_rle_data(std::ofstream &out) {
	const unsigned char max_chunk_length = 128;
	unsigned long npixels = width*height;
	unsigned long curpix = 0;
	while (curpix<npixels) {
		unsigned long chunkstart = curpix*bytespp;
		unsigned long curbyte = curpix*bytespp;
		unsigned char run_length = 1;
		bool raw = true;
		while (curpix+run_length<npixels && run_length<max_chunk_length) {
			bool succ_eq = true;
			for (int t=0; succ_eq && t<bytespp; t++) {
				succ_eq = (data[curbyte+t]==data[curbyte+t+bytespp]);
			}
			curbyte += bytespp;
			if (1==run_length) {
				raw = !succ_eq;
			}
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq) {
				break;
			}
			run_length++;
		}
		curpix += run_length;
		out.put(raw?run_length-1:run_length+127);
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write((char *)(data+chunkstart), (raw?run_length*bytespp:bytespp));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
	}
	return true;
}

TGAColor TGAImage::get(int x, int y) {
	if (!data || x<0 || y<0 || x>=width || y>=height) {
		return TGAColor();
	}
	return TGAColor(data+(x+y*width)*bytespp, bytespp);
}

bool TGAImage::set(int x, int y, TGAColor c) {
	if (!data || x<0 || y<0 || x>=width || y>=height) {
		return false;
	}
	memcpy(data+(x+y*width)*bytespp, c.raw, bytespp);
	return true;
}


int TGAImage::get_bytespp() {
	return bytespp;
}

int TGAImage::get_width() {
	return width;
}

int TGAImage::get_height() {
	return height;
}

bool TGAImage::flip_horizontally() {
	if (!data) return false;
	int half = width>>1;
	for (int i=0; i<half; i++) {
		for (int j=0; j<height; j++) {
			TGAColor c1 = get(i, j);
			TGAColor c2 = get(width-1-i, j);
			set(i, j, c2);
			set(width-1-i, j, c1);
		}
	}
	return true;
}

bool TGAImage::flip_vertically() {
	if (!data) return false;
	unsigned long bytes_per_line = width*bytespp;
	unsigned char *line = new unsigned char[bytes_per_line];
	int half = height>>1;
	for (int j=0; j<half; j++) {
		unsigned long l1 = j*bytes_per_line;
		unsigned long l2 = (height-1-j)*bytes_per_line;
		memmove((void *)line,      (void *)(data+l1), bytes_per_line);
		memmove((void *)(data+l1), (void *)(data+l2), bytes_per_line);
		memmove((void *)(data+l2), (void *)line,      bytes_per_line);
	}
	delete [] line;
	return true;
}

unsigned char *TGAImage::buffer() {
	return data;
}

void TGAImage::clear() {
	memset((void *)data, 0, width*height*bytespp);
}
void TGAImage::line(float x1, float y1, float x2, float y2, TGAColor color) // simple line
{
	float vx, vy;
	bool fl = true;
	if (x1 > x2) { vx = x1; x1 = x2; x2 = vx; vy = y1; y1 = y2; y2 = vy; fl = false; }


	{
		float dif = (y2 - y1) / (x2 - x1);

		while (x1 < x2) {
			set(x1, y1, color);
			x1 = x1 + 1.0;
			y1 = y1 + dif;
		}
		if (y1> y2) { vy = y1; y1 = y2; y2 = vy; }

		if (x1 == x2) while (y1 < y2) {
			set(x1, y1, color);
			y1 = y1 + 1.0;
		}
	}
	
}
void TGAImage::BRline(float x1, float y1, float x2, float y2, TGAColor color) // bresenham line
{
	float vx, vy;
	if (x1 > x2) { vx = x1; x1 = x2; x2 = vx; vy = y1; y1 = y2; y2 = vy; }
	float dx = x2 - x1;
	float dy = y2 - y1, dyn= -dy;
	float dl = dy / dx;
	if (dl < -1)
	{
		float d = 2 * dx - dyn;
		float x = x1, y = y1;
		do {
			set(x, y, color);
			if (d < 0)
				d += 2 * dx;
			else {
				d += 2 * (dx - dyn);
				++x;
			}
			--y;
		} while (y >= y2);
	}
	else if (dl < 0)
	{
		float d = 2 * dyn - dx;
		float x = x1, y = y1;
		do {
			set(x, y, color);
			if (d < 0)
				d += 2 * dyn;
			else {
				d += 2 * (dyn - dx);
				--y;
			}
			++x;
		} while (x <= x2);
	}
	else if (dl < 1)
	{
		float d = 2 * dy - dx;
		float x = x1, y = y1;
		do {
			set(x, y, color);
			if (d < 0)
				d += 2 * dy;
			else {
				d += 2 * (dy - dx);
				++y;
			}
			++x;
		} while (x <= x2);
	}
	else {
		float d = 2 * dx - dy;
		float x = x1, y = y1;
		do {
			set(x, y, color);
			if (d < 0)
				d += 2 * dx;
			else {
				d += 2 * (dx - dy);
				++x;
			}
			++y;
		} while (y <= y2);

	}
}
bool TGAImage::whichline()
{
	cout << "1-CDA, 2-Bresenham's line:" << endl;
	int a = 0;
	cin >> a;

	cout << "Enter the x1, y1, x2 and y2 values (0 to 500):" << endl;
	float x1, y1, x2, y2;
	cin >> x1 >> y1 >> x2 >> y2;
	bool flag = true;
	while (flag) {
		if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1>500 || x2>500 || y1>500 || y2>500)
		{
			cout << "Enter the x1, y1, x2 and y2 values (0 to 500):" << endl;
			cin >> x1 >> y1 >> x2 >> y2;
		}
		else flag = false;
	}
	float vx, vy;
	bool fl = true;
	if (x1 > x2) { vx = x1; x1 = x2; x2 = vx; vy = y1; y1 = y2; y2 = vy; fl = false; }
	
	cout << "Enter the color (1-white or 2-red):" << endl;
	int col;
	cin >> col;
	TGAColor color;

	switch (col)
	{
	case 1: color = white;
		break;
	case 2: color = red;
		break;
	}


	switch (a)
	{
	case 1:
	{
		line(x1, y1, x2, y2, color);
	}
	break;

	case 2:
	{
		BRline(x1, y1, x2, y2, color);
	}
	break;
	}
	return true;
}

void BRlinetri(int x1, int y1, int x2, int y2, int x3, int y3, vector <pair<int, int>> * PutToList) // triangle with bresenham line 
{
	int vx, vy;
	bool fl = true;
	if (x1 > x2) { vx = x1; x1 = x2; x2 = vx; vy = y1; y1 = y2; y2 = vy; fl = false; }
	float dx = x2 - x1, dx2 = x3 - x1;
	float dy = y2 - y1, dyn = -dy;
	float dl = dy / dx;

	bool sverxy = false;
	int yprov = dl*dx2 + y1;
	if (yprov< y3) sverxy = true;

	int x22 = x2, y22 = y2, y222 = y2;
	if (fl == false) {
		x22 = x2 + 1; y22 = y2 + 1; y222 = y2 - 1;
	}


	if (dl <= -1)
	{
		int d = 2 * dx - dyn;
		int x = x1, y = y1;
		do {
			if (fl == true) { PutToList->push_back(make_pair(x, y)); }
			else if (y < y1) { PutToList->push_back(make_pair(x, y)); }

			if (d < 0)
				d += 2 * dx;
			else {
				d += 2 * (dx - dyn);
				++x;
			}
			--y;
		} while (y > y222);
	}
	else if (dl < 0)
	{
		int d = 2 * dyn - dx;
		int x = x1, y = y1;// prevy = -1;
		bool nach = false;
		do {
			if (d < 0)
				d += 2 * dyn;
			else {
				d += 2 * (dyn - dx);
				if (sverxy == false) {
					if (fl == true) { PutToList->push_back(make_pair(x, y)); } //popped point at the end of the segment, include the first point
					else if ((y < y1) || (nach == true)) { PutToList->push_back(make_pair(x, y)); }//the popped point at the beginning of the segment
				}
				--y;
				if (sverxy == true) {
					if (fl == true) { PutToList->push_back(make_pair(x, y)); } //popped point at the end of the segment, include the first point
					else if ((y < y1) || (nach == true)) { PutToList->push_back(make_pair(x, y)); }//the popped point at the beginning of the segment
				}
			}
			++x;
			nach = true;
			if ((x == x22) && ((d + 2 * dyn) >= 0)) { PutToList->push_back(make_pair(x - 1, y)); }
		} while (x < x22);
	}
	else if (dl < 1)
	{
		int d = 2 * dy - dx;
		int x = x1, y = y1;
		bool nach = false;
		do {
			if (d < 0)
				d += 2 * dy;
			else {
				d += 2 * (dy - dx);
				if (sverxy == true) {
					if (fl == true) { PutToList->push_back(make_pair(x, y)); } //popped point at the end of the segment, include the first point
					else if ((y > y1) || (nach == true)) { PutToList->push_back(make_pair(x, y)); }//the popped point at the beginning of the segment, if necessary
				}
				++y;
				if (sverxy == false) {
					if (fl == true) { PutToList->push_back(make_pair(x, y)); } //popped point at the end of the segment, include the first point
					else if ((y > y1) || (nach == true)) { PutToList->push_back(make_pair(x, y)); }//the popped point at the beginning of the segment, if necessary 
				}
			}
			++x;
			nach = true;
		} while (x < x22);
	}
	else {
		int d = 2 * dx - dy;
		int x = x1, y = y1;
		do {
			if (fl == true) { PutToList->push_back(make_pair(x, y)); }
			else if (y > y1) { PutToList->push_back(make_pair(x, y)); }

			if (d < 0)
				d += 2 * dx;
			else {
				d += 2 * (dx - dy);
				++x;
			}
			++y;
		} while (y < y22);

	}
}
bool Ravno(const pair <int, int> &p1, int x, int  y)
{
	return ((p1.second == y) && (p1.first == x));
}
bool SortPairY(const pair <int, int> &p1, const pair <int, int> &p2)
{
	return p1.second < p2.second;
}

bool TGAImage::fill(int x1, int y1, int x2, int y2, int x3, int y3, TGAColor color)
{
	int xn, yn;
	vector <pair<int, int>> PutToList1;

	BRlinetri(x1, y1, x2, y2, x3, y3, &PutToList1);
	BRlinetri(x2, y2, x3, y3, x1, y1, &PutToList1);
	BRlinetri(x3, y3, x1, y1, x2, y2, &PutToList1);

	sort(PutToList1.begin(), PutToList1.end(), SortPairY);
	
	bool ver1 = false, ver2 = false, ver3 = false;
	for (int i = 0; i < size(PutToList1); ++i) {
		if (Ravno(PutToList1[i], x1, y1)) { ver1 = true; }
		if (Ravno(PutToList1[i], x2, y2)) { ver2 = true; }
		if (Ravno(PutToList1[i], x3, y3)) { ver3 = true; }

	}
	if (ver1 == false) { PutToList1.push_back(make_pair(x1, y1)); }
	if (ver2 == false) { PutToList1.push_back(make_pair(x2, y2)); }
	if (ver3 == false) { PutToList1.push_back(make_pair(x3, y3)); }
	sort(PutToList1.begin(), PutToList1.end(), SortPairY);

	for (int i = 1; i < size(PutToList1); ++i)
	{
		if (PutToList1[i - 1].second == PutToList1[i].second)
		{
			if (PutToList1[i - 1].first < PutToList1[i].first) {
				for (int j = PutToList1[i - 1].first; j <= PutToList1[i].first; ++j) { set(j, PutToList1[i - 1].second, color); }
			}
			else {
				for (int j = PutToList1[i].first; j <= PutToList1[i - 1].first; ++j) { set(j, PutToList1[i - 1].second, color); }
			}
		}
		else
		{
			if (((PutToList1[i - 1].second < PutToList1[i].second) && ((PutToList1[i - 1].second == PutToList1[0].second) || (PutToList1[i].second == PutToList1[size(PutToList1) - 1].second))) || ((PutToList1[i - 1].second < PutToList1[i].second) && (PutToList1[i].second < PutToList1[i + 1].second))) { set(PutToList1[i - 1].first, PutToList1[i - 1].second, color); };
		}

	}
	set(x1, y1, color);
	set(x2, y2, color);
	set(x3, y3, color);
	return true;
}
void TGAImage::zbuftriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, TGAColor color, int *zbufferr, bool setflag)
{
	TGAColor zcolor = TGAColor(0, 0, 0, 255);
	int Ax, Ay, Bx, By, Px, Py;
	float  Az, Bz, Pz, glubinaz;
	if (y1 > y2) { swap(x1, x2); swap(y1, y2); swap(z1, z2); }
	if (y1 > y3) { swap(x1, x3); swap(y1, y3); swap(z1, z3); }
	if (y2 > y3) { swap(x2, x3); swap(y2, y3); swap(z2, z3); }
	int deltay = y3 - y1;
	for (int i = 0; i < deltay; i++) {
		bool flagrybejy1 = i >(y2 - y1) || y2 == y1;
		int malayadeltay = flagrybejy1 ? y3 - y2 : y2 - y1;
		float raspolog = (float)i / deltay;
		float raspolog2 = (float)(i - (flagrybejy1 ? y2 - y1 : 0)) / malayadeltay;
		Ax = x1 + (x3 - x1)*raspolog;
		Ay = y1 + (y3 - y1)*raspolog;
		Az = z1 + (z3 - z1)*raspolog;

		Bx = flagrybejy1 ? x2 + (x3 - x2)*raspolog2 : x1 + (x2 - x1)*raspolog2;
		By = flagrybejy1 ? y2 + (y3 - y2)*raspolog2 : y1 + (y2 - y1)*raspolog2;
		Bz = flagrybejy1 ? z2 + (z3 - z2)*raspolog2 : z1 + (z2 - z1)*raspolog2;

		if (Ax > Bx) { swap(Ax, Bx); swap(Ay, By); swap(Az, Bz); }

		for (int j = Ax; j <= Bx; j++) { //go on x with fixed y
			float delx = Bx == Ax ? 1.0 : (float)(j - Ax) / (float)(Bx - Ax);
			Px = Ax + (Bx - Ax)*delx;
			Py = Ay + (By - Ay)*delx;
			Pz = Az + (Bz - Az)*delx;

			glubinaz = int(((Pz + 1) / 2) * 255);
			if ((Px >= 0) && (Px < sizeim) && (Py >= 0) && (Py < sizeim))
			{
				int index = Px + Py*sizeim;
				if (zbufferr[index] < glubinaz) {
					zbufferr[index] = glubinaz;
					if (setflag) {
						zcolor = TGAColor(glubinaz, glubinaz, glubinaz, 255);
						set(Px, Py, zcolor);
					}
				}
			}


		}

	}


}



float dot(triangle2 t1, triangle2 t2)
{
	return ((t1.geta())*(t2.geta()) +(t1.getb())*(t2.getb()) +(t1.getc())*(t2.getc()));
}
float max(float m1, float m2){
	return(m1 > m2 ? m1 : m2);
}
float vozvstep(float m1, int m2) {
	float rez = 1;
	for (int i = 0; i < m2; ++i) rez = rez * m1;
	return(rez);
}

void TGAImage::guro(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax)
{
	TGAColor zcolor = TGAColor(0, 0, 0, 255);
	int zz1, zz2, zz3;
	float L0 = 1, Ll = 1, Le = 0.5, dlin1=sqrt(nx1*nx1+ ny1*ny1 + nz1*nz1), dlin2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2), dlin3 = sqrt(nx3*nx3 + ny3*ny3 + nz3*nz3);
	float kd = 0.5, ks = 0.7, ka = 0.125, Losv1, Losv2, Losv3, p = 3, lx1, ly1, lz1, lx2, ly2, lz2, lx3, ly3, lz3, coef1, coef2, coef3;
	float vx = 0, vy = 0 ,vz = newsize * 2 , coefv, nvx, nvy, nvz, dotv1, dotv2, dotv3;
	coefv = (1 / sqrt(vx*vx + vy*vy + vz*vz));
	zz1 = floor(z1*newsize);
	zz2 = floor(z2*newsize);
	zz3 = floor(z3*newsize);

	//normalization
	triangle2 NnormV= triangle2(vx*coefv, vy*coefv, vz*coefv);

	lx1 = lix - newsize;
	ly1 = liy - newsize;
	lz1 = liz;
	coef1 = (1 / sqrt(lx1*lx1 + ly1*ly1 + lz1*lz1));
	triangle2 Nnormosve = triangle2(lx1*coef1, ly1* coef1, lz1*coef1);


	triangle2 ver1norm = triangle2((nx1), (ny1), (nz1));
	triangle2 ver2norm = triangle2((nx2), (ny2), (nz2));
	triangle2 ver3norm = triangle2((nx3), (ny3), (nz3));

	dotv1 = dot(NnormV, ver1norm);
	dotv2 = dot(NnormV, ver2norm);
	dotv3 = dot(NnormV, ver3norm);
	triangle2 S1 = triangle2(2 * dotv1 * (nx1 / dlin1) - vx*coefv, 2 * dotv1 * (ny1 / dlin1) - vy*coefv, 2 * dotv1 * (nz1 / dlin1) - vz*coefv);
	triangle2 S2 = triangle2(2 * dotv2 * (nx2 / dlin2) - vx*coefv, 2 * dotv2 * (ny2 / dlin2) - vy*coefv, 2 * dotv2 * (nz2 / dlin2) - vz*coefv);
	triangle2 S3 = triangle2(2 * dotv3 * (nx3 / dlin3) - vx*coefv, 2 * dotv3 * (ny3 / dlin3) - vy*coefv, 2 * dotv3 * (nz3 / dlin3) - vz*coefv);

	float dot1 = dot(Nnormosve, ver1norm);
	float dot2 = dot(Nnormosve, ver2norm);
	float dot3 = dot(Nnormosve, ver3norm);

	float dototr1 = vozvstep(dot(Nnormosve, S1), p);
	float dototr2 = vozvstep(dot(Nnormosve, S2), p);
	float dototr3 = vozvstep(dot(Nnormosve, S3), p);
	//he illumination at the vertices
	Losv1 = Le*ka + L0*(kd*max(0, dot1) + ks*max(0, dototr1));// vertex x1, y1, z1
	Losv2 = Le*ka + L0*(kd*max(0, dot2) + ks*max(0, dototr2));
	Losv3 = Le*ka + L0*(kd*max(0, dot3) + ks*max(0, dototr3));
	//Losv1 = L0*(kd*max(0, dot1));// simplified version, vertex x1, y1, z1 
	//Losv2 = L0*(kd*max(0, dot2));
	//Losv3 = L0*(kd*max(0, dot3));
	int Ax, Ay, Bx, By, Px, Py;
	float  Az, Bz, Pz, glubinaz, glubinaz2, Pl, Al, Bl;
	if (y1 > y2) { swap(x1, x2); swap(y1, y2); swap(z1, z2); swap(Losv1, Losv2); }
	if (y1 > y3) { swap(x1, x3); swap(y1, y3); swap(z1, z3); swap(Losv1, Losv3); }
	if (y2 > y3) { swap(x2, x3); swap(y2, y3); swap(z2, z3); swap(Losv3, Losv2); }
	int deltay = y3 - y1;
	for (int i = 0; i < deltay; i++) {
		bool flagrybejy1 = i > (y2 - y1) || y2 == y1;
		int malayadeltay = flagrybejy1 ? y3 - y2 : y2 - y1;
		float raspolog = (float)i / deltay;
		float raspolog2 = (float)(i - (flagrybejy1 ? y2 - y1 : 0)) / malayadeltay;

		Ax = x1 + (x3 - x1)*raspolog;
		Ay = y1 + (y3 - y1)*raspolog;
		Az = z1 + (z3 - z1)*raspolog;
		Al = Losv1 + (Losv3 - Losv1)*raspolog;

		Bx = flagrybejy1 ? x2 + (x3 - x2)*raspolog2 : x1 + (x2 - x1)*raspolog2;
		By = flagrybejy1 ? y2 + (y3 - y2)*raspolog2 : y1 + (y2 - y1)*raspolog2;
		Bz = flagrybejy1 ? z2 + (z3 - z2)*raspolog2 : z1 + (z2 - z1)*raspolog2;
		Bl = flagrybejy1 ? Losv2 + (Losv3 - Losv2)*raspolog2 : Losv1 + (Losv2 - Losv1)*raspolog2;

		if (Ax > Bx) { swap(Ax, Bx); swap(Ay, By); swap(Az, Bz); swap(Al, Bl); }

		for (int j = Ax; j <= Bx; j++) { 
			float delx = Bx == Ax ? 1.0 : (float)(j - Ax) / (float)(Bx - Ax);
			Px = Ax + (Bx - Ax)*delx;
			Py = Ay + (By - Ay)*delx;
			Pz = Az + (Bz - Az)*delx;
			Pl = Al + (Bl - Al)*delx;

			if (*plmax < Pl) *plmax = Pl;
			glubinaz = ((Pz + 1) / 2) * 255;
			if ((Px >= 0) && (Px < sizeim) && (Py >= 0) && (Py < sizeim))
			{
				int index = Px + Py*sizeim;
				if (zbuffer[index] < glubinaz) {
					zbuffer[index] = glubinaz;
					glubinaz2 = (Pl / 2) * 255;
					zcolor = TGAColor(glubinaz2, glubinaz2, glubinaz2, 255);
					set(Px, Py, zcolor);
				}
			}

		}


	}
}
void TGAImage::superguro(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, matrix4na4no2 M, matrix4na4no2 Ml, matrix3na3 Mmal, int *zbuffer2, float *plmax)
{
	
	TGAColor zcolor = TGAColor(0, 0, 0, 255);
	int zz1, zz2, zz3, zsize;
	float L0 = 1, Ll = 1, Le = 0.5, dlin1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1), dlin2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2), dlin3 = sqrt(nx3*nx3 + ny3*ny3 + nz3*nz3);
	float kd = 0.5, ks = 0.7, ka = 0.125, Losv1, Losv2, Losv3, p = 3, lx1, ly1, lz1, lx2, ly2, lz2, lx3, ly3, lz3, coef1, coef2, coef3;
	float vx = 0, vy = 0, vz = newsize * 2, coefv, nvx, nvy, nvz, dotv1, dotv2, dotv3;
	coefv = (1 / sqrt(vx*vx + vy*vy + vz*vz));
	zz1 = floor(z1*newsize);
	zz2 = floor(z2*newsize);
	zz3 = floor(z3*newsize);

	
	triangle2 NnormV = triangle2(vx*coefv, vy*coefv, vz*coefv);

	lx1 = lix - newsize;
	ly1 = liy - newsize;
	lz1 = liz;
	coef1 = (1 / sqrt(lx1*lx1 + ly1*ly1 + lz1*lz1));
	triangle2 Nnormosve = triangle2(lx1*coef1, ly1* coef1, lz1*coef1);


	triangle2 ver1norm = triangle2((nx1), (ny1), (nz1));
	triangle2 ver2norm = triangle2((nx2), (ny2), (nz2));
	triangle2 ver3norm = triangle2((nx3), (ny3), (nz3));

	dotv1 = dot(NnormV, ver1norm);
	dotv2 = dot(NnormV, ver2norm);
	dotv3 = dot(NnormV, ver3norm);
	triangle2 S1 = triangle2(2 * dotv1 * (nx1 / dlin1) - vx*coefv, 2 * dotv1 * (ny1 / dlin1) - vy*coefv, 2 * dotv1 * (nz1 / dlin1) - vz*coefv);
	triangle2 S2 = triangle2(2 * dotv2 * (nx2 / dlin2) - vx*coefv, 2 * dotv2 * (ny2 / dlin2) - vy*coefv, 2 * dotv2 * (nz2 / dlin2) - vz*coefv);
	triangle2 S3 = triangle2(2 * dotv3 * (nx3 / dlin3) - vx*coefv, 2 * dotv3 * (ny3 / dlin3) - vy*coefv, 2 * dotv3 * (nz3 / dlin3) - vz*coefv);

	float dot1 = dot(Nnormosve, ver1norm);
	float dot2 = dot(Nnormosve, ver2norm);
	float dot3 = dot(Nnormosve, ver3norm);

	float dototr1 = vozvstep(dot(Nnormosve, S1), p);
	float dototr2 = vozvstep(dot(Nnormosve, S2), p);
	float dototr3 = vozvstep(dot(Nnormosve, S3), p);
	
	Losv1 = Le*ka + L0*(kd*max(0, dot1) + ks*max(0, dototr1));
	Losv2 = Le*ka + L0*(kd*max(0, dot2) + ks*max(0, dototr2));
	Losv3 = Le*ka + L0*(kd*max(0, dot3) + ks*max(0, dototr3));
	//Losv1 = L0*(kd*max(0, dot1));
	//Losv2 = L0*(kd*max(0, dot2));
	//Losv3 = L0*(kd*max(0, dot3));


	int Ax, Ay, Bx, By, Px, Py;
	float  Az, Bz, Pz, glubinaz, glubinaz2, Pl, Al, Bl, Pzl, Pzc, glubinazshaid;
	if (y1 > y2) { swap(x1, x2); swap(y1, y2); swap(z1, z2); swap(Losv1, Losv2); }
	if (y1 > y3) { swap(x1, x3); swap(y1, y3); swap(z1, z3); swap(Losv1, Losv3); }
	if (y2 > y3) { swap(x2, x3); swap(y2, y3); swap(z2, z3); swap(Losv3, Losv2); }
	int deltay = y3 - y1;
	for (int i = 0; i < deltay; i++) {
		bool flagrybejy1 = i >(y2 - y1) || y2 == y1;
		int malayadeltay = flagrybejy1 ? y3 - y2 : y2 - y1;
		float raspolog = (float)i / deltay;
		float raspolog2 = (float)(i - (flagrybejy1 ? y2 - y1 : 0)) / malayadeltay;

		Ax = x1 + (x3 - x1)*raspolog;
		Ay = y1 + (y3 - y1)*raspolog;
		Az = z1 + (z3 - z1)*raspolog;
		Al = Losv1 + (Losv3 - Losv1)*raspolog;

		Bx = flagrybejy1 ? x2 + (x3 - x2)*raspolog2 : x1 + (x2 - x1)*raspolog2;
		By = flagrybejy1 ? y2 + (y3 - y2)*raspolog2 : y1 + (y2 - y1)*raspolog2;
		Bz = flagrybejy1 ? z2 + (z3 - z2)*raspolog2 : z1 + (z2 - z1)*raspolog2;
		Bl = flagrybejy1 ? Losv2 + (Losv3 - Losv2)*raspolog2 : Losv1 + (Losv2 - Losv1)*raspolog2;

		if (Ax > Bx) { swap(Ax, Bx); swap(Ay, By); swap(Az, Bz); swap(Al, Bl); }

		for (int j = Ax; j <= Bx; j++) { 
			float delx = Bx == Ax ? 1.0 : (float)(j - Ax) / (float)(Bx - Ax);
			Px = Ax + (Bx - Ax)*delx;
			Py = Ay + (By - Ay)*delx;
			Pz = Az + (Bz - Az)*delx;
			Pl = Al + (Bl - Al)*delx;

			zsize = floor(Pz*newsize);
			Normal  coordvtochkesveta = proizved2(proizved2(Normal(Px, Py, zsize, 1), M), Ml);
			

			glubinaz = ((Pz + 1) / 2) * 255;
			glubinazshaid =int( (((coordvtochkesveta.gety()) + 1) / 2) * 255);

			if ((Px >= 0) && (Px < sizeim) && (Py >= 0) && (Py < sizeim))
			{
				int index = Px + Py*sizeim;
				int index2 = coordvtochkesveta.getn() + coordvtochkesveta.getx()*sizeim;
				if (zbuffer[index] < glubinaz) {
					zbuffer[index] = glubinaz;
					if ((coordvtochkesveta.getn() >= 0) && (ceil(coordvtochkesveta.getn()) < sizeim) && (coordvtochkesveta.getx() >= 0) && (ceil(coordvtochkesveta.getx()) < sizeim))
					{
						if (zbuffer2[index2] < glubinazshaid) {

							glubinaz2 = (Pl / 2) * 255;
							zcolor = TGAColor(glubinaz2, glubinaz2, glubinaz2, 255);
							set(Px, Py, zcolor);
						}
						else
						{
							glubinaz2 = (Pl / 4) * 255;
							zcolor = TGAColor(glubinaz2, glubinaz2, glubinaz2, 255);
							set(Px, Py, zcolor);

						}
					}
					else
					{
						glubinaz2 = (Pl / 2) * 255;
						zcolor = TGAColor(glubinaz2, glubinaz2, glubinaz2, 255);
						set(Px, Py, zcolor);
					}
				}
			}

		}


	}


}
void TGAImage::phong(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax)
{
	TGAColor zcolor = TGAColor(0, 0, 0, 255);
	triangle2 S1, ver1norm, NnormV, Nnorm1;
	int zz1, zz2, zz3;
	float dototr1, dot1;
	float L0 = 1, Ll = 1, Le = 0.5, dlin1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1), dlin2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2), dlin3 = sqrt(nx3*nx3 + ny3*ny3 + nz3*nz3);
	float kd = 0.5, ks = 0.7, ka = 0.125, Losv1, Losv2, Losv3, p = 3, lx1, ly1, lz1, lx2, ly2, lz2, lx3, ly3, lz3, coef1, coef2, coef3;
	float vx = 0, vy = 0, vz = newsize * 2, coefv, nvx, nvy, nvz, dotv1, dotv2, dotv3;
	coefv = (1 / sqrt(vx*vx + vy*vy + vz*vz));
	NnormV = triangle2(vx*coefv, vy*coefv, vz*coefv);

	int Ax, Ay, Bx, By, Px, Py;
	float  Az, Bz, Pz, glubinaz, glubinaz2, Pxn, Pyn, Pzn, Axn, Ayn, Azn, Bxn, Byn, Bzn;

	if (y1 > y2) { swap(x1, x2); swap(y1, y2); swap(z1, z2); swap(nx1, nx2); swap(ny1, ny2); swap(nz1, nz2); }
	if (y1 > y3) { swap(x1, x3); swap(y1, y3); swap(z1, z3); swap(nx1, nx3); swap(ny1, ny3); swap(nz1, nz3); }
	if (y2 > y3) { swap(x2, x3); swap(y2, y3); swap(z2, z3); swap(nx3, nx2); swap(ny3, ny2); swap(nz3, nz2); }

	int deltay = y3 - y1;
	for (int i = 0; i < deltay; i++) {
		bool flagrybejy1 = i > (y2 - y1) || y2 == y1;
		int malayadeltay = flagrybejy1 ? y3 - y2 : y2 - y1;
		float raspolog = (float)i / deltay;
		float raspolog2 = (float)(i - (flagrybejy1 ? y2 - y1 : 0)) / malayadeltay;

		Ax = x1 + (x3 - x1)*raspolog;
		Ay = y1 + (y3 - y1)*raspolog;
		Az = z1 + (z3 - z1)*raspolog;
		
		Axn = nx1 + (nx3 - nx1)*raspolog;
		Ayn = ny1 + (ny3 - ny1)*raspolog;
		Azn = nz1 + (nz3 - nz1)*raspolog;

		Bx = flagrybejy1 ? x2 + (x3 - x2)*raspolog2 : x1 + (x2 - x1)*raspolog2;
		By = flagrybejy1 ? y2 + (y3 - y2)*raspolog2 : y1 + (y2 - y1)*raspolog2;
		Bz = flagrybejy1 ? z2 + (z3 - z2)*raspolog2 : z1 + (z2 - z1)*raspolog2;
		
		Bxn = flagrybejy1 ? nx2 + (nx3 - nx2)*raspolog2 : nx1 + (nx2 - nx1)*raspolog2;
		Byn = flagrybejy1 ? ny2 + (ny3 - ny2)*raspolog2 : ny1 + (ny2 - ny1)*raspolog2;
		Bzn = flagrybejy1 ? nz2 + (nz3 - nz2)*raspolog2 : nz1 + (nz2 - nz1)*raspolog2;

		if (Ax > Bx) { swap(Ax, Bx); swap(Ay, By); swap(Az, Bz); swap(Axn, Bxn); swap(Ayn, Byn); swap(Azn, Bzn); }

		for (int j = Ax; j <= Bx; j++) { 
			float delx = Bx == Ax ? 1.0 : (float)(j - Ax) / (float)(Bx - Ax);
			Px = Ax + (Bx - Ax)*delx;
			Py = Ay + (By - Ay)*delx;
			Pz = Az + (Bz - Az)*delx;

			Pxn = Axn + (Bxn - Axn)*delx;
			Pyn = Ayn + (Byn - Ayn)*delx;
			Pzn = Azn + (Bzn - Azn)*delx;

			
			glubinaz = ((Pz + 1) / 2) * 255;
			int index = Px + Py*sizeim;
			if (zbuffer[index] < glubinaz) {
				zbuffer[index] = glubinaz;
				

				zz1 = floor(Pz*newsize) + newsize;
				
				coef1 = (1 / sqrt(lix*lix + liy*liy + liz*liz));
				Nnorm1 = triangle2(lix*coef1, liy* coef1, liz*coef1);

				ver1norm = triangle2((Pxn / dlin1), (Pyn / dlin1), (Pzn / dlin1));
				dotv1 = dot(NnormV, ver1norm);
				S1 = triangle2(2 * dotv1 * (Pxn / dlin1) - vx*coefv, 2 * dotv1 * (Pyn / dlin1) - vy*coefv, 2 * dotv1 * (Pzn / dlin1) - vz*coefv);
				dot1 = dot(Nnorm1, ver1norm);
				dototr1 = vozvstep(dot(Nnorm1, S1), p);
				Losv1 = Le*ka + L0*(kd*max(0, dot1) + ks*max(0, dototr1));


				glubinaz2 = (Losv1 / 2) * 255;
				zcolor = TGAColor(glubinaz2, glubinaz2, glubinaz2, 255);
				set(Px, Py, zcolor);
			}

		}

	}
}
void TGAImage::texture(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax, float u1, float v1, float u2, float v2, float u3, float v3, TGAImage image2)
{
	TGAColor zcolor = TGAColor(0, 0, 0, 255);
	
	int zz1, zz2, zz3;
	int twidth = image2.get_width(), theight = image2.get_height();
	float L0 = 1, Ll = 1, Le = 0.5, dlin1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1), dlin2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2), dlin3 = sqrt(nx3*nx3 + ny3*ny3 + nz3*nz3);
	float kd = 0.5, ks = 0.7, ka = 0.125, Losv1, Losv2, Losv3, p = 3, lx1, ly1, lz1, lx2, ly2, lz2, lx3, ly3, lz3, coef1, coef2, coef3;
	float vx = 0, vy = 0, vz = newsize * 2, coefv, nvx, nvy, nvz, dotv1, dotv2, dotv3;
	{
		coefv = (1 / sqrt(vx*vx + vy*vy + vz*vz));
		zz1 = floor(z1*newsize) + newsize;
		zz2 = floor(z2*newsize) + newsize;
		zz3 = floor(z3*newsize) + newsize;

		
		triangle2 NnormV = triangle2(vx*coefv, vy*coefv, vz*coefv);

		lx1 = lix - 2500;
		ly1 = liy - 2500;
		lz1 = liz - 2500;
		coef1 = (1 / sqrt(lx1*lx1 + ly1*ly1 + lz1*lz1));
		triangle2 Nnormosve = triangle2(lx1*coef1, ly1* coef1, lz1*coef1);


		triangle2 ver1norm = triangle2((nx1), (ny1), (nz1));
		triangle2 ver2norm = triangle2((nx2), (ny2), (nz2));
		triangle2 ver3norm = triangle2((nx3), (ny3), (nz3));

		dotv1 = dot(NnormV, ver1norm);
		dotv2 = dot(NnormV, ver2norm);
		dotv3 = dot(NnormV, ver3norm);
		triangle2 S1 = triangle2(2 * dotv1 * (nx1 / dlin1) - vx*coefv, 2 * dotv1 * (ny1 / dlin1) - vy*coefv, 2 * dotv1 * (nz1 / dlin1) - vz*coefv);
		triangle2 S2 = triangle2(2 * dotv2 * (nx2 / dlin2) - vx*coefv, 2 * dotv2 * (ny2 / dlin2) - vy*coefv, 2 * dotv2 * (nz2 / dlin2) - vz*coefv);
		triangle2 S3 = triangle2(2 * dotv3 * (nx3 / dlin3) - vx*coefv, 2 * dotv3 * (ny3 / dlin3) - vy*coefv, 2 * dotv3 * (nz3 / dlin3) - vz*coefv);

		float dot1 = dot(Nnormosve, ver1norm);
		float dot2 = dot(Nnormosve, ver2norm);
		float dot3 = dot(Nnormosve, ver3norm);

		float dototr1 = vozvstep(dot(Nnormosve, S1), p);
		float dototr2 = vozvstep(dot(Nnormosve, S2), p);
		float dototr3 = vozvstep(dot(Nnormosve, S3), p);
		
		Losv1 = Le*ka + L0*(kd*max(0, dot1) + ks*max(0, dototr1));
		Losv2 = Le*ka + L0*(kd*max(0, dot2) + ks*max(0, dototr2));
		Losv3 = Le*ka + L0*(kd*max(0, dot3) + ks*max(0, dototr3));
		//Losv1 = L0*(kd*max(0, dot1));
		//Losv2 = L0*(kd*max(0, dot2));
		//Losv3 = L0*(kd*max(0, dot3));
	}

	int Ax, Ay, Bx, By, Px, Py;
	float  Az, Bz, Pz, glubinaz, glubinaz2, Pl, Al, Bl;
	float  Au, Bu, Pu, Av, Bv, Pv;
	int Utex, Vtex;
	if (y1 > y2) { swap(x1, x2); swap(y1, y2); swap(z1, z2); swap(Losv1, Losv2); swap(u1, u2); swap(v1, v2); }
	if (y1 > y3) { swap(x1, x3); swap(y1, y3); swap(z1, z3); swap(Losv1, Losv3); swap(u1, u3); swap(v1, v3); }
	if (y2 > y3) { swap(x2, x3); swap(y2, y3); swap(z2, z3); swap(Losv3, Losv2); swap(u3, u2); swap(v3, v2); }

	int deltay = y3 - y1;
	for (int i = 0; i < deltay; i++) {
		bool flagrybejy1 = i >(y2 - y1) || y2 == y1;
		int malayadeltay = flagrybejy1 ? y3 - y2 : y2 - y1;
		float raspolog = (float)i / deltay;
		float raspolog2 = (float)(i - (flagrybejy1 ? y2 - y1 : 0)) / malayadeltay;

		Ax = x1 + (x3 - x1)*raspolog;
		Ay = y1 + (y3 - y1)*raspolog;
		Az = z1 + (z3 - z1)*raspolog;
		Al = Losv1 + (Losv3 - Losv1)*raspolog;
		Au = u1 + (u3 - u1)*raspolog;
		Av = v1 + (v3 - v1)*raspolog;

		Bx = flagrybejy1 ? x2 + (x3 - x2)*raspolog2 : x1 + (x2 - x1)*raspolog2;
		By = flagrybejy1 ? y2 + (y3 - y2)*raspolog2 : y1 + (y2 - y1)*raspolog2;
		Bz = flagrybejy1 ? z2 + (z3 - z2)*raspolog2 : z1 + (z2 - z1)*raspolog2;
		Bl = flagrybejy1 ? Losv2 + (Losv3 - Losv2)*raspolog2 : Losv1 + (Losv2 - Losv1)*raspolog2;
		Bu = flagrybejy1 ? u2 + (u3 - u2)*raspolog2 : u1 + (u2 - u1)*raspolog2;
		Bv = flagrybejy1 ? v2 + (v3 - v2)*raspolog2 : v1 + (v2 - v1)*raspolog2;

		if (Ax > Bx) { swap(Ax, Bx); swap(Ay, By); swap(Az, Bz); swap(Al, Bl);  swap(Au, Bu); swap(Av, Bv); }

		for (int j = Ax; j <= Bx; j++) { 
			float delx = Bx == Ax ? 1.0 : (float)(j - Ax) / (float)(Bx - Ax);
			Px = Ax + (Bx - Ax)*delx;
			Py = Ay + (By - Ay)*delx;
			Pz = Az + (Bz - Az)*delx;
			Pl = Al + (Bl - Al)*delx;
			Pu = Au + (Bu - Au)*delx;
			Pv = Av + (Bv - Av)*delx;

			Utex = int(twidth * Pu);
			Vtex = int(theight *Pv);
			
			if (*plmax < Pl) *plmax = Pl;
			glubinaz = ((Pz) / 2) * 255;
			int index = Px + Py*sizeim;
			if (zbuffer[index] < glubinaz) {
				zbuffer[index] = glubinaz;
				glubinaz2 = (Pl / 2) * 255;
				zcolor = image2.get(Utex, Vtex);
				set(Px, Py, zcolor);
			}

		}


	}

}

void TGAImage::Circle(int x, int y, int R,  TGAColor color)
{
	int a, x1, x2, y1, y2;
	x2 = x + R; y2 = y;
	for (int a = 1; a <= 360; a++) {
		x1 = x2; y1 = y2;
		int xr = floor(R*cos(a));
		x2 = xr + x;
		y2 = floor(R*sin(a)) + x;
		line(x1, y1, x2, y2, color);
	}
}
void TGAImage::NCircle(int x0, int y0, int R, TGAColor color)
{

	float d, x, y;

	for (int x = 0; x <= R / sqrt(2); x++) {
		int y = (int)(sqrt(R*R - x*x));
		{
			set(x0 + x, y0 + y, color);
			set(x0 + y, y0 + x, color);
			set(x0 + y, y0 - x, color);
			set(x0 + x, y0 - y, color);
			set(x0 - x, y0 - y, color);
			set(x0 - y, y0 - x, color);
			set(x0 - y, y0 + x, color);
			set(x0 - x, y0 + y, color);
		}
	}
}
void TGAImage::BCircle(int x0, int y0, int R, TGAColor color)
{

	int d, x, y;

	d=1-R; x=0; y=R;

		set(x0 - R, y0, color);
		set(x0 + R, y0, color);
		set(x0, y0 + R, color);
		set(x0, y0 - R, color);

	do{		
		set(x0 + x, y0 - y, color);		
		if (d<0)
		d+=2*x+3;
		else{
		d+=2*(x-y)+5;
		--y;
		}
		++x;

		set(x0 + x, y0 - y, color);
		set(x0 - x, y0 + y, color);
		set(x0 + x, y0 + y, color);
		set(x0 - x, y0 - y, color);

		set(x0 - y, y0 + x, color);
		set(x0 + y, y0 - x, color);
		set(x0 + y, y0 + x, color);
		set(x0 - y, y0 - x, color);


	}while(x <= y);
	
	
}
bool TGAImage::whichcircle()
{
	
	cout << "1-circle, 2- parametric wide circle, 3-Bresenham's circle:" << endl;
	int c = 0;
	cin >> c;

	cout << "Enter the x, y center of circle (0 to 500):" << endl;
	float x0, y0;
	cin >> x0 >> y0;
	cout << "Enter the R radius (0 to 250):" << endl;
	float R;
	cin >> R;

	cout << "Enter the color (1-white or 2-red):" << endl;
	int col;
	cin >> col;
	TGAColor color;

	switch (col)
	{
	case 1: color = white;
		break;
	case 2: color = red;
		break;
	}

	switch (c)
	{
	case 1:
	{
		NCircle(x0, y0, R, color);
	}
	break;
	case 2:
	{
		Circle(x0, y0, R, color);
	}
	break;
	case 3:
	{
		BCircle(x0, y0, R, color);
	}
	break;
	}
	return true;
}


bool TGAImage::scale(int w, int h) {
	if (w<=0 || h<=0 || !data) return false;
	unsigned char *tdata = new unsigned char[w*h*bytespp];
	int nscanline = 0;
	int oscanline = 0;
	int erry = 0;
	unsigned long nlinebytes = w*bytespp;
	unsigned long olinebytes = width*bytespp;
	for (int j=0; j<height; j++) {
		int errx = width-w;
		int nx   = -bytespp;
		int ox   = -bytespp;
		for (int i=0; i<width; i++) {
			ox += bytespp;
			errx += w;
			while (errx>=(int)width) {
				errx -= width;
				nx += bytespp;
				memcpy(tdata+nscanline+nx, data+oscanline+ox, bytespp);
			}
		}
		erry += h;
		oscanline += olinebytes;
		while (erry>=(int)height) {
			if (erry>=(int)height<<1) 
				memcpy(tdata+nscanline+nlinebytes, tdata+nscanline, nlinebytes);
			erry -= height;
			nscanline += nlinebytes;
		}
	}
	delete [] data;
	data = tdata;
	width = w;
	height = h;
	return true;
}

