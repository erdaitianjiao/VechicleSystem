#ifndef MYSHELL_H
#define MYSHELL_H

#include <QTextStream>
#include <QThread>
#include <QString>
#include <QVector>
#include <QStringList>

class CommandDispatcher;

extern QTextStream in;
extern QTextStream out;

class VechilcleShell : public QThread {
public:
    explicit VechilcleShell(CommandDispatcher *dispatcher);

private:
    CommandDispatcher *m_dispatcher;
    QStringList m_history;
    int m_historyIdx = 0;

    void Shell();
    int DivCommand(const QString &command, QVector<QString> &argv);
    void run() override { this->Shell(); }

    void addHistory(const QString &cmd);
};

#endif // MYSHELL_H
