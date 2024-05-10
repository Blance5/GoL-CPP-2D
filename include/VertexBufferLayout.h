#pragma once

#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count; 
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            // 4 4 1
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_BYTE: return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout {
    private:
        std::vector<VertexBufferElement> m_Elements;
        unsigned int m_Stride;

    public:
        VertexBufferLayout() : m_Stride(0) {

        }

        void PrintElements() {
            for (int i = 0; i < m_Elements.size(); i++) {
                std::cout << "Element " << i << ":\n";
                std::cout << "Type: " << m_Elements[i].type << "\n";
                std::cout << "Count: " << m_Elements[i].count << "\n";
                std::cout << "Normalized: " << (int) m_Elements[i].normalized << "\n" << std::endl;
            }
        }

        // test
        void Push(unsigned int count, unsigned int gl_type) {
            switch (gl_type) {
                case GL_FLOAT:
                    m_Elements.push_back(VertexBufferElement{GL_FLOAT, count, GL_FALSE});
                    m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
                    break;
                case GL_UNSIGNED_INT:
                    m_Elements.push_back(VertexBufferElement{GL_UNSIGNED_INT, count, GL_FALSE});
                    m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
                    break;
                case GL_UNSIGNED_BYTE:
                    m_Elements.push_back(VertexBufferElement{GL_UNSIGNED_BYTE, count, GL_TRUE});
                    m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
                    break;
            }
        }


        // is & correct?

        // MAYBE RETURN const and not &
        inline const std::vector<VertexBufferElement> GetElements() const {return m_Elements;}
        inline unsigned int GetStride() const {return m_Stride;}
};

/*
template<>
void VertexBufferLayout::Push<float>(int count);

template<>
void VertexBufferLayout::Push<unsigned int>(int count);

template<>
void VertexBufferLayout::Push<unsigned char>(int count);
*/