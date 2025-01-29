#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>
#include <QTextCursor>

class Logger
{
private:
    QTextEdit* m_textEdit;
public:
    explicit Logger(QTextEdit* textEdit): m_textEdit(textEdit) {}
    void log(const QString& message);
};

#endif // LOGGER_H
