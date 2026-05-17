#include "SysfsDevice.h"
#include <QDebug>

SysfsDevice::SysfsDevice(const QString &sysfsPath, const QString &deviceName)
    : m_name(deviceName)
{
    m_file.setFileName(sysfsPath);
    if (!m_file.exists())
        qDebug() << "can't init" << m_name;
    m_filein = new QTextStream(&m_file);
}

SysfsDevice::~SysfsDevice()
{
    delete m_filein;
}

int SysfsDevice::on()
{
    if (!m_file.exists())
        return -1;

    if (!m_file.open(QIODevice::ReadWrite)) {
        qDebug() << m_file.errorString();
        return -1;
    }

    m_file.write("1");
    m_file.close();

    return getStatus();
}

int SysfsDevice::off()
{
    if (!m_file.exists())
        return -1;

    if (!m_file.open(QIODevice::ReadWrite)) {
        qDebug() << m_file.errorString();
        return -1;
    }

    m_file.write("0");
    m_file.close();

    return getStatus();
}

int SysfsDevice::getStatus()
{
    if (!m_file.exists())
        return -1;

    if (!m_file.open(QIODevice::ReadWrite)) {
        qDebug() << m_file.errorString();
        m_status = -1;
        return -1;
    }

    QString buf = m_filein->readLine();

    if (buf == "1")
        m_status = 1;
    else if (buf == "0")
        m_status = 0;
    else
        m_status = -1;

    if (m_status != -1)
        qDebug() << m_name << "status:" << m_status;
    else
        qDebug() << m_name << "status get wrong";

    m_file.close();
    return m_status;
}
