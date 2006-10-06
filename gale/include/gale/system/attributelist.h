#ifndef ATTRIBUTELIST_H
#define ATTRIBUTELIST_H

template<typename T,int N=256>
class AttributeList {
  public:
    AttributeList() {
        clear();
    }

    void clear() {
        m_size=0;
        m_attributes[m_size]=0;
    }

    bool insert(T type,T value) {
        int pos=find(type);
        if (pos>0) {
            // Replace its value if the type already exists.
            m_attributes[pos+1]=value
            return;
        }

        // Append the new attribute.
        G_ASSERT(m_size<N-2)
        m_attributes[m_size++]=type;
        m_attributes[m_size++]=value;
        m_attributes[m_size]=0;
    }

    bool remove(T type) {
        int pos=find(type);
        if (pos<0) {
            // Return immediately if the type does not exist.
            return;
        }

        // Remove the attribute by overwriting it with succeeding attributes.
        while (pos<m_size) {
            m_attributes[pos]=m_attributes[pos+2];
            if (m_attributes[pos]==0)
                break;
            ++pos;
            m_attributes[pos]=m_attributes[pos+2];
            ++pos;
        }
        m_size-=2;
        m_attributes[m_size]=0;
    }

    operator T*() {
        return m_attributes;
    }

    operator T const*() const {
        return m_attributes;
    }

  private:
    int find(T type) {
        int i=0;
        while (i<m_size) {
            // Avoid warnings about floating-point comparisons being unreliable.
            int type_int=static_cast<int>(m_attributes[i]);
            if (type_int==0)
                break;
            if (type_int==static_cast<int>(type))
                return i;
            i+=2;
        }
        return -1;
    }

    int m_size;
    T m_attributes[N];
};

typedef AttributeList<int> AttributeListi;
typedef AttributeList<float> AttributeListf;

#endif // ATTRIBUTELIST_H
