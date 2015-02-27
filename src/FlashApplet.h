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
#ifndef _FLASHAPPLET_H
#define _FLASHAPPLET_H

#include "Applet.h"
#include "FlashAppletCode.h"

/* Struct yanked from the SAM-BA for SAM4L application note */
struct _Mailbox {

    /** Command send to the monitor to be executed. */
    uint32_t command;
    /** Returned status, updated at the end of the monitor execution.*/
    uint32_t status;

    /** Input Arguments in the argument area. */
    union {

        /** Input arguments for the Init command.*/
        struct {

            /** Communication link used.*/
            uint32_t comType;
            /** Trace level.*/
            uint32_t traceLevel;
            /** Memory Bank to write in.*/
            uint32_t bank;

        } inputInit;

        /** Output arguments for the Init command.*/
        struct {

            /** Memory size.*/
            uint32_t memorySize;
            /** Buffer address.*/
            uint32_t bufferAddress;
            /** Buffer size.*/
            uint32_t bufferSize;
            struct {
                /** Lock region size in byte.*/
                uint16_t lockRegionSize;
                /** Number of Lock Bits.*/
                uint16_t numbersLockBits;
            } memoryInfo;
			/** extended infos.*/
			uint32_t pageSize;
			uint32_t nbPages;
			uint32_t appStartPage;			
        } outputInit;

        /** Input arguments for the Write command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputWrite;

        /** Output arguments for the Write command.*/
        struct {

            /** Bytes written.*/
            uint32_t bytesWritten;
        } outputWrite;

        /** Input arguments for the Read command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputRead;

        /** Output arguments for the Read command.*/
        struct {

            /** Bytes read.*/
            uint32_t bytesRead;

        } outputRead;

        /** Input arguments for the Full Erase command.*/
        /** NONE*/

         /** Input arguments for the Lock page command.*/
        struct {

            /** Sector number to be lock.*/
            uint32_t sector;

        } inputLock;

        /** Output arguments for the Lock  page command.*/
        /** NONE*/

        /*/ Input arguments for the Unlock  page command.*/
        struct {

            /** Sector number to be unlock.*/
            uint32_t sector;

        } inputUnlock;

        /** Output arguments for the Unlock  page command.*/
        /** NONE*/

         /** Input arguments for the set/clear GPNVM bits command.*/
        struct {

            /** Activates or Deactivates*/
            uint32_t action;
            /** NVM bit to set/clear*/
            uint32_t bitsOfNVM;

        } inputGPNVM;

        /** Output arguments for the set/clear GPNVM bits command.*/
        /** NONE*/

        /** Input arguments for the set secuity bit command.*/
        /** NONE*/

        /** Output arguments for the set secuity bit command.*/
        /** NONE*/

		/** Input arguments for the set/clear FUSES bits command.*/
        struct {
            /** bit values to be set*/
            uint64_t value;
            /** bitmask to select bits to write*/
            uint64_t writebitmask;
            /** bitmask to select bits to write*/
            uint64_t errors;
        } inputFUSES;

        /** Output arguments for the set/clear GPNVM bits command.*/
        struct {
            /** bit values to be set*/
            uint64_t value;
            /** bitmask to select bits to write*/
            uint64_t writebitmask;
            /** errors results*/
            uint64_t errors;
        } outputFUSES;


        /** Input arguments for the Read Unique SN command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
        } inputReadUniqueSN;

        /** Output arguments for the Read Unique SN command.*/
        /** NONE*/

        /** Input arguments for the Security command.*/
        struct {

            /** Activates*/
            uint32_t action;
        } inputSecurity;

        /** Output arguments for the Security command.*/
        struct {

            /** Bytes written.*/
            uint32_t secure_state;
        } outputSecurity;

        /** Input arguments for the Write command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputWriteUserPage;

        /** Output arguments for the Write command.*/
        struct {

            /** Bytes written.*/
            uint32_t bytesWritten;
        } outputWriteUserPage;
		
        /** Input arguments for the Read command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputReadUserPage;

        /** Output arguments for the Read command.*/
        struct {

            /** Bytes read.*/
            uint32_t bytesRead;

        } outputReadUserPage;
        /** Input arguments for the erase region command.*/
        struct {
            /** page.*/
            uint32_t page;
        } inputErasePage;

        /** Output arguments for the  erase region command.*/
		/*NONE*/
		
    } argument;
};


class FlashApplet : public Applet
{
public:
    FlashApplet(Samba& samba);
    virtual ~FlashApplet();

    virtual void run();

    /** Must call this before using any other applet command. */
    void init(uint32_t* pages, uint32_t* size, uint32_t* lockRegions);

    void write(uint32_t addr, const uint8_t* buffer, int size);

    void erase(uint32_t page);
    void setLockRegion(uint32_t region, bool enable);

private:
    static FlashAppletCode applet;
    Samba& _samba;
    uint32_t _bufAdd;
    uint32_t _bufSiz;
};

#endif // _FLASHAPPLET_H