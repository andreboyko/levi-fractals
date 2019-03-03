#include "fractalview.h"
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QDebug>
#include <QTimer>

FractalView::FractalView(QWidget *parent)
{
    Q_UNUSED(parent);
    setInitialState();

    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
}

void FractalView::resetState() {
    lineCoords.clear();
    setInitialState();
    this->update();
}

void FractalView::setInitialState() {
    iterationCount = 0;

    const auto w2 = initialWidth / 2;
    switch(type) {
        case FractalType::ISLAND:
        lineCoords.push_back(QLineF(w2, w2, -w2, w2));
        [[clang::fallthrough]]; case FractalType::PI_SHAPED:
            lineCoords.push_back(QLineF(w2, -w2, w2, w2));
            lineCoords.push_back(QLineF(-w2, w2, -w2, -w2));
            lineCoords.push_back(QLineF(-w2, -w2, w2, -w2));
            break;
        case FractalType::DEFAULT:
            lineCoords.push_back(QLineF(-w2, 0, w2, 0));
            break;
    }
}

void FractalView::setInitialWidth(int width) {
    this->initialWidth = width;
    resetState();
}

void FractalView::setFractalType(FractalType type) {
    this->type = type;
    resetState();
}

void FractalView::toggleAnimation(bool enabled) {
    if (enabled) {
        animationTimer->start(300);
    } else {
        animationTimer->stop();
    }
}

void FractalView::nextFrame() {
    if (iterationCount == 16) {
        resetState();
    } else {
        nextIter();
    }
}

void FractalView::setSquareWidth(int width) {
    this->viewportWidth = width;
    this->update();
}

void FractalView::nextIter() {
   Lines newLines;

   for (auto it = lineCoords.begin(); it != lineCoords.end(); ++it)
   {
       auto currentLine = *it;

       auto ss = currentLine.normalVector();
       ss.translate(currentLine.center() - currentLine.p1());
       ss.setLength(currentLine.length() / 2);

       auto newCentral = ss.p2();
       newLines.push_back(QLineF(currentLine.p1(), newCentral));
       newLines.push_back(QLineF(newCentral, currentLine.p2()));
   }

   lineCoords = newLines;
   iterationCount++;
   this->update();
}

void FractalView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    //set bg to white
    painter.fillRect(rect(), QBrush(Qt::white, Qt::SolidPattern));

    QPen myPen(Qt::black, 1, Qt::SolidLine);
    painter.setPen(myPen);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawText(rect().left(), rect().bottom(), QString("Iteration #%1").arg(iterationCount));

    //coordinate system
    int side = qMin(width(), height());
    qreal scaleFactor = side /viewportWidth;

    painter.translate(width() / 2, height() / 2);
    painter.scale(scaleFactor, scaleFactor);

    Lines::iterator it;
    for (it = lineCoords.begin(); it != lineCoords.end(); ++it) {
        painter.drawLine(*it);
    }
}
