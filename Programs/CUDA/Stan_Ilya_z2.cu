#include <iostream>
#include <fstream>
#define SQR(x) ((float)(x) * (float)(x))
#define bufSize 1024 * 1024
#define BLUR_PARAM 40
using namespace std;

// CIEXYZTRIPLE stuff
typedef int FXPT2DOT30;

struct CIEXYZ
{
	FXPT2DOT30 ciexyzX;
	FXPT2DOT30 ciexyzY;
	FXPT2DOT30 ciexyzZ;
};

struct CIEXYZTRIPLE
{
	CIEXYZ ciexyzRed;
	CIEXYZ ciexyzGreen;
	CIEXYZ ciexyzBlue;
};

// bitmap file header
struct BITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
};

// bitmap info header
struct BITMAPINFOHEADER
{
	unsigned int biSize;
	unsigned int biWidth;
	unsigned int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
	unsigned int biRedMask;
	unsigned int biGreenMask;
	unsigned int biBlueMask;
	unsigned int biAlphaMask;
	unsigned int biCSType;
	CIEXYZTRIPLE biEndpoints;
	unsigned int biGammaRed;
	unsigned int biGammaGreen;
	unsigned int biGammaBlue;
	unsigned int biIntent;
	unsigned int biProfileData;
	unsigned int biProfileSize;
	unsigned int biReserved;
};

// rgb quad
struct RGBQUAD
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
};

// read bytes
template <typename Type>
void read(std::ifstream &fp, Type &result, std::size_t size)
{
	fp.read(reinterpret_cast<char *>(&result), size);
}

template <typename Type>
void write(std::ofstream &fp, Type par, std::size_t size)
{
	fp.write(reinterpret_cast<char *>(&par), size);
}

unsigned char bitextract(const unsigned int byte, const unsigned int mask);
void readBMP(int argc, char **argv, int *&rgbInfo, BITMAPINFOHEADER &fileInfoHeader);
void writeBMP(char **argv, int *rgbInfo, BITMAPINFOHEADER fileInfoHeader);

texture<int, 2, cudaReadModeElementType> texRef;
__global__ void bilateralBlur(int *filteredImage, int W, int r)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int idy = blockIdx.y * blockDim.y + threadIdx.y;
	float sum0 = 0.0f, sum1 = 0.0f, sum2 = 0.0f, sum3 = 0.0f;
	float result0 = 0.0f, result1 = 0.0f, result2 = 0.0f, result3 = 0.0f;
	float c0, c1, c2, c3, cl0, cl1, cl2, cl3;
	int mask0 = 0xff, mask1 = 0xff00, mask2 = 0xff0000, mask3 = 0xff000000;
	int c = tex2D(texRef, idx, idy);

	int clr;
	c0 = (c & mask0) / 255.0f;
	c1 = ((unsigned int)(c & mask1) >> 8) / 255.0f;
	c2 = ((unsigned int)(c & mask2) >> 16) / 255.0f;
	c3 = ((unsigned int)(c & mask3) >> 24) / 255.0f;
	for (int ix = -r; ix <= r; ++ix)
		for (int iy = -r; iy <= r; ++iy)
		{
			clr = tex2D(texRef, idx + ix, idy + iy);
			cl0 = (float)(clr & mask0) / 255.0f;
			cl1 = (float)((unsigned int)(clr & mask1) >> 8) / 255.0f;
			cl2 = (float)((unsigned int)(clr & mask2) >> 16) / 255.0f;
			cl3 = (float)((unsigned int)(clr & mask3) >> 24) / 255.0f;
			float w0 = expf((-SQR(ix) - SQR(iy) - SQR(cl0 - c0)) / SQR(r));
			float w1 = expf((-SQR(ix) - SQR(iy) - SQR(cl1 - c1)) / SQR(r));
			float w2 = expf((-SQR(ix) - SQR(iy) - SQR(cl2 - c2)) / SQR(r));
			float w3 = expf((-SQR(ix) - SQR(iy) - SQR(cl3 - c3)) / SQR(r));
			result0 += w0 * cl0;
			result1 += w1 * cl1;
			result2 += w2 * cl2;
			result3 += w3 * cl3;
			sum0 += w0;
			sum1 += w1;
			sum2 += w2;
			sum3 += w3;
		}
	result0 /= sum0;
	result1 /= sum1;
	result2 /= sum2;
	result3 /= sum3;
	filteredImage[idx + idy * W] = ((unsigned int)(result3 * 255.0f) << 24) + ((unsigned int)(result2 * 255.0f) << 16) + ((unsigned int)(result1 * 255.0f) << 8) + (unsigned int)(result0 * 255.0f);
}

