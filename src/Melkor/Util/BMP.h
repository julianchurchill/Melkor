#ifndef BMP_H
#define BMP_H

#pragma warning( disable : 4786 )

#include <iostream>
#include <vector>
using namespace std;

class myRGB
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	myRGB() : r(0), g(0), b(0) {};
	myRGB(unsigned char r1, unsigned char g1, unsigned char b1) : r(r1), g(g1), b(b1) {};
};

class BMP
{
	private:

		// BMP File Header
		typedef struct {
			unsigned short int type;                 /* Magic identifier            */
			unsigned int size;                       /* File size in bytes          */
			unsigned short int reserved1, reserved2;
			unsigned int offset;                     /* Offset to image data, bytes */
		} HEADER;
		
		typedef struct {
			unsigned int size;               /* Header size in bytes      */
			int width,height;                /* Width and height of image */
			unsigned short int planes;       /* Number of colour planes   */
			unsigned short int bits;         /* Bits per pixel            */
			unsigned int compression;        /* Compression type          */
			unsigned int imagesize;          /* Image size in bytes       */
			int xresolution,yresolution;     /* Pixels per meter          */
			unsigned int ncolours;           /* Number of colours         */
			unsigned int importantcolours;   /* Important colours         */
		} INFOHEADER;

		HEADER header;
		INFOHEADER infoheader;

		vector<myRGB> data;

		int dataSize;
		int scanLinePad;
	
	public:

		// only allow 24bpp images to be created
		BMP() : data(NULL), dataSize(0) {};
		BMP(const BMP& b)
		{
			header = b.header;
			infoheader = b.infoheader;
			scanLinePad = b.scanLinePad;
			dataSize = b.dataSize;
			data = b.data;
		}
		virtual ~BMP() {};

		BMP& operator=(const BMP &b)
		{
			if(this!=&b)
			{
				header = b.header;
				infoheader = b.infoheader;
				scanLinePad = b.scanLinePad;
				dataSize = b.dataSize;
				data = b.data;
			}
			return *this;
		}

		bool create(int bitsPerPixel, int width, int height)
		{
			if(bitsPerPixel!=24)
				return false;
			// 24-bit colour therefore no colour palette required
			infoheader.bits = 24;

			// Setup headers

			// must be 'BM' 
			header.type = 'MB';
			header.reserved1 = 0;
			header.reserved2 = 0;
			// Offset from beginning of file to start of pixel data
			// i.e. sizeof(HEADER)+sizeof(INFOHEADER) if have no colour palette
			header.offset = sizeof(HEADER)+sizeof(INFOHEADER);
			// watch out for the compiler padding our struct
			if(sizeof(HEADER)==16) header.offset-=2;
			
			infoheader.height = height;
			infoheader.width = width;

			// work out how many bytes we need to pad each scan line with
			// for the given width and bytesPerPixel
			scanLinePad = 0;
			if(((infoheader.width*bytesPerPixel())%4)!=0)
				scanLinePad = 4-((infoheader.width*bytesPerPixel())%4);
			// file size in bytes
			header.size = header.offset + (abs(infoheader.height)*infoheader.width*bytesPerPixel()) 
				+ scanLinePad*infoheader.height;
			
			// no compression
			infoheader.compression = 0;
			infoheader.planes = 1;
			// header size in bytes
			infoheader.size = sizeof(INFOHEADER);
			// image size in bytes, can be 0 for 24-bit mode
			infoheader.imagesize = 0;
			// number of colours used, if 0 use all colours
			// specified by bits argument
			infoheader.ncolours = 0;
			// 0 = all colours are important
			infoheader.importantcolours = 0;
			infoheader.xresolution = 0;
			infoheader.yresolution = 0;

			// Setup data memory
			dataSize = width*height;
			data.resize(dataSize);
			vector<myRGB>::iterator iter = data.begin();
			for(;iter!=data.end();iter++)
				(*iter).r = (*iter).g = (*iter).b = 0;
			return true;
		}

		int getWidth() { return infoheader.width; };
		int getHeight() { return infoheader.height; };

