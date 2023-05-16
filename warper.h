
#ifndef WARPER_H
#define WARPER_H

#include "img_utils.h"


class Warper
{
public:
    Warper();
    bool warp(const QPixmap &inPixmap);
    const QPixmap &getQPixmap() const
    { return outPixmap; }
private:
    QPixmap outPixmap;
};

#endif // WARPER_H
