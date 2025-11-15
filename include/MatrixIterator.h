template<typename T, T defaultValue>
class SparseMatrixIterator
{
private:
    using RowIterator = typename std::map<size_t, Row<T, defaultValue>>::const_iterator;
    using ColIterator = typename std::map<size_t, T>::const_iterator;
    
    RowIterator m_rowIt;
    RowIterator m_rowEnd;
    ColIterator m_colIt;

public:
    SparseMatrixIterator(RowIterator rowBeg, RowIterator rowEnd) noexcept
        : m_rowIt(rowBeg)
        , m_rowEnd(rowEnd) 
    {
        SkipEmptyRows();
    }

private:
    void SkipEmptyRows() noexcept
	{
        while (m_rowIt != m_rowEnd && m_rowIt->second.empty())
		{
            ++m_rowIt;
        }
        if (m_rowIt != m_rowEnd)
		{
            m_colIt = m_rowIt->second.begin();
        }
    }

public:
    bool operator!=(const SparseMatrixIterator& other) const noexcept
	{
        return m_rowIt != other.m_rowIt;
    }

public:
    std::tuple<size_t, size_t, T> operator*() const noexcept
	{
        return {m_rowIt->first, m_colIt->first, m_colIt->second};
    }

public:
    SparseMatrixIterator& operator++() noexcept
	{
        if (m_rowIt == m_rowEnd)
		{
            return *this;
        }
        
        ++m_colIt;
        if (m_colIt == m_rowIt->second.end())
		{
            ++m_rowIt;
            SkipEmptyRows();
        }
        return *this;
    }

};