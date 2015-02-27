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
#include "FlashApplet.h"
#include "FlashAppletCode.h"
#include <cstddef>
#include <cstdio>
#include <unistd.h>
#include <stdexcept>
#include <cassert>

FlashApplet::FlashApplet(Samba& samba)
    : Applet(samba,
             0x20002000,
             applet.code,
             sizeof(applet.code),
             0x20002000,
             0x20007FF0,
             0x20002000),
      _samba(samba),
      _bufAdd(0),
      _bufSiz(0)
{
}

FlashApplet::~FlashApplet()
{
}


const static uint32_t MAILBOX = 0x20002040;

#define MBWRITE(X,V) _samba.writeWord(MAILBOX + (uint32_t)offsetof(_Mailbox, X), V)
#define MBREAD(X) _samba.readWord(MAILBOX + (uint32_t)offsetof(_Mailbox, X))

#define CMD_INIT            0x00
#define CMD_FULLERASE       0x01
#define CMD_WRITE           0x02
#define CMD_READ            0x03
#define CMD_LOCK            0x04
#define CMD_UNLOCK          0x05
#define CMD_GPNVM           0x06
#define CMD_SECURITY        0x07
#define CMD_ERASEBUFFER     0x08
#define CMD_BINARYPAGE      0x09
#define CMD_OTPREAD         0x0a
#define CMD_OTPWRITE        0x0b
#define CMD_LISTBADBLOCKS   0x10
#define CMD_TAGBLOCK        0x11
#define CMD_READUNIQUEId    0x12
#define CMD_ERASEBLOCKS     0x13
#define CMD_BATCHERASE      0x14
#define CMD_PMECCPARAM      0x15
#define CMD_PMECCBOOT       0x16
#define CMD_SWITCHECCMODE   0x17
#define CMD_TRIMFFSMODE     0x18
#define CMD_RWFUSES         0x40
#define CMD_WRITEUSERPAGE   0x41
#define CMD_READUSERPAGE    0x42
#define CMD_READUNIQUESN    0x43
#define CMD_ERASEPAGE       0x44


void FlashApplet::run()
{
    _samba.go(_start);
}

void
FlashApplet::init(uint32_t* pages, uint32_t* size, uint32_t* lockRegions)
{
    MBWRITE(command, CMD_INIT);
    run();

    uint32_t res = MBREAD(status);

    if (res)
    {
        printf("Error initialising applet: %x\n", res);
        throw std::runtime_error(""); // TODO
    }

    uint32_t mSize = MBREAD(argument.outputInit.memorySize);
    _bufAdd = MBREAD(argument.outputInit.bufferAddress);
    _bufSiz = MBREAD(argument.outputInit.bufferSize);
    uint32_t mLockBits = MBREAD(argument.outputInit.memoryInfo) >> 16;
    uint32_t mnPages = MBREAD(argument.outputInit.nbPages);

    assert(*pages == mnPages);
    assert((*size * *pages) == mSize);
    assert(*lockRegions == mLockBits);


    printf("Applet intialisation complete.\n"
           " memory size:         0x%x\n"
           " buffer address:      0x%x\n"
           " buffer size:         %u\n"
           " number of lock bits: %u\n"
           " page size:           %u\n"
           " number of pages:     %u\n"
           , mSize
           , _bufAdd
           , _bufSiz
           , mLockBits
           , MBREAD(argument.outputInit.pageSize)
           , mnPages
           );
}

void
FlashApplet::write(uint32_t addr, const uint8_t* buffer, int size)
{
    printf("\nWriting... %x %x\n", _bufAdd, _bufSiz);
 //   assert(size > 0);
    if (size == 0)
    {
        puts("no size...");
        return;
    }
    assert((uint32_t) size <= _bufSiz); // TODO handle this properly
    usleep(300000);
    _samba.write(_bufAdd, buffer, size);
    usleep(300000);
    printf("writing xmodem finished\n");
    MBWRITE(command, CMD_WRITE);
    MBWRITE(argument.inputWrite.memoryOffset, addr);
    MBWRITE(argument.inputWrite.bufferAddr, _bufAdd);
    MBWRITE(argument.inputWrite.bufferSize, size);
    printf("Executing code\n");
    run();
    usleep(300000);
    printf("Reading status\n");
    uint32_t result = MBREAD(status);
    if (result)
    {
        printf("Error occured while writing to flash\n");
        return;
    }
    uint32_t bytes_written = MBREAD(argument.outputWrite.bytesWritten);
    printf("Wrote %u bytes to address 0x%x\n", bytes_written, addr);
}


void
FlashApplet::erase(uint32_t page)
{
    printf("Erasing page %u\n", page);
    MBWRITE(command, CMD_ERASEPAGE);
    MBWRITE(argument.inputErasePage.page, page);
    run();
    usleep(30000);
    uint32_t result = MBREAD(status);
    assert(result == 0); // TODO
}


void
FlashApplet::setLockRegion(uint32_t region, bool enable)
{
    if (enable)
        MBWRITE(command, CMD_LOCK);
    else
        MBWRITE(command, CMD_UNLOCK);

    MBWRITE(argument.inputLock.sector, region);
    run();
    usleep(30000);

    uint32_t result = MBREAD(status);

    assert(result == 0);
}


//void
//FlashApplet::read(uint32_t addr, uint8_t* buffer, int size)
//{
