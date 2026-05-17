#include "MyShell.h"
#include "ShellCommands.h"

#include <QDebug>
#include <QTextStream>
#include <QThread>
#include <termios.h>
#include <unistd.h>

QTextStream in(stdin);
QTextStream out(stdout);

#define MAX_HISTORY 100

VechilcleShell::VechilcleShell(CommandDispatcher *dispatcher)
    : m_dispatcher(dispatcher)
{
    out << "Start VechicleShell" << Qt::endl;
    out << "use help for commands, ↑↓ for history" << Qt::endl;
    m_historyIdx = 0;
}

int VechilcleShell::DivCommand(const QString &command, QVector<QString> &argv)
{
    argv.clear();
    QString current;
    for (int i = 0; i < command.size(); i++) {
        if (command[i] == ' ') {
            if (!current.isEmpty()) { argv.append(current); current.clear(); }
        } else {
            current += command[i];
        }
    }
    if (!current.isEmpty()) argv.append(current);
    return argv.size();
}

// ---- History ----

void VechilcleShell::addHistory(const QString &cmd)
{
    QString trimmed = cmd.trimmed();
    if (trimmed.isEmpty()) return;
    if (!m_history.isEmpty() && m_history.last() == trimmed) return;
    m_history.append(trimmed);
    while (m_history.size() > MAX_HISTORY) m_history.removeFirst();
    m_historyIdx = m_history.size();
}

// ---- Shell loop with raw terminal ----

void VechilcleShell::Shell()
{
    struct termios origTermios;
    tcgetattr(STDIN_FILENO, &origTermios);

    struct termios raw = origTermios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    QString prompt = "Shell@Vechicle $ ";
    QString line;
    int pos = 0;
    QByteArray esc;
    bool inEsc = false;

    auto redraw = [&]() {
        out << "\r\033[K" << prompt << line << flush;
        int back = line.size() - pos;
        if (back > 0) out << QString("\033[%1D").arg(back) << flush;
    };

    out << prompt << flush;

    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) break;

        if (inEsc) {
            esc.append(c);
            if (esc == "\033[A") {
                // Up: prev history
                if (m_historyIdx > 0) {
                    m_historyIdx--;
                    line = m_history.at(m_historyIdx);
                    pos = line.size();
                    redraw();
                }
                inEsc = false; esc.clear(); continue;
            }
            if (esc == "\033[B") {
                // Down: next history
                if (m_historyIdx < m_history.size() - 1) {
                    m_historyIdx++;
                    line = m_history.at(m_historyIdx);
                    pos = line.size();
                } else {
                    m_historyIdx = m_history.size();
                    line.clear(); pos = 0;
                }
                redraw();
                inEsc = false; esc.clear(); continue;
            }
            if (esc == "\033[C") { if (pos < line.size()) pos++; redraw(); inEsc = false; esc.clear(); continue; }
            if (esc == "\033[D") { if (pos > 0) pos--; redraw(); inEsc = false; esc.clear(); continue; }
            if (esc.size() >= 3) { inEsc = false; esc.clear(); continue; }
            continue;
        }

        if (c == '\033') { inEsc = true; esc.clear(); continue; }
        if (c == '\r' || c == '\n') {
            out << "\r\n" << flush;
            if (!line.isEmpty()) {
                addHistory(line);
                m_historyIdx = m_history.size();
                QVector<QString> argv;
                if (DivCommand(line, argv) > 0)
                    m_dispatcher->dispatch(argv.size(), argv);
            }
            line.clear(); pos = 0;
            out << prompt << flush;
            continue;
        }
        if (c == 0x7f || c == '\b') { if (pos > 0) { pos--; line.remove(pos, 1); redraw(); } continue; }
        if (c == 3) { out << "^C\r\n" << flush; line.clear(); pos = 0; out << prompt << flush; continue; } // Ctrl+C
        if (c == 4) { out << "\r\n" << flush; break; } // Ctrl+D
        if (c == '\t') continue;
        if (c >= 32 && c < 127) { line.insert(pos, QChar(c)); pos++; redraw(); }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
    out << Qt::endl;
}
