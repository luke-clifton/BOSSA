///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2012, ShumaTech
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include "FlashCALW.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>

#define FCALW_KEY         0x5a

#define FCALW_FCR       0x400A0000
#define FCALW_FCMD      0x400A0004
#define FCALW_FSR       0x400A0008


/* Atmel 42023G - Table 14-5 */
#define FCALW_FCMD_NOP    0
#define FCALW_FCMD_WP     1
#define FCALW_FCMD_EP     2
#define FCALW_FCMD_CPB    3
#define FCALW_FCMD_LP     4
#define FCALW_FCMD_UP     5
#define FCALW_FCMD_EA     6
#define FCALW_FCMD_WGPB   7
#define FCALW_FCMD_EGPB   8
#define FCALW_FCMD_SSB    9
#define FCALW_FCMD_PGPFB 10
#define FCALW_FCMD_EAGPF 11
#define FCALW_FCMD_QPR   12
#define FCALW_FCMD_WUP   13
#define FCALW_FCMD_EUP   14
#define FCALW_FCMD_QPRUP 15
#define FCALW_FCMD_HSEN  16
#define FCALW_FCMD_HSDIS 17


FlashCALW::FlashCALW(Samba& samba,
                   const std::string& name,
                   uint32_t addr,
                   uint32_t pages,
                   uint32_t size,
                   uint32_t planes,
                   uint32_t lockRegions,
                   uint32_t user,
                   uint32_t stack,
                   bool canBootFlash)
    : Flash(samba, name, addr, pages, size, planes, lockRegions, user, stack),
      _canBootFlash(canBootFlash), _flashApplet(samba)
{
    assert(planes == 1);
    assert(pages <= 1024);
    assert(lockRegions <= 32);

    _flashApplet.init(&pages, &size, &lockRegions);
}

FlashCALW::~FlashCALW()
{
}

void
FlashCALW::eraseAll()
{
    for (uint32_t i = appStartPage(); i < _pages; i++)
    {
        _flashApplet.erase(i);
    }
}

void
FlashCALW::eraseAuto(bool enable)
{
    /* TODO: This doesn't need implementing right? */
}

bool
FlashCALW::isLocked()
{
//    if ((readFSR() & (0xffff << 16)) != 0)
//        return true;
//
    return false;
}

bool
FlashCALW::getLockRegion(uint32_t region)
{
//    if (region >= _lockRegions)
//        throw FlashRegionError();

//    return (readFSR() & (1 << (16 + region)));
    return 0;
}

void
FlashCALW::setLockRegion(uint32_t region, bool enable)
{
    _flashApplet.setLockRegion(region, enable);
    /*
    uint32_t page;

    if (region >= _lockRegions)
        throw FlashRegionError();

    if (enable != getLockRegion(region))
    {
        page = region * _pages / _lockRegions;
        waitFRDY();
        writeFCMD(enable ? FCALW_FCMD_LP : FCALW_FCMD_UP, page);
    }*/
}

bool
FlashCALW::getSecurity()
{
    //return (readFSR() & (1 << 4));
    return false;
}

void
FlashCALW::setSecurity()
{
 //   waitFRDY();
 //   writeFCMD(FCALW_FCMD_SSB, 0);
}

bool
FlashCALW::getBod()
{
    /* TODO:
     * return (readFSR0() & (1 << 8));
     */
    puts("Not getting Bod");
    return false;
}

void
FlashCALW::setBod(bool enable)
{
    /*
    waitFRDY();
    writeFCR0(enable ? EFC_FCMD_SGPB : EFC_FCMD_CGPB, 0);
    */
    puts("Not setting Bod");
}

bool
FlashCALW::getBor()
{
    /* TODO:
     * return (readFSR0() & (2 << 8));
     */
    puts("Not getting Bor");
    return false;
}

void
FlashCALW::setBor(bool enable)
{
    /* TODO:
    waitFRDY();
    writeFCR0(enable ? EFC_FCMD_SGPB : EFC_FCMD_CGPB, 1);
    */
    puts("Not setting Bor");
}

bool
FlashCALW::getBootFlash()
{
    return true;
}

void
FlashCALW::setBootFlash(bool enable)
{
    /* We can ONLY boot from flash */
}

void
FlashCALW::loadBuffer(const uint8_t* data, uint16_t size)
{
    assert(size);
    assert(data);
    assert(size < 1024); // TODO
    _bufferSize = size;
    memcpy(_buffer, data, size);
    printf("Loaded buffer with %u (%u) bytes\n", _bufferSize, size);
}
void
FlashCALW::writePage(uint32_t page)
{
    assert(_bufferSize);
    assert(_bufferSize <= _size);
    if (page >= _pages)
        throw FlashPageError();


    _flashApplet.write(_addr + page * _size, _buffer, _bufferSize);
}

void
FlashCALW::readPage(uint32_t page, uint8_t* data)
{
    if (page >= _pages)
        throw FlashPageError();

//    waitFRDY();
//    _samba.read(_addr + page * _size, data, _size);
}

void
FlashCALW::waitFRDY()
{
/*    uint32_t tries = 0;
    uint32_t fsr;

    while (++tries <= 500)
    {
        fsr = readFSR();
        if (fsr & (1 << 2))
            throw FlashLockError();

        if (fsr & 1)
            break;
        usleep(100);
    }

    if (tries > 500)
        throw FlashCmdError();
*/
}

uint32_t
FlashCALW::readFCR()
{
    return _samba.readWord(FCALW_FCR);
}

uint32_t
FlashCALW::readFCMD()
{
    return _samba.readWord(FCALW_FCMD);
}

void
FlashCALW::writeFCMD(uint8_t cmd, uint32_t pagen)
{
    _samba.writeWord(FCALW_FCMD, (FCALW_KEY << 24) | (pagen << 8) | cmd);
}

uint32_t
FlashCALW::readFSR()
{
    return _samba.readWord(FCALW_FSR);
}

uint32_t
FlashCALW::readFirstWord()
{
    return _samba.readWord(0x80000004);
}

