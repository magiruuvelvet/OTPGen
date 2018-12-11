#ifndef FRAMELESSCONTAINER_HPP
#define FRAMELESSCONTAINER_HPP

#include <QWidget>
#include <QRubberBand>
#include <QObject>
#include <QEvent>
#include <QRect>
#include <QPoint>
#include <QHoverEvent>
#include <QMouseEvent>

#include <memory>

// README:
//
//  If QWidget is on the stack, create the FramelessContainer also on the stack!
//    QWidget myWidget;
//    FramelessContainer container(&myWidget);
//
//  If QWidget is on the heap, create the FramelessContainer also on the heap!
//    QWidget *myWidget = new...
//    FramelessContainer *container = new... myWidget
//
//  Otherwise the gap will not appear and the window can't be resized!
//

class FramelessContainer : public QObject
{
    Q_OBJECT

public:
    enum Edge {
        None = 0x0,
        Left = 0x1,
        Top = 0x2,
        Right = 0x4,
        Bottom = 0x8,
        TopLeft = 0x10,
        TopRight = 0x20,
        BottomLeft = 0x40,
        BottomRight = 0x80,
    };
    Q_ENUM(Edge)
    Q_DECLARE_FLAGS(Edges, Edge)

    FramelessContainer(QWidget *target);

    inline void setBorderWidth(int w) {
        _borderWidth = w;
    }
    inline int borderWidth() const {
        return _borderWidth;
    }

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void mouseHover(QHoverEvent*);
    void mouseLeave(QEvent*);
    void mousePress(QMouseEvent*);
    void mouseRealese(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void updateCursorShape(const QPoint &);
    void calculateCursorPosition(const QPoint &, const QRect &, Edges &);

private:
    QWidget *_target = nullptr;
    std::shared_ptr<QRubberBand> _rubberband;
    bool _cursorchanged;
    bool _leftButtonPressed;
    Edges _mousePress = Edge::None;
    Edges _mouseMove = Edge::None;
    int _borderWidth;

    QPoint _dragPos;
    bool _dragStart = false;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FramelessContainer::Edges);

#endif // FRAMELESSCONTAINER_HPP