BITMAPFILEHEADER fileHeader;

int main(int argc, char *argv[])
{
	int *rgbInfo, *devPtr;
	BITMAPINFOHEADER fileInfoHeader;
	readBMP(argc, argv, rgbInfo, fileInfoHeader);
	unsigned long int pitch;
	// Здесь у нас имеется одномерный массив цветов пикселей в формате rgb
	// Применить ядро (Возможно, разделить картинку на равные части и в цикле загружать части картинки, и обрабатывать - как целое)
	// Записать новую картинку+
	cudaMallocPitch((void **)&devPtr, &pitch, fileInfoHeader.biWidth * sizeof(int), fileInfoHeader.biHeight);
	cudaBindTexture2D(nullptr, &texRef, (void *)devPtr, &texRef.channelDesc, fileInfoHeader.biWidth, fileInfoHeader.biHeight, pitch);
	cudaMemcpy2D(devPtr, pitch, rgbInfo, fileInfoHeader.biWidth * sizeof(int), fileInfoHeader.biWidth * sizeof(int), fileInfoHeader.biHeight, cudaMemcpyHostToDevice);
	//разделение
	

	bilateralBlur<<<dim3(fileInfoHeader.biWidth/30 + 1,fileInfoHeader.biHeight / 30 + 1,1), dim3(30,30,1)>>>(devPtr, pitch / 4, BLUR_PARAM);
	
	cudaMemcpy2D(rgbInfo, fileInfoHeader.biWidth * sizeof(int), devPtr, pitch, fileInfoHeader.biWidth * sizeof(int), fileInfoHeader.biHeight, cudaMemcpyDefault);

	writeBMP(argv, rgbInfo, fileInfoHeader);

	delete[] rgbInfo;
	cudaFree(devPtr);
	return 0;
}








unsigned char bitextract(const unsigned int byte, const unsigned int mask)
{
	if (mask == 0)
	{
		return 0;
	}

	// определение количества нулевых бит справа от маски
	unsigned int
		maskBufer = mask,
		maskPadding = 0;

	while (!(maskBufer & 1))
	{
		maskBufer >>= 1;
		maskPadding++;
	}

	// применение маски и смещение
	return (unsigned int)(byte & mask) >> maskPadding;
}

