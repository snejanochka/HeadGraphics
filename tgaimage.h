#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>
#include "formain.h"
#pragma pack(push,1)
struct TGA_Header {
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)



struct TGAColor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	TGAColor() : val(0), bytespp(1) {
	}

	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {
	}

	TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
	}

	TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) {
	}

	TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
		for (int i=0; i<bpp; i++) {
			raw[i] = p[i];
		}
	}

	TGAColor & operator =(const TGAColor &c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}
};


class TGAImage {
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp;

	bool   load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out);
public:
	enum Format {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
	bool read_tga_file(const char *filename);
	bool write_tga_file(const char *filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);
	bool set(int x, int y, TGAColor c);
	void line(float x1, float y1, float x2, float y2, TGAColor color);
	void BRline(float x1, float y1, float x2, float y2, TGAColor color);
	void Circle(int x, int y, int R, TGAColor color);
	void NCircle(int x0, int y0, int R, TGAColor color);
	void BCircle(int x0, int y0, int R, TGAColor color);
	bool whichcircle();
	bool whichline();

	bool fill(int x1, int y1, int x2, int y2, int x3, int y3, TGAColor color);
	void zbuftriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, TGAColor color, int *zbuffer, bool setflag);
	void guro(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax);
	void superguro(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, matrix4na4no2 M, matrix4na4no2 Ml, matrix3na3 Mmal, int *zbuffer2, float *plmax);
	void phong(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax);
	void texture(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, float nx1, float ny1, float nz1, float nx2, float ny2, float nz2, float nx3, float ny3, float nz3, int lix, int liy, int liz, int newsize, int *zbuffer, float *plmax, float u1, float v1, float u2, float v2, float u3, float v3, TGAImage image2);
	~TGAImage();
	TGAImage & operator =(const TGAImage &img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char *buffer();
	void clear();
};

#endif //__IMAGE_H__
