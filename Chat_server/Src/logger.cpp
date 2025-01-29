#include "logger.h"

void Logger::log(const QString& message)
{  
    QMetaObject::invokeMethod(
        m_textEdit,
        [this, message]() {m_textEdit->append(message);},
        Qt::QueuedConnection);
}

