#pragma once

#include <QVector3D>

class Vertex
{
public:
    Q_DECL_CONSTEXPR Vertex();
    Q_DECL_CONSTEXPR explicit Vertex(const QVector3D& position);

    [[nodiscard]] Q_DECL_CONSTEXPR const QVector3D& position() const;
    void setPosition(const QVector3D& position);

    static const int PositionTupleSize = 3;
    [[nodiscard]] static Q_DECL_CONSTEXPR int positionOffset();
    [[nodiscard]] static Q_DECL_CONSTEXPR int stride();

private:
    QVector3D m_position;
};

Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

Q_DECL_CONSTEXPR inline Vertex::Vertex() {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position) : m_position(position) {}

Q_DECL_CONSTEXPR inline const QVector3D& Vertex::position() const { return m_position; }
void inline Vertex::setPosition(const QVector3D& position) { m_position = position; }

Q_DECL_CONSTEXPR inline int Vertex::positionOffset() { return offsetof(Vertex, m_position); }
Q_DECL_CONSTEXPR inline int Vertex::stride() {
    return sizeof(Vertex);
}
