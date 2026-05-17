#include "ShellCommands.h"
#include "HardwareManager.h"
#include "CameraDevice.h"
#include "MusicPlayer.h"
#include <QTextStream>

extern QTextStream out;

namespace {

void dispatchDeviceCommand(const QString &deviceName, int argc, const QVector<QString> &argv)
{
    if (argc != 2) {
        out << "usage: " << argv[0] << " <on|off|status>" << endl;
        return;
    }

    auto *dev = HardwareManager::instance().getDevice(deviceName);
    if (!dev) {
        out << "device not found: " << deviceName << endl;
        return;
    }

    if (argv[1] == "on")
        dev->on();
    else if (argv[1] == "off")
        dev->off();
    else if (argv[1] == "status")
        dev->getStatus();
    else
        out << "unknown subcommand: " << argv[1] << endl;
}

} // anonymous namespace

// ---- HelpCommand ----

void HelpCommand::execute(int argc, const QVector<QString> &argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    out << "command list:" << endl;
    // Commands are registered in the dispatcher; the dispatcher's commandNames()
    // is used for listing. For now, print a simple message.
    out << "    help" << endl;
    out << "    light <on|off|status>" << endl;
    out << "    beep <on|off|status>" << endl;
    out << "    camera <preview|photo|record|stop>" << endl;
    out << "    music <play|pause|resume|stop|next|prev> [file]" << endl;
}

// ---- LightCommand ----

void LightCommand::execute(int argc, const QVector<QString> &argv)
{
    dispatchDeviceCommand("light", argc, argv);
}

// ---- BeepCommand ----

void BeepCommand::execute(int argc, const QVector<QString> &argv)
{
    dispatchDeviceCommand("beep", argc, argv);
}

// ---- CameraCommand ----

static CameraDevice *getOrCreateCamera()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam) {
        cam = HardwareManager::instance().createDevice<CameraDevice>("/dev/video0");
    }
    return cam;
}

void CameraCommand::execute(int argc, const QVector<QString> &argv)
{
    if (argc < 2) {
        out << "usage: camera <preview|photo|record|stop>" << endl;
        return;
    }

    if (argv[1] == "preview") {
        auto *cam = getOrCreateCamera();
        if (cam->on() == 0)
            out << "camera preview started" << endl;
        else
            out << "camera preview failed" << endl;

    } else if (argv[1] == "photo") {
        if (argc < 3) {
            out << "usage: camera photo <filepath>" << endl;
            return;
        }
        auto *cam = getOrCreateCamera();
        if (cam->capturePhoto(argv[2]) == 0)
            out << "photo saved to " << argv[2] << endl;
        else
            out << "photo capture failed" << endl;

    } else if (argv[1] == "record") {
        if (argc < 3) {
            out << "usage: camera record <filepath>" << endl;
            return;
        }
        auto *cam = getOrCreateCamera();
        if (cam->startRecording(argv[2]) == 0)
            out << "recording started, saving to " << argv[2] << endl;
        else
            out << "recording failed" << endl;

    } else if (argv[1] == "stop") {
        auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
        if (!cam) {
            out << "camera not initialized" << endl;
            return;
        }
        cam->off();
        out << "camera stopped" << endl;

    } else {
        out << "unknown subcommand: " << argv[1] << endl;
    }
}

// ---- MusicCommand ----

MusicCommand::MusicCommand(MusicPlayer *player)
    : m_player(player) {}

void MusicCommand::execute(int argc, const QVector<QString> &argv)
{
    if (argc < 2) {
        out << "usage: music <play|pause|resume|stop|next|prev> [filepath]" << endl;
        return;
    }

    if (argv[1] == "play") {
        if (argc < 3) {
            out << "usage: music play <filepath>" << endl;
            return;
        }
        QStringList list;
        list.append(argv[2]);
        m_player->setPlaylist(list);
        m_player->play(argv[2]);
        out << "playing: " << argv[2] << endl;

    } else if (argv[1] == "pause") {
        m_player->pause();
        out << "paused" << endl;

    } else if (argv[1] == "resume") {
        m_player->resume();
        out << "resumed" << endl;

    } else if (argv[1] == "stop") {
        m_player->stop();
        out << "stopped" << endl;

    } else if (argv[1] == "next") {
        m_player->next();
        out << "next track" << endl;

    } else if (argv[1] == "prev") {
        m_player->prev();
        out << "previous track" << endl;

    } else {
        out << "unknown subcommand: " << argv[1] << endl;
    }
}

// ---- CommandDispatcher ----

void CommandDispatcher::registerCommand(ICommand *cmd)
{
    m_commands.insert(cmd->name(), cmd);
}

void CommandDispatcher::dispatch(int argc, const QVector<QString> &argv)
{
    if (argc == 0)
        return;

    ICommand *cmd = m_commands.value(argv[0], nullptr);
    if (cmd)
        cmd->execute(argc, argv);
    else
        out << "Unknown command: " << argv[0] << endl;
}

QStringList CommandDispatcher::commandNames() const
{
    return m_commands.keys();
}