void writeBMP(char **argv, int *rgbInfo, BITMAPINFOHEADER fileInfoHeader)
{ // полный rgbInfo сюда заносим
	char *fileName = argv[2];
	std::ofstream fileStream;
	fileStream.open(fileName, std::ifstream::binary);
	if (!fileStream)
	{
		std::cout << "Error opening file '" << fileName << "'." << std::endl;
		exit(0);
	}
	write(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
	write(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
	write(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	write(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	write(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));
	//.................................................

	// информация изображения
	write(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

	// bmp core
	if (fileInfoHeader.biSize >= 12)
	{
		write(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
		write(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
		write(fileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
		write(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
	}

	// получаем информацию о битности
	int colorsCount = (unsigned)fileInfoHeader.biBitCount >> 3;
	if (colorsCount < 3)
	{
		colorsCount = 3;
	}

	int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
	int maskValue = (1 << bitsOnColor) - 1;

	// bmp v1
	if (fileInfoHeader.biSize >= 40)
	{
		write(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
		write(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
		write(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
		write(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
		write(fileStream, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
		write(fileStream, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
	}

	// bmp v2
	fileInfoHeader.biRedMask = 0;
	fileInfoHeader.biGreenMask = 0;
	fileInfoHeader.biBlueMask = 0;

	if (fileInfoHeader.biSize >= 52)
	{
		write(fileStream, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
		write(fileStream, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
		write(fileStream, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
	}

	// если маска не задана, то ставим маску по умолчанию
	if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0)
	{
		fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
		fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
		fileInfoHeader.biBlueMask = maskValue;
	}

	// bmp v3
	if (fileInfoHeader.biSize >= 56)
	{
		write(fileStream, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
	}
	else
	{
		fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
	}

	// bmp v4
	if (fileInfoHeader.biSize >= 108)
	{
		write(fileStream, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
		write(fileStream, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
		write(fileStream, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
		write(fileStream, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
		write(fileStream, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
	}

	// bmp v5
	if (fileInfoHeader.biSize >= 124)
	{
		write(fileStream, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
		write(fileStream, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
		write(fileStream, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
		write(fileStream, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
	}
	int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;
	for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++)
	{
		for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++)
		{
			write(fileStream, rgbInfo[i * fileInfoHeader.biWidth + j], 3);
		}
		for (int j = 0; j < linePadding; j++)
			write(fileStream, 0, sizeof(int));
	}
}

void readBMP(int argc, char **argv, int *&rgbInfo, BITMAPINFOHEADER &fileInfoHeader)
{
	unsigned char rgbRed, rgbGreen, rgbBlue, rgbReserved;
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " file_name" << std::endl;
		exit(0);
	}

	char *fileName = argv[1];

	// открываем файл
	std::ifstream fileStream(fileName, std::ifstream::binary);
	if (!fileStream)
	{
		std::cout << "Error opening file '" << fileName << "'." << std::endl;
		exit(0);
	}

	// заголовок изображения
	read(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
	read(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
	read(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	read(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	read(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

	if (fileHeader.bfType != 0x4D42)
	{
		std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
		exit(0);
	}

	// информация изображения
	read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

	// bmp core
	if (fileInfoHeader.biSize >= 12)
	{
		read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
		read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
		read(fileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
		read(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
	}

	// получаем информацию о битности
	int colorsCount = fileInfoHeader.biBitCount >> 3;
	if (colorsCount < 3)
	{
		colorsCount = 3;
	}

	int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
	int maskValue = (1 << bitsOnColor) - 1;

	if (fileInfoHeader.biSize >= 40)
	{
		read(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
		read(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
		read(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
		read(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
		read(fileStream, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
		read(fileStream, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
	}

	// bmp v2
	fileInfoHeader.biRedMask = 0;
	fileInfoHeader.biGreenMask = 0;
	fileInfoHeader.biBlueMask = 0;

	if (fileInfoHeader.biSize >= 52)
	{
		read(fileStream, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
		read(fileStream, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
		read(fileStream, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
	}

	// если маска не задана, то ставим маску по умолчанию
	if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0)
	{
		fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
		fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
		fileInfoHeader.biBlueMask = maskValue;
	}

	// bmp v3
	if (fileInfoHeader.biSize >= 56)
	{
		read(fileStream, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
	}
	else
	{
		fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
	}

	// bmp v4
	if (fileInfoHeader.biSize >= 108)
	{
		read(fileStream, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
		read(fileStream, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
		read(fileStream, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
		read(fileStream, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
		read(fileStream, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
	}

	// bmp v5
	if (fileInfoHeader.biSize >= 124)
	{
		read(fileStream, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
		read(fileStream, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
		read(fileStream, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
		read(fileStream, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
	}

	// проверка на поддерку этой версии формата
	if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
		fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124)
	{
		std::cout << "Error: Unsupported BMP format." << std::endl;
		exit(0);
	}

	if (fileInfoHeader.biBitCount != 16 && fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32)
	{
		std::cout << "Error: Unsupported BMP bit count." << std::endl;
		exit(0);
	}

	if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3)
	{
		std::cout << "Error: Unsupported BMP compression." << std::endl;
		exit(0);
	}

	// rgb info
	rgbInfo = new int[fileInfoHeader.biHeight * fileInfoHeader.biWidth];

	// определение размера отступа в конце каждой строки
	int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

	// чтение
	unsigned int bufer;
	for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++)
	{
		for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++)
		{
			read(fileStream, bufer, fileInfoHeader.biBitCount / 8);
			rgbRed = bitextract(bufer, fileInfoHeader.biRedMask);
			rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
			rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
			rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);
			rgbInfo[i * fileInfoHeader.biWidth + j] = (rgbReserved << 24) + (rgbRed << 16) + (rgbGreen << 8) + rgbBlue;
		}
		fileStream.seekg(linePadding, std::ios_base::cur);
	}
}
