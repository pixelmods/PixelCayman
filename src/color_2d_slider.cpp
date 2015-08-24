/**

@author Mattia Basaglia

@section License

    Copyright (C) 2015 Mattia Basaglia

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Color Widgets.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "color_2d_slider.hpp"
#include "color_utils.hpp"
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

namespace color_widgets {

static const double selector_radius = 6;

class Color2DSlider::Private
{
public:
    qreal hue = 1, sat = 1, val = 1;
    Component comp_x = Saturation;
    Component comp_y = Value;
    QImage square;

    qreal PixHue(float x, float y)
    {
        if ( comp_x == Hue )
            return x;
        if ( comp_y == Hue )
            return y;
        return hue;
    }

    qreal PixSat(float x, float y)
    {
        if ( comp_x == Saturation )
            return x;
        if ( comp_y == Saturation )
            return y;
        return sat;
    }

    qreal PixVal(float x, float y)
    {
        if ( comp_x == Value )
            return x;
        if ( comp_y == Value )
            return y;
        return val;
    }

    void renderSquare(const QSize& size)
    {
        square = QImage(size, QImage::Format_RGB32);

        for ( int y = 0; y < size.height(); ++y )
        {
            qreal yfloat = 1 - qreal(y) / size.height();
            for ( int x = 0; x < size.width(); ++x )
            {
                qreal xfloat = qreal(x) / size.width();
                square.setPixel( x, y, QColor::fromHsvF(
                    PixHue(xfloat, yfloat),
                    PixSat(xfloat, yfloat),
                    PixVal(xfloat, yfloat)
                ).rgb());
            }
        }
    }

    QPointF selectorPos(const QSize& size)
    {
        QPointF pt;
        switch ( comp_x )
        {
            case Hue:       pt.setX(size.width()*hue); break;
            case Saturation:pt.setX(size.width()*sat); break;
            case Value:     pt.setX(size.width()*val); break;
        }
        switch ( comp_y )
        {
            case Hue:       pt.setY(size.height()*hue); break;
            case Saturation:pt.setY(size.height()*sat); break;
            case Value:     pt.setY(size.height()*val); break;
        }
        return pt;
    }

    void setColorFromPos(const QPoint& pt, const QSize& size)
    {
        QPointF ptfloat(
            qBound(0.0, qreal(pt.x()) / size.width(), 1.1),
            qBound(0.0, qreal(pt.y()) / size.height(), 1.1)
        );
        switch ( comp_x )
        {
            case Hue:       hue = ptfloat.x(); break;
            case Saturation:sat = ptfloat.x(); break;
            case Value:     val = ptfloat.x(); break;
        }
        switch ( comp_y )
        {
            case Hue:       hue = ptfloat.y(); break;
            case Saturation:sat = ptfloat.y(); break;
            case Value:     val = ptfloat.y(); break;
        }
    }
};

Color2DSlider::Color2DSlider(QWidget* parent)
    : QWidget(parent), p(new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

Color2DSlider::~Color2DSlider()
{
    delete p;
}

QColor Color2DSlider::color() const
{
    return QColor::fromHsvF(p->hue, p->sat, p->val);
}

QSize Color2DSlider::sizeHint() const
{
    return {128, 128};
}

qreal Color2DSlider::hue() const
{
    return p->hue;
}

qreal Color2DSlider::saturation() const
{
    return p->sat;
}

qreal Color2DSlider::value() const
{
    return p->val;
}

Color2DSlider::Component Color2DSlider::componentX() const
{
    return p->comp_x;
}

Color2DSlider::Component Color2DSlider::componentY() const
{
    return p->comp_y;
}

void Color2DSlider::setColor(const QColor& c)
{
    p->hue = c.hsvHueF();
    p->sat = c.saturationF();
    p->val = c.valueF();
    p->renderSquare(size());
    update();
    emit colorChanged(color());
}

void Color2DSlider::setHue(qreal h)
{
    p->hue = h;
    p->renderSquare(size());
    update();
    emit colorChanged(color());
}

void Color2DSlider::setSaturation(qreal s)
{
    p->sat = s;
    p->renderSquare(size());
    update();
    emit colorChanged(color());
}

void Color2DSlider::setValue(qreal v)
{
    p->val = v;
    p->renderSquare(size());
    update();
    emit colorChanged(color());
}

void Color2DSlider::setComponentX(Color2DSlider::Component componentX)
{
    if ( componentX != p->comp_x )
    {
        p->comp_x = componentX;
        p->renderSquare(size());
        update();
        emit componentXChanged(p->comp_x);
    }
}

void Color2DSlider::setComponentY(Color2DSlider::Component componentY)
{
    if ( componentY != p->comp_y )
    {
        p->comp_y = componentY;
        p->renderSquare(size());
        update();
        emit componentXChanged(p->comp_y);
    }
}

void Color2DSlider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(0,0,p->square);

    painter.setPen(QPen(p->val > 0.5 ? Qt::black : Qt::white, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(p->selectorPos(size()), selector_radius, selector_radius);
}

void Color2DSlider::mousePressEvent(QMouseEvent* event)
{
    p->setColorFromPos(event->pos(), size());
    update();
}

void Color2DSlider::mouseMoveEvent(QMouseEvent* event)
{
    p->setColorFromPos(event->pos(), size());
    update();
}

void Color2DSlider::mouseReleaseEvent(QMouseEvent* event)
{
    p->setColorFromPos(event->pos(), size());
    update();
}

void Color2DSlider::resizeEvent(QResizeEvent* event)
{
    p->renderSquare(event->size());
    update();
}


} // namespace color_widgets
