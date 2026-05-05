/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: color_button.cpp
 *  Copyright (c) 2024-2026 scofieldzhu
 *
 *  MIT License
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#include "color_button.h"
#include <QMouseEvent>
#include <QColorDialog>
#include <QPainter>

ColorButton::ColorButton(QWidget *parent)
    : QLabel(parent)
{
}

void ColorButton::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() & Qt::LeftButton){
        QColor color = QColorDialog::getColor(Qt::white, this, tr("Please choose color"));
        if(color.isValid()) {
            setColor(color);
        }
    }
}

void ColorButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), color_);
    painter.setPen(Qt::black);
    painter.drawText(rect(), Qt::AlignCenter, text());
}

void ColorButton::setColor(const QColor &clr)
{
    color_ = clr;
    QString stylesheet = QString("background-color:rgb(%1, %2, %3)").arg(clr.red()).arg(clr.green()).arg(clr.blue());
    setStyleSheet(stylesheet);
    update();
    emit colorChanged(color_);
}