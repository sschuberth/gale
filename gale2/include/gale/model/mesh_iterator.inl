/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/// An abstract base class that serves as an interface for mesh iterators.
class G_NO_VTABLE Iterator
{
  public:

    Iterator(Mesh const& mesh,int vi=0,int ni=0)
    :   m_mesh(mesh)
    ,   m_num_vertices(mesh.vertices.getSize())
    ,   m_vi(vi)
    ,   m_ni(ni)
    ,   m_neighbors(&mesh.neighbors[m_vi])
    {
        // Cannot call pure virtual methods in a base class' constructor, see
        // http://www.parashift.com/c%2B%2B-faq-lite/strange-inheritance.html#faq-23.5
    }

    /// Returns \c true if the two iterators' vertex and neighbor indices
    /// are equal, \c false otherwise.
    bool operator==(Iterator const& other) const {
        return m_vi==other.m_vi && m_ni==other.m_ni;
    }

    /// Returns \c true if the two iterators' vertex and neighbor indices
    /// are not equal, \c false otherwise.
    bool operator!=(Iterator const& other) const {
        return !(*this==other);
    }

    /// Moves the iterator forward to the next element in the mesh.
    Iterator operator++() {
        do {
            iterate();
        } while (m_vi<m_num_vertices && !next());

        return *this;
    }

  protected:

    /// Iterates to the next vertex / neighbor pair.
    void iterate() {
        if (++m_ni>=m_neighbors->getSize()) {
            m_ni=0;
            m_neighbors=&m_mesh.neighbors[++m_vi];
        }
    }

    /// Determines the element for the current vertex / neighbor, if any.
    virtual bool next()=0;

    Mesh const& m_mesh; ///< The mesh being iterated over.
    int m_num_vertices; ///< Number of vertices in the mesh.

    int m_vi; ///< Index of the first vertex.
    int m_ni; ///< Index of the first vertex' neighbor.

    IndexArray const* m_neighbors; ///< The current vertex' neighbors.
};

/// A class to iterate over the edges in a mesh.
class EdgeIterator:public Iterator
{
  public:

    /// Associates the iterator with the given \a mesh and the edge defined
    /// by vertex index \vi and its neighbor index \a ni.
    EdgeIterator(Mesh const& mesh,int vi=0,int ni=0)
    :   Iterator(mesh,vi,ni)
    {
        // Initialize unless this is the end iterator.
        while (m_vi<m_num_vertices && !next()) {
            iterate();
        }
    }

    /// Returns the index of the edge's first vertex.
    int indexA() const {
        return m_vi;
    }

    /// Returns the index of the edge's second vertex.
    int indexB() const {
        return (*m_neighbors)[m_ni];
    }

    /// Returns the edge's first vertex.
    math::Vec3f const& vertexA() const {
        return m_mesh.vertices[indexA()];
    }

    /// Returns the edge's second vertex.
    math::Vec3f const& vertexB() const {
        return m_mesh.vertices[indexB()];
    }

  private:

    /// Determines whether the current vertex / neighbor pair is a valid edge.
    bool next() {
        // The below break criterion defines a (somewhat arbitrary) relation on
        // the vertex indices to ensure walking each edge only in one direction.
        return m_neighbors->getSize()>0 && indexA()<indexB();
    }
};

/// A class to iterate over the primitives in a mesh. Note that both sides
/// of stand-alone primitives of a (non-manifold) mesh are iterated.
class PrimitiveIterator:public Iterator
{
  public:

    /// Associates the iterator with the given \a mesh and the primitive defined
    /// by the edge from vertex index \vi to its neighbor index \a ni.
    PrimitiveIterator(Mesh const& mesh,int vi=0,int ni=0)
    :   Iterator(mesh,vi,ni)
    {
        // Initialize unless this is the end iterator.
        if (m_vi<m_num_vertices) {
            next();
        }
    }

    /// Returns the primitive's vertex indices.
    IndexArray const& indices() const {
        return m_primitive;
    }

  private:

    /// Determines the primitive for the current vertex / neighbor, if any.
    bool next() {
        bool exists=true;

        // Handle point primitives.
        if (m_neighbors->getSize()==0) {
            m_primitive.setSize(1);
            m_primitive[0]=m_vi;
        }
        // Handle line primitives.
        else if (m_neighbors->getSize()==1) {
            m_primitive.setSize(2);
            m_primitive[0]=m_vi;
            m_primitive[1]=(*m_neighbors)[m_ni];
        }
        else {
            // Only accept the primitive indices if they are "in order" to
            // avoid vertex permutations of the same primitive.
            m_mesh.orbit(m_vi,(*m_neighbors)[m_ni],m_primitive);
            for (int i=1;i<m_primitive.getSize();++i) {
                if (m_primitive[i]<m_primitive[0]) {
                    exists=false;
                    break;
                }
            }
        }

        return exists;
    }

    IndexArray m_primitive; ///< The current primitive's indices.
};
