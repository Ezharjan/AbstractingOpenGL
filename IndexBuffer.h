#pragma once

class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();
	
	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const {return m_Count;} // Set a getter after the set of m_Count(count)

private:
	unsigned int m_RendererID;
	unsigned int m_Count;
};
