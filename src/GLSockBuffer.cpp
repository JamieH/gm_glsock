#include "Common.h"
#include "GLSockBuffer.h"
#include "BufferMgr.h"

namespace GLSockBuffer {

CGLSockBuffer::CGLSockBuffer( const char* pData, unsigned int cubBuffer )
{
	m_nOffset = 0;	
	m_nReferences = 0;

	m_Buf.assign(pData, cubBuffer);
}

CGLSockBuffer::CGLSockBuffer( void )
{
	m_nOffset = 0;
	m_nReferences = 0;
}

CGLSockBuffer::~CGLSockBuffer( void )
{
}

unsigned int CGLSockBuffer::Write( const char* pData, unsigned int cubBuffer )
{
	try
	{
		unsigned int nSpaceLeft = (m_Buf.size() - m_nOffset);
		if( nSpaceLeft < cubBuffer )
		{
			m_Buf.resize(m_Buf.size() + (cubBuffer - nSpaceLeft));
		}

		memcpy((char*)m_Buf.c_str() + m_nOffset, pData, cubBuffer);
		m_nOffset += cubBuffer;
	}
	catch (std::exception& ex)
	{
		UNREFERENCED_PARAM(ex);
		return 0;
	}
	return cubBuffer;
}

unsigned int CGLSockBuffer::Read( char* pData, unsigned int cubBuffer, bool bMatchSize)
{
	unsigned int nRead = cubBuffer;

	try
	{
		if( m_nOffset + nRead > m_Buf.size() )
		{
			if( bMatchSize )
				return 0;
			nRead = m_Buf.size() - m_nOffset;
		}

		memcpy(pData, m_Buf.c_str() + m_nOffset, nRead);
		m_nOffset += nRead;
	}
	catch (std::exception& ex)
	{
		UNREFERENCED_PARAM(ex);
		return 0;
	}

	return nRead;
}

bool CGLSockBuffer::Seek( unsigned int nOffset, unsigned int nMethod )
{
	switch(nMethod)
	{
	case SOCKBUFFER_SEEK_SET:
		{
			if( nOffset > m_Buf.size() )
				return false;
			m_nOffset = nOffset;
		}
		break;
	case SOCKBUFFER_SEEK_CUR:
		{
			if( m_nOffset + nOffset > m_Buf.size() )
				return false;
			m_nOffset += nOffset;
		}
		break;
	case SOCKBUFFER_SEEK_END:
		{
			if( nOffset > m_Buf.size() )
				return false;
			m_nOffset = m_Buf.size() - nOffset;
		}
		break;
	default:
		return false;
	}

	return true;
}

unsigned int CGLSockBuffer::Tell( void )
{
	return m_nOffset;
}

unsigned int CGLSockBuffer::Size( void )
{
	return m_Buf.size();
}

const char* CGLSockBuffer::Buffer( void )
{
	return m_Buf.c_str();
}

bool CGLSockBuffer::EOB()
{
	return m_nOffset == m_Buf.size();
}

bool CGLSockBuffer::Empty()
{
	return m_Buf.empty();
}

bool CGLSockBuffer::Clear( unsigned int nPos, unsigned int cSize )
{
	try
	{
		m_Buf.erase(nPos, cSize);
	}
	catch(std::exception& ex)
	{
		UNREFERENCED_PARAM(ex);
		return false;
	}
	return true;
}

int CGLSockBuffer::Reference()
{
	m_nReferences++;
	return m_nReferences;
}

int CGLSockBuffer::Unreference()
{
	m_nReferences--;
	if( m_nReferences <= 0 )
	{
		g_pBufferMgr->Remove(this);
		delete this;
	}
	return m_nReferences;
}


} // GLSockBuffer
