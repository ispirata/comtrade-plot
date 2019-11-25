#include "ComtradeChannel.h"

ComtradeChannel::ComtradeChannel(QObject *parent)
    : QObject(parent)
    , m_index(0)
{
}

ComtradeChannel::~ComtradeChannel()
{
}

int ComtradeChannel::index() const
{
    return m_index;
}

QString ComtradeChannel::id() const
{
    return m_id;
}

QString ComtradeChannel::phase() const
{
    return m_phase;
}

QString ComtradeChannel::ccbm() const
{
    return m_ccbm;
}

void ComtradeChannel::setIndex(int index)
{
    m_index = index;
}

void ComtradeChannel::setId(const QString &id)
{
    m_id = id;
}

void ComtradeChannel::setPhase(const QString &phase)
{
    m_phase = phase;
}

void ComtradeChannel::setCCBM(const QString &ccbm)
{
    m_ccbm = ccbm;
}
