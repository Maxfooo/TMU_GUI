#ifndef TMUHEATGRADIENT_H
#define TMUHEATGRADIENT_H

#include <QStringList>
#include <QString>

class TmuHeatGradient
{
public:
    TmuHeatGradient();

    QStringList getGradProfNames() {return gradProfileNames;}

private:
    QStringList gradProfileNames;
};

#endif // TMUHEATGRADIENT_H