		int bytesPerPixel() const
		{
			if(infoheader.bits==8)
				return 1;
			else if(infoheader.bits==16)
				return 2;
			else if(infoheader.bits==24)
				return 3;
			else
				throw "Unknown number of bytes per pixel";
		}

		// index is the row, remember image is stored bottom-up
		// so index 0 indicates the last line in the image
		bool getScanLine(vector<myRGB>& scanLineContents, int index)
		{
			if(index>=infoheader.height)
				return false;
			int start = index*infoheader.width;
			int end = start+infoheader.width;
			scanLineContents.clear();
			vector<myRGB>::const_iterator citer = data.begin()+start;
			for(;citer!=data.begin()+end;citer++)
				scanLineContents.push_back(*citer);
			return true;
		}
		bool setScanLine(const vector<myRGB>& scanLineContents, int index)
		{
			if(index>=infoheader.height)
				return false;
			int start = index*infoheader.width;
			int end = start+infoheader.width;
			vector<myRGB>::iterator iter = data.begin()+start;
			vector<myRGB>::const_iterator citer = scanLineContents.begin();
			for(;iter!=data.begin()+end;iter++)
				*iter = (*citer++);
			return true;
		}

		// only load 24bpp images
		bool load(const char* filename)
		{
			ifstream in(filename, ios::binary);
			if(!in)
			{
				cout << "Could not open file for loading: " << filename << endl;
				return false;
			}

			// load header
			in.read(reinterpret_cast<char*>(&header.type), sizeof(unsigned short int));
			in.read(reinterpret_cast<char*>(&header.size), sizeof(unsigned int));
			in.read(reinterpret_cast<char*>(&header.reserved1), sizeof(unsigned short int));
			in.read(reinterpret_cast<char*>(&header.reserved2), sizeof(unsigned short int));
			in.read(reinterpret_cast<char*>(&header.offset), sizeof(unsigned int));
			
			// load infoheader
			in.read(reinterpret_cast<char*>(&infoheader.size), sizeof(unsigned int));
			in.read(reinterpret_cast<char*>(&infoheader.width), sizeof(int));
			in.read(reinterpret_cast<char*>(&infoheader.height), sizeof(int));
			in.read(reinterpret_cast<char*>(&infoheader.planes), sizeof(unsigned short int));
			in.read(reinterpret_cast<char*>(&infoheader.bits), sizeof(unsigned short int));
			in.read(reinterpret_cast<char*>(&infoheader.compression), sizeof(unsigned int));
			in.read(reinterpret_cast<char*>(&infoheader.imagesize), sizeof(unsigned int));
			in.read(reinterpret_cast<char*>(&infoheader.xresolution), sizeof(int));
			in.read(reinterpret_cast<char*>(&infoheader.yresolution), sizeof(int));
			in.read(reinterpret_cast<char*>(&infoheader.ncolours), sizeof(unsigned int));
			in.read(reinterpret_cast<char*>(&infoheader.importantcolours), sizeof(unsigned int));

			if(infoheader.bits!=24)
			{
				cout << "This version of the BMP class can only handle 24bpp bitmap images, not ";
				cout << infoheader.bits << "bpp images." << endl;
				return false;
			}

			// load data, removing extra scan line padding
			scanLinePad = 0;
			if(((infoheader.width*bytesPerPixel())%4)!=0)
				scanLinePad = 4-((infoheader.width*bytesPerPixel())%4);
			dataSize = infoheader.width*infoheader.height;
			data.resize(dataSize);

			// read one scanline at a time
			vector<myRGB>::iterator iter = data.begin();
			unsigned char b;
			int byteCount = 0;
			for(int i=0;i<infoheader.height;i++)
			{
				for(int j=0;j<infoheader.width;j++)
				{
					// myRGB values are stored in reverse order BGR...
					in.read(reinterpret_cast<char*>(&(*iter).b), sizeof(unsigned char));
					in.read(reinterpret_cast<char*>(&(*iter).g), sizeof(unsigned char));
					in.read(reinterpret_cast<char*>(&(*iter).r), sizeof(unsigned char));
					byteCount+=3;
					iter++;
				}
				// now read in the scan line padding bytes and discard
				for(j=0;j<scanLinePad;j++)
				{
					in.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
					byteCount++;
				}
			}

			return true;
		}

