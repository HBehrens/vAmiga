// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Carbon.HIToolbox

class PreferencesWindow: DialogWindow {
    
    override func keyDown(with event: NSEvent) {
                
        track()
        
        if let parent = delegate as? PreferencesController {
            if parent.keyDown(with: MacKey.init(event: event)) {
                return
            }
        }
        
        // The controller wasn't interested. Process it as usual.
        interpretKeyEvents([event])
    }

    override func flagsChanged(with event: NSEvent) {

        track()
        
        /* We do not allow the Cmd and Opt keys as emulation keys because they
         * interact directly with the OS when pressed in combination with
         * certain other keys.
         */
        
        let controller = delegate as? PreferencesController
        
        switch Int(event.keyCode) {
            
        case kVK_Shift where event.modifierFlags.contains(.shift):
            controller?.keyDown(with: MacKey.shift)
        case kVK_RightShift where event.modifierFlags.contains(.shift):
            controller?.keyDown(with: MacKey.rightShift)
            
        /*
        case kVK_Control where event.modifierFlags.contains(.control):
            controller?.keyDown(with: MacKey.control)
        case kVK_RightControl where event.modifierFlags.contains(.control):
            controller?.keyDown(with: MacKey.rightControl)
        */
        
        case kVK_Option where event.modifierFlags.contains(.option):
            controller?.keyDown(with: MacKey.option)
        case kVK_RightOption where event.modifierFlags.contains(.option):
            controller?.keyDown(with: MacKey.rightOption)

        default:
            break
        }
    }
}
