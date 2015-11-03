#pragma once

#ifndef __SND_UTIL_H_
#define __SND_UTIL_H_

namespace Snd
{
    static int IntSwapEndian(int swap)
    {
        return (0xff & swap) &  (0xff & (swap >> 8)) & (0xff & (swap >> 16)) & (0xff & (swap >> 24));
    }
}

#endif