		bool save(const char* filename)
		{
			ofstream fos(filename, ios::binary);
			if(!fos)
			{
				cout << "Could not open or create file for save: " << filename << endl;
				return false;
			}

			// save header
			fos.write(reinterpret_cast<char*>(&header.type), sizeof(unsigned short int));
			fos.write(reinterpret_cast<char*>(&header.size), sizeof(unsigned int));
			fos.write(reinterpret_cast<char*>(&header.reserved1), sizeof(unsigned short int));
			fos.write(reinterpret_cast<char*>(&header.reserved2), sizeof(unsigned short int));
			fos.write(reinterpret_cast<char*>(&header.offset), sizeof(unsigned int));
			
			// save infoheader
			fos.write(reinterpret_cast<char*>(&infoheader.size), sizeof(unsigned int));
			fos.write(reinterpret_cast<char*>(&infoheader.width), sizeof(int));
			fos.write(reinterpret_cast<char*>(&infoheader.height), sizeof(int));
			fos.write(reinterpret_cast<char*>(&infoheader.planes), sizeof(unsigned short int));
			fos.write(reinterpret_cast<char*>(&infoheader.bits), sizeof(unsigned short int));
			fos.write(reinterpret_cast<char*>(&infoheader.compression), sizeof(unsigned int));
			fos.write(reinterpret_cast<char*>(&infoheader.imagesize), sizeof(unsigned int));
			fos.write(reinterpret_cast<char*>(&infoheader.xresolution), sizeof(int));
			fos.write(reinterpret_cast<char*>(&infoheader.yresolution), sizeof(int));
			fos.write(reinterpret_cast<char*>(&infoheader.ncolours), sizeof(unsigned int));
			fos.write(reinterpret_cast<char*>(&infoheader.importantcolours), sizeof(unsigned int));

			// save data, adding extra scan line padding
			vector<myRGB>::iterator iter = data.begin();
			for(int i=0;i<infoheader.height;i++)
			{
				for(int j=0;j<infoheader.width;j++)
				{
					// myRGB values are stored in reverse order BGR...
					fos.write(reinterpret_cast<char*>(&(*iter).b), sizeof(unsigned char));
					fos.write(reinterpret_cast<char*>(&(*iter).g), sizeof(unsigned char));
					fos.write(reinterpret_cast<char*>(&(*iter).r), sizeof(unsigned char));
					iter++;
				}
				// now write the scan line padding bytes
				unsigned char b = 0;
				for(j=0;j<scanLinePad;j++)
					fos.write(reinterpret_cast<char*>(&b), sizeof(unsigned char));
			}

			return true;
		}

		void printInfo()
		{
			cout << "HEADER" << endl;
			cout << "Type: " << header.type << " should be " << 'M'*256+'B' << endl;
			cout << "Size: " << header.size << endl;
			cout << "Reserved1: " << header.reserved1 << endl;
			cout << "Reserved2: " << header.reserved2 << endl;
			cout << "Offset: " << header.offset << endl;
			
			cout << "INFOHEADER" << endl;
			cout << "Size: " << infoheader.size << endl;
			cout << "Width: " << infoheader.width << endl;
			cout << "Height: " << infoheader.height << endl;
			cout << "Planes: " << infoheader.planes << endl;
			cout << "Bits: " << infoheader.bits << endl;
			cout << "Compression: " << infoheader.compression << endl;
			cout << "Imagesize: " << infoheader.imagesize << endl;
			cout << "XResolution: " << infoheader.xresolution << endl;
			cout << "YResolution: " << infoheader.yresolution << endl;
			cout << "NColours: " << infoheader.ncolours << endl;
			cout << "ImportantColours: " << infoheader.importantcolours << endl;
		}
};

#endif