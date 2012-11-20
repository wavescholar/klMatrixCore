#ifndef KL_IMAGE_BUFFER_H
#define KL_IMAGE_BUFFER_H
#include "kl_util.h"
#include <deque>
#include <string>
#include <map>
#include <limits>

#ifdef _WIN64
 #include <malloc.h>
#endif

class klRect{
public:
	klRect(int ix,int iy,unsigned int iw,unsigned int ih)
		: x(ix), y(iy),w(iw),h(ih)
	{
	}
	int x;
	int y;

	unsigned int w;
	unsigned int h;
};

class klRasterBuffer {
public:
 
    virtual const unsigned char * buffer() const = 0;
  
	virtual unsigned char * buffer() = 0;
   
	virtual size_t numBands() const = 0;
   
	virtual size_t width() const = 0;
  
	virtual size_t height() const = 0;

    virtual size_t sampleBits() const = 0;

    virtual size_t bandStrideBits() const = 0;
 
	virtual size_t xStrideBits() const = 0;

    virtual size_t yStrideBits() const = 0;

    virtual ~klRasterBuffer() {};
   
	inline size_t bandStride() const { return bandStrideBits() / bitsPerByte(); }
   
	inline size_t xStride() const { return xStrideBits() / bitsPerByte(); }
    
	inline size_t yStride() const { return yStrideBits() / bitsPerByte(); }

protected:
    static inline size_t bitsPerByte() { return std::numeric_limits<unsigned char>::digits; }
};

class klReferenceCountedRasterBuffer : public klRasterBuffer, public klRefCount<klMutex>
{
public :
    klReferenceCountedRasterBuffer(){};
};

typedef klSmartPtr<klReferenceCountedRasterBuffer> klRasterBufferReference;

typedef klSmartPtr<klReferenceCountedRasterBuffer> klRasterBufferPointer;

klRasterBufferReference createRasterBuffer(
    unsigned char *buffer,  // caller owns
    size_t sampleBits,
    size_t bandStrideBits,
    size_t xStrideBits,
    size_t yStrideBits,
    size_t numBands,
    size_t width,
    size_t height
);

// A klRasterBuffer that does not have any ownership of the buffer.
class klUnmanagedRasterBuffer : public klReferenceCountedRasterBuffer {
public:
    klUnmanagedRasterBuffer(
        unsigned char *buffer,
        size_t sampleBits,
        size_t bandStrideBits,
        size_t xStrideBits,
        size_t yStrideBits,
        size_t numBands,
        size_t width,
        size_t height
    ) : m_buffer(buffer),
        m_sampleBits(sampleBits),
        m_numBands(numBands),
        m_width(width),
        m_height(height),
        m_bandStrideBits(bandStrideBits),
        m_xStrideBits(xStrideBits),
        m_yStrideBits(yStrideBits)
    {
    }

    const unsigned char *buffer() const { return m_buffer; }
    unsigned char *buffer() { return m_buffer; }

    size_t sampleBits() const { return m_sampleBits; }

    size_t numBands() const { return m_numBands; }
    size_t width() const { return m_width; }
    size_t height() const { return m_height; }

    size_t bandStrideBits() const { return m_bandStrideBits; }
    size_t xStrideBits() const { return m_xStrideBits; }
    size_t yStrideBits() const { return m_yStrideBits; }

private:
    unsigned char * m_buffer;

    size_t m_sampleBits;
    size_t m_numBands;
    size_t m_width;
    size_t m_height;

    size_t m_bandStrideBits;
    size_t m_xStrideBits;
    size_t m_yStrideBits;
};

//A klRasterBuffer that manages ownership of the buffer. 
template<typename Sample> class klPackedHeapRasterBuffer
    : public klReferenceCountedRasterBuffer {
public:
    klPackedHeapRasterBuffer<Sample>(
        size_t width,
        size_t height,
        size_t bands = 1,
        size_t alignment = 32  
    ) : width_(width), height_(height),
        bands_(bands),_ownBuffer(true)
        
    {
		//bbcrevisit - implement with ippMalloc
        alignment=0;
		lineStride_ =  ( sizeof(Sample)*  (width_ * bands_ ));
                      
        buffer_ = new unsigned char[(height_ * lineStride_) ];
        alignedBuffer_ = buffer_ ;

    }

klPackedHeapRasterBuffer<Sample>(
        size_t width,
        size_t height,
        size_t bands ,
		unsigned char * buffer,
        size_t alignment = 32  
    ) : width_(width), height_(height),
        bands_(bands), _ownBuffer(false)
        
    {
		//bbcrevisit - implement with ippMalloc
        alignment=0;
		lineStride_ =  ( sizeof(Sample)*  (width_ * bands_ ));
                      
        buffer_ = buffer;
        alignedBuffer_ = buffer_ ;

    }

    virtual ~klPackedHeapRasterBuffer<Sample>() { delete[] buffer_; }

    virtual const unsigned char *buffer() const { return alignedBuffer_; }
    virtual unsigned char *buffer() { return alignedBuffer_; }

    virtual size_t numBands() const { return bands_; }
    virtual size_t width() const { return width_; }
    virtual size_t height() const { return height_; }

    virtual size_t bandStrideBits() const { return sampleBits(); }
    virtual size_t xStrideBits() const { return numBands() * bandStrideBits(); }
    virtual size_t yStrideBits() const { return lineStride_ * bitsPerByte(); }

    virtual size_t sampleBits() const { return bitsPerByte() * sizeof(Sample); }

private:
	bool _ownBuffer;
    size_t width_;
    size_t height_;
    size_t bands_;
    size_t lineStride_;
    unsigned char * buffer_;
    unsigned char * alignedBuffer_;

};

//A klRasterBuffer that uses IPP to allocate and free the image buffer.
template<typename TYPE> class klIPPRasterBuffer
    : public klReferenceCountedRasterBuffer {
public:
    klIPPRasterBuffer<TYPE>(
        size_t width,
        size_t height,
        size_t bands = 1        
    ) : _width(width), _height(height),
        _bands(bands)
        
    {
		if(typeid(TYPE)== typeid(float))
		{
			if(_bands ==1)
			{
				_buffer = (unsigned char*) ippiMalloc_32f_C1( _width, height, &_lineStride );
			}
			if(_bands==3)
			{
				_buffer = (unsigned char*)  ippiMalloc_32f_C3( _width, height, &_lineStride );
			}
		}


		if(typeid(TYPE)== typeid(unsigned char))
		{
			if(_bands ==1)
			{
				_buffer = (unsigned char*)  ippiMalloc_8u_C1( _width, height, &_lineStride );
			}
			if(_bands==3)
			{
				_buffer = (unsigned char*)  ippiMalloc_8u_C3( _width, height, &_lineStride );
			}
		}

    }
    virtual ~klIPPRasterBuffer<TYPE>() {ippiFree(_buffer); }

    virtual const unsigned char *buffer() const { return _buffer; }
    virtual unsigned char *buffer() { return _buffer; }

    virtual size_t numBands() const { return _bands; }
    virtual size_t width() const { return _width; }
    virtual size_t height() const { return _height; }

    virtual size_t bandStrideBits() const { return sampleBits(); }
    virtual size_t xStrideBits() const { return numBands() * bandStrideBits(); }
    virtual size_t yStrideBits() const { return _lineStride * bitsPerByte(); }

    virtual size_t sampleBits() const { return bitsPerByte() * sizeof(TYPE); }

private:
    int _width;
    int _height;
    int _bands;
    int _lineStride;
    unsigned char * _buffer;

};


#endif // #define KL_IMAGE_BUFFER_H189