// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController: NSMenuItemValidation {
    
    open func validateMenuItem(_ item: NSMenuItem) -> Bool {
        
        let powered = amiga.poweredOn
        let running = amiga.running
        let paused = amiga.paused
        let recording = amiga.screenRecorder.recording
        let counter = amiga.screenRecorder.recordCounter
        
        var dfn: DriveProxy { return amiga.df(item.tag)! }
        
        func validateURLlist(_ list: [URL], image: NSImage) -> Bool {
            
            let slot = item.tag % 10
            
            if let url = myAppDelegate.getRecentlyUsedURL(slot, from: list) {
                item.title = url.lastPathComponent
                item.isHidden = false
                item.image = image
            } else {
                item.title = ""
                item.isHidden = true
                item.image = nil
            }
            
            return true
        }
        
        switch item.action {
            
        // Machine menu
        case #selector(MyController.resetConfigAction(_:)):
            return !powered

        case #selector(MyController.captureScreenAction(_:)):
            item.title = recording ? "Stop Recording" : "Record Screen"
            return true

        case #selector(MyController.exportVideoAction(_:)):
            return counter > 0

        // Edit menu
        case #selector(MyController.stopAndGoAction(_:)):
            item.title = running ? "Pause" : "Continue"
            return true
            
        case #selector(MyController.powerAction(_:)):
            item.title = powered ? "Power Off" : "Power On"
            return true
            
        case #selector(MyController.stepIntoAction(_:)),
             #selector(MyController.stepOverAction(_:)),
             #selector(MyController.stopAndGoAction(_:)):
            return paused

        // View menu
        case #selector(MyController.toggleStatusBarAction(_:)):
            item.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
            return true
            
        // Keyboard menu
        case #selector(MyController.mapCmdKeysAction(_:)):
            item.state = (myAppDelegate.eventTap != nil) ? .on : .off
            return true
            
        // Drive menu
        case #selector(MyController.insertRecentDiskAction(_:)):
            
            return validateURLlist(myAppDelegate.recentlyInsertedDiskURLs, image: smallDisk)
            
        case  #selector(MyController.ejectDiskAction(_:)),
              #selector(MyController.exportDiskAction(_:)):
            return dfn.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction0(_:)):
            return amiga.df0.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction1(_:)):
            return amiga.df1.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction2(_:)):
            return amiga.df2.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction3(_:)):
            return amiga.df3.hasDisk
            
        case #selector(MyController.exportRecentDiskAction(_:)):
            switch item.tag {
            case 0: return validateURLlist(myAppDelegate.recentlyExportedDisk0URLs, image: smallDisk)
            case 10: return validateURLlist(myAppDelegate.recentlyExportedDisk1URLs, image: smallDisk)
            case 20: return validateURLlist(myAppDelegate.recentlyExportedDisk2URLs, image: smallDisk)
            case 30: return validateURLlist(myAppDelegate.recentlyExportedDisk3URLs, image: smallDisk)
            default: fatalError()
            }
            
        case #selector(MyController.writeProtectAction(_:)):
            item.state = dfn.hasWriteProtectedDisk() ? .on : .off
            return dfn.hasDisk
            
        case #selector(MyController.dragAndDropTargetAction(_:)):
            item.state = dfn === dragAndDropDrive ? .on : .off
            return true
                                    
        default:
            return true
        }
    }
    
    func hideOrShowDriveMenus() {
        
        myAppDelegate.df0Menu.isHidden = !config.df0Connected
        myAppDelegate.df1Menu.isHidden = !config.df1Connected
        myAppDelegate.df2Menu.isHidden = !config.df2Connected
        myAppDelegate.df3Menu.isHidden = !config.df3Connected
    }
    
    //
    // Action methods (App menu)
    //
    
    @IBAction func preferencesAction(_ sender: Any!) {
        
        if myAppDelegate.prefController == nil {
            myAppDelegate.prefController =
                PreferencesController.make(parent: self,
                                           nibName: NSNib.Name("Preferences"))
        }
        myAppDelegate.prefController!.showWindow(self)
        myAppDelegate.prefController!.refresh()
    }
    
    @IBAction func resetConfigAction(_ sender: Any!) {
        
        track()
        
        UserDefaults.resetRomUserDefaults()
        UserDefaults.resetHardwareUserDefaults()
        UserDefaults.resetPeripheralsUserDefaults()
        UserDefaults.resetCompatibilityUserDefaults()
        UserDefaults.resetAudioUserDefaults()
        UserDefaults.resetVideoUserDefaults()
        
        amiga.suspend()
        config.loadRomUserDefaults()
        config.loadHardwareUserDefaults()
        config.loadPeripheralsUserDefaults()
        config.loadCompatibilityUserDefaults()
        config.loadAudioUserDefaults()
        config.loadVideoUserDefaults()
        amiga.resume()
    }
    
    //
    // Action methods (File menu)
    //
    
    func openConfigurator(tab: String = "") {
        
        if configurator == nil {
            let name = NSNib.Name("Configuration")
            configurator = ConfigurationController.make(parent: self, nibName: name)
        }
        configurator?.showSheet(tab: tab)
    }
    
    @IBAction func configureAction(_ sender: Any!) {
        
        openConfigurator()
    }
 
    @IBAction func inspectorAction(_ sender: Any!) {
        
        if inspector == nil {
            inspector = Inspector.make(parent: self, nibName: "Inspector")
        }
        inspector?.showWindow(self)
    }
    
    @IBAction func monitorAction(_ sender: Any!) {
        
        if monitor == nil {
            monitor = Monitor.make(parent: self, nibName: "Monitor")
        }
        monitor?.showWindow(self)
    }
 
    @IBAction func consoleAction(_ sender: Any!) {
        
        if renderer.console.isVisible {
            renderer.console.close(delay: 0.25)
        } else {
            renderer.console.open(delay: 0.25)
        }
    }
    
    @IBAction func takeSnapshotAction(_ sender: Any!) {
        
        amiga.requestUserSnapshot()
    }
    
    @IBAction func restoreSnapshotAction(_ sender: Any!) {
        
        if !restoreLatestSnapshot() {
            NSSound.beep()
        }
    }
    
    @IBAction func browseSnapshotsAction(_ sender: Any!) {
        
        if snapshotBrowser == nil {
            let name = NSNib.Name("SnapshotDialog")
            snapshotBrowser = SnapshotDialog.make(parent: self, nibName: name)
        }
        snapshotBrowser?.showSheet()
    }
    
    @IBAction func takeScreenshotAction(_ sender: Any!) {
        
        track()
        
        // Determine screenshot format
        let format = ScreenshotSource.init(rawValue: pref.screenshotSource)!
        
        // Take screenshot
        guard let screen = renderer.screenshot(source: format) else {
            track("Failed to create screenshot")
            return
        }

        // Convert to Screenshot object
        let screenshot = Screenshot.init(screen: screen, format: pref.screenshotTarget)

        // Save to disk
        try? screenshot.save(id: mydocument.bootDiskID)
    }
    
    @IBAction func browseScreenshotsAction(_ sender: Any!) {
        
        if screenshotBrowser == nil {
            let name = NSNib.Name("ScreenshotDialog")
            screenshotBrowser = ScreenshotDialog.make(parent: self, nibName: name)
        }
        screenshotBrowser?.checksum = amiga.df0.fnv
        screenshotBrowser?.showSheet()
    }
    
    @IBAction func captureScreenAction(_ sender: Any!) {
        
        track("Recording = \(amiga.screenRecorder.recording)")
        
        if amiga.screenRecorder.recording {
            
            amiga.screenRecorder.stopRecording()
            return
        }
        
        if !amiga.screenRecorder.hasFFmpeg {
            showMissingFFmpegAlert()
            return
        }
        
        var rect: CGRect
        if pref.captureSource == 0 {
            rect = renderer.textureRectAbs
        } else {
            rect = renderer.entire
        }
        
        track("Cature source = \(pref.captureSource)")
        track("(\(rect.minX),\(rect.minY)) - (\(rect.maxX),\(rect.maxY))")
        
        let success = amiga.screenRecorder.startRecording(rect,
                                                          bitRate: pref.bitRate,
                                                          aspectX: pref.aspectX,
                                                          aspectY: pref.aspectY)
        
        if !success {
            showFailedToLaunchFFmpegAlert()
            return
        }
        
    }
    
    @IBAction func exportVideoAction(_ sender: Any!) {
        
        track()
        
        let name = NSNib.Name("ExportVideoDialog")
        let exporter = ExportVideoDialog.make(parent: self, nibName: name)
        
        exporter?.showSheet()
    }
    
    //
    // Action methods (Edit menu)
    //
    
    @IBAction func paste(_ sender: Any!) {
        
        track()
        
        let pasteBoard = NSPasteboard.general
        guard let text = pasteBoard.string(forType: .string) else {
            track("Cannot paste. No text in pasteboard")
            return
        }
        
        keyboard.autoType(text)
    }
    
    @IBAction func stopAndGoAction(_ sender: Any!) {
        
        amiga?.stopAndGo()
    }
    
    @IBAction func stepIntoAction(_ sender: Any!) {
        
        needsSaving = true
        amiga?.stepInto()
    }
    
    @IBAction func stepOverAction(_ sender: Any!) {
        
        needsSaving = true
        amiga?.stepOver()
    }
    
    @IBAction func resetAction(_ sender: Any!) {
        
        track()
        amiga.hardReset()
        amiga.powerOn()
        amiga.run()
    }
    
    @IBAction func powerAction(_ sender: Any!) {
        
        var error: ErrorCode = .OK

        if amiga.poweredOn {
            amiga.pause()
            amiga.powerOff()
            return
        }
        
        if amiga.isReady(&error) {
            amiga.powerOn()
            amiga.run()
        } else {
            mydocument.showConfigurationAltert(error)
        }
    }
    
    //
    // Action methods (View menu)
    //
    
    @IBAction func toggleStatusBarAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.toggleStatusBarAction(sender)
        }
        
        showStatusBar(!statusBar)
    }
    
    //
    // Action methods (Keyboard menu)
    //
    
    @IBAction func keyboardAction(_ sender: Any!) {
        
        // Open the virtual keyboard as a sheet
        if virtualKeyboard == nil {
            virtualKeyboard = VirtualKeyboardController.make(parent: self)
        }
        virtualKeyboard?.showSheet()
    }
     
    @IBAction func mapCmdKeysAction(_ sender: Any!) {
        
        myAppDelegate.mapCommandKeys = !myAppDelegate.mapCommandKeys
        refreshStatusBar()
    }
    
    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        amiga.keyboard.releaseAllKeys()
    }
    
    @IBAction func delKeyAction(_ sender: Any!) {
        
        track()
        type(keyCode: AmigaKeycode.delete)
    }

    @IBAction func helpKeyAction(_ sender: Any!) {
        
        track()
        type(keyCode: AmigaKeycode.help)
    }
    
    func type(keyCode: Int, completion: (() -> Void)? = nil) {
        
        DispatchQueue.global().async {
            
            self.amiga.keyboard.pressKey(keyCode)
            usleep(useconds_t(20000))
            self.amiga.keyboard.releaseKey(keyCode)
            completion?()
        }
    }
    
    //
    // Action methods (Disk menu)
    //
    
    @IBAction func newDiskAction(_ sender: NSMenuItem!) {
        
        track()

        // Get drive type
        let type = DriveType.init(rawValue: config.dfnType(sender.tag))
        
        // Create a blank disk
        var adf: ADFFileProxy
        switch type {
        case .DD_35: adf = ADFFileProxy.make(with: .INCH_35, density: .DD)
        case .HD_35: adf = ADFFileProxy.make(with: .INCH_35, density: .HD)
        case .DD_525: adf = ADFFileProxy.make(with: .INCH_525, density: .DD)
        default: fatalError()
        }
        
        // Write file system
        adf.formatDisk(pref.blankDiskFormat, bootBlock: pref.bootBlock)
        
        // Insert disk into drive
        amiga.diskController.insert(sender.tag, file: adf)

        myAppDelegate.clearRecentlyExportedDiskURLs(drive: sender.tag)
    }
    
    /*
    @IBAction func newDiskAction(_ sender: NSMenuItem!) {
        
        track()

        // Get drive type
        let type = DriveType.init(rawValue: config.dfnType(sender.tag))
        
        // Create a blank disk
        var adf: ADFFileProxy
        switch type {
        case .DD_35: adf = ADFFileProxy.make(with: .INCH_35, density: .DD)
        case .HD_35: adf = ADFFileProxy.make(with: .INCH_35, density: .HD)
        case .DD_525: adf = ADFFileProxy.make(with: .INCH_525, density: .DD)
        default: fatalError()
        }
        
        // Write file system
        adf.formatDisk(pref.blankDiskFormat, bootBlock: pref.bootBlock)
        
        // Insert disk into drive
        amiga.diskController.insert(sender.tag, file: adf)

        myAppDelegate.clearRecentlyExportedDiskURLs(drive: sender.tag)
    }
    */
    
    @IBAction func insertDiskAction(_ sender: NSMenuItem!) {
        
        // Ask the user if an unsafed disk should be replaced
        if !proceedWithUnexportedDisk(drive: sender.tag) { return }
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = true
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["adf", "img", "ima", "dms", "exe", "adz", "zip", "gz"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in

            if result == .OK, let url = openPanel.url {
                self.insertDiskAction(from: url, drive: sender.tag)
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: NSMenuItem!) {
        
        let drive = sender.tag / 10
        let slot  = sender.tag % 10
        
        insertRecentDiskAction(drive: drive, slot: slot)
    }

    func insertRecentDiskAction(drive: Int, slot: Int) {
        
        if let url = myAppDelegate.getRecentlyInsertedDiskURL(slot) {
            insertDiskAction(from: url, drive: drive)
        }
    }
    
    func insertDiskAction(from url: URL, drive: Int) {
        
        track("insertDiskAction \(url) drive \(drive)")
        
        let types: [FileType] = [ .ADF, .HDF, .DMS, .EXE, .DIR ]
        
        do {
            // Try to create a file proxy
            try mydocument.createAttachment(from: url, allowedTypes: types)

            // Ask the user if an unsafed disk should be replaced
            if !proceedWithUnexportedDisk(drive: drive) { return }
            
            if let file = mydocument.amigaAttachment as? DiskFileProxy {
                
                // Insert the disk
                amiga.diskController.insert(drive, file: file)
                        
                // Remember the URL
                myAppDelegate.noteNewRecentlyInsertedDiskURL(url)
            }
            
        } catch {
            
            (error as? VAError)?.cantOpen(url: url)
        }
    }
    
    @IBAction func writeProtectAction(_ sender: NSMenuItem!) {
        
        amiga.suspend()
        amiga.df(sender)!.toggleWriteProtection()
        amiga.resume()
    }
    
    @IBAction func exportRecentDiskDummyAction0(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction1(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction2(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction3(_ sender: NSMenuItem!) {}
    
    @IBAction func exportRecentDiskAction(_ sender: NSMenuItem!) {
        
        track()
        
        let drive = sender.tag / 10
        let slot  = sender.tag % 10
                
        exportRecentDiskAction(drive: drive, slot: slot)
    }
    
    func exportRecentDiskAction(drive nr: Int, slot: Int) {
        
        track("drive: \(nr) slot: \(slot)")
        
        if let url = myAppDelegate.getRecentlyExportedDiskURL(slot, drive: nr) {
            
            do {
                try mydocument.export(drive: nr, to: url)
                
            } catch let error as VAError {
                error.warning("Cannot export disk to file \"\(url.path)\"")
            } catch {
                fatalError()
            }
        }
    }

    @IBAction func clearRecentlyInsertedDisksAction(_ sender: NSMenuItem!) {
        
        myAppDelegate.recentlyInsertedDiskURLs = []
    }
    
    @IBAction func clearRecentlyExportedDisksAction(_ sender: NSMenuItem!) {
        
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: sender.tag)
    }
    
    @IBAction func ejectDiskAction(_ sender: NSMenuItem!) {
        
        if proceedWithUnexportedDisk(drive: sender.tag) {
            amiga.diskController.eject(sender.tag)
            myAppDelegate.clearRecentlyExportedDiskURLs(drive: sender.tag)
        }
    }
    
    @IBAction func exportDiskAction(_ sender: NSMenuItem!) {
        
        let nibName = NSNib.Name("ExporterDialog")
        let exportPanel = ExporterDialog.make(parent: self, nibName: nibName)
        exportPanel?.showSheet(forDrive: sender.tag)
    }
    
    @IBAction func dragAndDropTargetAction(_ sender: NSMenuItem!) {
        
        let drive = amiga.df(sender)
        dragAndDropDrive = (dragAndDropDrive == drive) ? nil : drive
    }
}
