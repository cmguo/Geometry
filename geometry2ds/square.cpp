#include "square.h"

Square::Square(Resource * res)
    : Rectangle(res)
{
}

Square::Square(QPointF const & pt)
    : Rectangle(pt)
{
}

Square::Square(Square const & o)
    : Rectangle(o)
{
}

void Square::movePoint(const QPointF &pt)
{
    QPointF st = points_.first();
    QSizeF sz = QSizeF(qAbs(pt.x() - st.x()), qAbs(pt.y() - st.y()));
    QPointF p(pt);
    if (sz.width() > sz.height()) {
        p.setY(pt.y() >= st.y() ? st.y() + sz.width() : st.y() - sz.width());
    } else {
        p.setX(pt.x() >= st.x() ? st.x() + sz.height() : st.x() - sz.height());
    }
    Rectangle::movePoint(p);
}

bool Square::move(int elem, QPointF const &pt)
{
    QPointF p(pt);
    if (elem < 4) {
        QPointF st = point((elem + 2) % 4);
        QSizeF sz = QSizeF(qAbs(pt.x() - st.x()), qAbs(pt.y() - st.y()));
        if (sz.width() > sz.height()) {
            p.setY(pt.y() >= st.y() ? st.y() + sz.width() : st.y() - sz.width());
        } else {
            p.setX(pt.x() >= st.x() ? st.x() + sz.height() : st.x() - sz.height());
        }
    } else {
        elem -= 4;
        QPointF st = point((elem + 2) % 4);
        QSizeF sz = QSizeF(qAbs(pt.x() - st.x()), qAbs(pt.y() - st.y()));
        if (elem % 2) {
            p.setX(pt.x() >= st.x() ? st.x() + sz.height() : st.x() - sz.height());
        } else {
            p.setY(pt.y() >= st.y() ? st.y() + sz.width() : st.y() - sz.width());
        }
    }
    return Rectangle::move(elem, p);
}
