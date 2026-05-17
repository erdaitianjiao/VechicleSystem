#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include <QString>
#include <QMap>
#include <QVector>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(int argc, const QVector<QString> &argv) = 0;
    virtual QString name() const = 0;
    virtual QString helpText() const = 0;
};

class HelpCommand : public ICommand {
public:
    void execute(int argc, const QVector<QString> &argv) override;
    QString name() const override { return "help"; }
    QString helpText() const override { return "help - show available commands"; }
};

class LightCommand : public ICommand {
public:
    void execute(int argc, const QVector<QString> &argv) override;
    QString name() const override { return "light"; }
    QString helpText() const override { return "light <on|off|status> - control headlight"; }
};

class BeepCommand : public ICommand {
public:
    void execute(int argc, const QVector<QString> &argv) override;
    QString name() const override { return "beep"; }
    QString helpText() const override { return "beep <on|off|status> - control buzzer"; }
};

class CameraCommand : public ICommand {
public:
    void execute(int argc, const QVector<QString> &argv) override;
    QString name() const override { return "camera"; }
    QString helpText() const override { return "camera <preview|photo|record|stop> - control camera"; }
};

class MusicPlayer;

class MusicCommand : public ICommand {
public:
    explicit MusicCommand(MusicPlayer *player);
    void execute(int argc, const QVector<QString> &argv) override;
    QString name() const override { return "music"; }
    QString helpText() const override { return "music <play|pause|resume|stop|next|prev> [file] - control music player"; }
private:
    MusicPlayer *m_player;
};

class CommandDispatcher {
public:
    void registerCommand(ICommand *cmd);
    void dispatch(int argc, const QVector<QString> &argv);
    QStringList commandNames() const;

private:
    QMap<QString, ICommand *> m_commands;
};

#endif // SHELLCOMMANDS_H
