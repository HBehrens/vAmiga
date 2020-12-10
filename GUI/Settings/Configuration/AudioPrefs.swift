// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func refreshAudioTab() {
        
        let config = amiga.config.audio
        
        // In
        audVol0.doubleValue = exp2(config.vol.0 / 0.0000025) * 100
        audVol1.doubleValue = exp2(config.vol.1 / 0.0000025) * 100
        audVol2.doubleValue = exp2(config.vol.2 / 0.0000025) * 100
        audVol3.doubleValue = exp2(config.vol.3 / 0.0000025) * 100
        audPan0.doubleValue = config.pan.0
        audPan1.doubleValue = config.pan.1
        audPan2.doubleValue = config.pan.2
        audPan3.doubleValue = config.pan.3

        // Out
        audVolL.doubleValue = exp2(config.volL) * 100
        audVolR.doubleValue = exp2(config.volR) * 100
        audSamplingMethod.selectItem(withTag: config.samplingMethod.rawValue)
        audFilterType.selectItem(withTag: config.filterType.rawValue)
        audFilterAlwaysOn.state = config.filterAlwaysOn ? .on : .off
        audFilterAlwaysOn.isEnabled = config.filterType != .FILT_NONE

        // Buttons
        audPowerButton.isHidden = !bootable
    }

    @IBAction func audVol0Action(_ sender: NSSlider!) {

        track()
        config.vol0 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol1Action(_ sender: NSSlider!) {

        track()
        config.vol1 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol2Action(_ sender: NSSlider!) {

        track()
        config.vol2 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol3Action(_ sender: NSSlider!) {

        track()
        config.vol3 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan0Action(_ sender: NSSlider!) {
        
        track()
        config.pan0 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan1Action(_ sender: NSSlider!) {
        
        track()
        config.pan1 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan2Action(_ sender: NSSlider!) {
        
        track()
        config.pan2 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan3Action(_ sender: NSSlider!) {
        
        track()
        config.pan3 = sender.integerValue
        refresh()
    }

    @IBAction func audVolLAction(_ sender: NSSlider!) {
        
        track()
        config.volL = sender.integerValue
        refresh()
    }

    @IBAction func audVolRAction(_ sender: NSSlider!) {
        
        config.volR = sender.integerValue
        refresh()
    }

    @IBAction func audSamplingMethodAction(_ sender: NSPopUpButton!) {
        
        config.samplingMethod = sender.selectedTag()
        refresh()
    }
    
    @IBAction func audFilterTypeAction(_ sender: NSPopUpButton!) {
        
        config.filterType = sender.selectedTag()
        refresh()
    }

    @IBAction func audFilterAlwaysOnAction(_ sender: NSButton!) {

        config.filterAlwaysOn = sender.state == .on
        refresh()
    }
    
    @IBAction func audPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        
        switch sender.selectedTag() {
        case 0: config.loadAudioDefaults(AudioDefaults.std)
        case 1: config.loadAudioDefaults(AudioDefaults.stereo)
        case 2: config.loadAudioDefaults(AudioDefaults.mono)
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func audDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveAudioUserDefaults()
    }
}
