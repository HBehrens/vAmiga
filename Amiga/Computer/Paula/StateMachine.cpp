// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Amiga.h"

StateMachine::StateMachine()
{
    // Register snapshot items
    registerSnapshotItems(vector<SnapshotItem> {

        { &state,        sizeof(state),        0 },

        { &audlenLatch,  sizeof(audlenLatch),  0 },
        { &audlen,       sizeof(audlen),       0 },
        { &audperLatch,  sizeof(audperLatch),  0 },
        { &audper,       sizeof(audper),       0 },
        { &audvolLatch,  sizeof(audvolLatch),  0 },
        { &audvol,       sizeof(audvol),       0 },
        { &auddatLatch,  sizeof(auddatLatch),  0 },
        { &auddat,       sizeof(auddat),       0 },
        { &audlcLatch,   sizeof(audlcLatch),   0 },
    });
}

void
StateMachine::setNr(uint8_t nr)
{
    assert(nr < 4);
    this->nr = nr;

    switch (nr) {

        case 0: setDescription("StateMachine 0"); return;
        case 1: setDescription("StateMachine 1"); return;
        case 2: setDescription("StateMachine 2"); return;
        case 3: setDescription("StateMachine 3"); return;

        default: assert(false);
    }
}

void
StateMachine::_initialize()
{
    agnus = &amiga->agnus;
    paula = &amiga->paula;
}

void
StateMachine::_inspect()
{
    // Prevent external access to variable 'info'
    pthread_mutex_lock(&lock);

    info.state = state;
    info.audlenLatch = audlenLatch;
    info.audlen = audlen;
    info.audperLatch = audperLatch;
    info.audper = audper;
    info.audvolLatch = audvolLatch;
    info.audvol = audvol;
    info.auddatLatch = auddatLatch;
    info.auddat = auddat;
    info.audlcLatch = audlcLatch;

    pthread_mutex_unlock(&lock);
}

AudioChannelInfo
StateMachine::getInfo()
{
    AudioChannelInfo result;

    pthread_mutex_lock(&lock);
    result = info;
    pthread_mutex_unlock(&lock);

    return result;
}

void
StateMachine::pokeAUDxLEN(uint16_t value)
{
    debug(AUD_DEBUG, "pokeAUD%dLEN(%X)\n", nr, value);

    audlenLatch = value;
}

void
StateMachine::pokeAUDxPER(uint16_t value)
{
    debug(AUD_DEBUG, "pokeAUD%dPER(%X)\n", nr, value);

    audperLatch = value;
}

void
StateMachine::pokeAUDxVOL(uint16_t value)
{
    debug(AUD_DEBUG, "pokeAUD%dVOL(%X)\n", nr, value);

    /* Behaviour: 1. Only the lowest 7 bits are evaluated.
     *            2. All values greater than 64 are treated as 64 (max volume).
     */
    audvolLatch = MIN(value & 0x7F, 64);
}

void
StateMachine::pokeAUDxDAT(uint16_t value)
{
    debug(AUD_DEBUG, "pokeAUD%dDAT(%X)\n", nr, value);

    auddatLatch = value;

    /* "In interrupt-driven operation, transfer to the main loop (states 010
     *  and 011) occurs immediately after data is written by the processor."
     * [HRM]
     */
    if (!dmaMode() && !irqIsPending()) {

        audvol = audvolLatch;
        audper += audperLatch;
        paula->pokeINTREQ(0x8000 | (0x80 << nr));
    }
}

bool
StateMachine::dmaMode()
{
    return amiga->agnus.audDMA(nr);
}

bool
StateMachine::irqIsPending()
{
    return GET_BIT(amiga->paula.intreq, 7 + nr);
}

int16_t
StateMachine::execute(DMACycle cycles)
{
    switch(state) {

        case 0b000: // State 0 (Idle)

            audlen = audlenLatch;
            agnus->audlc[nr] = audlcLatch;
            audper = 0;
            state = 0b001;
            break;

        case 0b001: // State 1

            if (audlen > 1) audlen--;

            // Trigger Audio interrupt
            paula->pokeINTREQ(0x8000 | (0x80 << nr));

            state = 0b101;
            break;

        case 0b010: // State 2

            audper -= cycles;

            if (audper < 0) {

                audper += audperLatch;
                audvol = audvolLatch;

                // Put out the high byte
                auddat = HI_BYTE(auddatLatch);

                // Switch forth to state 3
                state = 0b011;
            }
            break;

        case 0b011: // State 3

            // Decrease the period counter
            audper -= cycles;

            // Only continue if the period counter did underflow
            if (audper > 1) break;

            // Reload the period counter
            audper += audperLatch;

            // ??? Can't find this in the state machine (from WinFellow?)
            audvol = audvolLatch;

            // Put out the low byte
            auddat = LO_BYTE(auddatLatch);

            // Read the next two samples from memory
            auddatLatch = agnus->doAudioDMA(nr);

            // Switch to next state
            state = 0b010;

            // Perform DMA mode specific action
            if (dmaMode()) {

                // Decrease the length counter
                if (audlen > 1) {
                    audlen--;
                } else {
                    audlen = audlenLatch;
                    agnus->audlc[nr] = audlcLatch;

                    // Trigger Audio interrupt
                    paula->pokeINTREQ(0x8000 | (0x80 << nr));
                }

            // Perform non-DMA mode specific action
            } else {

                // Trigger Audio interrupt
                paula->pokeINTREQ(0x8000 | (0x80 << nr));

                // Go idle if the audio IRQ hasn't been acknowledged
                if (irqIsPending()) state = 0b000;
            }

            break;

        case 0b101: // State 5

            audvol = audvolLatch;

            // (2)
            audper = 0; // ???? SHOULD BE: audper += audperLatch;

            // Read the next two samples from memory
            auddatLatch = agnus->doAudioDMA(nr);

            if (audlen > 1) {
                audlen--;
            } else {
                audlen = audlenLatch;
                agnus->audlc[nr] = audlcLatch;

                // Trigger Audio interrupt
                paula->pokeINTREQ(0x8000 | (0x80 << nr));
            }

            // Transition to state 2
            state = 0b010;
            break;

        default:
            assert(false);
            break;

    }

    return (int8_t)auddat * audvolLatch;
}
