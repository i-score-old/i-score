#pragma once
#include<QObject>
#include <QKeyEvent>

#include "MaquetteView.hpp"

class GlobalEventFilter: public QObject
{
    MaquetteView* _view{};

public:
    GlobalEventFilter(MaquetteView* scene, QObject* parent):
        QObject(parent),
        _view(scene)
    {
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* ev = static_cast<QKeyEvent*>(event);
			if(ev->key() == Qt::Key_Space ||
			   ev->key() == Qt::Key_Comma ||
			   ev->key() == Qt::Key_Enter ||
			   ev->key() == Qt::Key_Period)
            {
                _view->sub_keyPressEvent(ev);
                return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }
};
